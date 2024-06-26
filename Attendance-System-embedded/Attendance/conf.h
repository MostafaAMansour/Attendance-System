// define pins for RFID
#define RFID_SS 17
#define RFID_RST 16
// define select pin for SD card module
#define CS_SD 5
// Wifi Settings
// #define WIFI_SSID "TP-Link_957E"
// #define WIFI_PASS "99504090"
// #define WIFI_SSID "HP_kh_79"
// #define WIFI_PASS "kh@21@qwer"
#define WIFI_SSID "Radda"
#define WIFI_PASS "123456789"
// char ftp_server[] = "192.168.1.1";
// char ftp_user[] = "admin";
// char ftp_pass[] = "admin";

// char ftp_server[] = "radda.freevar.com";
// char ftp_user[] = "radda.freevar.com";
// char ftp_pass[] = "123456789";

char ftp_server[] = "teeest.freevar.com";
char ftp_user[] = "teeest.freevar.com";
char ftp_pass[] = "123456789";

#define SuccessfullySendLED 26 // LED endicates that files uploaded succesfully
#define FailedSendLED 27 // LED endicates that files uploaded failed
#define ConnectingLed 14 // LED endicates that WIFI is connected
#define ButtonSend 25

#define RedLed 2 // LED endicates that there is no rfid card
#define GreenLed 15 // LED endicates that RFID read succesfully
#define YellowLed 4 // // LED endicates that there is no sd card or rtc not connected

#define BUFSIZE 16000