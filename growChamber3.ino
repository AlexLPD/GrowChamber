//germinadoraV3.ino

//time definitions------------------------------------------
#define segundo	1000
#define minuto	(60*segundo)
#define hora	(60*minuto)

void setup() {

}

void loop() {

}


void watering()
{
unsigned long mtbWatering = 0; 
unsigned long mtbWateringDelay = (1*minuto); 

unsigned long maxHumTimmer = 0; 
unsigned long maxHumDelay = (10* minuto); 

float H = 0; //for the humidity 
float minH = 20; 
float maxH = 30; 

typedef enum{	watSTM1, //start the watering machine  
				watSTM2, //see if the minum timmer between waterings has elapsed 
				watSTM3, //see if the current H is below minH 
				watSTM4, //turn on the fogger and the fan 
				watSTM5, //we reach max hum? yes next step, no wait here
				watSTM6, //stop the fogger and the fan 
				watSTM7, //start max humidity timmer 
				watSTM8, //has the max watering time elapsed? no next step, yes save timme for min wat time / got to watSTM11
 				watSTM9, //is the H < the minHinWatering?  yes, start the fogger, no wait next step
 				watSTM10,//is the H > maxHum ? yes stop the fogger and the fan, no back to watSTM8
 				watSTM11,//exit watering machine end fuction. 
 			}	watSTM;  /

	switch (watSTM) 
	{
	    case watSTM1:
	      	//min time between waterings elapsed
			if(millis() - mtbWatering > mtbWateringDelay) watSTM = watSTM2; 
	      	else watSTM = watSTM1; 
	    	break;
	    
	    case watSTM2:
	    	//is the humidity below Hmin level?   
	      	if( H < Hmin) watSTM = watSTM3; 
	      	else watSTM = watSTM2; 
	    	break;
	    
	    case watSTM3: 
	    	//turn on the fogger and the fan and move on 
	    	// fogger on 
	    	// fan On 
	    	watSTM = watSTM4; 
	    	break; 
	    
	    case watSTM4: 
	    	//we reach max humidiy? no, wait here yes next step
	    	if(H > maxHum) watSTM = watSTM5; 
	    	else watSTM = watSTM4; 
	    break; 
	    
	    case watSTM5: 
	    	//fogger off 
	    	//fan off
	    	watSTM = watSTM6; 
	    break; 
		
		case watSTM6:
			//start the max humidity timmer
			//poner una guarda booleana aca 
			maxHumTimmer = millis();
			watSTM = watSTM7; 
			break; 

		case watSTM7:
			//has the maximun watering time elapsed? 
			if(millis() - maxHumTimmer > maxHumDelay)
				{	
					//no  watSTM = watSTM8;  
					mtbWatering = millis(); //save the time for minimun time between waterings
					watSTM = watSTM__; //if the timmer ends move to the state no. 
				}
				else
				{
					watSTM = watSTM8;
				}

			break; 
		
		case watSTM8:
			if( H < minHinWatering) 
				{
					//turn on the fogger
				}
				else watSTM = watSTM9; 
			break;
		
		case watSTM9:
			if( H > maxH)
				{
					//turn off the fogger 
				}
				else 
					{

					}
			break; 
		
		case watSTM10:
			break;
		
		case watSTM11:
			break;
	}



	{
		if( H < minH)
		{
			//turn on the fogger
			//trun on the fan 

		}

	}






}
