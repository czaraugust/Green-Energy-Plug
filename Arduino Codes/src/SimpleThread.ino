#include "Thread.h"
#include "ThreadController.h"
#include <TimerOne.h>
#include "EmonLib.h"
#include <SoftwareSerial.h>

EnergyMonitor emon1;

#define WINDOW 45 //30 ssegundos de amostras para retirar o ruído
#define TEMPO  60000 // Uma hora de amostras
int pinRele = 7;



//VARIÁVEIS DO SENSOR DE CORRENTE HALL
 int conta =0;
 double sensorHallValue=0.00000;

 double delta=0.00000;
 double acumulador =0.0000;

 bool estadoRele = false;

 SoftwareSerial portOne(9, 8);



class battery: public Thread{
public:
    //VARIÁVEIS DO SENSOR DE TENSÃO
    double vout = 0.0;

    double vin = 0.0;
    double R1 = 30000.0; //
    double R2 = 7500.0; //
    int value = 0;
    double currentDCread = 0.0;
    double currentDC = 0.0;
    void run(){

    value = analogRead(A4);
    vout = (value * 5.0) / 1024.0; // see text
    vin = vout / (R2/(R1+R2));


    //ESSA PARTE!!!!!
    int ACSoffset = 2500;
    currentDCread = analogRead(A5);
    double Voltage = (currentDCread / 1023.0) * 5000; // Gets you mV
    currentDC = ((Voltage - ACSoffset) / 66);


    //Serial.println(currentDC);

    runned();
    }
};


class CurrentSensor: public Thread{
public:
  double Irms;

    void run(){
      Irms = emon1.calcIrms(1480)-delta;
        if(Irms <0.03){
        Irms=0.00000;
      }
        conta++;
        acumulador = acumulador + Irms;
        //acumulador =  Irms;
        //Serial.println(acumulador);

        //  Serial.println(conta);

      runned();
    }
};

class OnOff: public Thread{
public:

    void run(){
        portOne.listen();
        if (portOne.available()) {

       char c = portOne.read();

        if (c== '@'){
          estadoRele =true;
       }
       else if (c =='&'){
         estadoRele = false;
       }
     }
      runned();
    }
};

    OnOff rele = OnOff();

    battery batt = battery();

    CurrentSensor sensorHall = CurrentSensor();

    ThreadController controller = ThreadController();

void timerCallback(){
	controller.run();
}

void setup(){

	 Serial.begin(115200);
   portOne.begin(115200);

     emon1.current(0,111.1);

     pinMode(pinRele, OUTPUT);
     double cIrms = 0;

     double vIrms[WINDOW];
     for (int i=0; i<WINDOW; i++) {
         vIrms[i] = emon1.calcIrms(1480);
         //Serial.println(i);
          //Serial.println(vIrms[i]);
        if (i>14){

           cIrms = cIrms + vIrms[i];

           delay(100);
         }

        }

         delta = cIrms/(WINDOW-14);
         Serial.print("Delta ");
         Serial.println(delta);






         batt.setInterval(10000);
         rele.setInterval(100);
         sensorHall.setInterval(1000);

    	 controller.add(&sensorHall);
       controller.add(&batt);
       controller.add(&rele);



	 Timer1.initialize(20000);
	 Timer1.attachInterrupt(timerCallback);
	 Timer1.start();

}

void loop(){

      Serial.print("B");
      Serial.print(batt.currentDC);
      Serial.print("|");
      Serial.print(batt.vin);

      delay(5000);

      Serial.print("A");
      Serial.print(acumulador/conta);
      acumulador =0;
      conta =0;

      delay(5000);


      if (estadoRele){

        digitalWrite(pinRele, HIGH);
        delay(1000);

      }
      else{

        digitalWrite(pinRele, LOW);
        delay(1000);
      }

      Serial.print(" ");
      Serial.println(estadoRele);

      delay(TEMPO);


}
