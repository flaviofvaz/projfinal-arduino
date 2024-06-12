#include <tuple>
#include "Components.h"

int numPorts = 16;

int idPorts [16] = {A0, A1, A2, A3, A4, A5 A6, A7, A8, A9, A10, A11, A12, A13, A14, A15};
int componentsConnected [16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
Component* componentInstances [16];

int stubFunction(int port)
{
    return 0;
}

void processComponents(void)
{
    for (int i = 0; i < numPorts; i++)
    {
        if (componentsConnected[i] == 1 || componentsConnected[i] == 2)
        {
            componentInstances[i]->process();
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
    for (int i = 0; i < numComponents; i++)
    {
        /* check if there's component connected to the port and what component is it: 
        1 - button, 2 - potentiometer, 3 - led, 4 - buzzer, 0 - nothing */

        int componentType = stubFunction(idPorts[i]);

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
                componentInstances[i] = new ButtonComponent(idPorts[i]);
                break;
            case 2:
                Serial.println("Potentiometer connected in port " + idPorts[i]);
                componentInstances[i] = new PotentiometerComponent(idPorts[i]);
                break;
            case 3:
                Serial.println("Led connected in port " + idPorts[i]);
                componentInstances[i] = new LedComponent(idPorts[i]);
                break;
            case 4:
                Serial.println("Buzzer connected in port " + idPorts[i]);
                componentInstances[i] = new BuzzerComponent(idPorts[i]);
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