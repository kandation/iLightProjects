#include "ESP8266WiFi.h"
#include "HTTPSRedirect.h"
#include "DebugMacros.h"

// Fill ssid and password with your network credentials
const char* ssid = "";
const char* password = "";

const char* host = "script.google.com";
// Replace with your own script id to make server side changes
const char *GScriptId = "AKfycbx7cup7-dfKPlmfDQVlyXO6Co0QbBm2ae9cJgcyaKcPfk0hMoM";
const int httpsPort = 443;

// echo | openssl s_client -connect script.google.com:443 |& openssl x509 -fingerprint -noout
const char* fingerprint = "C2:00:62:1D:F7:ED:AF:8B:D1:D5:1D:24:D6:F0:A1:3A:EB:F1:B4:92";


// Write to Google Spreadsheet
String url = String("/macros/s/") + GScriptId + "/exec";
HTTPSRedirect* client = nullptr;


void accessWifi() {
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void networkStep_01hostConnection() {
  // Use HTTPSRedirect class to create a new TLS connection
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(false);
  client->setContentTypeHeader("application/text");
}

void networkStep_02CheckHost() {
  // Try to connect for a maximum of 5 times
  bool flag = false;
  for (int i = 0; i < 5; i++) {
    int retval = client->connect(host, httpsPort);
    if (retval == 1) {
      flag = true;
      break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  if (!flag) {
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return;
  }
}

void network_getValue() {
  client->GET(url, host);
  String line = client->getResponseBody();
  Serial.println("--------");
  Serial.println(line);
}

int getPM25Level(){
  String line = client->getResponseBody();
  return line.toInt();
}

int network_secureGetPM25Lavel(){
  static int error_count = 0;
  static int connect_count = 0;
  const unsigned int MAX_CONNECT = 5;
  static bool flag = false;
  int value = -2;

  if (!flag) {
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    flag = true;
    client->setPrintResponseBody(false);
    client->setContentTypeHeader("application/text");
  }

  if (client != nullptr){
    if (!client->connected()){
      client->connect(host, httpsPort);
    }
  }
  else{
    DPRINTLN("Error creating client object!");
    error_count = 5;
  } 



  /*if (connect_count > MAX_CONNECT) {
    //error_count = 5;
    connect_count = 0;
    flag = false;
    delete client;
    return -99;
  }*/


  if (client->GET(url, host)) {
    ++connect_count;
    value = getPM25Level();

  }
  else {
    ++error_count;
    DPRINT("Error-count while connecting: ");
    DPRINTLN(error_count);
  }

  if (error_count > 3) {
    Serial.println("Halting processor...");
    delete client;
    client = nullptr;
    Serial.flush();
    //ESP.deepSleep(0);
  }

 return value;
}

void setup() {
  Serial.begin(115200);
  Serial.flush();

  accessWifi();
  networkStep_01hostConnection();
  networkStep_02CheckHost();



  //delete HTTPSRedirect object;
  delete client;
  client = nullptr;
}

void loop() {
  Serial.println(network_secureGetPM25Lavel());
  delay(1000);

}
