#include "Arduino.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include "ESP32_FTPClient.h"
#include <MFRC522.h> // for the RFID
#include "SPI.h"     // for the RFID and SD card module
#include "SD.h"      // for the SD card
#include <Wire.h>
#include <RTClib.h> // for the RTC
#include "FS.h"
#include "conf.h"

#define validate               \
    if (!ftp.isConnected())    \
    {                          \
        ftp.CloseConnection(); \
        file.close();          \
        return 0;              \
    }

uint8_t b[BUFSIZE];

RTC_DS3231 rtc;
MFRC522 mfrc522(RFID_SS, RFID_RST);
ESP32_FTPClient ftp(ftp_server, ftp_user, ftp_pass, 5000, 2);

const char *rootDir = "/";
char PWFile[35];
char TmpPWFile[35];
char PWD[25];

char uidString[40];
// Init array that will store new NUID
byte nuidPICC[4];
DateTime now;

void setup(void)
{
    Serial.begin(115200);
    pinMode(SuccessfullySendLED, OUTPUT);
    pinMode(FailedSendLED, OUTPUT);
    pinMode(ConnectingLed, OUTPUT);
    pinMode(ButtonSend, INPUT);
    pinMode(RedLed, OUTPUT);
    pinMode(GreenLed, OUTPUT);
    pinMode(YellowLed, OUTPUT);

    SPI.begin();
    mfrc522.PCD_Init();
    if (!rtc.begin())
    {
        Serial.println("Couldn't find RTC");
        digitalWrite(YellowLed, HIGH);
    }
    // if (rtc.lostPower())
    // {
    //     Serial.println("RTC power failure, reset the time!");
    //     digitalWrite(YellowLed, HIGH);
    // }
    if (!SD.begin(CS_SD))
    {
        Serial.println("Card Mount Failed");
        digitalWrite(YellowLed, HIGH);
        return;
    }
    now = rtc.now();
    sprintf(PWD, "/%02d-%02d-%02d", now.day(), now.month(), now.year());
    sprintf(PWFile, "/%02d-%02d-%02d/%02d-%02d.csv", now.day(), now.month(), now.year(), now.hour(), now.minute());
    digitalWrite(RFID_SS, HIGH);
    delay(1);
    digitalWrite(CS_SD, LOW);
    if (!SD.exists(PWD))
    {
        createDir(SD, PWD);
    }
}

void loop(void)
{
    if (digitalRead(ButtonSend) == HIGH)
    {
        uploadFile();
    }
    digitalWrite(RFID_SS, LOW);
    delay(1);
    digitalWrite(CS_SD, HIGH);

    digitalWrite(RedLed, HIGH);
    if (!mfrc522.PICC_IsNewCardPresent())
        return;
    if (!mfrc522.PICC_ReadCardSerial())
        return;
    if (mfrc522.uid.uidByte[0] == nuidPICC[0] && mfrc522.uid.uidByte[1] == nuidPICC[1] && mfrc522.uid.uidByte[2] == nuidPICC[2] && mfrc522.uid.uidByte[3] == nuidPICC[3])
    {
        return;
    }

    for (byte i = 0; i < 4; i++)
    {
        nuidPICC[i] = mfrc522.uid.uidByte[i];
    }

    now = rtc.now();
    sprintf(uidString, "%02x:%02x:%02x:%02x, %02d:%02d:%02d\n", nuidPICC[0], nuidPICC[1], nuidPICC[2], nuidPICC[3], now.hour(), now.minute(), now.second());
    Serial.println(uidString);

    digitalWrite(RFID_SS, HIGH);
    delay(1);
    digitalWrite(CS_SD, LOW);
    if (!appendFile(SD, PWFile, uidString))
        return;

    digitalWrite(RedLed, LOW);
    digitalWrite(GreenLed, HIGH);
    delay(500);
    digitalWrite(GreenLed, LOW);
}

bool sendFile()
{
    bool state = 1;
    int del = 0;
    char *dir[10] = {0};
    char pathCpy[strlen(PWD) + 1] = {0};
    strcpy(pathCpy, PWD);

    for (int i = 1, j = 0; pathCpy[i] != 0; ++i)
    {
        if (pathCpy[i - 1] == '/')
        {
            pathCpy[i - 1] = 0;
            dir[j] = &pathCpy[i];
            del++;
            j++;
        }
    }

    digitalWrite(RFID_SS, HIGH);
    delay(1);
    digitalWrite(CS_SD, LOW);
    now = rtc.now();
    sprintf(TmpPWFile, "/%02d-%02d-%02d/%02d-%02d.csv", now.day(), now.month(), now.year(), now.hour(), now.minute());
    SD.rename(PWFile, TmpPWFile);
    strcpy(PWFile, TmpPWFile);
    File file = SD.open(PWFile, "r");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.println("Connecting Wifi...");
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        digitalWrite(ConnectingLed, HIGH);
        delay(100);
        Serial.print(".");
        digitalWrite(ConnectingLed, LOW);
        delay(100);
        if(millis() - start >= 120000){
            file.close();
            return 0;
        }
    }
    digitalWrite(ConnectingLed, HIGH);

    Serial.println("");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    ftp.OpenConnection();
    if (!ftp.isConnected())
    {
        file.close();
        return 0;
    }
    ftp.ChangeWorkDir(rootDir);
    validate;
    for (int i = 0; i < del; ++i)
    {
        ftp.MakeDir(dir[i]);
        ftp.resetConnection();
        ftp.ChangeWorkDir(dir[i]);
        validate;
        Serial.println(dir[i]);
    }
    ftp.InitFile("Type I");
    validate;
    ftp.NewFile(file.name());
    validate;
    for (int i = 0, n = file.size() / BUFSIZE; i < n; ++i)
    {
        file.read(b, sizeof(b));
        ftp.WriteData(b, sizeof(b));
        validate;
    }
    file.read(b, file.size() % BUFSIZE);
    ftp.WriteData(b, file.size() % BUFSIZE);
    validate;
    file.close();
    ftp.CloseFile();
    ftp.CloseConnection();
    return 1;
}

void uploadFile(void)
{
    digitalWrite(SuccessfullySendLED, LOW);
    digitalWrite(FailedSendLED, LOW);
    digitalWrite(ConnectingLed, LOW);

    bool status = sendFile();
    if (status)
    {
        digitalWrite(SuccessfullySendLED, HIGH);
        digitalWrite(FailedSendLED, LOW);
        now = rtc.now();
        sprintf(PWFile, "/%02d-%02d-%02d/%02d-%02d.csv", now.day(), now.month(), now.year(), now.hour(), now.minute() + 1);
    }
    else
    {
        digitalWrite(SuccessfullySendLED, LOW);
        digitalWrite(FailedSendLED, HIGH);
    }
}

bool appendFile(fs::FS &fs, const char *path, const char *message)
{
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file)
    {
        Serial.println("Failed to open file for appending");
        return 0;
    }
    if (file.print(message))
    {
        Serial.println("Message appended");
    }
    else
    {
        Serial.println("Append failed");
    }
    file.close();
    return 1;
}

void createDir(fs::FS &fs, const char *path)
{
    Serial.printf("Creating Dir: %s\n", path);
    if (fs.mkdir(path))
    {
        Serial.println("Dir created");
    }
    else
    {
        Serial.println("mkdir failed");
    }
}
