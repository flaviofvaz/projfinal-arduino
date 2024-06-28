#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <GFButton.h>
#include <RotaryEncoder.h>
#include <LinkedList.h>

class Component 
{
  public:
    int ports[2];
    int gridPosition;      
    Component();
    virtual void process();
};

class OutputComponent: public Component 
{
  public:
    OutputComponent();
    virtual void workButton ();
    virtual void workPotentiometer (bool up);
};

class InputComponent: public Component
{
  public:
    InputComponent();
    virtual void process(LinkedList<OutputComponent*> *linkedComponents);
};

class ButtonComponent: public InputComponent 
{
  private:
    unsigned long lastPressed = 0;
    unsigned int debounceTimer = 50;
    bool buttonPressed;
  public:
    GFButton* arduinoButton;
    ButtonComponent(int port1, int port2, int position);
    ~ButtonComponent();
    virtual void process(LinkedList<OutputComponent*> *linkedComponents);
};

class PotentiometerComponent: public InputComponent 
{
  private:
  int maxValue = 360;
  int minValue = 0;
  public:
    PotentiometerComponent(int port1, int port2, int position);
    ~PotentiometerComponent();
    virtual void process(LinkedList<OutputComponent*> *linkedComponents);
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
    void turnOn();
    void turnOff();
  public:
    LedComponent(int port1, int port2, int position);
    ~LedComponent();
    void workPotentiometer(bool up);
    void workButton();
};

class BuzzerComponent: public OutputComponent 
{
  private:
    bool buzzerOn = false;
    int buzzTimer = 500;
    float buzzFrequency = 220.0;
    void turnOn();
    void turnOff();
  public:
    BuzzerComponent(int port1, int port2, int position);
    ~BuzzerComponent();
    void workPotentiometer(bool up);
    void workEncoder(bool up);
    void workButton();
};

#endif