#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <SPI.h>
#include <ADS1299.h>


// #define TESTMODE


/* SPI pins */
#ifdef ALTERNATE_PINS
#define VSPI_MISO 2
#define VSPI_MOSI 4
#define VSPI_SCLK 0
#define VSPI_SS 33

#define HSPI_MISO 26
#define HSPI_MOSI 27
#define HSPI_SCLK 25
#define HSPI_SS 32
#else
#define VSPI_MISO MISO
#define VSPI_MOSI MOSI
#define VSPI_SCLK SCK
#define VSPI_SS SS

#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_SCLK 14
#define HSPI_SS 15
#endif

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
#define VSPI FSPI
#endif

#define DRDY 4
#define START 18
#define RESET 17
#define PWDN 16

static const uint32_t spiClk =  1000000; // 1 MHz
SPIClass * hspi = NULL;
static const char *my_ssid  = "EEG_AP" ;
static const char *my_password  = "1234567890";
static const uint8_t Buff_size = 5;
WiFiClient client;
WiFiServer server(80);


void setup() {
  // put your setup code here, to run once:

 hspi = new SPIClass(HSPI);
  pinMode(DRDY, INPUT); //DRDY ADS1299
  pinMode(START, OUTPUT); //START ADS1299
  pinMode(RESET, OUTPUT); //RESET ADS1299
  pinMode(PWDN, OUTPUT); //PWDN ADS1299
  digitalWrite(PWDN, LOW);
  digitalWrite(RESET, LOW);
  digitalWrite(START, LOW);
  
  
#ifndef ALTERNATE_PINS
  //initialise vspi with default pins
  //SCLK = 18, MISO = 19, MOSI = 23, SS = 5
  hspi->begin();
#else
  //alternatively route through GPIO pins of your choice
  hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS); //SCLK, MISO, MOSI, SS
#endif
pinMode(hspi->pinSS(), OUTPUT); //HSPI SS


delay(1000); 
Serial.begin(250000);
Serial.println();
Serial.println("Configuring access point...");
/* You can remove the password parameter if you want the AP to be open. */
if (!WiFi.softAP(my_ssid, my_password)) {
    Serial.println("Soft AP creation failed.");
  while(1);
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
  Serial.println("Server started");


}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client = server.accept();   // listen for incoming clients
  ADS1299 ads1(hspi->pinSS(),hspi,DRDY,spiClk);

  //power up timing for ADS1299:
  digitalWrite(PWDN, HIGH);
  delay(1);
  digitalWrite(RESET, HIGH);
  delay(1);
  digitalWrite(START, HIGH);
  ads1.StopReadC();
  ads1.Stop();

  if (client) {                             // if you get a client,
    Serial.println("New Client.");      
    uint8_t buffer [28]{0};
    buffer[0] = 0xaa;
    const uint8_t* buf_ad = &buffer[0];    

  #ifdef TESTMODE

  //check ads1299 is live:
  while (62 != ads1.ReadReg(ID)){}

    //program ADS1299 number 1 :
  ads1.WriteReg(CONFIG1, 0x96);
  ads1.WriteReg(CONFIG2, 0xD5);
  ads1.WriteReg(CONFIG3, 0xe8);

  while (0x96 != ads1.ReadReg(CONFIG1)){} //150
  Serial.println("CONFIG1:");
  Serial.println(ads1.ReadReg(CONFIG1));

  while (0xD5 != ads1.ReadReg(CONFIG2)){} //213
  Serial.println("CONFIG2:");
  Serial.println(ads1.ReadReg(CONFIG2));

  while (0xe8 != ads1.ReadReg(CONFIG3)){} //232
  Serial.println("CONFIG3:");
  Serial.println(ads1.ReadReg(CONFIG3));

  ads1.WriteReg(CH1SET, 0x65);
  ads1.WriteReg(CH2SET, 0x05);
  ads1.WriteReg(CH3SET, 0x05);
  ads1.WriteReg(CH4SET, 0x05);
  ads1.WriteReg(CH5SET, 0x05);
  ads1.WriteReg(CH6SET, 0x05);
  ads1.WriteReg(CH7SET, 0x05);
  ads1.WriteReg(CH8SET, 0x05);


  while (0x65 != ads1.ReadReg(CH1SET)){} //232
  Serial.println("CHSET:");
  Serial.println(ads1.ReadReg(CH1SET));
  
    while (0x05 != ads1.ReadReg(CH2SET)){} //232
  Serial.println("CH2SET:");
  Serial.println(ads1.ReadReg(CH2SET));

    while (0x05 != ads1.ReadReg(CH3SET)){} //232
  Serial.println("CH3SET:");
  Serial.println(ads1.ReadReg(CH3SET));

    while (0x05 != ads1.ReadReg(CH4SET)){} //232
  Serial.println("CH4SET:");
  Serial.println(ads1.ReadReg(CH4SET));

    while (0x05 != ads1.ReadReg(CH5SET)){} //232
  Serial.println("CH5SET:");
  Serial.println(ads1.ReadReg(CH5SET));
  
    while (0x05 != ads1.ReadReg(CH6SET)){} //232
  Serial.println("CH6SET:");
  Serial.println(ads1.ReadReg(CH6SET));

    while (0x05 != ads1.ReadReg(CH7SET)){} //232
  Serial.println("CH7SET:");
  Serial.println(ads1.ReadReg(CH7SET));

    while (0x05 != ads1.ReadReg(CH8SET)){} //232
  Serial.println("CH8SET:");
  Serial.println(ads1.ReadReg(CH8SET));

  #else


    while (62 != ads1.ReadReg(ID)){}

    //program ADS1299 number 1 :
  ads1.WriteReg(CONFIG1, 0x96);
  ads1.WriteReg(CONFIG2, 0xD5);
  ads1.WriteReg(CONFIG3, 0xe8);

  while (0x96 != ads1.ReadReg(CONFIG1)){} //150
  Serial.println("CONFIG1:");
  Serial.println(ads1.ReadReg(CONFIG1));

  while (0xD5 != ads1.ReadReg(CONFIG2)){} //213
  Serial.println("CONFIG2:");
  Serial.println(ads1.ReadReg(CONFIG2));

  while (0xe8 != ads1.ReadReg(CONFIG3)){} //232
  Serial.println("CONFIG3:");
  Serial.println(ads1.ReadReg(CONFIG3));

  ads1.WriteReg(CH1SET, 0x81);
  ads1.WriteReg(CH2SET, 0x60);
  ads1.WriteReg(CH3SET, 0x81);
  ads1.WriteReg(CH4SET, 0x81);
  ads1.WriteReg(CH5SET, 0x81);
  ads1.WriteReg(CH6SET, 0x81);
  ads1.WriteReg(CH7SET, 0x81);
  ads1.WriteReg(CH8SET, 0x81);


  while (0x81 != ads1.ReadReg(CH1SET)){} //232
  Serial.println("CHSET:");
  Serial.println(ads1.ReadReg(CH1SET));
  
    while (0x60 != ads1.ReadReg(CH2SET)){} //232
  Serial.println("CH2SET:");
  Serial.println(ads1.ReadReg(CH2SET));

    while (0x81 != ads1.ReadReg(CH3SET)){} //232
  Serial.println("CH3SET:");
  Serial.println(ads1.ReadReg(CH3SET));

    while (0x81 != ads1.ReadReg(CH4SET)){} //232
  Serial.println("CH4SET:");
  Serial.println(ads1.ReadReg(CH4SET));

    while (0x81 != ads1.ReadReg(CH5SET)){} //232
  Serial.println("CH5SET:");
  Serial.println(ads1.ReadReg(CH5SET));
  
    while (0x81 != ads1.ReadReg(CH6SET)){} //232
  Serial.println("CH6SET:");
  Serial.println(ads1.ReadReg(CH6SET));

    while (0x81 != ads1.ReadReg(CH7SET)){} //232
  Serial.println("CH7SET:");
  Serial.println(ads1.ReadReg(CH7SET));

    while (0x81 != ads1.ReadReg(CH8SET)){} //232
  Serial.println("CH8SET:");
  Serial.println(ads1.ReadReg(CH8SET));

  // ads1.WriteReg(BIAS_SENSN, 0x02);
  // ads1.WriteReg(BIAS_SENSP, 0x02);





  #endif

    ads1.Start();
    ads1.ReadDataC();

    delay(100);

    while (client.connected()) {
 
      while(!digitalRead(DRDY)){
          ads1.ReadData(&buffer[1],27);
          client.write(buf_ad,28);
      }
    }
  }
}
