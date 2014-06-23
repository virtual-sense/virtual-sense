/*
 *	CO2Reader.java
 * 
 *	Copyright (c) 2011 DiSBeF, University of Urbino.
 * 
 *	This file is part of VirtualSense.
 * 
 *	VirtualSense is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	VirtualSense is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 * 
 *	You should have received a copy of the GNU General Public License
 *	along with VirtualSense.  If not, see <http://www.gnu.org/licenses/>.
 */

import javax.virtualsense.network.*;
import javax.virtualsense.network.protocols.minpath.*;
import javax.virtualsense.sensors.*;
import javax.virtualsense.network.Packet;
import javax.virtualsense.actuators.Leds;
import javax.virtualsense.sensors.Temperature;
import javax.virtualsense.sensors.Pressure;
import javax.virtualsense.sensors.Light;
import javax.virtualsense.powermanagement.PowerManager;
import javax.virtualsense.VirtualSense;
import javax.virtualsense.digitalio.DigitalPin;
import javax.virtualsense.digitalio.bus.*;



/**
 * Collegio multi user C02Reader app
 * 
 * @author Emanuele Lattanzi
 *
 */
public class TempReader
{
	static short nodeId = -1;
	     
    public static void motemain() {   		
        boolean state = true;
        short index = 0;
        short value = 0;
        char label[] = {'-','t','e','m','p'};  //temp
        char label2[] = {'p','r','e','s','s'};  //press  
        
        nodeId = VirtualSense.getNodeId();
        Network myNetwork = new Network(Protocol.MINPATH);
        
        Leds.setLed(0, false); 
        Leds.setLed(1, false); 
        Leds.setLed(2, false);
        
        while(true) { 
        	System.out.print("TEMP - nodeId: ");
    		System.out.println(nodeId);
    		
    		DataMsg data = new DataMsg(nodeId, index++);
    		
    		
    		while(value <= 0 || value >= 9999){
				value = Temperature.getValue();
				System.out.print("Rt");
			}
			
    		data.value = value;
    		data.label = label;
    		System.out.print("TEMP - temp: ");
	   		System.out.println(data.value);
	   		
	   		Leds.setLed(0, state);        		
    		myNetwork.send(data);
    		VirtualSense.printTime();
            System.out.println("TEMP - packet sent");    		
    		
            
	   		data = new DataMsg(nodeId, index);
	   		value = 0;
	   		
	   		while((value <= 700 || value >= 1200)){ // For reduce reading error of barometer
				value = Pressure.getValue();
				System.out.print("Rp");
			}
    		
	   		data.value = value;
	   		data.label = label2;
	   		
	   		System.out.print("pressue: ");
	   		System.out.println(data.value);
	   		
	   		Leds.setLed(0, state);        		
    		myNetwork.send(data);
    		VirtualSense.printTime();
            System.out.println("PRESS - packet sent");    		
    		
    		
	   		state =! state;
    		
    		// Sleep period
    		Thread.sleep(35000);
			Thread.sleep(35000);
				
    	}
    } 
    
}
    
    

