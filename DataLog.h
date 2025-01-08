#ifndef DEVICECONFIG_H_
#define DEVICECONFIG_H_

#include <Arduino.h>
#include "FlashVariant.hpp"

#define EEPROM_MAX_SIZE 4096
#define LOG_START_ADDRESS 0
#define LOG_END_ADDRESS EEPROM_MAX_SIZE

template <typename LogEntry> 
class DataLog{
  public:
    //Erase all Log Entries from LOG_START_ADDRESS to LOG_END_ADDRESS
    int wipeLog();

    //Add an Entry to the Log. If Memory is full, oldest Entry will be overwritten
    int addEntry(LogEntry* entry);

    //Read Entry from Log. 0 for oldest, -1 for newest.
    int readEntry(int number, LogEntry* entry);

    //Get current number of Entries in the Log
    int numOfEntries();

    //Get maximum number of Entries the Log can hold at a time
    int maxNumOfEntries();

    //Get the highest index, which shows how many Entries have been written since the last wipeLog()
    //Is limited to uint16_t, so 65.535
    int getHighestIndex();

  private:
    int writeEntryToEEPROM(int address, uint16_t index, LogEntry* entry);
    int readEntryFromEEPROM(int address, LogEntry* entry);
    int getOldestEntryAddress();
    void getAddressToOverwrite(int &address, uint16_t &index);

    //stringify for various logdata
    void wakeupReasonToString(uint8_t code, String& reason);
    void binaryToString(uint8_t status, String& binstatus);
    void timeToString(uint64_t timeInS, String& time);
    
};

#include "DataLog.hpp"

#endif // DEVICECONFIG_H_
