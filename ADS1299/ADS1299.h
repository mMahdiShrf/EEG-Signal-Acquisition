/*
library to program and communicate with ADS1299 IC by ESP32 by M Mahdi Sharifian
*/

#ifndef ADS1299_h

#define ADS1299_h

#include <Arduino.h>
#include <SPI.h>

class ADS1299 {
private:
    /* data */
    int _CSPIN;
    SPIClass* _SPI;
    int _DRDY;
    uint32_t _SPICLK;

public:
    ADS1299(int sspin, SPIClass* spi, int drdy, int spiclk);
    ~ADS1299();
    uint8_t OneByteCommand(uint8_t CMD_VAL);
    uint8_t ReadReg(uint8_t reg);
    void WriteReg(uint8_t reg, uint8_t newReg);
    void ReadData(uint8_t* buffer, uint32_t size);
    void WakeUp();
    void Reset();
    void Start();
    void Stop();
    void ReadDataC();
    void StopReadC();
    void ReadData();
};

const uint8_t WAKEUP = 0b00000010; // Wake-up from standby mode
const uint8_t STANDBY = 0b00000100; // Enter Standby mode
const uint8_t RESET = 0b00000110; // Reset the device
const uint8_t START = 0b00001000; // Start and restart (synchronize) conversions
const uint8_t STOP = 0b00001010; // Stop conversion
const uint8_t RDATAC = 0b00010000; // Enable Read Data Continuous mode (default mode at power-up)
const uint8_t SDATAC = 0b00010001; // Stop Read Data Continuous mode
const uint8_t RDATA = 0b00010010; // Read data by command; supports multiple read back

#define ID 0x00
const uint8_t CONFIG1 = 0x01;
#define CONFIG2 0x02
#define CONFIG3 0x03
#define LOFF 0x04
#define CH1SET 0x05
#define CH2SET 0x06
#define CH3SET 0x07
#define CH4SET 0x08
#define CH5SET 0x09
#define CH6SET 0x0A
#define CH7SET 0x0B
#define CH8SET 0x0C
#define BIAS_SENSP 0x0D
#define BIAS_SENSN 0x0E
#define LOFF_SENSP 0x0F
#define LOFF_SENSN 0x10
#define LOFF_FLIP 0x11
#define LOFF_STATP 0x12
#define LOFF_STATN 0x13
#define GPIO 0x14
#define MISC1 0x15
#define MISC2 0x16
#define CONFIG4 0x17

#endif