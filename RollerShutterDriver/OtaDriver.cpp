#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "OtaDriver.h"

void OtaDriver::Init(Logger &logger)
{
	this->logger = logger;

	// Port defaults to 8266
	// ArduinoOTA.setPort(8266);

	// Hostname defaults to esp8266-[ChipID]
	// ArduinoOTA.setHostname("myesp8266");

	// No authentication by default
	// ArduinoOTA.setPassword("admin");

	// Password can be set with it's md5 value as well
	// MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
	// ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

	ArduinoOTA.onStart([this]() {
		String type;
		if (ArduinoOTA.getCommand() == U_FLASH)
			type = "sketch";
		else // U_SPIFFS
			type = "filesystem";

		// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
		this->logger.LogLine("Start updating " + type);
	});
	ArduinoOTA.onEnd([this]() {
		this->logger.LogLine("\nEnd");
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	});
	ArduinoOTA.onError([this](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) this->logger.LogLine("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) this->logger.LogLine("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) this->logger.LogLine("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) this->logger.LogLine("Receive Failed");
		else if (error == OTA_END_ERROR) this->logger.LogLine("End Failed");
	});
	ArduinoOTA.begin();
	logger.LogLine("OTA Ready");
	logger.Log("IP address: ");
	logger.LogLine(WiFi.localIP().toString());
}

void OtaDriver::Update()
{
	ArduinoOTA.handle();
}