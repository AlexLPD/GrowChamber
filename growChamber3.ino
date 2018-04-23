//germinadoraV3.ino
//Gracias especiales a Surbyte por la colaboración. 



//Library to time management---------------------------------------- 
#include <TimeLib.h>
#include <TimeAlarms.h>
//--------------------------time definitions
#define segundo	 1000
#define minuto	(60*segundo)
#define hora	(60*minuto)

// LCD
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#define BACKLIGHT_PIN 13
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);  // Set the LCD I2C address

//DTH reading and constructors---------------------------------------
#include <dht.h>
dht DHT;
#define DHT11_PIN 5
float T = 0;       //var to hold temperature measurement average 
float H = 0;       //var to hold  humidity measurement average 
float rawT = 0;    //raw measurements 
float rawH = 0; 
enum sensor {sensor_ok, sensor_error}   //variable to read the status of the sensor 
sensor = sensor_ok; 
unsigned long sensor_time = 0; 

//MeanFilter---------------------------------------------------------------
#include "MeanFilterLib.h"
MeanFilter<float> temperature(6);
MeanFilter<float> humidity(6);





//=======================GLOBAL VARIABLES==========================//

//watering variables-------------------------------------------------
unsigned long mtbWatering 		= 0; 			//min time var 
unsigned long mtbWateringDelay 	= (1*minuto);	//min time between watterings 
unsigned long maxHumTimmer 		= 0; 			//max hum start time 
unsigned long maxHumDelay 		= (10* minuto); //max hum delay time 

//float H = 0; 					//For humidity filter measure  
float minH = 20;				//for the min humidity, will changue in program day or nigth 
float maxH = 30; 				//maximun Hum, will changue in program day or nigth 
float minHInWatering = 29.5;	//minimun Humidty in Watering, changue in program day or nigth  


//=================================================================//
//---------------------------VOID SETUP----------------------------//
//=================================================================//
void setup() 
{
	setTime(13, 03, 00,20,04,18); 					//SET THE TIME, UNCCOMENT IF RTC IS USED
  	Alarm.alarmRepeat( 8, 0,  0, VariablesDia);  	//Set day watering setings 8:00 am 
  	Alarm.alarmRepeat( 20, 0, 0, VariablesNoche);  	//Set nigth watering setings 8:00 pm 
    
    lcd.begin(20, 4);								//LCD SETINGS                   

	lcd.setCursor(0,0);
    lcd.print(F("INICIANDO PROGRAMA"));
}


//=================================================================//
//---------------------------VOID LOOP-----------------------------//
//=================================================================//
void loop() {
	
	//alarm.delay(1000);

}




//=================================================================//
//---------------------------FUNCTIONS-----------------------------//
//=================================================================//

// functions to be called when an alarm triggers:
/*
void VariablesDia(){
  	Serial.println("Ajuste variables dia"); 
	minH = 20;					//min hum in day time 	
	maxH = 100;					//max hum in day time 
	minHInWatering = 80;		//min hum in day watering 

	mtbWatering 		= 0; 
	mtbWateringDelay 	= (10*minuto); 

	maxHumTimmer 		= 0; 
	maxHumDelay 		= (10* minuto);
}

void VariablesNoche(){   
  	Serial.println("Ajuste variables noche"); 
	minH = 0;					//min hum in ngt time 	
	maxH = 80;					//max hum in ngt time 
	minHInWatering = 60;		//min hum in ngt watering 

	mtbWatering 		= 0; 
	mtbWateringDelay 	= (30*minuto); 

	maxHumTimmer 		= 0; 
	maxHumDelay 		= (10* minuto);
}
*/

// WATERING STATE MACHINE
void watering() {
	
	typedef enum {	watSM1, 	//see if the minum timmer between waterings has elapsed 
					watSM2, 	//see if the current H is below minH 
					watSM3, 	//turn on the fogger and the fan 
					watSM4, 	//we reach max hum? yes next step, no wait here
					watSM5, 	//stop the fogger and the fan 
					watSM6, 	//start max humidity timmer 
					watSM7, 
					watSM8, 	//has the max watering time elapsed? no next step, yes save timme for min wat time / got to watSM11
	 				watSM9, 	//
	 				watSM10,	//save time for min time between watering and moce foward 
	 				watSM11,	//turn off fogger and fan and move foward.
	 				watSM12,	//exit from watering machine on main loop 
	 			}	watSMOp;  	//

	watSMOp watSM  = watSM1; 

	switch (watSM) { 	//watering State Machine 
	    case watSM1:
			      	//min time between waterings elapsed
					if(millis() - mtbWatering > mtbWateringDelay) watSM = watSM2; 
			      	else { watSM = watSM1; }
			    	break;
	    
	    case watSM2:
			    	//is the humidity below Hmin level?   
			      	if( H < minH) watSM = watSM3; 
			      	else {watSM = watSM2; }
			    	break;
	    
	    case watSM3: 
			    	//turn on the fogger and the fan and move on 
			    	// fogger on 
			    	// fan On 
			    	watSM = watSM4; 
			    	break; 
	    
	    case watSM4: 
			    	//we reach max humidiy? no-wait here, yes-next step
			    	if(H > maxH) watSM = watSM5; 
			    	else { watSM = watSM4; }
			    	break; 
	    
	    case watSM5: 
			    	//if we make the high humidity stop the fogger and the fan and move on
			    	//fogger off 
			    	//fan off
			    	watSM = watSM6; 
			    	break; 
				
		case watSM6:
					//start the max humidity timmer
					//poner una guarda booleana aca 
					maxHumTimmer = millis();
					watSM = watSM7; 
					break; 

		case watSM7:
					//has the maximun watering time elapsed? 
					if(millis() - maxHumTimmer > maxHumDelay)
						{	
							mtbWatering = millis();  //save the time for minimun time between waterings
							watSM = watSM10;         //if we end of wattering go and end the machine. 
						}
						else //if not elapsed move on
						{
							watSM = watSM8;
						}

					break; 
		
		case watSM8:
					if( H > maxH)
						{
							//turn off the fogger 
						}
						else 
							{
								watSM = watSM9; // go to next step
							}
					break;
		
		case watSM9:
					if( H < minHInWatering) 
					{
						//start the fogger
						watSM = watSM7; 
					}
						else {watSM = watSM7; }
					break; 
		
		case watSM10:
					//take time for min time between watering. 
					//reset boolean guard for take time 
					watSM = watSM11; 
					break;
			
		case watSM11:
					//turn off fogger, turn off fan 
					//move foward 
					break;
	
		case watSM12:
					break; 	//insert instruction to end thw watering state machine.  
	}

}


//FUCTION TO READ DTH------------------------------------------------------------
void readDTH22()
{
        //----Read the dth11 sensor 
        DHT.read11(DHT11_PIN);
        
        //----asign value to numerals; 
        rawT = DHT.temperature, 1;
        rawH = DHT.humidity, 1;
        
        //----chek for sensor error;
        if(rawT == -999) sensor = sensor_error;  else sensor = sensor_ok; 
        if(rawH == -999) sensor = sensor_error;  else sensor = sensor_ok;    

        //----if the sensor was ok the calculate average
        //----calculate average of each one 
        if(sensor == sensor_ok)
            {
              T = temperature.AddValue(rawT);
              H = humidity.AddValue(rawH);
              
              //---restart the timmer for reading dth
              sensor_time = millis();  
            }
              //----if the sensor have an error, return 0 to array. 
              else
              {
                T = 0; 
                H = 0; 
              }
}




/*
PROYECT DESCRIPTION; 
Grow chamber control V_3
Arduino Mega 2560

-----------PIN OUT------------
DTH22 DATA - PIN 5 

LCD SDA    - PIN SDA 
LCD SCL    - PIN SCL

SD MISO	   - PIN 50
SD MOSI	   - PIN 51
SD SCK	   - PIN 52
SD CS 	   - PIN 53

RELAY1     - PIN     FOGGER 
RELAY2     - PIN     FAN 
TIP120     - PIN     LED STRIPS 

Functions;

1.- Read and calculate an temperature and humidity average                    
2.- Chek for error on the sensor and display / dont send data if error        
3.- watering machine 
4.- Show temperature and humidity on LCD display 
5.- Water low level alarm 
6.- ligthing management machine 
7.- sd card logger of events, temperatures and humidity
8.- add a watchdog to the program 
*/
