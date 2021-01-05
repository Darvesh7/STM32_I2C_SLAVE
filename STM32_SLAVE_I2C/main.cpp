#include "BMP280.h"
#include "MPU6050.h"
#include "mbed.h"
#include <cstdio>
#include <cstring>

Serial pc(USBTX, USBRX);

BMP280 BMP(I2C_SDA, I2C_SCL);

I2CSlave slave(PB_11, PB_10);

Thread bmp_thread;
Thread i2c_thread;

void BMP_Data(void);
void I2C_COMMUNICATION(void);

AnalogIn ain(A0);

int temperature = 0;
int pressure = 0;

int i2c_temp;
int i2c_pressure;



void setup() {
  BMP.initialize();
  bmp_thread.start(BMP_Data);
}

int main() {
  setup();

  slave.address(0x48 << 1);

  char buf[10];
  char msg[] = "slave";
  static int m = 0;

  while (1) 
  {

    int i = slave.receive();
    switch (i)
    {
    case I2CSlave::ReadAddressed:
         slave.write(msg[m]);
         m++;
         if(m > sizeof(msg))
         {
            m = 0;
         }
        break;
    case I2CSlave::WriteGeneral:
      slave.read(buf, 10);
      printf("Read G: %s\n", buf);
      break;

    case I2CSlave::WriteAddressed:
      {
      slave.read(buf, 1);
      char j = buf[0];
        switch (j)
        {
            case 0x01:
            {
                sprintf(msg,"%d",temperature);
                //strcpy(msg, "role ");
                break;
            }
            case 0x02:
            {
                sprintf(msg,"%d",pressure);
                //strcpy(msg, "role ");
                break;
            }
            default:
                break;
        }
      
      break;
      }
    }
    for (int i = 0; i < 10; i++) 
    {
      buf[i] = 0; // Clear buffer
    }

    // sprintf(msg,"%d %d",temperature, pressure);
     //slave.write(msg, strlen(msg) + 1);
  }
}

void BMP_Data(void) 
{
  while (1) 
  {
    BMP.initialize();

    pressure = BMP.getPressure();
    printf("Pressure = %d\n", pressure);

    temperature = BMP.getTemperature();
    printf("Temperature = %d\n", temperature);



    ThisThread::sleep_for(2000);
  }
}
