#ifndef UNITTESTS_H_
#define UNITTESTS_H_

#include <Arduino.h>
#include <ArduinoUnit.h>
#include "DataLog.h"

#if defined(CONFIG_PLATFORM_8721D) || defined(BOARD_RTL8722D) || defined(BOARD_RTL8722DM_MINI) || defined(BOARD_RTL8720DN_BW16)
  #include <sys_api.h>
#endif

struct TestLogEntry{
  int8_t rssi;
  uint32_t timestamp;
  uint8_t wakeupreason;
  uint8_t status;
  uint16_t capacitive1calibrate;
  uint16_t capacitive2calibrate;
  int16_t capacitive1measure;
  int16_t capacitive2measure;
  int16_t batteryvoltage;
};

#endif //UNIT_TESTS_H_