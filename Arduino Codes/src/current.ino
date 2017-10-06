//
// #include "EmonLib.h"
// #define WINDOW 30 //30 ssegundos de amostras para retirar o ruído
// #define TEMPO 60 // Uma hora de amostras
//
//
// int contador =0;
// double valor=0;
// double vIrms[WINDOW];
// double cIrms = 0;
// double delta=0.00000;
//
// EnergyMonitor emon1;
// void setup()
// {
//   emon1.current(0, 111.1);
//   Serial.begin(115200);
//
//        for (int i=0; i<WINDOW; i++) {
//            vIrms[i] = emon1.calcIrms(1480);
//            Serial.println(i);
//             Serial.println(vIrms[i]);
//           if (i>14){
//
//              cIrms = cIrms + vIrms[i];
//
//              delay(100);
//            }
//
//           }
//
//            delta = cIrms/(WINDOW-15);
//       double zero = 0.0000;
//       Serial.print(zero, 4);
//       Serial.flush();
//       Serial.print("Delta ");
//       Serial.println(delta);
//
//
// }
//
// void loop()
// {
//                                      while(contador <TEMPO)
//                                      {
//
//                                       unsigned long timethen = millis();
//                                       double Irms = emon1.calcIrms(1480)-delta;
//                                       Serial.println(Irms,5);
//                                       Serial.flush();
//                                       unsigned long atraso = millis() - timethen;
//
//                                       if(Irms <0.03){
//                                         Irms=0.00000;
//                                       }
//
//                                     valor = valor +(Irms*210.0000);
//                                   //  Serial.println(valor);
//                                        contador++;
//                                        delay(1000);
//                                       }
//
//   double final = ((valor/TEMPO)/1000.0000);
//   valor =0;
//   contador = 0;
//   Serial.print("FINAL:  ");
//   Serial.println(final,4);
//   Serial.flush();
// }
