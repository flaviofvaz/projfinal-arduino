#include <string>
#include <list>
#include <tuple>
#include "Components.h"

using namespace std;

class Component 
{      
  public:          
    tuple<int, int> ports;  
    Component(tuple<int, int> ports) 
    { 
      self.ports = ports;
    }
};

class OutputComponent: public Component 
{
  public:
    OutputComponent(tuple<int, int> ports): Component(ports) {};
    
    void workButton () {};
    void workPotentiometer (bool up) {};
};

class InputComponent: public Component
{
  public:
    InputComponent(tuple<int, int> ports): Component(ports) {};
    void process(list<OutputComponent>) {};
}

class ButtonComponent: public InputComponent 
{
  private:
    GFButton* arduinoButton;
  public:
    ButtonComponent(tuple<int, int> ports): InputComponent(ports) 
    {
      arduinoButton = new GFButton(ports[0]);
    }
    
    void process(list<OutputComponent> components)
    {
      arduinoButton->process();
      if (arduinoButton->isPressed())
      {
        for (auto const& c : components) 
        {
          c->workButton();
        }
      }
    }
};

class PotentiometerComponent: public InputComponent
{
  private: 
    RotaryEncoder* encoder;
    int lastPosition = 0;
    void tickDoEncoder() 
    {
      self.encoder->tick();
    }
    void loopComponents(list<OutputComponent> components, bool up)
    {
      for (auto const& c : components) 
        {
          c->workPotentiometer(up);
        }
    }
  public:
    PotentiometerComponent(tuple<int, int> ports): InputComponent(ports) 
    {
      encoder = new encoder(ports[0], ports[1]);
      int origin1 = digitalPinToInterrupt(ports[0]);
      attachInterrupt(origin1, tickDoEncoder, CHANGE);
      int origin2 = digitalPinToInterrupt(ports[1]);
      attachInterrupt(origem2, tickDoEncoder, CHANGE);
    }
    void process(list<OutputComponent> components)
    {
      int currentPosition = encoder.getPosition();
      if (currentPosition > lastPosition) 
      {
        up = true;
        loopComponents(true);
      }
      else if (currentPosition < lastPosition)
      {
        up = false;
        loopComponents(false);
      }
      lastPosition = currentPosition;
    }
}

class LedComponent: public OutputComponent 
{
  private:
    bool ledOn = false;
    bool ledBlinking = false;
    unsigned long lastInstant = 0;
    unsigned long blinkInterval = 2000;
    unsigned long blinkIntervalSteps = 100;
    unsigned long blinkIntervalMax = 5000;
    unsigned long blinkIntervalMin = 500;

    void turnOn(void)
    {
      digitalWrite(self.ports[0], LOW);
      self.ledOn = true;
    }
    void turnOff(void)
    {
      digitalWrite(self.ports[0], HIGH);  
      self.ledOn = false;
    }
  public:
    LedComponent(tuple<int, int> ports): OutputComponent(ports) 
    {
      pinMode(ports[0], OUTPUT);
    }
    void workPotentiometer(bool up)
    {
      if (up)
      {
        self.blinkInterval -= self.blinkIntervalSteps;
        if (self.blinkInterval < self.blinkIntervalMin)
        {
          self.blinkInterval = self.blinkIntervalMin;
        }
      }
      else
      {
        self.blinkInterval += self.blinkIntervalSteps;
        if (self.blinkInterval > self.blinkIntervalMax)
        {
          self.blinkInterval = self.blinkIntervalMax;
        }
      }
    }
    void workButton(void)
    {
      if (ledBlinking)
      {
        self.ledBlinking = false;
        self.turnOff();
      }
      else
      {
        self.ledBlinking = true;
        self.lastInstant = millis();
        self.turnOn();
      }
    }
    void process(void)
    {
      if (self.ledBlinking)
      {
        unsigned long currentInstant = millis();
        if (currentInstant > self.lastInstant + self.blinkInterval) 
        {
          if (self.ledOn)
          {
            self.turnOff();
          }
          else
          {
            self.turnOn();
          }
          self.lastInstant = currentInstant;
        }
      }
    }
};