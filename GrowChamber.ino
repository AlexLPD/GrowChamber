/*
================================================================================
//control_de_germinadora.ino
================================================================================

    File........... control_de_germinadora.ino
    Purpose........ manage all capabilityes of a germination chamber
    Author......... Alejandro Santiago Sevilla 
    E-mail......... ale.ikon.10@gmail.com
    Started........ 14/04/2018
    Finished....... 
    Updated........ --/--/----


================================================================================
   Program description
================================================================================
This program control; read relative humidity and temperature of the wroging chamber, 
controls the watering cycles by reading a minimun relative humidity, the watering 
to a maximun humidity level, and remain in this maximun humidity for a determined 
time once the humidity drops to a low level, there is a second timmer for wait 
a minumun between cycles. 
The program is divided in three serveral stages; Day time, nigth time, and wait reason 
being the water need for the plants is different in day and nigth, wait will be 
only be used to an "open door" scenario, when we wait until re stablish the normal 
operation. 
All this information is show in a LCD 20x04 display. 

Water low lever indicator in led and in sound must be a prior. 

In addition a "No Sufficient Water for the day " fuction can be implemented to 
avoid let the crops without water in a 24hr cycle.

Paralell to this operation, the gear has an a build in datalogger, thath register;
temperature, humidity, number of waterings, time of the begin of the waterings 
time of end of the waterings and if there any alarm in the system. 

================================================================================
   Notes
================================================================================


================================================================================
  Updates
================================================================================

*/
//Time definitions--------------------------------------------------------------
#define segundo 1000
#define minuto  (60*segundo)
#define hora    (60* minuto)

//Master program Variables and Fuctions-----------------------------------------
//-------------------------------------Grow chamber status----------------------
enum growChamberStatusOptions {dayTime, nigthTime, Wait}; 
growChamberStatusOptions growChamber = dayTime; 
//-------------------------------------Temperature and humidity-----------------
//DTH Library --------------------------------------------------------------
#include <dht.h>
dht DHT;
#define DHT11_PIN 14
float T = 0;       //var to hold temperature measurement average 
float H = 0;       //var to hold  humidity measurement average 
float rawT = 0;    //raw measurements 
float rawH = 0; 
enum sensor {sensor_ok, sensor_error}   //variable to read the status of the sensor 
sensor = sensor_ok; 

//MeanFilter---------------------------------------------------------------
#include "MeanFilterLib.h"
MeanFilter<float> temperature(6);
MeanFilter<float> humidity(6);

//Timmer for reading the DTH sensor----------------------------------------
unsigned long sensor_time = 0;
unsigned long sensor_delay = (6* segundo);

//fake sensor
int fakereading = 0; 
const byte pot = A0; 
int pastH = 0; 

//-------------------------------------Low Water sensor and alarms--------------
//-------------------------------------Display management-----------------------
//-------------------------------------HARDWARE PINS----------------------------
const byte fogger = 13; 
const byte foggerFan = 14; 


//Day Time Variables------------------------------------------------------------
//state machine variables for tipes of machines during the day 
	enum STMhumOptions {LowHumidity, HighHumidity, OkHumidity}; 
	     STMhumOptions STMhum  = OkHumidity; 
//state machine variables in low humidity conditions
enum STMlowHumidityOptions {minTimeBtwWateringElapsed, 
							startFogger,
							humReachLevel,
							timeHumReachLevel, 
			  				endWatering, backToSTMhum}; 
		
		STMlowHumidityOptions STMLOW = minTimeBtwWateringElapsed; 
//------------------Humidity levels---------------------------------------------
int dayMaxHum = 70; 
int dayMinHum = 40; 
//------------------Timers------------------------------------------------------
bool startMaxHTimmer = true; 
unsigned long dayTimeInMaxHumDelay    = (20*segundo);  //periodo de max humedad 
unsigned long dayTimeInMaxHumStartTime = 0; 

unsigned long dayTimeBtwWatering = (20*segundo); 
unsigned long dayTimeWateringEnd = 0; 				//the time wattering end 

//------------------For print just once on the state machine. 
int STMLOWpast = 0;
int cuSecWateringnEnd  = 0;
int pasSecWateringEnd  = 0; 
								

//Nighth Time Variables---------------------------------------------------------
//---------------------Humidity levels------------------------------------------
int ngtMaxHum = 50; 
int ngtMinHum = 20; 
//---------------------Timers---------------------------------------------------
unsigned long ngtTimmerInMaxHum    = (2*minuto); 
unsigned long ngtTimmerBtwWatwring = (10*minuto); 

//Wait Time Variables-----------------------------------------------------------
//---------------------Timers----------------------------------------------------





//===============================================================================//
//==================================VOID SETUP===================================//
//===============================================================================//
void setup() 
{
  	Serial.begin(9600); 
  	delay(50);
  	Serial.println("Ha iniciado el programa"); 
	
	pinMode(fogger,    INPUT_PULLUP);
	pinMode(foggerFan, INPUT_PULLUP);

	pinMode(fogger,    OUTPUT);
	pinMode(foggerFan, OUTPUT);

	digitalWrite(fogger,    HIGH);
	digitalWrite(foggerFan, HIGH);
}


//===============================================================================//
//==================================VOID LOOP====================================//
//===============================================================================//
void loop() 
{
	fakereading =  analogRead(pot); 
	H = map(fakereading, 0, 1023, 20, 100); 
		
		if( H != pastH )
		{
			Serial.print("Humedad de "); Serial.println(H);		
			pastH = H; 
		}

	growChamberInDayTime();
	delay(80); 

}

//===============================================================================//
//-----------------------------------FUNCTIONS-----------------------------------//
//-------------------------------------------------------------------------------//


void growChamberInDayTime()
{
	//In day time reac to the different scenarios
	if(STMhum == OkHumidity)
		{
			if(H < dayMinHum) STMhum = LowHumidity;  
			if(H > dayMaxHum) STMhum = HighHumidity; 
		}


	if(STMhum == LowHumidity)
	{
		if(STMLOW != STMLOWpast) //for print just once the state machine 
		{
			Serial.print("---->STM "); Serial.println(STMLOW); 
			STMLOWpast = STMLOW; 
		}

		switch(STMLOW) //state machien for watering the crops. 
		{
		//if the minumum time btw watterign  has elapsed, move foward
		case (minTimeBtwWateringElapsed):					
						
			//For print the timmer at a second resolution
				if(millis() - dayTimeWateringEnd < dayTimeBtwWatering)
				{
					cuSecWateringnEnd = (dayTimeBtwWatering - (millis() - dayTimeWateringEnd)) /1000;  

					if(cuSecWateringnEnd != pasSecWateringEnd)
						{
							Serial.println(cuSecWateringnEnd);	
							pasSecWateringEnd = cuSecWateringnEnd; 
						}
				}
				else {	STMLOW = startFogger; }
			break; 
				
		//start the fogger and fand and move foward
		case (startFogger):

			digitalWrite(fogger, LOW);
			digitalWrite(foggerFan, LOW);
			Serial.println("------Fogger ON");
			Serial.println("------Fogger Fan ON");
							
			STMLOW = humReachLevel; 
			break; 

		case (humReachLevel):
				if(H > dayMaxHum)  						//stop fogger 
				{
					digitalWrite(fogger, HIGH);
					digitalWrite(foggerFan, HIGH);
					Serial.println("Fogger OFF");
					Serial.println("Fogger OFF"); 
						
					if(startMaxHTimmer == true)			//start the max humidity timmer
						{
							dayTimeInMaxHumStartTime = millis(); 
							startMaxHTimmer = false; 
							STMLOW = timeHumReachLevel;
						}
				}
			break;

		//if the timmer of wattering has elapsed move foward
		case(timeHumReachLevel):
			  //if(millis() < dayTimeInMaxHumStartTime + dayTimeInMaxHumStartTime) //evit desbordaiento
				if(millis() - dayTimeInMaxHumStartTime < dayTimeInMaxHumDelay)
				{
					//while wattering dont allow the humidity 
					//to drop below this level
					if(H < dayMaxHum - 10)  //start fogger
						{
							digitalWrite(fogger, LOW);
							digitalWrite(foggerFan, LOW);
							Serial.println("Fogger OFF");
							Serial.println("Fogger Fan OFF"); 
						}
				}
				else
				{
					startMaxHTimmer = true; 	//restart function 
					STMLOW = endWatering; 		//changue the next step STM 	
				}
			break;
				
		//if the humidity ok and timmer elapsed move foward 
		case (endWatering): 
				//stop fogger
				digitalWrite(fogger, HIGH);
				digitalWrite(foggerFan, HIGH);
				STMLOW = backToSTMhum; 
				break;

		//reset variables and go back to sense humidity
		case (backToSTMhum): 
				//take time at the end of watering
				dayTimeWateringEnd = millis();
				Serial.println("Se ha salido de baja humedad"); 
				STMhum = OkHumidity; 
				STMLOW = minTimeBtwWateringElapsed; //restart the machine 
				break;

		}			
	}
}//--End of daytime machine 







void growCahmberInNigthTime()
{

}



void growChamberInWaitTime()
{

}




//FUCTION TO READ DTH------------------------------------------------------------
void readDTH11()
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

    //-----print data on serial 
    Serial.print("H = "); Serial.println(H); 
}
