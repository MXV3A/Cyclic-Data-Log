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
  IEEPROMClass IEEPROM;
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
      memcpy(&t, &FlashMemory.buf[address], sizeof(T)); return t;
    }

    template<typename T>
    const T &put(int address, const T &t){
      memcpy(&FlashMemory.buf[address], &t, sizeof(T)); return t;
    }

    void write(int address, uint8_t val){FlashMemory.buf[address] = val;}

  };
  IEEPROMClass IEEPROM;
#endif

#if defined(STM32_CORE_VERSION)
  #include <EEPROM.h>
  //STM32 emulated eeprom overwrites the whole flash for every single command, so it has to be buffered
  //it's also super slow
  
  class IEEPROMClass{
    public:
    bool begin(size_t size){
		if(!loaded){EEPROM.get(0, this->buffer); loaded = true;} return size == 4096 ? true : false;
	}
		
    bool commit(){return EEPROM.put(0, this->buffer);}
    void end(){return;}

    template<typename T>
    T &get(int address, T &t){
      memcpy(&t, &this->buffer[address], sizeof(T)); return t;
    }

    template<typename T>
    const T &put(int address, const T &t){
      memcpy(&this->buffer[address], &t, sizeof(T)); return t;
    }

    void write(int address, uint8_t val){this->buffer[address] = val;}
	
	bool loaded = false;
	uint8_t buffer[4096];

  };
  IEEPROMClass IEEPROM;
#endif

#endif // FLASHVARIANT_H_
