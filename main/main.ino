#include "Components.h"

int numPorts = 16;
int inputPortsNum = 4;
int outputPortsNum = 4; 

int idPorts [16] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15};
int signalPorts [16][3] = { {0,0,0}, {0,0,1}, {0,0,2}, {0,0,3}, {0,0,4}, {0,0,5}, {0,0,6}, {0,0,7}, {11,0,8}, {0,0,9}, {13,5,10}, {0,0,11}, {0,0,12}, {0,0,13}, {0,0,14}, {0,0,15} };

int componentsConnected [16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
LinkedList<OutputComponent*> *links = new LinkedList<OutputComponent*>();

InputComponent* inputComponentInstances [16];
OutputComponent* outputComponentInstances [16];

int linkedComponents[4][4];

void initLinks()
{
  for (int i = 0; i < inputPortsNum; i++)
  {
    for (int j = 0; j < outputPortsNum; j++)
    {
      linkedComponents[i][j] = 0;
    }
  }
}

int stubFunction(int port)
{
  if (port == A8)
  {
    return 1;
  }
  else if (port == A10)
  {
    return 4;
  }
  return 0;
}

// STUB FUNCTION
void isLinked(int portA, int portB)
{
  if (idPorts[portA] == A8 && idPorts[portB] == A10)
  {
    linkedComponents[portA][portB] = 1;
    Serial.println("ligacao " + String(signalPorts[portA][2]) + " " + String(signalPorts[portB][2]));
  }
}

void stubGetComponentsConnected(int port)
{

  // free memory
  while (links->size() > 0)
  {
    links->remove(0);
  }

  links->add(outputComponentInstances[10]);
}

void processComponents(void)
{
  for (int i = 0; i < numPorts; i++)
  {
    if (componentsConnected[i] == 1 || componentsConnected[i] == 2)
    {
      stubGetComponentsConnected(i);
      inputComponentInstances[i]->process(links); 
    }
  }
}

void setup() 
{
  Serial.begin(9600);
  Serial.println("iniciou arduino");
  initLinks();
}

void loop() 
{
  // loop through all ports
  for (int i = 0; i < numPorts; i++)
  {
    /* check if there's component connected to the port and what component is it: 
    1 - button, 2 - potentiometer, 3 - led, 4 - buzzer, 0 - nothing */

    int componentType = stubFunction(idPorts[i]);
    int currentComponent = componentsConnected[i];

    // if component found is the same of the one connected, continue
    if (componentType != currentComponent)
    {
      // update connected component in current port
      componentsConnected[i] = componentType;

      // if component found is different from the one connected, "delete" the old one and create a new one
      switch (componentType)
      {
        case 0: // nothing
          //finishedConnection(i, currentComponent);
          delete(outputComponentInstances[i]);
          break;
        case 1:
          inputComponentInstances[i] = new ButtonComponent(signalPorts[i][0], signalPorts[i][1], signalPorts[i][2]);
          // Serial.println(signalPorts[i][0]);
          // Serial.println(signalPorts[i][1]);
          break;
        case 2:
          //inputComponentInstances[i] = new EncoderComponent(signalPorts[i][0], signalPorts[i][1], signalPorts[i][2]);

          Serial.println(signalPorts[i][0]);
          Serial.println(signalPorts[i][1]);
          break;
        case 3:
          outputComponentInstances[i] = new LedComponent(signalPorts[i][0], signalPorts[i][1], signalPorts[i][2]);
          // Serial.println(signalPorts[i][0]);
          // Serial.println(signalPorts[i][1]);
          break;
        case 4:
          outputComponentInstances[i] = new BuzzerComponent(signalPorts[i][0], signalPorts[i][1], signalPorts[i][2]);

          Serial.println(signalPorts[i][0]);
          Serial.println(signalPorts[i][1]);
          break;
        default: // to be implemented components
          break;
      }
    }
  }

  // process components
  processComponents();
}