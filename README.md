# GrowChamber
Program to control a growing chamber 
This program will control a growing chamber in all its factors. 
Also will datalog all the activities on a micro SD card. 

Turn on and off the fogger unit
turn on and off the fan unit 
turn on and off the interior led

The growing chamber program: 
Reads the interior huidity using a DTH11 and average it using a libray. 
Select day time/ nigth time or wait time, the wait time will be used only in a scenario of open door. 

Monitor the water level in the fogger unit and has an a bult in alarm fuction.
In a second stage of develment, will have a secondary water reservoir to pum watter to 
the fogger unit until reach a proper level, allowing more independece and comfort. 

The watterign time; 
For the wattering time we will use this factors: 
//inster graph here 

This is the watering machine, we will be using; 

GerminadoraV3hardware.ino;

Cointains all the data and pin, coneection necessary for the module to work properly 
LCD 20x04 in cluided and working
SD ADAPOR included
RTC included 

Function to; Print once at the the power up, the header of the data; including the date and time, columns names.
Function to; Log data on  sd; the date, time, hum, temperature 
Function to; average the DTH measurement data
Function to; Display the changue of the T and humity on display 

Will add to the waterring machine code and need to be added to the sd file saving;  
The state of the system machine
The state of the subsystem machine 

In the future, will need to add: ligth sensor to read the ligth sensor readint o the sd file. 

