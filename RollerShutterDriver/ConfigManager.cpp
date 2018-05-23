#include "ConfigManager.h"
#include <ArduinoJson.h>
#include "FS.h"

void ConfigManager::Init(Logger &logger)
{
	this->logger = logger;

	if (!SPIFFS.begin())
	{
		logger.LogLine("Failed to mount file system");
		return;
	}
}

bool ConfigManager::SaveConfig()
{
	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& json = jsonBuffer.createObject();

	json["wifiname"] = WifiName;
	json["wifipass"] = WifiPass;
	json["infourl"] = InfoUrl;
	json["port"] = Port;
	json["uri"] = Uri;

	File configFile = SPIFFS.open("/config.json", "w");
	if (!configFile) 
	{
		logger.LogLine("Failed to open config file for writing");
		return false;
	}
	json.prettyPrintTo(Serial);
	json.printTo(configFile);

	configFile.close();

	ESP.restart();

	return true;
}

bool ConfigManager::LoadConfig()
{
	File configFile;
	for (int i = 0; i < 2; ++i)
	{
		configFile = SPIFFS.open("/config.json", "r");
		if (!configFile)
		{
			logger.LogLine("Config file doesn't exist. Creating new...");
			if (SaveConfig())
				continue;
		}
	}

	if (!configFile)
	{
		logger.LogLine("Failed to open config file");
		return false;
	}

	size_t size = configFile.size();
	if (size > 1024) 
	{
		logger.LogLine("Config file size is too large");
		return false;
	}

	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);

	// We don't use String here because ArduinoJson library requires the input
	// buffer to be mutable. If you don't use ArduinoJson, you may as well
	// use configFile.readString instead.
	configFile.readBytes(buf.get(), size);

	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& json = jsonBuffer.parseObject(buf.get());

	if (!json.success())
	{
		logger.LogLine("Failed to parse config file");
		configFile.close();
		return false;
	}

	const char* wifiname = json["wifiname"];
	const char* wifipass = json["wifipass"];
	const char* infourl = json["infourl"];
	const char* port = json["port"];
	const char* uri = json["uri"];

	logger.LogLine("Loaded config");
	
	WifiName = wifiname;
	WifiPass = wifipass;
	InfoUrl = infourl;
	Port = port;
	Uri = uri;

	configFile.close();

	return true;
}
