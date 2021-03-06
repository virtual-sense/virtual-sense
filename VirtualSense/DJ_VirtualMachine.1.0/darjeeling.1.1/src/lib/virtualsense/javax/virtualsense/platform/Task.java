/*
 *	Task.java
 * 
 *      Copyright (c) 2013 DiSBeF, University of Urbino.
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
package javax.virtualsense.platform;

import javax.darjeeling.vm.Infusion;
import javax.darjeeling.vm.InfusionUnloadDependencyException;

/**
 *
 * @author Lattanzi
 */
public class Task {
	protected short executionContextID;
	protected short infusionID;
	protected boolean loaded;
	protected boolean running;
	
	// creates a new Thread using the task and returns the thread id
	private static native short _loadExecutionContext(short executionContextID);
	
	// return the number of default task found in the task table
	//TODO: the name default should be changed these are tasks saved in the table
	// they are not always "default"
	private static native short _getDefaultTasksNumber();
	
	private static native short _getDefaultExecutionContextID(short index);
	
	private static native boolean _defaultNeedToLoad(short index);
	
	private static native boolean _defaultNeedToStart(short index);
	
	// start the Task , calls the 'motemain' method on the Task
	private static native void _start(short infusionID, short executionContextID);
	
	private static native void _stop(short infusionID);
	//private static native void _sendInfo(short infusionID, boolean loaded, boolean running);
	
	
	
	protected Task(short executionContextID){
		this.executionContextID = executionContextID;
		this.infusionID = _loadExecutionContext(executionContextID);
		this.running = false;
		this.loaded = true;
	}
	
	protected static short getDefaultTasksNumber(){
		return _getDefaultTasksNumber();
	}
	
	protected static short getDefaultExecutionContextID(short index){
		return _getDefaultExecutionContextID( index);
	}
	
	protected static boolean defaultNeedToLoad(short index){
		return _defaultNeedToLoad(index);
	}
	
	protected static boolean defaultNeedToStart(short index){
		return _defaultNeedToStart(index);		
	}
	
    protected void startExecution(){ 
    	// creates thread and runs it
    	_start(this.infusionID, this.executionContextID);
    	this.running = true;
    }
    protected void stopExecution(){
    	//kill all threads belonging to the infusion
    	_stop(this.infusionID);  
    	this.running = false;
    }  
    protected void unload(){
    	// kill threads and unload the infusion
    	Infusion toRemove = Infusion.getInfusion(this.infusionID);
    	try{
    		toRemove.unload();
    		this.loaded = false;
    		this.infusionID = -1;
    		this.executionContextID = -1;
    	}catch(InfusionUnloadDependencyException ex){
    		System.out.println("Problem unloading infusion task");
    	}

    }    
}
