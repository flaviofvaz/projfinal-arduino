#include "Components.h"

Component::Component() {};

void Component::process() {};

void OutputComponent::process() {};
void OutputComponent::workButton() {};
void OutputComponent::workPotentiometer(bool up) {}; 
void InputComponent::process() {};

ButtonComponent::ButtonComponent(int port1, int port2) 
{
  ports[0] = port1;
  ports[1] = port2;
  arduinoButton = new GFButton(port1);
  arduinoButton->setPressHandler(ButtonComponent::controlPress);
  Serial.println("instanciou botao");

};

void ButtonComponent::controlPress()
{
  Serial.println("apertou botao");
  for (int i = 0; i < connectedComponents->size(); i++) 
  {
    connectedComponents->get(i).workButton();
    Serial.println(i);
  }
}
    
void ButtonComponent::process()
{
  arduinoButton->process();
};

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

void LedComponent::turnOn(void)
{
  digitalWrite(ports[0], LOW);
  ledOn = true;
};
void LedComponent::turnOff(void)
{
  digitalWrite(ports[0], HIGH);  
  ledOn = false;
};
LedComponent::LedComponent(int ports [2]) 
{
  pinMode(ports[0], OUTPUT);
};
void LedComponent::workPotentiometer(bool up)
{
  if (up)
  {
    blinkInterval -= blinkIntervalSteps;
    if (blinkInterval < blinkIntervalMin)
    {
      blinkInterval = blinkIntervalMin;
    }
  }
  else
  {
    blinkInterval += blinkIntervalSteps;
    if (blinkInterval > blinkIntervalMax)
    {
      blinkInterval = blinkIntervalMax;
    }
  }
};
void LedComponent::workButton(void)
{
  if (ledBlinking)
  {
    ledBlinking = false;
    turnOff();
  }
  else
  {
    ledBlinking = true;
    lastInstant = millis();
    turnOn();
  }
};
void LedComponent::process(void)
{
  if (ledBlinking)
  {
    unsigned long currentInstant = millis();
    if (currentInstant > lastInstant + blinkInterval) 
    {
      if (ledOn)
      {
        turnOff();
      }
      else
      {
        turnOn();
      }
      lastInstant = currentInstant;
    }
  }
};