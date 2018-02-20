################################################################################
# Analog Read to Voltage
#
# Created by Zerynth Team 2015 CC
# Authors: G. Baldi, D. Mazzei
################################################################################

import streams
import adc      # import the adc module  

#create a serial port stream with default parameters
streams.serial()

#define the analog pin for reading the value 
inputAnalog = A0

#set the pin as input with INPUT_ANALOG,
pinMode(inputAnalog,INPUT_ANALOG)

while True:
       
    #read the input on analog pin 0
    sensorValue = adc.read(inputAnalog)
    
    #convert the analog reading (which goes from 0 - 4095.0) to a voltage (0 - 3.3V):
    voltage = sensorValue * (3.3 / 4095.0)
    
    #print out the raw and converted values:
    print("sensor raw value:", sensorValue,"Voltage:",voltage)
    
    sleep(300)


