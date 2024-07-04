#include "Components.h"

int inputPortsNum = 4;
int outputPortsNum = 4; 

int inputSignalPorts[4][4] = { {A0, 2, 3, 45}, {A1, 18, 19, 46}, {A2, A8, A9, 47}, {A3, A10, A11, 48} }; 
int outputSignalPorts[4][4] = { {A4, 22, 23, 49}, {A5, 6, 25, 50}, {A6, 26, 27, 51}, {A7, 28, 29, 52} };

int inputComponentsConnected[4] = {0, 0, 0, 0};
int outputComponentsConnected[4] = {0, 0, 0, 0};
int linkedComponents[4][4] = { {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};

InputComponent* inputComponentInstances[4];
OutputComponent* outputComponentInstances[4];

LinkedList<OutputComponent*> *links = new LinkedList<OutputComponent*>();


#pragma region CheckConnections

typedef struct {
  char nome[50];
  int resistencia;
  int numero;
} Componentes;

Componentes listaC[8];


int checa_port(int port) 
{
  int valor = analogRead(port);
  if (valor <= 40) 
  {
    return 0;
  } 
  else 
  {
    return 1;
  }
}

float checa_resistencia(int resis, int entrada) 
{
  int valorAnalogico = analogRead(entrada);
  //Serial.println(entrada);
  //Serial.println(valorAnalogico);
  float Vout = 5 * (4700. / (resis + 4700));
  float valSaida = (Vout * 1023) / 5;
  //Serial.println(valorAnalogico);
  //Serial.println(valSaida);

  float diff = (valSaida - valorAnalogico) / valorAnalogico;
  return diff;
}

int teste_componente(int port) 
{
  int ind = -1;

  if (checa_port(port)) 
  {
    //Serial.println(port);
    for (int j = 0; j < 8; j++) 
    {
      float diff = checa_resistencia(listaC[j].resistencia, port);
      //Serial.println(diff);
      if (sqrt(diff * diff) <= 0.06) 
      {
        ind = j;
        //Serial.println(String(port)+": " +listaC[ind].nome);
        return listaC[ind].numero;
      }
    }
    if (ind == -1) 
    {
      //Serial.println(String(port)+": Nenhum Componente");
      return 0;
    }
  } 
  else 
  {
    return 0;
  }
}
#pragma endregion CheckConnection


int checkLink(int inPort, int outPort)
{
  pinMode(outPort, INPUT_PULLUP);
  delay(10);
  if (digitalRead(outPort) == LOW)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

// Checks if component in port inPort is linked to component in outPort
bool isLinked(int inPort, int outPort)
{ 
  pinMode(outputSignalPorts[outPort][3], INPUT_PULLUP);
  delay(10);

  bool linked = false;
  if (digitalRead(outputSignalPorts[outPort][3]) == LOW)
  {
    linked = true;
  }

  if (linked)
  {
    if (linkedComponents[inPort][outPort] == 0)
    {
      Serial.println("ligacao " + String(inPort) + " " + String(outPort) + " feita");
      linkedComponents[inPort][outPort] = 1;
    } 
    return true;
  }
  else
  {
    // if it was connected but it isn't anymore then we say it's disconnected
    if (linkedComponents[inPort][outPort] == 1)
    {
      Serial.println("ligacao " + String(inPort) + " " + String(outPort) + " desfeita");
      linkedComponents[inPort][outPort] = 0;
    }
    return false;
  }
}

void getComponentsConnected(int inPort)
{
  // free memory
  while (links->size() > 0)
  {
    links->remove(0);
  }

  // set input port to output as send 5v
  pinMode(inputSignalPorts[inPort][3], OUTPUT);
  digitalWrite(inputSignalPorts[inPort][3], LOW);

  // build links matrice
  for (int outPort = 0; outPort < outputPortsNum; outPort++)
  {
    if (outputComponentsConnected[outPort] != 0)
    {
      if(isLinked(inPort, outPort))
      {
        links->add(outputComponentInstances[outPort]);
      }
    }
  }
  // back to normal
  digitalWrite(inputSignalPorts[inPort][3], HIGH);
}

void processComponents(void)
{
  for (int inputPort = 0; inputPort < inputPortsNum; inputPort++)
  {
    if (inputComponentsConnected[inputPort] != 0)
    {
      getComponentsConnected(inputPort);
      inputComponentInstances[inputPort]->process(links); 
    }
  }
}

void tickEncoders()
{
  // check all input ports
  for(int i = 0; i < inputPortsNum; i++)
  {
    // if there's an encoder in that port
    if (inputComponentsConnected[i] == 3)
    {
      // tick encoder
      inputComponentInstances[i]->tick();
    }
  }
}

void setup() 
{
  Serial.begin(9600);
  Serial.println("init");

  Componentes cp1, cp2, cp3, cp4, cp5, cp6, cp7, cp8;

  strcpy(cp1.nome, "button");
  cp1.resistencia = 100;
  cp1.numero = 1;

  strcpy(cp2.nome, "potentiometer");
  cp2.resistencia = 1000;
  cp2.numero = 2;

  strcpy(cp3.nome, "encoder");
  cp3.resistencia = 2150;
  cp3.numero = 3;

  strcpy(cp4.nome, "light sensor");
  cp4.resistencia = 3300;
  cp4.numero = 4;

  strcpy(cp5.nome, "led");
  cp5.resistencia = 10000;
  cp5.numero = 5;

  strcpy(cp6.nome, "buzzer");
  cp6.resistencia = 4750;
  cp6.numero = 6;

  strcpy(cp7.nome, "servo");
  cp7.resistencia = 15000;
  cp7.numero = 7;

  strcpy(cp8.nome, "nada");
  cp8.resistencia = 500;
  cp8.numero = 8;

  listaC[0] = cp1;
  listaC[1] = cp2;
  listaC[2] = cp3;
  listaC[3] = cp4;
  listaC[4] = cp5;
  listaC[5] = cp6;
  listaC[6] = cp7;
  listaC[7] = cp8;
}


void loop() 
{
  int componentType;
  int currentComponent;
  int i;

  // loop through all input ports
  for (i = 0; i < inputPortsNum; i++)
  {
    /* check if there's component connected to the port and what component is it: 
    1 - button, 2 - potentiometer, 3 - encoder, 4 - light sensor, 0 - nothing */

    //int componentType = stubFunction(inputIdPorts[i]);
    componentType = teste_componente(inputSignalPorts[i][0]);
    currentComponent = inputComponentsConnected[i];

    // if component found is the same of the one connected, continue
    if (componentType != currentComponent)
    {
      // if component found is different from the one connected, "delete" the old one and create a new one
      switch (componentType)
      {
        case 0: // nothing
          delete(inputComponentInstances[i]);

          // update connected component in current port to 0
          inputComponentsConnected[i] = 0;

          break;
        case 1:
          inputComponentInstances[i] = new ButtonComponent(inputSignalPorts[i][1], inputSignalPorts[i][2], i);

          // update connected component in current port
          inputComponentsConnected[i] = componentType;
          
          break;
        case 2:
          inputComponentInstances[i] = new PotentiometerComponent(inputSignalPorts[i][1], inputSignalPorts[i][2], i);

          // update connected component in current port
          inputComponentsConnected[i] = componentType;
          
          break;
        case 3:
          inputComponentInstances[i] = new EncoderComponent(inputSignalPorts[i][1], inputSignalPorts[i][2], i);
          attachInterrupt(digitalPinToInterrupt(inputSignalPorts[i][1]), tickEncoders, CHANGE); // LAME
          attachInterrupt(digitalPinToInterrupt(inputSignalPorts[i][2]), tickEncoders, CHANGE); // ASS

          // update connected component in current port
          inputComponentsConnected[i] = componentType;
          
          break;
        case 4:
          inputComponentInstances[i] = new LightSensorComponent(inputSignalPorts[i][1], inputSignalPorts[i][2], i);

          // update connected component in current port
          inputComponentsConnected[i] = componentType;
          
          break;
        default: 
          break;
      }
    }
  }

  // loop through all input ports
  for (i = 0; i < outputPortsNum; i++)
  {
    /* check if there's component connected to the port and what component is it: 
    5 - led, 6 - buzzer, 7 - servo, 8 - empty, 0 - nothing */

    //int componentType = stubFunction(outputIdPorts[i]);
    componentType = teste_componente(outputSignalPorts[i][0]);
    currentComponent = outputComponentsConnected[i];

    // if component found is the same of the one connected, continue
    if (componentType != currentComponent)
    {
      // if component found is different from the one connected, "delete" the old one and create a new one
      switch (componentType)
      {
        case 0: // nothing
          delete(outputComponentInstances[i]);
          
          // update connected component in current port to 0
          outputComponentsConnected[i] = 0;

          break;
        case 5:
          outputComponentInstances[i] = new LedComponent(outputSignalPorts[i][1], outputSignalPorts[i][2], i);

          // update connected component in current port
          outputComponentsConnected[i] = componentType;

          break;
        case 6:
          outputComponentInstances[i] = new BuzzerComponent(outputSignalPorts[i][1], outputSignalPorts[i][2], i);
          
          // update connected component in current port
          outputComponentsConnected[i] = componentType;
          
          break;
        case 7:
          outputComponentInstances[i] = new ServoComponent(outputSignalPorts[i][1], outputSignalPorts[i][2], i);

          // update connected component in current port
          outputComponentsConnected[i] = componentType;

          break;
        default: 
          break;
      }
    }
  }
  
  // REMOVE THISSSSSSSSSSSS
  //isLinked(45, 49);
  
  //process components
  processComponents();

  delay(100);
}