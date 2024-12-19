#include "DataLog.h"

template <typename LogEntry> 
int DataLog<LogEntry>::wipeLog(){
  IEEPROM.begin(EEPROM_MAX_SIZE);
  for(int i=LOG_START_ADDRESS; i<LOG_END_ADDRESS; i++){
    IEEPROM.write(i, 0xFF);
  }
  IEEPROM.commit();
  IEEPROM.end();
  return 0;
}

template <typename LogEntry> 
int DataLog<LogEntry>::addEntry(LogEntry *entry){
  int address;
  uint16_t index;
  getAddressToOverwrite(address, index);
  return writeEntryToEEPROM(address, index, entry);
}

template <typename LogEntry> 
int DataLog<LogEntry>::readEntry(int number, LogEntry* entry){
  if(abs(number) > maxNumOfEntries()) return -1;
  int address;
  uint16_t index;
  if(number < 0){
    getAddressToOverwrite(address, index);
    for(int i=number; i<0; i++){
      address -= sizeof(LogEntry)+sizeof(uint16_t);
      if(address < LOG_START_ADDRESS) address = LOG_START_ADDRESS + ((maxNumOfEntries() -1) * sizeof(LogEntry)+sizeof(uint16_t));
    }
  }else{
    address = getOldestEntryAddress();
    for(int i=0; i<number; i++){
      address += sizeof(LogEntry)+sizeof(uint16_t);
      if(address > (int)(LOG_END_ADDRESS - sizeof(LogEntry)+sizeof(uint16_t))) address = LOG_START_ADDRESS;
    }
  }
  return readEntryFromEEPROM(address, entry);
}

template <typename LogEntry>
int DataLog<LogEntry>::numOfEntries(){
  int address;
  uint16_t index;
  getAddressToOverwrite(address, index);
  LogEntry checkentry;
  if(readEntryFromEEPROM(address, &checkentry) < 0) return (address - LOG_START_ADDRESS) / (sizeof(LogEntry)+sizeof(uint16_t));
  return maxNumOfEntries();
}

template <typename LogEntry> 
int DataLog<LogEntry>::maxNumOfEntries(){
  return floor((LOG_END_ADDRESS - LOG_START_ADDRESS) / (sizeof(LogEntry)+sizeof(uint16_t)));
}

template <typename LogEntry> 
int DataLog<LogEntry>::getHighestIndex(){
  int address;
  uint16_t index;
  getAddressToOverwrite(address, index);
  return index-1;
}

template <typename LogEntry> 
int DataLog<LogEntry>::writeEntryToEEPROM(int address, uint16_t index, LogEntry* entry){
  if(address > (int)(LOG_END_ADDRESS-(sizeof(LogEntry)+sizeof(uint16_t)))) return -1;
  IEEPROM.begin(EEPROM_MAX_SIZE);
  IEEPROM.put(address, index);
  IEEPROM.put(address+sizeof(index), *entry);
  IEEPROM.commit();

  uint16_t checkindex = 0;
  IEEPROM.get(address, checkindex);
  IEEPROM.end();
  if(checkindex != index) return -1;
  return 0;
}

template <typename LogEntry> 
int DataLog<LogEntry>::readEntryFromEEPROM(int address, LogEntry* entry){
  if(address > (int)(LOG_END_ADDRESS-(sizeof(LogEntry)+sizeof(uint16_t)))) return -1;
  IEEPROM.begin(EEPROM_MAX_SIZE);
  uint16_t index;
  IEEPROM.get(address, index);
  IEEPROM.get(address+sizeof(index), *entry);
  IEEPROM.end();
  if(index == 0xFFFF || index == 0x0 ) return -1;
  return index;
}

template <typename LogEntry>
void DataLog<LogEntry>::getAddressToOverwrite(int &address, uint16_t &index){
  address = LOG_START_ADDRESS;
  IEEPROM.begin(EEPROM_MAX_SIZE);
  IEEPROM.get(address, index);
  uint16_t lastindex = index;
  //find unwritten memory or oldest index
  while((index == lastindex+1 || index == lastindex) && index != 0xFFFF && index != 0x0){
    lastindex = index;
    address += (sizeof(LogEntry)+sizeof(index));
    if(address > (int)(LOG_END_ADDRESS - (sizeof(LogEntry)+sizeof(index)))) address = LOG_START_ADDRESS;
    IEEPROM.get(address, index);
  }
  IEEPROM.end();
  if(lastindex == 0x0 || lastindex == 0xFFFF) index = 1;
  else index = lastindex + 1;
}

template <typename LogEntry>
int DataLog<LogEntry>::getOldestEntryAddress(){
  int address;
  uint16_t index;
  getAddressToOverwrite(address, index);
  LogEntry checkentry;
  if(readEntryFromEEPROM(address, &checkentry) < 0){
      if(index == 1) return -1;
      else return LOG_START_ADDRESS;
  }
  return address;
}

template <typename LogEntry> 
void DataLog<LogEntry>::binaryToString(uint8_t status, String& binstatus){
  binstatus = "";
  for(int i=7; i>=0; i--){
    if((status>>i) & 1 ) binstatus += "1";
    else binstatus += "0";
  }
}

template <typename LogEntry> 
void DataLog<LogEntry>::wakeupReasonToString(uint8_t code, String& reason){
  switch(code){
#if defined(ESP32) || defined(ESP8266)
    case ESP_SLEEP_WAKEUP_UNDEFINED:
      reason = "Reset";
      break;
    case ESP_SLEEP_WAKEUP_EXT0:
      reason = "GPIO Single Signal";
      break;
    case ESP_SLEEP_WAKEUP_EXT1:
      reason = "GPIO Multi Signal";
      break;
    case ESP_SLEEP_WAKEUP_TIMER:
      reason = "Timer";
      break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      reason = "Touch";
      break;
#endif
    default:
      reason = "Unknown";
  }
}

template <typename LogEntry> 
void DataLog<LogEntry>::timeToString(uint64_t timeInS, String& time){
  int seconds = timeInS % 60;
  int minutes = (timeInS / 60) % 60;
  int hours = (timeInS / 3600) % 24;
  int days = (timeInS / 86400);
  time = String(days);
  time += "d:";
  time += String(hours);
  time += "h:";
  time += String(minutes);
  time += "m:";
  time += String(seconds);
  time += "s";
}