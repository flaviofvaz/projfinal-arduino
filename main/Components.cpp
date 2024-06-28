#include "Components.h"

Component::Component() {};

void Component::process() {};

OutputComponent::OutputComponent() {};
void OutputComponent::workButton() {Serial.println("WORK BUTTON CAIU NA CLASSE BASE!");};
void OutputComponent::workPotentiometer(bool up) {Serial.println("WORK POTENTIOMETER CAIU NA CLASSE ERRADA!");}; 

InputComponent::InputComponent() {};
void InputComponent::process(LinkedList<OutputComponent*> *linkedComponents) {};

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
  if (arduinoButton->isPressed())
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
//////////// ENCODER COMPONENT IMPLEMENTATION /////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////*/
#pragma region Encoder
// void EncoderComponent::tickDoEncoder() 
// {
//   encoder->tick();
// };

// void EncoderComponent::loopComponents(LinkedList<OutputComponent> components, bool up)
// {
//   //for (auto const& c : components) 
//   //  {
//    //   c->workPotentiometer(up);
//     //}
// };

// EncoderComponent::EncoderComponent(int ports [2]) 
// {
//   encoder = new RotaryEncoder(ports[0], ports[1]);
// };

// void EncoderComponent::setup()
// {
//   int origin1 = digitalPinToInterrupt(ports[0]);
//   attachInterrupt(origin1, tickDoEncoder, CHANGE);
//   int origin2 = digitalPinToInterrupt(ports[1]);
//   attachInterrupt(origin2, tickDoEncoder, CHANGE);
// };

// void EncoderComponent::process(LinkedList<OutputComponent> components)
// {
//  int currentPosition = encoder->getPosition();
//  bool up;
//  if (currentPosition > lastPosition) 
//  {
//    up = true;
//    loopComponents(components, true);
//  }
//  else if (currentPosition < lastPosition)
//  {
//    up = false;
//    loopComponents(components, false);
//  }
//  lastPosition = currentPosition;
// };
#pragma endregion Encoder

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
  ports[0] = port1;
  ports[1] = port2;
  gridPosition = position;

  pinMode(ports[0], INPUT);
  Serial.println("potenciomentro " + String(gridPosition) + " conectado");
};

PotentiometerComponent::~PotentiometerComponent() 
{
  Serial.println("potenciomentro " + String(gridPosition) + " desconectado");
};
    
void PotentiometerComponent::process(LinkedList<OutputComponent*> *linkedComponents)
{
  int value = map(analogRead(ports[0]), 0, 1023, minValue, maxValue);
  Serial.println(value);
};
#pragma endregion Potentiometer

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
  Serial.println("led " + String(gridPosition) + " ligado");
  digitalWrite(ports[0], HIGH);
  ledOn = true;
};

void LedComponent::turnOff(void)
{
  Serial.println("led " + String(gridPosition) + " desligado");
  digitalWrite(ports[0], LOW);  
  ledOn = false;
};

LedComponent::LedComponent(int port1, int port2, int position) 
{
  gridPosition = position;
  ports[0] = port1;
  ports[1] = port2;
  pinMode(port1, OUTPUT);
  Serial.println("led " + String(gridPosition) + " conectado");
};

LedComponent::~LedComponent() 
{
  digitalWrite(ports[0], LOW);
  Serial.println("led " + String(gridPosition) + " desconectado");
};

void LedComponent::workPotentiometer(bool up) { };

void LedComponent::workButton(void)
{
  if (ledOn)
  {
    turnOff();
  }
  else
  {
    turnOn();
  }
};
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

void BuzzerComponent::workPotentiometer(bool up) {};
void BuzzerComponent::workEncoder(bool up) {};
#pragma endregion Buzzer










