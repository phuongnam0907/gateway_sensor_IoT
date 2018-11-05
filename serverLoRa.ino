#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95;

int led = 13;

void setup() 
{
  pinMode(led, OUTPUT);     
  Serial.begin(9600);
  while (!Serial) ;
  if (!rf95.init());
}

void loop()
{
  if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
    {
      if(buf[0] == 'A' && buf[1] == 'C' && buf[2] == 'K'){
        delay(10);
        //Serial.println("YES");
        uint8_t data[] = "SEQ";
        delay(100);
        rf95.send(data, sizeof(data));
        rf95.waitPacketSent();
        delay(1);
        if (rf95.waitAvailableTimeout(5000))
        {
          // Should be a message for us now   
          uint8_t da[RH_RF95_MAX_MESSAGE_LEN];
          uint8_t ta = sizeof(da);
          if (rf95.recv(da, &ta))
          {
            Serial.write(da,ta);
            // Send a reply
            delay(100);
            uint8_t sendData[] = "OK";
            rf95.send(sendData, sizeof(sendData));
            rf95.waitPacketSent();
          }
          else {}
        }
      }
    }
    else {}
  }

  
}
