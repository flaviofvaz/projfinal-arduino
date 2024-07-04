#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <GFButton.h>
#include <RotaryEncoder.h>
#include <LinkedList.h>
#include <Servo.h>

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
    virtual ~OutputComponent();
    virtual void workButton();
    virtual void workPotentiometer(int value);
    virtual void workEncoder(int value);
    virtual void workLightSensor(int value);
};

class InputComponent: public Component
{
  public:
    InputComponent();
    virtual ~InputComponent();
    virtual void tick();
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
  int levelThreshold = 2;
  int currentPosition;
  public:
    PotentiometerComponent(int port1, int port2, int position);
    ~PotentiometerComponent();
    virtual void process(LinkedList<OutputComponent*> *linkedComponents);
};

class EncoderComponent: public InputComponent 
{
  private:
  int maxValue = 180;
  int minValue = 0;
  int currentPosition;
  public:
    RotaryEncoder *encoder;
    EncoderComponent(int port1, int port2, int position);
    ~EncoderComponent();

    int lastPosition = 0;
    void tick();
    void process(LinkedList<OutputComponent*> *linkedComponents);
};

class LightSensorComponent: public InputComponent 
{
  private:
    int maxValue = 100;
    int minValue = 0;
    int levelThreshold = 10;
    int currentValue;
  public:
    LightSensorComponent(int port1, int port2, int position);
    ~LightSensorComponent();

    void process(LinkedList<OutputComponent*> *linkedComponents);
};

class LedComponent: public OutputComponent 
{
  private:
    bool ledOn = false;
    void turnOn();
    void turnOff();
  public:
    LedComponent(int port1, int port2, int position);
    ~LedComponent();
    void workButton();
    void workPotentiometer(int value);
    void workEncoder(int value);
    void workLightSensor(int value);
};

class BuzzerComponent: public OutputComponent 
{
  private:
    bool buzzerOn = false;
    int buzzTimer = 500;
    float buzzFrequency = 220.0;
    float currentFrequency = 0.0;
    int beepThreshold = 80;
    void turnOn();
    void turnOff();
  public:
    BuzzerComponent(int port1, int port2, int position);
    ~BuzzerComponent();
    void workButton();
    void workPotentiometer(int value);
    void workEncoder(int value);
    void workLightSensor(int value);  
};

class ServoComponent: public OutputComponent 
{
  public:
    ServoComponent(int port1, int port2, int position);
    ~ServoComponent();
    Servo servo;
    void workButton();
    void workPotentiometer(int value);
    void workEncoder(int value);
    void workLightSensor(int value);
};

#endif