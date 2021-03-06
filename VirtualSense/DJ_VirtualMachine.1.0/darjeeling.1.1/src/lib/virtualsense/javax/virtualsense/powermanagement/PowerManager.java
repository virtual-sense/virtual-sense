/**
 *	PowerManager.java
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
package javax.virtualsense.powermanagement;

/**
 * Controls power manager of the board 
 * based on modified contiki operating system.
 * 
 * @author Emanuele Lattanzi
 *
 */
public class PowerManager
{
	//public static final short MCU_1MHZ 		= 1; //Not running
	public static final short MCU_4MHZ 			= 4;
	public static final short MCU_8MHZ 			= 8;
	public static final short MCU_12MHZ 		= 12;
	public static final short MCU_16MHZ 		= 16;
	public static final short MCU_20MHZ 		= 20;
	public static final short MCU_25MHZ 		= 25;
	
	/*public static native int getBatteryVoltage();
	public static native int getSolarVoltage();
	public static native int getSolarCurrent(); */
	
	/**
	 * Controls the system clock by programming it.
	 * During interrupt period the MCU remains in LPM3.
	 * In LPM3 approximatively the MCU will consume 2.1 uA.
	 * @param millis number of millisecond between a clock interrupt
	 * 
	 **/
	public static native void setSystemClockMillis(int millis);
	
	
	/**
	 * Controls the system clock by programming it.
	 * During interrupt period the MCU remains in LPM3.
	 * In LPM3 approximatively the MCU will consume 2.1 uA.
	 * @param factor interrupt period multiplier factor
	 * 
	 **/
    public static native void slowDownClockByFactor(int factor);
    

    /**
	 * Puts the MCU in standby mode (LPM3). 
	 * In LPM3 approximatively the MCU will consume 2.1 uA.  
	 * Wakeup from LPM3 is possible through all enabled interrupts.
	 **/
    public static native void standby();
    
    
    /**
	 * Puts the MCU in off mode (LPM4). 
	 * In LPM4 approximatively the MCU will consume 1.3 uA.  
	 * Wakeup from LPM4 is possible through all enabled interrupts.
	 **/
    public static native void deepSleep();
    
    
    /**
	 * Hibernates the system by writing machine state on the 
	 * non-volatile memory. The MCU will be putted on the LPM4.5 state.
	 * Approximatively the MCU will consume 0.1 uA. 
	 * The MCU can be woken up by un interrupt over P1 or P2 
	 * port. When woken up the system will reboot an the exectuion 
	 * will check the ibernated state and resume it if found.
	 * If an ibernation state is found the execution of the VM will restart
	 * from the subsequent statement. Otherwise the execution 
	 * will restart from main 
	 **/
    public static native void systemHibernation();
    
    
    /**
     * 
     */
    public static native void scheduleRTCInterruptAfter(int minutes);
    
    
    /**
     * Hibernates the system for the specified time.  
	 * The MCU can be woken up by un interrupt over P1 or P2 port. 
     */
    public static void systemHibernation(int minutes){
    	scheduleRTCInterruptAfter(minutes);
    	systemHibernation();
    }
    
    
    /**
     * Puts the MCU in off mode (LPM4) for the specified time. 
     * MCU can wake up from LPM4 through all enabled interrupts or at the end of specified time.
     */
    public static void deepSleep(int minutes){
    	scheduleRTCInterruptAfter(minutes);
    	deepSleep();
    }
    
    
    /**
     * Sect the execution frequency of MCU.
     */
    public static native void setMCUFrequency(short freq);
}
