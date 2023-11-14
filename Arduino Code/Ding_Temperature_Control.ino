/********************************************************
   PID Temperature Control for Microscopes, Microfluidic Chambers, and Custom Incubators
   By: Tony Liu @ Ding Lab
 ********************************************************/

// Include libraries for sensors, LCD, and PID
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "Adafruit_MCP9808.h"
#include <LiquidCrystal_I2C.h>
#include <QuickPID.h>

// Declare pin numbers
const int PROBEPIN = 9;
const int HEATERPIN = 6;
const int TARGETPIN = 1;

// Declare tempearture variables
float targetTemp, Input, Output;
double probeTemp, mcpTemp, mcpTemp2, avgTemp;

// PID gains; change these according to your setup
float Kp = 505, Ki = 62, Kd = 92;

// Constructors for sensors
OneWire oneWire(PROBEPIN);
DallasTemperature probe(&oneWire);
LiquidCrystal_I2C lcd(0x27,20,4);
Adafruit_MCP9808 mcp = Adafruit_MCP9808();
Adafruit_MCP9808 mcp2 = Adafruit_MCP9808();
QuickPID myPID(&Input, &Output, &targetTemp);

// Reads potentiometer input and converts to 20-60 Degree for the target temperature
void changeTargetTemp()
{
  // Read potentiometer input from 0 to 1023
  int targetInput = analogRead(TARGETPIN);

  // Converts potentiometer input to 20-60 degree target temperature
  targetTemp = map(targetInput, 0, 1023, 20, 60);
}

//Reads all temperature sensors and potentiometer
void readInput() 
{
  // Reads the target temperature from the potentiometer
  changeTargetTemp();

  // Prints the target temperature to the serial monitor
  Serial.print("Target:");
  Serial.print(targetTemp);
  Serial.print(" ");

  // Prints the target temperature to the top right of the LCD
  lcd.setCursor(9, 0);
  lcd.print("T:");
  lcd.print(targetTemp, 2);

  // Updates the temperature reading from the probe sensor
  probe.requestTemperatures(); 
  probeTemp = probe.getTempCByIndex(0);

  // Prints the probe temperature to the serial monitor
  Serial.print("| Internal:");
  Serial.print(probeTemp, 3);
  Serial.print(" ");
  
  // Updates the temperature reading from the surface MCP sensor
  mcpTemp = mcp.readTempC();

  // Prints the surface MCP temperature to the serial monitor
  Serial.print("| Surface:");
  Serial.print(mcpTemp, 3);
  Serial.print(" ");
  
  // Updates the temperature reading from the ambient MCP sensor
  mcpTemp2 = mcp2.readTempC();

  // Prints the ambient MCP temperature to the serial monitor
  Serial.print("| Ambient:");
  Serial.print(mcpTemp2, 3);
  Serial.print(" ");
  
  // Prints the ambient MCP temperature to the bottom left of the LCD
  lcd.setCursor(0, 1);
  lcd.print("A:");
  lcd.print(mcpTemp2, 2);

  // Calculates the average temperature between the probe and surface MCP
  if(probeTemp > 15 && mcpTemp > 15) // Only calculates average if both sensors are above 15 degrees; prevents unstable readings
  {
    avgTemp = (probeTemp + mcpTemp) / 2.0;  
    Input = avgTemp;
    Serial.print("| Average:");
    Serial.print(avgTemp, 3);
    Serial.print(" ");
    
    // Prints the average temperature to the top left of the LCD
    lcd.setCursor(0, 0);
    lcd.print("I:");
    lcd.print(avgTemp, 2);
  }
}

// Setup function runs once at the beginning of the program
// Initializes sensors, LCD, and PID
void setup()
{  
  // Initialize serial monitor, LCD, and probe sensor
  Serial.begin(9600);
  while(!Serial);
  lcd.begin(16, 2);
  probe.begin();

  // Initialize MCP sensors; if not found, print error message and pause program
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
  
  // Set MCP sensor resolution to 0.0625 °C
  mcp.setResolution(3);
  mcp2.setResolution(3);
  
  // Set heater and target pins to output mode
  pinMode(HEATERPIN, OUTPUT);
  pinMode(TARGETPIN, INPUT);
  
  // Initialize LCD and turn on backlight
  lcd.init();
  lcd.backlight();

  // Set initial target temperature to 25 degrees
  targetTemp = 25;

  // Apply PID gains
  myPID.SetTunings(Kp, Ki, Kd);

  // Turn the PID on
  myPID.SetMode(myPID.Control::automatic);
}

// Loop function runs repeatedly after setup
// During every loop, the program reads temperature, computes PID output, and writes to LCD
void loop()
{
  // Reads input, computes PID, and writes output
  readInput();
  myPID.Compute();
  analogWrite(HEATERPIN, Output);

  // Prints the output percentage to the bottom right of the LCD
  lcd.setCursor(9, 1);
  float outputPercent = Output/255*100;
  lcd.print("O:");

  // Adjusts the number of decimal places depending on the output percentage
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

  // Prints new line to serial monitor to serperate each loop
  Serial.print("| Output:");
  Serial.print(outputPercent);
  Serial.println("%");

  // Delays for 100 ms to allow the algorithm and sensors to update
  delay(100);
}
