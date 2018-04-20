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
float minHInWatering = 29.5;  

 typedef enum {	watSM1, //see if the minum timmer between waterings has elapsed 
				watSM2, //see if the current H is below minH 
				watSM3, //turn on the fogger and the fan 
				watSM4, //we reach max hum? yes next step, no wait here
				watSM5, //stop the fogger and the fan 
				watSM6, //start max humidity timmer 
				watSM7, 
				watSM8, //has the max watering time elapsed? no next step, yes save timme for min wat time / got to watSM11
 				watSM9, //
 				watSM10,//save time for min time between watering and moce foward 
 				watSM11,//turn off fogger and fan and move foward.
 				watSM12,//exit from watering machine on main loop 
 			}	watSMOp;  //

watSMOp watSM  = watSM1; 

	switch (watSM) //watering State Machine 
	{
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
