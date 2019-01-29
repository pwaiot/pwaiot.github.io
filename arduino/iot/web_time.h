//If you use double quotes for your #include rather than angle brackets it will look locally
#include <config.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

//prototypes
void webtime_setup();
void webtime_handle();
void webtime_forceUpdate();

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
WiFiUDP ntpUDP;
 
//int16_t utc = myUTC; //UTC -3:00 Brazil
uint32_t currentMillis = 0;
uint32_t previousMillis = 0;
 
NTPClient timeClient(ntpUDP, "a.st1.ntp.br", myUTC*3600, 60000);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void webtime_setup() {
  timeClient.begin();
  timeClient.update();

  setTime(timeClient.getEpochTime()); //set system time
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void webtime_forceUpdate() {
    timeClient.forceUpdate();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void checkOST(void) {
    currentMillis = millis(); //current time in ms
    //logic of time verification
    if (currentMillis - previousMillis > 1000) {
        previousMillis = currentMillis;    //current time
        USESERIAL.printf("Time Epoch: %d: ", timeClient.getEpochTime());
        USESERIAL.println(timeClient.getFormattedTime());
        
        setTime(timeClient.getEpochTime()); //set system time
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void webtime_handle() {
    //call time verification
    checkOST();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
