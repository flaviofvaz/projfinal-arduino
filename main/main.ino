#include "Components.h"

int inputPortsNum = 4;
int outputPortsNum = 4; 

int inputIdPorts[4] = { A0, A1, A2, A3 };
int outputIdPorts[4] = { A12, A13, A14, A15 };

int inputSignalPorts[4][3] = { {11,0,0}, {0,0,1}, {0,0,2}, {0,0,3} }; 
int outputSignalPorts[4][3] = { {13,5,0}, {2,0,1}, {0,0,2}, {0,0,3} };

int inputComponentsConnected[4] = {0, 0, 0, 0};
int outputComponentsConnected[4] = {0, 0, 0, 0};
int linkedComponents[4][4] = { {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};

InputComponent* inputComponentInstances[4];
OutputComponent* outputComponentInstances[4];

LinkedList<OutputComponent*> *links = new LinkedList<OutputComponent*>();

// STUB FUNCTION - REPLACE WITH FUNCTION THAT CHECKS WHAT COMPONENT IS IN THAT PORT
int stubFunction(int port)
{
  if (port == A0)
  {
    return 1;
  }
  else if (port == A12)
  {
    return 4;
  }
  else if (port == A13)
  {
    return 3;
  }
  return 0;
}

// STUB FUNCTION - REPLACE WITH FUNCTION THAT CHECKS IF COMPONENT IN inPort IS LINKED TO COMPONENT IN outPort
int isLinked(int inPort, int outPort)
{
  //Serial.println(outPort);
  if (inPort == 0 && outPort == 0) // replace by a check if it's linked
  {
    if (linkedComponents[inPort][outPort] == 0)
    {
      Serial.println("ligacao " + String(inputSignalPorts[inPort][2]) + " " + String(outputSignalPorts[outPort][2]) + " feita");
      linkedComponents[inPort][outPort] = 1;
    } 
    return 1;
  }
  else
  {
    // if it was connected but it isn't anymore then we say it's disconnected
    if (linkedComponents[inPort][outPort] == 1)
    {
      Serial.println("ligacao " + String(inputSignalPorts[inPort][2]) + " " + String(outputSignalPorts[outPort][2]) + " desfeita");
      linkedComponents[inPort][outPort] = 0;
    }
    return 0;
  }
}

void stubGetComponentsConnected(int inPort)
{
  // free memory
  while (links->size() > 0)
  {
    links->remove(0);
  }

  // build links matrice
  for (int outPort = 0; outPort < outputPortsNum; outPort++)
  {
    if (outputComponentsConnected[outPort] == 3 || outputComponentsConnected[outPort] == 4)
    {
      if(isLinked(inPort, outPort))
      {
        //Serial.println("Adicionando a lista do componente de entrada " + String(inPort) + " o componente de saida " + String(outPort));
        links->add(outputComponentInstances[outPort]);
      }
    }
  }
}

void processComponents(void)
{
  for (int inputPort = 0; inputPort < inputPortsNum; inputPort++)
  {
    if (inputComponentsConnected[inputPort] == 1 || inputComponentsConnected[inputPort] == 2)
    {
      stubGetComponentsConnected(inputPort);
      inputComponentInstances[inputPort]->process(links); 
    }
  }
}

void setup() 
{
  Serial.begin(9600);
  Serial.println("iniciou arduino");
}

void loop() 
{
  // loop through all input ports
  for (int i = 0; i < inputPortsNum; i++)
  {
    /* check if there's component connected to the port and what component is it: 
    1 - button, 2 - potentiometer, 3 - led, 4 - buzzer, 0 - nothing */

    int componentType = stubFunction(inputIdPorts[i]);
    int currentComponent = inputComponentsConnected[i];

    // if component found is the same of the one connected, continue
    if (componentType != currentComponent)
    {
      // update connected component in current port
      inputComponentsConnected[i] = componentType;

      // if component found is different from the one connected, "delete" the old one and create a new one
      switch (componentType)
      {
        case 0: // nothing
          delete(inputComponentInstances[i]);
          break;
        case 1:
          inputComponentInstances[i] = new ButtonComponent(inputSignalPorts[i][0], inputSignalPorts[i][1], inputSignalPorts[i][2]);
          break;
        case 2:
          //inputComponentInstances[i] = new EncoderComponent(inputSignalPorts[i][0], inputSignalPorts[i][1], inputSignalPorts[i][2]);
          break;
        default: // to be implemented components
          break;
      }
    }
  }

  // loop through all input ports
  for (int i = 0; i < outputPortsNum; i++)
  {
    /* check if there's component connected to the port and what component is it: 
    1 - button, 2 - potentiometer, 3 - led, 4 - buzzer, 0 - nothing */

    int componentType = stubFunction(outputIdPorts[i]);
    int currentComponent = outputComponentsConnected[i];

    // if component found is the same of the one connected, continue
    if (componentType != currentComponent)
    {
      // update connected component in current port
      outputComponentsConnected[i] = componentType;

      // if component found is different from the one connected, "delete" the old one and create a new one
      switch (componentType)
      {
        case 0: // nothing
          delete(outputComponentInstances[i]);
          break;
        case 3:
          outputComponentInstances[i] = new LedComponent(outputSignalPorts[i][0], outputSignalPorts[i][1], outputSignalPorts[i][2]);
          break;
        case 4:
          outputComponentInstances[i] = new BuzzerComponent(outputSignalPorts[i][0], outputSignalPorts[i][1], outputSignalPorts[i][2]);
          break;
        default: // to be implemented components
          break;
      }
    }
  }
  // process components
  processComponents();
}