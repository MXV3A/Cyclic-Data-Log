#include "UnitTests.h"



test(wipeLog){
  //Create test ground
  DataLog<TestLogEntry> datalog;

  //Test Function
  int wiperesult = datalog.wipeLog();
  assertEqual(wiperesult, 0);

  TestLogEntry readentry;
  int readresult = datalog.readEntry(0, &readentry);
  assertEqual(readresult, -1);
}

test(wipeWriteReadLog) {
  //Create test ground
  DataLog<TestLogEntry> datalog;
  //index, timestamp, wakeup, status, calibrate1, calibrate2, capacity1, capacity2, rssi
  TestLogEntry writeentry = {-110, 0, 0, 0x0c, 100, 200, 10, 15, 2600};
  datalog.wipeLog();
  int result = datalog.addEntry(&writeentry);
  assertEqual(result, 0);

  //Test function
  TestLogEntry readentry;
  result = datalog.readEntry(0, &readentry);
  assertEqual(result, 1);

  //assert that read Log matches expected Log
  assertEqual(writeentry.status, readentry.status);
  assertEqual(writeentry.capacitive1calibrate, readentry.capacitive1calibrate);
  assertEqual(writeentry.capacitive2calibrate, readentry.capacitive2calibrate);
  assertEqual(writeentry.capacitive1measure, readentry.capacitive1measure);
  assertEqual(writeentry.capacitive2measure, readentry.capacitive2measure);
  assertEqual(writeentry.batteryvoltage, readentry.batteryvoltage);
  assertEqual(writeentry.rssi, readentry.rssi);
}

test(WriteCount12Log) {
  //Create test ground
  DataLog<TestLogEntry> datalog;
  //timestamp, wakeup, status, calibrate1, calibrate2, capacity1, capacity2, rssi
  TestLogEntry writeentry = {55, 0, 0, 0x02, 11, 22, 33, 44, 3100};
  datalog.wipeLog();

  //Test function
  for(int i=0; i<12; i++){
    int result = datalog.addEntry(&writeentry);
    assertEqual(result, 0);
  }
  int count = datalog.numOfEntries();
  assertEqual(count, 12);

  count = datalog.getHighestIndex();
  assertEqual(count, 12);
}

test(WriteRead10Log) {
  //Create test ground
  DataLog<TestLogEntry> datalog;
  //timestamp, wakeup, status, calibrate1, calibrate2, capacity1, capacity2, rssi
  TestLogEntry writeentry = {55, 0, 0, 0x02, 11, 22, 33, 44, 3100};
  datalog.wipeLog();

  //Test function
  for(int i=0; i<10; i++){
    int result = datalog.addEntry(&writeentry);
    assertEqual(result, 0);
  }

  //assert that actualConfig matches expectedConfig
  TestLogEntry readentry;
  for(int i=0; i<10; i++){
    assertEqual(datalog.readEntry(i, &readentry), i+1);
    assertEqual(writeentry.status, readentry.status);
    assertEqual(writeentry.capacitive1calibrate, readentry.capacitive1calibrate);
    assertEqual(writeentry.capacitive2calibrate, readentry.capacitive2calibrate);
    assertEqual(writeentry.capacitive1measure, readentry.capacitive1measure);
    assertEqual(writeentry.capacitive2measure, readentry.capacitive2measure);
    assertEqual(writeentry.batteryvoltage, readentry.batteryvoltage);
    assertEqual(writeentry.rssi, readentry.rssi);
  }
}

test(WriteReadLast5Log) {
  //Create test ground
  DataLog<TestLogEntry> datalog;
  //timestamp, wakeup, status, calibrate1, calibrate2, capacity1, capacity2, rssi
  TestLogEntry writeentry = {1, 0, 0, 0x02, 11, 22, 33, 44, 3100};
  datalog.wipeLog();

  //Test function
  for(int i=0; i<10; i++){
    writeentry.rssi = 50+i;
    int result = datalog.addEntry(&writeentry);
    assertEqual(result, 0);
  }

  //assert that actualConfig matches expectedConfig
  TestLogEntry readentry;
  for(int i=-1; i>-6; i--){
    assertNotEqual(datalog.readEntry(i, &readentry), -1);
    assertEqual(readentry.rssi, 60+i);
  }
}

test(OverflowLog) {
  //Create test ground
  DataLog<TestLogEntry> datalog;
  //timestamp, wakeup, status, calibrate1, calibrate2, capacity1, capacity2, rssi
  TestLogEntry writeentry = {24, 1, 5, 0x02, 11, 22, 33, 55, 2321};
  datalog.wipeLog();

  //Trigger Overflow
  for(int i=0; i<100; i++){
    int result = datalog.addEntry(&writeentry);
    assertEqual(result, 0);
  }

  //all 100 entries were written
  int highestindex = datalog.getHighestIndex();
  assertEqual(highestindex, 100);

  //log is full
  int numofentries = datalog.numOfEntries();
  assertEqual(numofentries, datalog.maxNumOfEntries());
  assertMore(numofentries, 0);
}

//THIS HAS TO BE THE FIRST TEST
test(AAAkeepLog_afterreset){
  //Create config with seed
  DataLog<TestLogEntry> datalog;
  TestLogEntry writeentry = {2600, 0, 1, 0x04, 99, 88, 77, 66, 55};

  //1. assert that actual Log matches expected Log
  TestLogEntry readentry;
  bool correct = true;
  datalog.readEntry(0, &readentry);
  if(readentry.wakeupreason != writeentry.wakeupreason || 
        readentry.status != writeentry.status || 
        readentry.capacitive1calibrate != writeentry.capacitive1calibrate ||
        readentry.capacitive2calibrate != writeentry.capacitive2calibrate ||
        readentry.capacitive1measure != writeentry.capacitive1measure ||
        readentry.capacitive2measure != writeentry.capacitive2measure ||
        readentry.batteryvoltage != writeentry.batteryvoltage ||
        readentry.rssi != writeentry.rssi) correct = false;

  //2. if not yet in save log & restart
  if(!correct){
    datalog.wipeLog();
    datalog.addEntry(&writeentry);
    ESP.restart();
  }else pass();
}

