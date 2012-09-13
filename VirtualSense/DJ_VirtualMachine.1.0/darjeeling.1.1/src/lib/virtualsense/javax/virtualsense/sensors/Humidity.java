/*
 *	Temperature.java
 * 
 *  Copyright (c) 2011 DiSBeF, University of Urbino.
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
package javax.virtualsense.sensors ;

/**
 * Reads humidity from the sensor.
 * 
 * @author Emanuele Lattanzi
 *
 */
public class Humidity
{
	private static final short HUMIDITY_CHANNEL = 2;
   
	
	/**
	 * Reads humidity value from the HIH5030-31 sensor.	  
	 * @return measured humidity.
	 */
	public static int getValue(){
		int hum = 0;
		short vValue = ADC.read(HUMIDITY_CHANNEL);
		hum = (((vValue *1000) / 30) -15150 )/636;
		return hum;
	}
}