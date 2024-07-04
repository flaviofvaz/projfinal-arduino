#include "Components.h"

Component::Component() {};

void Component::process() {};

OutputComponent::OutputComponent() {};
OutputComponent::~OutputComponent() {};

void OutputComponent::workButton() {Serial.println("WORK BUTTON CAIU NA CLASSE BASE!");};
void OutputComponent::workPotentiometer(int value) {Serial.println("WORK POTENTIOMETER CAIU NA CLASSE ERRADA!");}; 
void OutputComponent::workEncoder(int value) {Serial.println("WORK ENCODER CAIU NA CLASSE ERRADA!");}; 
void OutputComponent::workLightSensor(int value) {Serial.println("WORK LIGHT SENSOR CAIU NA CLASSE ERRADA!");}; 

InputComponent::InputComponent() {};
InputComponent::~InputComponent() {};
void InputComponent::process(LinkedList<OutputComponent*> *linkedComponents) {Serial.println("PROCESS CAIU NA CLASSE BASE");};
void InputComponent::tick() {Serial.println("TICK CAIU NA CLASSE BASE");};

/*/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////
///////////// BUTTON COMPONENT IMPLEMENTATION /////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////*/
#pragma region Button
ButtonComponent::ButtonComponent(int port1, int port2, int position) 
{
  ports[0] = port1;
  ports[1] = port2;
  gridPosition = position;
  buttonPressed = false;
  arduinoButton = new GFButton(port1);
  Serial.println("botao " + String(gridPosition) + " conectado");
};

ButtonComponent::~ButtonComponent() 
{
  delete(arduinoButton);
  Serial.println("botao " + String(gridPosition) + " desconectado");
};
    
void ButtonComponent::process(LinkedList<OutputComponent*> *linkedComponents)
{
  if (!arduinoButton->isPressed())
  {
    lastPressed = millis();
    if (!buttonPressed)
    {
      buttonPressed = true;
      
      // process button press 
      Serial.println("botao " + String(gridPosition) + " apertado");
      for (int i = 0; i < linkedComponents->size(); i++) 
      {
        linkedComponents->get(i)->workButton();
      }
    }    
  }
  else
  {
    // treat debounce
    if (millis() > lastPressed + debounceTimer)
    {
      if (buttonPressed)
      {
        Serial.println("botao " + String(gridPosition) + " solto");
      }
      buttonPressed = false;
    }
  }
};
#pragma endregion Button

/*/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////
//////// POTENTIOMETER COMPONENT IMPLEMENTATION ///////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////*/
#pragma region Potentiometer
PotentiometerComponent::PotentiometerComponent(int port1, int port2, int position) 
{
  PotentiometerComponent::ports[0] = port1;
  PotentiometerComponent::ports[1] = port2;
  PotentiometerComponent::gridPosition = position;

  pinMode(port1, INPUT);
  Serial.println("potenciomentro " + String(gridPosition) + " conectado");

  int value = analogRead(ports[0]);

  int mapped_value = map(value, 0, 1023, PotentiometerComponent::minValue, PotentiometerComponent::maxValue);
  PotentiometerComponent::currentPosition = mapped_value;

  Serial.println("potenciometro " + String(PotentiometerComponent::gridPosition) + " girar " + String(mapped_value));
};

PotentiometerComponent::~PotentiometerComponent() 
{
  Serial.println("potenciomentro " + String(PotentiometerComponent::gridPosition) + " desconectado");
};
    
void PotentiometerComponent::process(LinkedList<OutputComponent*> *linkedComponents)
{
  int value = analogRead(PotentiometerComponent::ports[0]);
  int mapped_value = map(value, 0, 1023, PotentiometerComponent::minValue, PotentiometerComponent::maxValue);

  if (abs(mapped_value - PotentiometerComponent::currentPosition) > PotentiometerComponent::levelThreshold)
  {
    Serial.println("potenciometro " + String(PotentiometerComponent::gridPosition) + " girar " + String(mapped_value));
    for (int i = 0; i < linkedComponents->size(); i++) 
    {
      linkedComponents->get(i)->workPotentiometer(mapped_value);
    }
    PotentiometerComponent::currentPosition = mapped_value;
  }
};
#pragma endregion Potentiometer

/*/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////
//////////// ENCODER COMPONENT IMPLEMENTATION /////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////*/
#pragma region Encoder

EncoderComponent::EncoderComponent(int port1, int port2, int position) 
{
  EncoderComponent::ports[0] = port1;
  EncoderComponent::ports[1] = port2;
  EncoderComponent::gridPosition = position;

  EncoderComponent::encoder = new RotaryEncoder(port1, port2);

  EncoderComponent::currentPosition = EncoderComponent::encoder->getPosition();

  Serial.println("encoder " + String(EncoderComponent::gridPosition) + " conectado");
};

EncoderComponent::~EncoderComponent() 
{
  Serial.println("encoder " + String(EncoderComponent::gridPosition) + " desconectado");
};

void EncoderComponent::process(LinkedList<OutputComponent*> *linkedComponents)
{
  int value = encoder->getPosition();
  int clipped_value;
  if (value > maxValue)
  {
    clipped_value = maxValue;
  }
  else if (value < minValue)
  {
    clipped_value = minValue;
  }
  else
  {
    clipped_value = value;
  }

  if (clipped_value != currentPosition)
  {
    Serial.println("encoder " + String(EncoderComponent::gridPosition) + " girar " + String(clipped_value));
    for (int i = 0; i < linkedComponents->size(); i++) 
    {
      linkedComponents->get(i)->workEncoder(clipped_value);
    }

    // update current position
    EncoderComponent::currentPosition = clipped_value;
  }
};

void EncoderComponent::tick() 
{
  encoder->tick();
};
#pragma endregion Encoder

/*/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////
/////////// LIGHT SENSOR COMPONENT IMPLEMENTATION /////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////*/
#pragma region Light_Sensor
LightSensorComponent::LightSensorComponent(int port1, int port2, int position) 
{
  ports[0] = port1;
  ports[1] = port2;
  gridPosition = position;
  
  pinMode(port1, INPUT);
  currentValue = analogRead(port1);

  Serial.println("sensor_luz " + String(gridPosition) + " conectado");
};

LightSensorComponent::~LightSensorComponent() 
{
  Serial.println("sensor_luz " + String(gridPosition) + " desconectado");
};

void LightSensorComponent::process(LinkedList<OutputComponent*> *linkedComponents)
{
  int value = analogRead(ports[0]);

  if (abs(value - currentValue) > levelThreshold)
  {
    int clipped_value;
    if (value > maxValue)
    {
      clipped_value = maxValue;
    }
    else if (value < minValue)
    {
      clipped_value = minValue;
    }
    else
    {
      clipped_value = value;
    }

    Serial.println("sensor_luz " + String(gridPosition) + " nivel " + String(clipped_value));
    for (int i = 0; i < linkedComponents->size(); i++) 
    {
      linkedComponents->get(i)->workEncoder(value);
    }
    currentValue = value;
  }
};
#pragma endregion Encoder

/*/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////
//////////////// LED COMPONENT IMPLEMENTATION /////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////*/
#pragma region Led
void LedComponent::turnOn(void)
{
  Serial.println("led " + String(LedComponent::gridPosition) + " ligado");
  digitalWrite(LedComponent::ports[0], HIGH);
  LedComponent::ledOn = true;
};

void LedComponent::turnOff(void)
{
  Serial.println("led " + String(LedComponent::gridPosition) + " desligado");
  digitalWrite(LedComponent::ports[0], LOW);  
  LedComponent::ledOn = false;
};

LedComponent::LedComponent(int port1, int port2, int position) 
{
  LedComponent::gridPosition = position;
  LedComponent::ports[0] = port1;
  LedComponent::ports[1] = port2;
  pinMode(port1, OUTPUT);
  Serial.println("led " + String(gridPosition) + " conectado");
};

LedComponent::~LedComponent() 
{
  digitalWrite(ports[0], LOW);
  Serial.println("led " + String(gridPosition) + " desconectado");
};

void LedComponent::workButton(void)
{
  if (LedComponent::ledOn)
  {
    LedComponent::turnOff();
  }
  else
  {
    LedComponent::turnOn();
  }
};

void LedComponent::workPotentiometer(int value) 
{
  int mapped_value = map(value, 0, 360, 50, 200);
  analogWrite(LedComponent::ports[0], mapped_value);
  if(mapped_value >= 100)
  {
    if (!LedComponent::ledOn)
    {
      LedComponent::ledOn = true;
      Serial.println("led " + String(LedComponent::gridPosition) + " ligado");
    }
  }
  else
  {
    if(LedComponent::ledOn)
    {
      LedComponent::ledOn = false;
      Serial.println("led " + String(LedComponent::gridPosition) + " desligado");
    }
  }
};

void LedComponent::workEncoder(int value) 
{
  int mapped_value = map(value, 0, 180, 50, 200);
  analogWrite(LedComponent::ports[0], mapped_value);
  if(mapped_value >= 100)
  {
    if (!LedComponent::ledOn)
    {
      LedComponent::ledOn = true;
      Serial.println("led " + String(LedComponent::gridPosition) + " ligado");
    }
  }
  else
  {
    if(LedComponent::ledOn)
    {
      LedComponent::ledOn = false;
      Serial.println("led " + String(LedComponent::gridPosition) + " desligado");
    }
  }
};
void LedComponent::workLightSensor(int value) {};

#pragma endregion Led

/*/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////
//////////// BUZZER COMPONENT IMPLEMENTATION //////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////*/
#pragma region Buzzer
BuzzerComponent::BuzzerComponent(int port1, int port2, int position) 
{
  gridPosition = position;
  ports[0] = port1;
  ports[1] = port2;

  pinMode(ports[0], OUTPUT);
  pinMode(ports[1], OUTPUT);
  digitalWrite(ports[1], LOW);

  Serial.println("buzzer " + String(gridPosition) + " conectado");
};

BuzzerComponent::~BuzzerComponent() 
{
  digitalWrite(ports[0], LOW);
  Serial.println("buzzer " + String(gridPosition) + " desconectado");
};

void BuzzerComponent::turnOn(void)
{
  Serial.println("buzzer " + String(gridPosition) + " ligado");
  tone(ports[0], buzzFrequency);
  buzzerOn = true;
};

void BuzzerComponent::turnOff(void)
{
  Serial.println("buzzer " + String(gridPosition) + " desligado");
  noTone(ports[0]);  
  buzzerOn = false;
};

void BuzzerComponent::workButton(void) 
{
  if (buzzerOn)
  {
    turnOff();
  }
  else
  {
    turnOn();
  }
};

void BuzzerComponent::workPotentiometer(int value) 
{
  int mapped_value = map(value, 0, 1023, 0, 600);
  if (mapped_value > 0)
  {
    buzzerOn = true;
    noTone(ports[0]);
    tone(ports[0], float(mapped_value));
  }
  else
  {
    buzzerOn = false;
    noTone(ports[0]);
  }
};

void BuzzerComponent::workEncoder(int value) 
{
  int clipped_value;

  if (value > 500)
  {
    clipped_value = 500;
  }
  else if (value < 0)
  {
    clipped_value = 0;
  }
  else
  {
    clipped_value = value;
  }

  if (clipped_value > 0)
  {
    buzzerOn = true;
    noTone(ports[0]);
    tone(ports[0], float(clipped_value));
  }
  else
  {
    buzzerOn = false;
    noTone(ports[0]);
  }
};

void BuzzerComponent::workLightSensor(int value) 
{
  if (value > beepThreshold)
  {
    turnOn();
  }
  else
  {
    turnOff();
  }
};
#pragma endregion Buzzer

/*/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////
///////////// SERVO COMPONENT IMPLEMENTATION //////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////*/
#pragma region Servo
ServoComponent::ServoComponent(int port1, int port2, int position) 
{
  gridPosition = position;
  ports[0] = port1;
  ports[1] = port2;

  servo.attach(port1);
  Serial.println("servo " + String(gridPosition) + " conectado");
};

ServoComponent::~ServoComponent() 
{
  Serial.println("servo " + String(gridPosition) + " desconectado");
  servo.detach();
};

// Goes to 0
void ServoComponent::workButton(void) 
{
  servo.write(0);
};

// Go to mapp
void ServoComponent::workPotentiometer(int value) 
{
  int mapped_value = map(value, 0, 1023, 0, 180);
  servo.write(mapped_value);
};

void ServoComponent::workEncoder(int value) 
{
  int clipped_value;

  if (value > 180)
  {
    clipped_value = 180;
  }
  else if (value < 0)
  {
    clipped_value = 0;
  }
  else
  {
    clipped_value = value;
  }

  servo.write(clipped_value);
};

void ServoComponent::workLightSensor(int value) 
{
  servo.write(value);
};
#pragma endregion Buzzer










