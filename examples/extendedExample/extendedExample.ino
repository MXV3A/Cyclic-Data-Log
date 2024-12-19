/* Extended Example for the DataLog Library
 * 
 * Behaviour:
 * The ESP32 receives commands over Serial
 * "printlog" prints all log entries
 * "wipelog" erases all log entries
 * After 30 seconds a new entry is added to the log
 * The ESP32 goes to sleep for 30 seconds or until touched
 */

#include <DataLog.h>
#include "driver/temp_sensor.h"

int tempRead(){
  temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT(); //-10 -> 80
  temp_sensor_set_config(temp_sensor);
  temp_sensor_start();
  float temperature;
  temp_sensor_read_celsius(&temperature);
  return (int)temperature;
}

class LogEntry{
public:
  void createNew(){
    this->timestamp = (uint32_t)time(NULL);
    this->wakeupreason = esp_sleep_get_wakeup_cause();
    this->touchvalue = touchRead(GPIO_NUM_0);
    this->temperature = tempRead();
  }

  String headerToString(){return "Timestamp, Wakeup, Touch, Temperature\r\n";}

  String toString(){return (String)timestamp+", "+wakeupreason+", "+touchvalue+", "+temperature+"\r\n";}

  uint32_t timestamp;
  uint8_t wakeupreason;
  uint16_t touchvalue;
  uint16_t temperature;
};

void setup() {
  Serial.begin(115200);
  pinMode(GPIO_NUM_0, INPUT);
  DataLog<LogEntry> log;

  //Receive Serial commands for 30 seconds
  Serial.println("Type a command: wipelog, printlog, exit");
  String command = "";
  uint32_t starttime = millis();
  while(millis() < starttime + 30000){
    if(Serial.available()){
      command += (char)Serial.read();
      if(command.endsWith("\r")){
        //Command 1: Erase Log
        if(command.indexOf("wipelog") != -1){
          Serial.println("Erasing Log");
          log.wipeLog();
        }
        //Command 2: Printing all Entries
        else if(command.indexOf("printlog") != -1){
          LogEntry entry;
          Serial.print(entry.headerToString());
          for(int i=0; i<log.numOfEntries(); i++){
            log.readEntry(i, &entry);
            Serial.print(entry.toString());
          }
        }
        else if(command.indexOf("exit") != -1) break;
        command = "";
      }
    }
    delay(5);
  }

  //Add a new Entry to the Log
  Serial.println("Adding New Entry");
  LogEntry entry;
  entry.createNew();
  log.addEntry(&entry);

  //Sleep for 30 seconds
  Serial.println("Sleeping");
  esp_sleep_enable_timer_wakeup(((uint64_t)30)*1000000);
  esp_sleep_enable_touchpad_wakeup();
  esp_deep_sleep_start();
}

void loop() {}
