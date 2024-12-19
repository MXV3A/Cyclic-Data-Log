#ifndef FLASHVARIANT_H_
#define FLASHVARIANT_H_

#include <Arduino.h>

#if defined(ESP32) || defined(ESP8266)
  #include <EEPROM.h>

  class IEEPROMClass{
    public:
    bool begin(size_t size){return EEPROM.begin(size);}
    bool commit(){return EEPROM.commit();}
    void end(){EEPROM.end();}

    template<typename T>
    T &get(int address, T &t){return EEPROM.get(address, t);}

    template<typename T>
    const T &put(int address, const T &t){return EEPROM.put(address, t);}

    void write(int address, uint8_t val){EEPROM.write(address, val);}

  };
  extern IEEPROMClass IEEPROM;
#endif

#if defined(CONFIG_PLATFORM_8721D) || defined(BOARD_RTL8722D) || defined(BOARD_RTL8722DM_MINI) || defined(BOARD_RTL8720DN_BW16)
  #include <FlashMemory.h>

  class IEEPROMClass{
    public:
    bool begin(size_t size){FlashMemory.read(); return size == 4096 ? true : false;}
    bool commit(){FlashMemory.update(); return true;}
    void end(){return;}

    template<typename T>
    T &get(int address, T &t){
      //uint8_t* bytes = reinterpret_cast<const uint8_t*>(&t);
      //for(int i=0; i<sizeof(T); i++) FlashMemory.buf[]
      memcpy(&t, &FlashMemory.buf[address], sizeof(T)); return t;
    }

    template<typename T>
    const T &put(int address, const T &t){
      memcpy(&FlashMemory.buf[address], &t, sizeof(T)); return t;
    }

    void write(int address, uint8_t val){FlashMemory.buf[address] = val;}

  };
  extern IEEPROMClass IEEPROM;
#endif

#endif // FLASHVARIANT_H_
