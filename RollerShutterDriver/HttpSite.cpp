#include "HttpSite.h"
#include "index.h"
#include "config.h"

void HttpSite::Init(ConfigManager configManager, Logger logger,
	void (*receivedLevelFunc)(int))
{
	server = new ESP8266WebServer(80);
	this->configManager = configManager;
	this->logger = logger;
	this->receivedLevelFunc = receivedLevelFunc;

	server->on("/", std::bind(&HttpSite::Index, this));
	server->on("/config", std::bind(&HttpSite::Config, this));
	server->on("/save", HTTPMethod::HTTP_POST, std::bind(&HttpSite::ConfigPost, this));
	server->on("/resetsettings", HTTPMethod::HTTP_POST, std::bind(&HttpSite::ResetSettings, this));
	server->on("/changelevel", std::bind(&HttpSite::ChangeLevel, this));

	server->begin();
	logger.LogLine("HTTP server started");
}

void HttpSite::Update()
{
	server->handleClient();
}

void HttpSite::SetStatus(String status)
{
	this->status = status;
}

void HttpSite::Index()
{
	logger.LogLine("request for /");
	String html = page_index;
	html.replace("{status}", status);
	server->send(200, "text/html", html);
}

void HttpSite::Config()
{
	logger.LogLine("request for /config");
	String html = page_config;
	html.replace("{wifiname}", configManager.WifiName);
	html.replace("{wifipass}", configManager.WifiPass);
	html.replace("{infourl}", configManager.InfoUrl);
	html.replace("{port}", configManager.Port);
	html.replace("{uri}", configManager.Uri);
	server->send(200, "text/html", html);
}

void HttpSite::ConfigPost()
{
	logger.LogLine("post to /config");
	String wifiname = server->arg("wifiname");
	String wifipass = server->arg("wifipassword");
	String infourl = server->arg("infourl");
	String port = server->arg("port");
	String uri = server->arg("uri");

	configManager.WifiName = wifiname;
	configManager.WifiPass = wifipass;
	configManager.InfoUrl = infourl;
	configManager.Port = port;
	configManager.Uri = uri;

	configManager.SaveConfig();

	Config();
}

void HttpSite::ResetSettings()
{
	logger.LogLine("post to /resetsettings");

	configManager.WifiName = "";
	configManager.WifiPass = "";
	configManager.InfoUrl = "";
	configManager.Port = "";
	configManager.Uri = "";

	configManager.SaveConfig();

	Config();
}

void HttpSite::ChangeLevel()
{
	logger.LogLine("get from /changelevel");

	String stringlevel = server->arg("level");

	if (stringlevel != "")
	{
		int level = atoi(stringlevel.c_str());
		receivedLevelFunc(level);
		server->send(200, "text/html", "Got level");
	}
}

void HttpSite::SendInformationAboutLevel(int level)
{
	if (configManager.InfoUrl != "")
	{
		String url = configManager.InfoUrl;
		String port = configManager.Port;
		String uri = configManager.Uri;

		String levelString(level);
		uri.replace("{level}", levelString);
		logger.Log("sending info about level ");
		logger.Log(url);
		logger.Log(":");
		logger.Log(port);
		logger.LogLine(uri);

		WiFiClient client;
		if (!client.connect(url, atoi(port.c_str()))) {
			logger.LogLine("connection failed");
			return;
		}

		// This will send the request to the server
		client.print(String("GET ") + uri + " HTTP/1.1\r\n" +
			"Host: " + url + "\r\n" +
			"Connection: close\r\n\r\n");
		delay(10);

		// Read all the lines of the reply from server and print them to Serial
		logger.LogLine("Respond:");
		while (client.available()) {
			String line = client.readStringUntil('\r');
			logger.Log(line);
		}

		logger.LogLine();
		logger.LogLine("closing connection");
	}
}
