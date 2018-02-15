// nrf24_server.pde
// -*- mode: C++ -*-
//Code for the car to receive and handle messages
//Based on the messages received,
//the car moves, stops or turns its wheels
//Components used:
//SN7544 motor controller
//NRF24L01 transreceiver
//6-14 V DC motor for rotating the rear wheels
//Servo motor for turning the front wheels

#include <SPI.h>
#include <RH_NRF24.h>
#include <SoftwareServo.h>

RH_NRF24 nrf24;    //Instance for the radio (NRF24L01)
int speedPin = 3;  //For the sn7544 motor controller
int thrust_motor_pin1 = 6;  // For controlling the sn7544 motor controller
int thrust_motor_pin2 = 7; //For controlling the sn7544 motor controller
int thrust_motor_speed;  //Speed value (0-255) of the main motor
SoftwareServo servo_motor;  //Instance for the servo motor
int servo_motor_pin = 0; //Needs to be PWN
int servo_motor_pos = 0; //Position of the servo at the beginning
//For lighting the direction signal leds
int left_sign_pin = A5;  
int right_sign_pin = A4;
int brake_sign_pin = A3;
int left_on = 0;  //For creating a blinking effect for turning signals
int right_on = 0; // with simple rotation of a boolean value


void setup() 
{
  //Serial used for debugging and calibration of communication
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
  //Set the following pins as outputs to send signals to pins of sn7544
  pinMode(speedPin, OUTPUT);
  pinMode(thrust_motor_pin1, OUTPUT); 
  pinMode(thrust_motor_pin2, OUTPUT);   
  servo_motor.attach(A0);  //Servo receives signal from the analog 0 pin
  //Set the following pins for the signaling leds
  pinMode(left_sign_pin, OUTPUT);
  pinMode(right_sign_pin, OUTPUT);
  pinMode(brake_sign_pin, OUTPUT);
  
}

void loop()
{
  
  if (nrf24.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (nrf24.recv(buf, &len))
    {
      Serial.print("got request: ");
      Serial.println((char*)buf);
      
      //Go forward
      if (buf[0] == 'F')
      {
        digitalWrite(thrust_motor_pin1, LOW);
        digitalWrite(thrust_motor_pin2, HIGH);
        thrust_motor_speed = 250; 
        analogWrite(speedPin, thrust_motor_speed);       

      }
      
      //Brake, i.e. stop the motor
      if (buf[0] == 'B')
      {

        digitalWrite(thrust_motor_pin1, LOW);
        digitalWrite(thrust_motor_pin2, HIGH);
        thrust_motor_speed = 0; 
        analogWrite(speedPin, thrust_motor_speed);
        digitalWrite(brake_sign_pin, HIGH);  //Signal braking
      }
      else
        digitalWrite(brake_sign_pin, LOW);  //Turn the brake signal led off
        
      //Turn left
      if (buf[0] == 'L')
      {
        //Turn the servo to the left
        if (servo_motor_pos > 0)
        {
          servo_motor_pos--;
          servo_motor.write(servo_motor_pos);
        }
        delay(25);  //Wait for the servo to reach the position
        if (left_on == 0)
          left_on = 1;
        else
          left_on = 0;
        digitalWrite(left_sign_pin, left_on);
        
      }
      else
        digitalWrite(left_sign_pin, LOW);
        
      //Turn right
      if (buf[0] == 'R')
      {
        //Turn the servo to the right
        if (servo_motor_pos < 180)
        {
          servo_motor_pos++;
          servo_motor.write(servo_motor_pos);
        }
        delay(25);  //Wait for the servo to reach the position
        if (right_on == 0)
          right_on = 1;
        else
          right_on = 0;
        digitalWrite(right_sign_pin, right_on);
      }
      else
        digitalWrite(right_sign_pin, LOW);
        
      // Send a reply
      SoftwareServo::refresh();  //Update the servo
      uint8_t data[] = "And hello back to you";
      nrf24.send(data, sizeof(data));
      nrf24.waitPacketSent();
      Serial.println("Sent a reply");
    }
    else
    {
      Serial.println("recv failed");
    }
  }
}


