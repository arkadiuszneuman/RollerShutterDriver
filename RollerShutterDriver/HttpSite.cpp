#include "HttpSite.h"

const char MAIN_page[] PROGMEM = R"=====(
	<HTML>
		<HEAD>
				<TITLE>My first web page</TITLE>
		</HEAD>
	<BODY>
		<CENTER>
				<B>Hello page</B>
		</CENTER>	
	</BODY>
	</HTML>
	)=====";

void HttpSite::Init()
{
	server = new ESP8266WebServer(80);
	server->on("/", std::bind(&HttpSite::HandleRoot, this));
	server->begin();
	Serial.println("HTTP server started");
}

void HttpSite::Update()
{
	server->handleClient();          //Handle client requests
}

void HttpSite::HandleRoot()
{
	Serial.println("request for /");
	String s = MAIN_page; //Read HTML contents
	server->send(200, "text/html", s); //Send web page
}
