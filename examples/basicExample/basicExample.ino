#include <DataLog.h>
#if defined(BOARD_RTL8720DN_BW16)
  #include <sys_api.h>
#endif

#define SAMPLE_PIN 9

struct Entry{
    int timestamp;
    int sensor1;
    int counter;
};

void setup(){
    Serial.begin(115200);
    Serial.println("Log Start");
    pinMode(SAMPLE_PIN, INPUT);

    DataLog<Entry> log;

    //Read last Entry
    Entry entry;
    if(log.readEntry(-1, &entry) != -1){
        Serial.println((String)"Previous Entry:" 
                      +"Time "+entry.timestamp
                      +" Sensor1 "+entry.sensor1
                      +" Counter "+entry.counter);
    }

    //Write new Entry
    Entry newentry = {(int)(millis()), digitalRead(SAMPLE_PIN), entry.counter+1};
    log.addEntry(&newentry);

    delay(6000);

    //Restart on different Platforms
    #if defined(ESP32) || defined(ESP8266)
      ESP.restart();
    #elif defined(BOARD_RTL8720DN_BW16)
      sys_reset();
    #elif defined(STM32_CORE_VERSION)
      NVIC_SystemReset();
    #endif
}

void loop(){}
