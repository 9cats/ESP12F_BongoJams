#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <AsyncElegantOTA.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#define FORMAT_LITTLEFS_IF_FAILED true

// Replace with your network credentials
#define HOST "Skox_Release_0"
const char* ap_ssid     = HOST;
const char* ap_password = "123456789";

const char* sta_ssid     = "dxxy16-402-2";
const char* sta_password = "dxxy16402";

AsyncWebServer server(80);

/* HTTP 服务初始化 */
void Server_Init()
{
  server.end();
  server.serveStatic("/", LittleFS, "/")
  .setCacheControl("max-age=31536000");  //挂载静态网页
  server.onNotFound([](AsyncWebServerRequest* request) {
    File file = LittleFS.open("/404/index.html", "r");
    request->send(LittleFS, "/404/index.html", "text/html", false);
    file.close();
  });
  AsyncElegantOTA.begin(&server);  //挂载 OTA
  server.begin();                  //开启 HTTP服务
  Serial.println("HTTP server started");
}

/* WiFi 初始化 */
void WiFi_Init()
{
  Serial.println("WiFi Setting ...");

  /* 同时开启 STA 和 AP 模式 */
  WiFi.mode(WIFI_AP_STA);
  /*-------- Soft-AP --------*/
  WiFi.softAP(ap_ssid, ap_password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  /*-------- Station --------*/
  // WiFi.begin();
  WiFi.begin(sta_ssid, sta_password);

  //读取默认STA配置
  Serial.print("Connecting to ");
  Serial.println(sta_ssid);
  Serial.print("password:");
  Serial.println(sta_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  /* 开启 mDNS */
  Serial.println("Start mDNS Server!");
  MDNS.begin(HOST);
}

/* 文件系统初始化 */
void FS_Init()
{
  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
}

void setup()
{
  /* GPIO */
  Serial.begin(115200);

  /* LittleFS */
  FS_Init();

  /* WiFi */
  WiFi_Init();
  Server_Init();
}

void loop()
{
  MDNS.update();
  delay(1);
}