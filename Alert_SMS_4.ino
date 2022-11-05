#define trigPin 6
#define echoPin 7
#include <SoftwareSerial.h>
int duration, distance;
char Received_SMS; //The full received SMS is stored here as a character (with phone sending number and date/time) 
short PowerON=-1;  //Its been used later in the program to show if there is the word "PowerON" within the received SMS. "-1" means they are not found
String objectText;  //Here's the SMS that we are going  send to the phone number.
SoftwareSerial gsm(4, 5);//connect RXD to pin 5 , connect TXD to pin 4

void setup() {
     gsm.begin(9600); // The GSM  module is set to trasmit and receive bits in serial at 9600 bits/second i.e. 9600 baud rate.
     Serial.begin(9600); //The Arduino Uno R3 is set for serial communication at the same speed (9600baud rate) as GSM module for coherent communication.
     Serial.println("\nInitializing\n");
     pinMode(trigPin, OUTPUT); // The trigger Pin is set as an output pin. Thus signals are sent from Arduino to the trigger of sensor via this pin.
     pinMode(echoPin, INPUT); // Echo pin is set as an input pin,therefore, Arduino receives digital pulses from echo terminal.
     ReceiveMode();  //Calling the function that puts the SIM800L module on receiving SMS mode.
}

void loop(){
  String RSMS; //This is where we put the received SMS.
               
             
 while(gsm.available()>0) { 
      Received_SMS=gsm.read();  //"char Received_SMS" is now containing the full SMS received
      Serial.print(Received_SMS);   //Show it on the serial monitor (optional)     
      RSMS.concat(Received_SMS);    //Concatenate the "char Recevied_SMS" to "String RSMS" which makes the "RSMS" contain the SMS received as a string. The received SMS cannot be stored directly as string.
      PowerON=RSMS.indexOf("PowerON"); //And this is why we changed from char to String, it's to be able to use this function "indexOf"
                                        //"indexOf function looks for the substring "x" within the String (here RSMS) and gives us its index or position
                                        //For example if found at the beginning it will give "0" after 1 character it will be "1"
                                        //If it's not found it will give "-1", so the variables are integers
      if(PowerON!=-1){                         //If "Power                              ON" word is found within the SMS, it means that PowerON has other value than -1 so we can proceed
           Serial.println("\nfound Power ON\n");
           delay(2000);
           while (true) {
           sensorExecution(); // Calling the function that triggers the sensor.
           }  
         }
         }
}



void sensorExecution (){        
        digitalWrite(trigPin, HIGH);  // High pulse is triggered  for 10 microseconds on the ultrasonic sensor. 
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        duration = pulseIn(echoPin, HIGH); // The sound pulses are received by the sensor and output to the arduino as digital pulses
        distance = (duration*0.034)/2;    // Duration taken for input pulse from echopin to go LOW from HIGH is used to calculate distance. This distance is in cm.
      
      if (distance >= 10 || distance <= 0){   // If an object is within a range of 0 to 10 cm, the sensor sends a signal that triggers a text being sent. 
                                              // If not the sensor continues calculating the range of objects.
          Serial.println("\nNo object detected");
          Serial.print("\tdistance= ");Serial.print(distance);
          delay(1000);
          }
      else {
          Serial.println("\nObject detected");
          Serial.print("\tdistance= ");Serial.print(distance);
          Serial.println("\nSending text...\n");
          objectText = "Object in proximity.\ndistance = "+String (distance,7)+" cm"; 
          objectDetected ();
          }
      }
  
  void ReceiveMode(){       //Set the SIM800L Receive mode
        gsm.println("AT"); //If everything is Okay it will show "OK" on the serial monitor
        Serialcom();
        gsm.println("AT+CMGF=1"); // Configuring TEXT mode
        Serialcom();
        gsm.println("AT+CNMI=2,2,0,0,0"); //Configure the SIM800L on how to manage the Received SMS.
        Serialcom();
        }
 
 void Serialcom(){ //This is used with ReceiveMode function, it's okay to use for tests with Serial monitor
        delay(500);
        while(Serial.available()){                                                                      
            gsm.write(Serial.read());//Forward what Serial port received to Software Serial Port
            }
        while(gsm.available()){                                                                     
            Serial.write(gsm.read());//Forward what Software Serial port received to Serial Port
            }
        }
 
 void objectDetected (){  
       gsm.println("AT+CMGF=1\r");    //Sets the GSM Module in Text Mode
       gsm.println("AT+CMGS=\"+254797878582\"\r"); // Replace x with your mobile number(not the number which insert in to gsm module)
       delay(1000);
       gsm.print(objectText);
       delay(1000);       
       gsm.print((char)26);// ASCII code of CTRL+Z. This is used terminate our text message. 
       delay(8000);
      }
  
