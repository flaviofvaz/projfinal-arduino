#include <GFButton.h>
#include <RotaryEncoder.h>
#include <tuple>

class Component 
{      
  public:          
    tuple<int, int> ports;  
    Component(tuple<int, int> ports) {};
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
    void process(list<OutputComponent> components) {};
}

class ButtonComponent: public InputComponent 
{
  private:
    GFButton* arduinoButton;
  public:
    ButtonComponent(tuple<int, int> ports): InputComponent(ports) {};
};

class PotentiometerComponent: public InputComponent 
{
  private:
    RotaryEncoder* encoder;
    int lastPosition = 0;
    void tickDoEncoder() {};
    void loopComponents(list<OutputComponent> components, bool up) {};
  public:
    PotentiometerComponent(tuple<int, int> ports): InputComponent(ports) {};
};

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

    void turnOn() {};
    void turnOff() {};

  public:
    LedComponent(tuple<int, int> ports): OutputComponent(ports) {};
    void workPotentiometer(bool up) {};
    void workButton() {};
};