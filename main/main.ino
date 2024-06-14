#include "Components.h"

int numPorts = 16;

int idPorts [16] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15};
int signalPorts [16][2] = { {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0} };
int componentsConnected [16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  
InputComponent* inputComponentInstances [16];
OutputComponent* outputComponentInstances [16];

int stubFunction(int port, int signalPorts[][2], int portIdx)
{
  if (portIdx == 8)
  {
    signalPorts[portIdx][0] = 11;
    signalPorts[portIdx][1] = 12;
    return 1;
  }
  else if (portIdx == 10)
  {
    signalPorts[portIdx][0] = 13;
    signalPorts[portIdx][1] = 0;
    return 3;
  }
  return 0;
}

void stubGetComponentsConnected(int port, LinkedList<OutputComponent> outComponents)
{
  outComponents.add(*(outputComponentInstances[10]));  
}

void processComponents(void)
{
    for (int i = 0; i < numPorts; i++)
    {
        if (componentsConnected[i] == 1 || componentsConnected[i] == 2)
        {
          LinkedList<OutputComponent> outComponents;
          stubGetComponentsConnected(i, outComponents);
          inputComponentInstances[i]->connectedComponents = &outComponents;
          inputComponentInstances[i]->process(); 
        }
    }
}

void setup() 
{
    Serial.begin(9600);
}

void loop() 
{
    // loop through all ports
    for (int i = 0; i < numPorts; i++)
    {
        /* check if there's component connected to the port and what component is it: 
        1 - button, 2 - potentiometer, 3 - led, 4 - buzzer, 0 - nothing */

        int componentType = stubFunction(idPorts[i], signalPorts, i);

        // if component found is the same of the one connected, continue
        if (componentType == componentsConnected[i])
        {
            continue;
        }

        // if component found is different from the one connected, "delete" the old one and create a new one
        switch (componentType)
        {
            case 0: // nothing
                break;
            case 1:
                Serial.println("Button connected in port " + idPorts[i]);
                inputComponentInstances[i] = new ButtonComponent(signalPorts[i][0], signalPorts[i][1]);
                Serial.println(signalPorts[i][0]);
                Serial.println(signalPorts[i][1]);
                Serial.println(i);
                break;
            case 2:
                Serial.println("Encoder connected in port " + idPorts[i]);
                //inputComponentInstances[i] = new EncoderComponent(signalPorts[i]);
                Serial.println(signalPorts[i][0]);
                Serial.println(signalPorts[i][1]);
                break;
            case 3:
                Serial.println("Led connected in port " + idPorts[i]);
                outputComponentInstances[i] = new LedComponent(signalPorts[i]);
                Serial.println(signalPorts[i][0]);
                Serial.println(signalPorts[i][1]);
                break;
            case 4:
                Serial.println("Buzzer connected in port " + idPorts[i]);
                //componentInstances[i] = new BuzzerComponent(signalPorts[i]);
                break;
            default: // to be implemented components
                break;
        }
        // update connected component in current port
        componentsConnected[i] = componentType;
    }
    // process components
    processComponents();
}