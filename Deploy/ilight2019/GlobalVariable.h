#ifndef GLOBAALVARIABLE_H
#define GLOBAALVARIABLE_H

#include <Arduino.h>
/* ----------------- PM25 WIFI -------------------*/
// Fill ssid and password with your network credentials
const char *ssid = "";
const char *password = "";

const char *host = "script.google.com";
// Replace with your own script id to make server side changes
const char *GScriptId = "AKfycbx7cup7-dfKPlmfDQVlyXO6Co0QbBm2ae9cJgcyaKcPfk0hMoM";
const int httpsPort = 443;

// echo | openssl s_client -connect script.google.com:443 |& openssl x509 -fingerprint -noout
const char *fingerprint = "C2:00:62:1D:F7:ED:AF:8B:D1:D5:1D:24:D6:F0:A1:3A:EB:F1:B4:92";

// Write to Google Spreadsheet
String url = String("/macros/s/") + GScriptId + "/exec";
#endif