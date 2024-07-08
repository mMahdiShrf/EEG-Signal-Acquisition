#include "ADS1299.h"

ADS1299::ADS1299(int sspin, SPIClass* spi, int drdy, int spiclk)
{
    _CSPIN = sspin;
    _SPI = spi;
    _DRDY = drdy;
    _SPICLK = spiclk;
    _SPI->beginTransaction(SPISettings(_SPICLK, MSBFIRST, SPI_MODE1));
}

ADS1299::~ADS1299()
{
    _SPI->endTransaction();
}

uint8_t ADS1299::OneByteCommand(uint8_t CMD_VAL)
{
    uint8_t OUT;
    // _SPI->beginTransaction(SPISettings(_SPICLK, MSBFIRST, SPI_MODE1));
    digitalWrite(_CSPIN, LOW);
    OUT = _SPI->transfer(CMD_VAL);
    digitalWrite(_CSPIN, HIGH);
    // _SPI->endTransaction();
    return OUT;
}

uint8_t ADS1299::ReadReg(uint8_t reg)
{
    const uint8_t COMMAND[3] { reg + 0x20, 0x00, 0x00 };
    const uint8_t* Address_Command = (uint8_t*)(&COMMAND);
    uint8_t Recieve[3];
    // _SPI->beginTransaction(SPISettings(_SPICLK, MSBFIRST, SPI_MODE1));
    digitalWrite(_CSPIN, LOW);
    _SPI->transferBytes(Address_Command, (uint8_t*)&Recieve[0], 3);
    delay(1);
    digitalWrite(_CSPIN, HIGH);
    // _SPI->endTransaction();
    return Recieve[2];
}

void ADS1299::WriteReg(uint8_t reg, uint8_t newReg)
{
    const uint8_t COMMAND[3] { reg + 0x40, 0x00, newReg };
    const uint8_t* Address_Command = (uint8_t*)(&COMMAND);
    // _SPI->beginTransaction(SPISettings(_SPICLK, MSBFIRST, SPI_MODE1));
    digitalWrite(_CSPIN, LOW);
    _SPI->transferBytes(Address_Command, NULL, 3);
    digitalWrite(_CSPIN, HIGH);
    // _SPI->endTransaction();
}

void ADS1299::ReadData(uint8_t* buffer, uint32_t size)
{

    digitalWrite(_CSPIN, LOW);
    _SPI->transferBytes(NULL, buffer, size);
    digitalWrite(_CSPIN, HIGH);
}

void ADS1299::WakeUp()
{
    OneByteCommand(WAKEUP);
}
void ADS1299::Reset()
{
    OneByteCommand(RESET);
}
void ADS1299::Start()
{
    OneByteCommand(START);
}
void ADS1299::Stop()
{
    OneByteCommand(STOP);
}
void ADS1299::ReadDataC()
{
    OneByteCommand(RDATAC);
}
void ADS1299::StopReadC()
{
    OneByteCommand(SDATAC);
}
void ADS1299::ReadData()
{
    OneByteCommand(RDATA);
}
