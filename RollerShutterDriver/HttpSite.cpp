#include "HttpSite.h"
#include "index.h"
#include "config.h"

void HttpSite::Init()
{
	server = new ESP8266WebServer(80);

	server->on("/", std::bind(&HttpSite::Index, this));
	server->on("/config", std::bind(&HttpSite::Config, this));
	server->on("/save", HTTPMethod::HTTP_POST, std::bind(&HttpSite::ConfigPost, this));

	server->begin();
	Serial.println("HTTP server started");
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
	Serial.println("request for /");
	String html = page_index;
	html.replace("{status}", status);
	server->send(200, "text/html", html);
}

void HttpSite::Config()
{
	Serial.println("request for /config");
	String html = page_config;
	html.replace("{wifiname}", "some name");
	html.replace("{wifipass}", "a");
	server->send(200, "text/html", html);
}

void HttpSite::ConfigPost()
{
	Serial.println("post to /config");
	String wifiname = server->arg("wifiname");
	String wifipass = server->arg("wifipassword");

	Serial.println(wifiname);
	Serial.println(wifipass);

	Config();
}
