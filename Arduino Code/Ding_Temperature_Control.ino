/********************************************************
   PID Temperature Control
   By: Tony Liu @ Ding Lab
 ********************************************************/
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "Adafruit_MCP9808.h"
#include <LiquidCrystal_I2C.h>
#include <QuickPID.h>

//Pins
const int PROBEPIN = 9;
const int HEATERPIN = 6;
const int TARGETPIN = 1;

//Variables
float targetTemp, Input, Output;
double probeTemp, mcpTemp, mcpTemp2, avgTemp;
float Kp = 505, Ki = 62, Kd = 92; //Change this according to your setup

//Constructors for sensors
OneWire oneWire(PROBEPIN);
DallasTemperature probe(&oneWire);
LiquidCrystal_I2C lcd(0x27,20,4);
Adafruit_MCP9808 mcp = Adafruit_MCP9808();
Adafruit_MCP9808 mcp2 = Adafruit_MCP9808();
QuickPID myPID(&Input, &Output, &targetTemp);

//Helper function to calculate double for map function, use this if you need decimal target temperature
double mapf(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//Takes in potentiometer input and converts to 20-60 Degree target temperature
void changeTargetTemp()
{
  int targetInput = analogRead(TARGETPIN);//1023 - smoothInput();
  // Serial.print(targetInput);
  targetTemp = map(targetInput, 0, 1023, 20, 60);
}

//Reads all temperature sensors and potentiometer
void readInput() 
{
  //potentiometer reading and display
  changeTargetTemp();
  Serial.print("Target:");
  Serial.print(targetTemp);
  Serial.print(" ");

  lcd.setCursor(9, 0);
  lcd.print("T:");
  lcd.print(targetTemp, 2);
  // lcd.print('C');

  //Internal Probe
  probe.requestTemperatures(); 
  probeTemp = probe.getTempCByIndex(0);
  Serial.print("Internal: ");
  Serial.print(probeTemp, 3);
  Serial.print(" ");
  
  //Surface MCP Setup
  mcpTemp = mcp.readTempC();
  Serial.print("Surface:");
  Serial.print(mcpTemp, 3);
  Serial.print(" ");
  
  //Ambient MCP Setup
  mcpTemp2 = mcp2.readTempC();
  Serial.print("Ambient:");
  Serial.print(mcpTemp2, 3);
  Serial.print(" ");
  
  lcd.setCursor(0, 1);
  lcd.print("A:");
  lcd.print(mcpTemp2, 2);

  if(probeTemp > 15 && mcpTemp > 15)
  {
    avgTemp = (probeTemp + mcpTemp) / 2.0;  
    Input = avgTemp;
    Serial.print("Average:");
    Serial.print(avgTemp, 3);
    Serial.println("  ");
    
    lcd.setCursor(0, 0);
    lcd.print("I:");
    lcd.print(avgTemp, 2);
  }
}

void setup()
{  
  Serial.begin(9600);
  while(!Serial);
  lcd.begin(16, 2);
  probe.begin();

  if (!mcp.begin(0x18)) 
  {
    Serial.println("Couldn't find MCP9808!");
    while (1);
  }
  if (!mcp2.begin(0x19)) 
  {
    Serial.println("Couldn't find MCP9808 Number 2 (Ambient)!");
    while (1);
  }
  
  mcp.setResolution(3);
  mcp2.setResolution(3);
  
  pinMode(HEATERPIN, OUTPUT);
  pinMode(TARGETPIN, INPUT);
  
  lcd.init();
  lcd.backlight();

    targetTemp = 25;

  //apply PID gains
  myPID.SetTunings(Kp, Ki, Kd);

  //turn the PID on
  myPID.SetMode(myPID.Control::automatic);
}

void loop()
{
  readInput();
  myPID.Compute();
  analogWrite(HEATERPIN, Output);

  lcd.setCursor(9, 1);
  float outputPercent = Output/255*100;
  lcd.print("O:");
  if(outputPercent == 100)
  {
    lcd.print(outputPercent, 0);
    lcd.print("% ");
  }
  else if(outputPercent < 10)
  {
    lcd.print(outputPercent, 2);
    lcd.print("% ");
  }
  else
  {
    lcd.print(outputPercent, 1);
    lcd.print('%');
  }
  Serial.print("\n");
  delay(100);
}
