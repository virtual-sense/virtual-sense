/*
 *	Accelerometer.java
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
 package javax.virtualsense.sensors.evm.accelerometer ;


import javax.virtualsense.digitalio.bus.*;

/**
 * Driver for FXLS8471Q 3-Axis accelerometer sensor of evm.
 * 
 * @author Matteo Dromedari
 *
 */
public class Accelerometer
{	
	// FXLS8471Q I2C address
	private static final byte FXLS8471Q_ADR = 0x1E; 	// with pins SA0=0, SA1=0
	
	// FXLS8471Q internal register addresses
	private static final byte FXLS8471Q_STATUS = 0x00;
	private static final byte FXLS8471Q_WHOAMI = 0x0D;
	private static final byte FXLS8471Q_XYZ_DATA_CFG = 0x0E;
	private static final byte FXLS8471Q_CTRL_REG1 = 0x2A;
	private static final byte FXLS8471Q_WHOAMI_VAL = 0x6A;

	// number of bytes to be read from FXLS8471Q
	private static final int FXLS8471Q_READ_LEN = 7;	// status plus 3 accelerometer channels
	
	
	
	
	public Accelerometer() {
		
		I2C.enable();
		// read and check the FXLS8471Q WHOAMI register
		byte read[] = I2C.readBurst(FXLS8471Q_ADR, FXLS8471Q_WHOAMI, (short)1);
		
		if(read[0] == FXLS8471Q_WHOAMI_VAL) {
			System.out.print("whoami done val: ");
			
		}
		// write 0000 0000 = 0x00 to accelerometer control register 1 to place FXLS8471Q into
		// standby
		// [7-1] = 0000 000
		// [0]: active=0
		byte write2[] = {(byte)0x00};
		if(I2C.writeBurst(FXLS8471Q_ADR, FXLS8471Q_CTRL_REG1, write2)) {
			System.out.println("sensor in active");
		}		
		// write 0000 0001= 0x01 to XYZ_DATA_CFG register
		// [7]: reserved
		// [6]: reserved
		// [5]: reserved
		// [4]: hpf_out=0
		// [3]: reserved
		// [2]: reserved
		// [1-0]: fs=01 for accelerometer range of +/-4g with 0.488mg/LSB
		byte write3[] = {(byte)0x01};
		if(I2C.writeBurst(FXLS8471Q_ADR, FXLS8471Q_XYZ_DATA_CFG, write3)) {
			System.out.println("cfg OK");
		}
		// write 0001 0101b = 0x15 to accelerometer control register 1
		// [7-6]: aslp_rate=00
		// [5-3]: dr=010 for 200Hz data rate
		// [2]: lnoise=1 for low noise mode
		// [1]: f_read=0 for normal 16 bit reads
		// [0]: active=1 to take the part out of standby and enable sampling
		byte write4[] = {(byte)0x15};
		if(I2C.writeBurst(FXLS8471Q_ADR, FXLS8471Q_CTRL_REG1, write3)) {
			System.out.println("start 16bit read OK");
		}		
	}
	
	/**
	 * Reads 3 channel accelerometer data from FXLS8471Q sensor.	 
	 * @return 3 axis acceleration vector.
	 */
	public AccVector getValue() {
		
		AccVector vec = new AccVector();
		// read status and the three channels of accelerometer data from
		// FXLS8471Q (7 bytes)	
		byte buffer[] = I2C.readBurst(FXLS8471Q_ADR, FXLS8471Q_STATUS, (short)FXLS8471Q_READ_LEN);
		
		if(buffer.length == FXLS8471Q_READ_LEN) {
			// copy the 14 bit accelerometer byte data into 16 bit words
			
			
			
			int tmp = buffer[1] << 24;
			tmp = tmp | (0x00FF0000 & (buffer[2] << 16));
			vec.x = (short)(tmp >> 18);
		
			
			tmp = buffer[3] << 24;
			tmp = tmp | (0x00FF0000 & (buffer[4] << 16));
			vec.y = (short)(tmp >> 18);
			
			
			tmp = buffer[5] << 24;			
			tmp = tmp | (0x00FF0000 & (buffer[6] << 16));			
			vec.z = (short)(tmp >> 18);
			
		}
		else
			vec = null;
		
		return vec;
	}
	
	/*public void printByte(byte b){
		int mask = 0b10000000;
		for(int i = 0; i < 8; i++){
			if((b & mask)==mask)
				System.out.print("1");
			else
				System.out.print("0");
			mask = (byte)(mask >>(byte)1);
				
		}
		System.out.println("");
	}*/
	
	/*public void printShort(short b){
		int mask = 0b1000000000000000;
		System.out.println("");
		for(int i = 0; i < 16; i++){
			if((b & mask)==mask)
				System.out.print("1");
			else
				System.out.print("0");
			mask = (short)(mask >>(short)1);
				
		}
		System.out.println("");
	}*/
	
	/*public void printInt(int b){
		int mask = 0b10000000000000000000000000000000;
		System.out.println("");
		for(int i = 0; i < 32; i++){
			if((b & mask)==mask)
				System.out.print("1");
			else
				System.out.print("0");
			mask = (int)(mask >>(int)1);			
				
		}
		System.out.println("");
	}*/
	/*public void printInt2(int b){
		int mask = 0b1000000000000000;
		int copyB = b;
		b = b >> 16;
		System.out.println("");
		for(int i = 0; i < 16; i++){
			if((b & mask)==mask)
				System.out.print("1");
			else
				System.out.print("0");
			mask = (short)(mask >>(short)1);
				
		}
		mask = 0b1000000000000000;
		b = copyB;
		System.out.print(" ");
		for(int i = 0; i < 16; i++){
			if((b & mask)==mask)
				System.out.print("1");
			else
				System.out.print("0");
			mask = (short)(mask >>(short)1);
				
		}
		System.out.println("");
	}*/
	
}
