#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <GFButton.h>
#include <RotaryEncoder.h>
#include <LinkedList.h>

class Component 
{
  public:
    int ports[2];      
    Component();
    void process();
};

class OutputComponent: public Component 
{
  public:
    void workButton ();
    void process();
    void workPotentiometer (bool up);
};

class InputComponent: public Component
{
  public:
    static LinkedList<OutputComponent>* connectedComponents;
    virtual void process();
};

class ButtonComponent: public InputComponent 
{   
  public:
    GFButton* arduinoButton;
    static LinkedList<OutputComponent>* connectedComponents;
    ButtonComponent(int port1, int port2);
    void process();
    static void controlPress();
};

// class EncoderComponent: public InputComponent 
// {
//   public:
//     RotaryEncoder *encoder;
//     EncoderComponent(int ports [2]);
//     int lastPosition = 0;
//     static void tickDoEncoder();
//     void loopComponents(LinkedList<OutputComponent> components, bool up);
//     void process(LinkedList<OutputComponent> components);
//     void setup();
// };

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

    void turnOn();
    void turnOff();

  public:
    LedComponent(int ports [2]);
    void workPotentiometer(bool up);
    void workButton();
    void process();
};
#endif