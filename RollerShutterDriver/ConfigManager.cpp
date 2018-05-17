#include "ConfigManager.h"
#include <ArduinoJson.h>
#include "FS.h"

void ConfigManager::Init()
{
	if (!SPIFFS.begin())
	{
		Serial.println("Failed to mount file system");
		return;
	}
}

bool ConfigManager::SaveConfig()
{
	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& json = jsonBuffer.createObject();

	json["wifiname"] = WifiName;
	json["wifipass"] = WifiPass;

	File configFile = SPIFFS.open("/config.json", "w");
	if (!configFile) 
	{
		Serial.println("Failed to open config file for writing");
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
			Serial.println("Config file doesn't exist. Creating new...");
			if (SaveConfig())
				continue;
		}
	}

	if (!configFile)
	{
		Serial.println("Failed to open config file");
		return false;
	}

	size_t size = configFile.size();
	if (size > 1024) 
	{
		Serial.println("Config file size is too large");
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
		Serial.println("Failed to parse config file");
		configFile.close();
		return false;
	}

	const char* wifiname = json["wifiname"];
	const char* wifipass = json["wifipass"];

	Serial.println("Loaded config");
	Serial.println(wifiname);
	Serial.println(wifipass);
	
	WifiName = wifiname;
	WifiPass = wifipass;

	configFile.close();

	return true;
}
