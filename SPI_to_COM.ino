
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include "dataRo.h"

#define LED  13
#define CS_SD 10
struct Ro_header msg_header;
struct Ro_message_status msg_stat;
struct Ro_message_navigation msg_nav;
struct Ro_message_battery msg_batt;
struct Ro_message_gps   msg_gps;
struct Ro_message_drogue msg_drogue;
struct Ro_message_level msg_level;

const uint64_t pipes[2] = {0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
// Hardware configuration
RF24 radio(5,4);
/**
     * Set Power Amplifier (PA) level to one of four levels:
     * RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH and RF24_PA_MAX
     *
     * The power levels correspond to the following output levels respectively:
     * NRF24L01: -18dBm, -12dBm,-6dBM, and 0dBm
*/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  printf_begin();
  pinMode(LED,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(CS_SD, OUTPUT);
  digitalWrite(CS_SD,HIGH);
  if(!radio.begin())
    Serial.println("Fail RADIO");
  //radio.setPALevel(RF24_PA_MAX);
  radio.setPALevel(RF24_PA_LOW);
  //radio.enableAckPayload();
  //radio.enableDynamicPayloads();
  radio.setCRCLength(RF24_CRC_8);
  radio.setRetries(1,5);
  radio.setAutoAck(true);
  //radio.maskIRQ(0,0,0);  
  //radio.setChannel(9);
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(9);
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
  //Serial.println(micros());
  radio.startListening();
  
  Serial.println(micros()); 
  radio.printDetails();
  delay(1000);
  

/*
msg_header.rocket_id = 34;
msg_header.ground_id = 255;
msg_header.msg_type = 2;

msg_nav.mode = 0xFE02;
msg_nav.speed1 =   40032;
msg_nav.speed2 =   9654;
msg_nav.altitude1 = 1335;
msg_nav.altitude2 = 9453;
*/
}



void loop() {
  // put your main code here, to run repeatedly:
  /*
  Serial.write((const uint8_t *)&msg_header, sizeof(msg_header));
  delay(1);
  Serial.write((const uint8_t *)&msg_nav, sizeof(msg_nav));
  Serial.flush();
  delay(100);*/
 
  while (radio.available()) {   // While there is data ready
        radio.read( &msg_header, sizeof(msg_header));
        digitalWrite(13,HIGH);
        //Serial.println(msg_header.rocket_id);
        //Serial.println(msg_header.ground_id);
        //Serial.println(msg_header.msg_type);
        //Serial.println();
        //radio.flush_rx();
        delayMicroseconds(500);
        digitalWrite(13,LOW);
        if (msg_header.ground_id == 255 & msg_header.rocket_id == 128){
          switch (msg_header.msg_type){
          case 1:
            radio.read( &msg_stat, sizeof(msg_stat));
            if(msg_stat.mode==0xFE01){
              Serial.write((const uint8_t *)&msg_header, sizeof(msg_header));
              delayMicroseconds(500);
              Serial.write((const uint8_t *)&msg_stat, sizeof(msg_stat));
              //Serial.println("STAT");
              radio.flush_rx();
            }
            break;
            
          case 2:
            
            radio.read( &msg_nav, sizeof(msg_nav));
            if(msg_nav.mode==0xFE02){
              Serial.write((const uint8_t *)&msg_header, sizeof(msg_header));
              //Serial.flush();
              delayMicroseconds(500);
              Serial.write((const uint8_t *)&msg_nav, sizeof(msg_nav));
              //Serial.println("NAV");
              radio.flush_rx();
            }
            break;
          case 3:
            radio.read( &msg_batt, sizeof(msg_batt));
            if(msg_batt.mode==0xFE03){
              Serial.write((const uint8_t *)&msg_header, sizeof(msg_header));
              delayMicroseconds(500);
              Serial.write((const uint8_t *)&msg_batt, sizeof(msg_batt));
              radio.flush_rx();
            }
            break;
          case 4:
            radio.read( &msg_gps, sizeof(msg_gps));
            if(msg_gps.mode==0xFE04){
              Serial.write((const uint8_t *)&msg_header, sizeof(msg_header));
              delayMicroseconds(500);
              Serial.write((const uint8_t *)&msg_gps, sizeof(msg_gps));
              //Serial.println("GPS");
              radio.flush_rx();
              //Serial.println(msg_gps.latitude);
             //Serial.println(msg_gps.longitude);
            }
            break;
          case 5:
            radio.read( &msg_drogue, sizeof(msg_drogue));
            if(msg_drogue.mode==0xFE05){
              Serial.write((const uint8_t *)&msg_header, sizeof(msg_header));
              delayMicroseconds(500);
              Serial.write((const uint8_t *)&msg_drogue, sizeof(msg_drogue));//Serial.println("GPS");
              //Serial.println("DROGUE");
              radio.flush_rx();
            }
            break;
          case 255:
            Serial.write((const uint8_t *)&msg_header, sizeof(msg_header));
          default:
            break;
            
        }
        }
        
      }
     
}


void serialEvent(void) {

  radio.stopListening();
  radio.flush_tx();
  radio.setAutoAck(false);
  while(radio.getChannel() != 79){
    radio.setChannel(79);
    delay(1);
  }
  //delay(10);
  while (Serial.available()) {
    // get the new byte:
    Serial.readBytes(( uint8_t *)&msg_header, sizeof(msg_header));
    //Serial.println(msg_header.rocket_id);
    //Serial.println(msg_header.ground_id);
    //Serial.println(msg_header.msg_type);
    delayMicroseconds(600);
    switch (msg_header.msg_type){
          case 1:
            break;
          case 5:
            Serial.readBytes( ( uint8_t *)&msg_drogue, sizeof(msg_drogue));
            if(msg_drogue.mode==0xFE05){
              radio.writeFast(&msg_header, sizeof(msg_header));
              delayMicroseconds(600);
              radio.writeFast(&msg_drogue, sizeof(msg_drogue));
              digitalWrite(LED,HIGH);
              delay(1000);
              digitalWrite(LED,LOW);
            }
            break;
          case 6:
            Serial.readBytes( ( uint8_t *)&msg_level, sizeof(msg_level));
            if(msg_level.mode==0xFE06){
              radio.writeFast(&msg_header, sizeof(msg_header));
              delayMicroseconds(600);
              radio.writeFast(&msg_level, sizeof(msg_level));
              digitalWrite(LED,HIGH);
              delay(1000);
              digitalWrite(LED,LOW);
            }
            break;
          case 255:
            Serial.readBytes(( uint8_t *)&msg_header, sizeof(msg_header));
          default:
            return;
            
        }
  }
  radio.setAutoAck(true);
  while(radio.getChannel() != 9){
    radio.setChannel(9);
    delay(1);
  }
  
  radio.startListening();
  Serial.flush();
  radio.flush_tx();
  radio.flush_rx();
  
}
