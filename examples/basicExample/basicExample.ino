#include <DataLog.h>

struct Entry{
    int timestamp;
    int sensor1;
    int sensor2;
}

void setup(){
    Serial.begin(115200);
    pinMode(GPIO_NUM_4, INPUT);

    DataLog<Entry> log;

    Entry readentry;
    if(log.readEntry(-1, &readentry) != -1){
        Serial.println("Logged Sensor Value: "+String(readentry.sensor1));
    }

    Entry newentry = {time(NULL), analogRead(GPIO_NUM_4), 25};
    log.addEntry(&newentry);

    delay(10000);
    ESP.restart();
}

void loop{}
