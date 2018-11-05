#include <SPI.h>
#include <RH_RF95.h>

#define STATE_SEND_DATA_0 1
#define STATE_WAIT_ACK_0 2
#define STATE_SEND_DATA_1 3
#define STATE_WAIT_ACK_1 4
#define STATE_TIME_OUT 5

RH_RF95 rf95;

int stateDFA = 0;

void setup() 
{
  Init();
}
 
void loop() 
{
  switch(stateDFA){
    case STATE_SEND_DATA_0:
      sendData0();
      break;
    case STATE_WAIT_ACK_0:
      waitACK0();
      break;
    case STATE_SEND_DATA_1:
      sendData1();
      break;
    case STATE_WAIT_ACK_1:
      waitACK1();
      break;
    case STATE_TIME_OUT:
      timeout(10000);
      break;
    default:
      break;
  }
}

void Init(){
  Serial.begin(9600);//Mở cổng Serial ở mức 9600
  while (!Serial);
  if (!rf95.init())
    Serial.println("init failed");
  pinMode(A0, INPUT);
  
  stateDFA = STATE_SEND_DATA_0;
}

void sendData0(){
  Serial.println("STATE_SEND_DATA_0");
  
  uint8_t data[] = "ACK"; 
  rf95.send(data, sizeof(data));
  rf95.waitPacketSent();
  delay(100);
  
  stateDFA = STATE_WAIT_ACK_0;
}

void sendData1(){
  Serial.println("STATE_SEND_DATA_1");
  
  int value = analogRead(A0);
  float hum = (float)value*100/1024;
  Serial.print("Value: ");
  Serial.print(value);
  Serial.print(" - ");
  Serial.print(hum);
  Serial.println("%");
  
  uint8_t data[4];
  data[0] = 1;
  data[1] = 2;
  data[2] = value >>8;
  data[3] = (value<<8)>>8;
  rf95.send(data, sizeof(data));
  rf95.waitPacketSent();

  stateDFA = STATE_WAIT_ACK_1;
}

void waitACK0(){
  Serial.println("STATE_WAIT_ACK_0");
  
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf95.waitAvailableTimeout(5000))
  { 
    if (rf95.recv(buf, &len))
   {
     if(buf[0] == 'S' && buf[1] == 'E' && buf[2] == 'Q'){
      Serial.print("got reply: ");
      Serial.println((char*) buf);
      stateDFA = STATE_SEND_DATA_1;
     } else {
      timeout(2000);
      stateDFA = STATE_SEND_DATA_0;
     }
    }
    else
    {
      Serial.println("recv failed");
      timeout(2000);
      stateDFA = STATE_SEND_DATA_0;
    }
  }
  else
  {
    Serial.println("No reply, is rf95_server running?");
    timeout(2000);
    stateDFA = STATE_SEND_DATA_0;
  }
  
}

void waitACK1(){
  Serial.println("STATE_WAIT_ACK_1");
  
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf95.waitAvailableTimeout(5000))
  { 
    if (rf95.recv(buf, &len))
   {
     if(buf[0] == 'O' && buf[1] == 'K'){
      Serial.print("got reply: ");
      Serial.println((char*) buf);
      stateDFA = STATE_TIME_OUT;
     } else {
      timeout(2000);
      stateDFA = STATE_SEND_DATA_0;
     }
    }
    else
    {
      Serial.println("recv failed");
      timeout(2000);
      stateDFA = STATE_SEND_DATA_0;
    }
  }
  else
  {
    Serial.println("No reply, is rf95_server running?");
    timeout(2000);
    stateDFA = STATE_SEND_DATA_0;
  }
}

void timeout(long timer){
  Serial.print("Wait ");
  Serial.print(timer/1000);
  Serial.println("secs\n");
  delay(timer);
  if(stateDFA == STATE_TIME_OUT){
    stateDFA = STATE_SEND_DATA_0;
  } else Serial.println("Wrong....\n");
}
