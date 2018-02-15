// nrf24_client.pde
// -*- mode: C++ -*-
//Code for the remote controller
//By pressing the 4 direction buttons 
//a message corresponding to the pressed direction
//is sent to the to the transceiver in the car
//by the transceiver in the remote controller
//Components used:
//NRF24L01 radio transceiver
//4 push-buttons
//4 resistors (couple of hundreds of ohms each)


#include <SPI.h>
#include <RH_NRF24.h>


RH_NRF24 nrf24;  //Instance for the radio transceiver

void setup() 
{
  //Serial is used for debugging and calibration of radio communication
  Serial.begin(9600);
  while (!Serial) 
    ; // wait for serial port to connect. Needed for Leonardo only
  if (!nrf24.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");
  //Set the following pins as inputs to read signals from the controller board
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT); 
}


void loop()
{
  uint8_t data[] = "M: Message";
  //Send forward message
  if (digitalRead(2) == HIGH)
  {
    Serial.println("Pressed up");
    data[0] = 'F';
  }
  //Send brake message
  if (digitalRead(3) == HIGH)
  {
    Serial.println("Pressed brake");
    data[0] = 'B';
  }
  //Send turn left message
  if (digitalRead(4) == HIGH)
  {
    Serial.println("Pressed left");
    data[0] = 'L';
  }
  //Send turn right message
  if (digitalRead(5) == HIGH)
  {
    Serial.println("Pressed right");
    data[0] = 'R';
  }

  Serial.println("Sending to nrf24_server");
  // Send a message to nrf24_server
  
  nrf24.send(data, sizeof(data));
  
  nrf24.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  
  
  
  if (nrf24.waitAvailableTimeout(500))
  { 
    // Should be a reply message for us now   
    if (nrf24.recv(buf, &len))
    {
      Serial.print("got reply: ");
      Serial.println((char*)buf);
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  else
  {
    Serial.println("No reply, is nrf24_server running?");
  }
}

