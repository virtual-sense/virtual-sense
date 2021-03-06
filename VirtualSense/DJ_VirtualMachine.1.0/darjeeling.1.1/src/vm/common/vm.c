/*
 *	vm.c
 *
 *	Copyright (c) 2008 CSIRO, Delft University of Technology.
 *
 *	This file is part of Darjeeling.
 *
 *	Darjeeling is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	Darjeeling is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with Darjeeling.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "common/vm.h"
#include "common/heap/heap.h"
#include "common/global_id.h"
#include "common/execution/execution.h"
#include "common/util.h"
#include "common/infusion.h"
#include "common/debug.h"
#include "common/panic.h"

//#define DEBUG_LOG(...) printf(__VA_ARGS__)
/**
 *
 *
 * @author Niels Brouwers
 */
// TODO WTF!!
int32_t dj_timer_getTimeMillis();
#define MAX_SCHEDULE_TIME 2147483647L;

/**
 * Constructs a new virtual machine context.
 * @return a newly constructed virtual machine instance or NULL if fail (out of memory)
 */
dj_vm *dj_vm_create()
{
	dj_vm *ret = (dj_vm*)dj_mem_alloc(sizeof(dj_vm), CHUNKID_VM);
    if(ret == NULL) return NULL;

	// initialise linked lists to 0 elements
	ret->infusions = NULL;
	ret->threads = NULL;

	// no threads running
	ret->currentThread = NULL;
	ret->threadNr = 0;

	// no monitors
	ret->monitors = NULL;
	ret->numMonitors = 0;

	// no system infusion loaded
	ret->systemInfusion = NULL;

	return ret;
}

/**
 * Loads the previously hibernated virtual machine context.
 * The VM is always allocated as the first object
 * @return the loaded virtual machine instance or NULL if fail (VM not found)
 */
dj_vm *dj_vm_load_from_heap(char *heapPointer)
{
	return (void*)heapPointer + sizeof(heap_chunk);
}


/**
 * Destroys a virtual machine instance
 *
 */
void dj_vm_destroy(dj_vm *vm)
{
	dj_mem_free(vm);
}

/**
 * Adds an infusion to a virtual machine.
 * @param vm the virtual machine to add the infusion to
 * @param infusion the infusion to add
 */
void dj_vm_addInfusion(dj_vm *vm, dj_infusion *infusion)
{
	dj_infusion *tail = vm->infusions;
	if (tail!=NULL)
		while (tail->next!=NULL)
			tail = tail->next;

	if (tail==NULL)
	{
		// list is empty, add as first element
		vm->infusions = infusion;
		infusion->class_base = CHUNKID_JAVA_START;
	}
	else
	{
		// add to the end of the list
		tail->next = infusion;
		infusion->class_base = tail->class_base + dj_di_parentElement_getListSize(tail->classList);
	}

	// the new infusion is the last element,
	// so its next should be NULL
	infusion->next = NULL;


}

/**
 * Counts the number of loaded infusions in a virtual  machine.
 * @param vm the virtual machine to count the infusions of
 * @return the number of loaded infusions
 */
int dj_vm_countInfusions(dj_vm *vm)
{
	int ret = 0;
	dj_infusion *tail = vm->infusions;

	while (tail!=NULL)
	{
		tail = tail->next;
		ret++;
	}

	return ret;

}

uint8_t dj_vm_getInfusionId(dj_vm * vm, dj_infusion * infusion)
{
	int count = 0;
	dj_infusion *tail = vm->infusions;

	while (tail != NULL)
	{
		if (tail == infusion)
			return count;
		tail = tail->next;
		count++;
	}

	return 0;
}

/**
 * Gets an infusion by index.
 * @param vm the virtual machine to get a loaded infusion from
 * @param index the index of the infusion
 * @return the requested infusion, or NULL if index out of range.
 */
dj_infusion * dj_vm_getInfusion(dj_vm *vm, int index)
{
	int count = 0;
	dj_infusion *tail = vm->infusions;

	while (tail!=NULL)
	{
		if (index==count)
			return tail;
		tail = tail->next;
		count++;
	}

	return NULL;
}

/**
 * Gets a loaded infusion for a given infusion name.
 * @param vm the virtual machine to lookup the infusion in
 * @param name the name of the infusion. Has to be a NULL-terminated ascii string in program memory.
 * @return the infusion for the given name, or NULL if no such infusion is found
 */
dj_infusion *dj_vm_lookupInfusion(dj_vm *vm, dj_di_pointer name)
{
	dj_infusion *finger = vm->infusions;

	while (finger!=NULL)
	{
		if (dj_str_equals(dj_di_header_getInfusionName(finger->header), name)) return finger;
		finger = finger->next;
	}
	return NULL;
}

/**
 * Loads an infusion into the virtual machine.
 * @param vm the virtual machine object to load the infusion into
 * @param di a di pointer to the infusion file in program space
 * @return a newly loaded infusion, or NULL in case of fail
 */
dj_infusion *dj_vm_loadInfusion(dj_vm *vm, dj_di_pointer di)
{
	int i;
	dj_infusion *ret;
	dj_di_pointer element;
	dj_di_pointer staticFieldInfo = DJ_DI_NOT_SET;
	dj_di_pointer infusionList = DJ_DI_NOT_SET;

	// iterate over the child elements, and find the static
	// field size info block. We need this info to allocate
	// the memory to hold the static fields for this
	// infusion
	for (i=0; i<dj_di_getListSize(di); i++)
	{
		element = dj_di_getListElement(di, i);
		switch (dj_di_element_getId(element))
		{
		case STATICFIELDINFO:
			staticFieldInfo = element;
			break;
		case INFUSIONLIST:
			infusionList = element;
			break;
		}
	}

	// Check if each of the required elements were found
	if (staticFieldInfo==DJ_DI_NOT_SET||infusionList==DJ_DI_NOT_SET)
		dj_panic(DJ_PANIC_MALFORMED_INFUSION);

	// allocate the Infusion struct
	ret = dj_infusion_create(staticFieldInfo, dj_di_infusionList_getSize(infusionList));

	// if we're out of memory, let the caller handle it
	if (ret==NULL)	return NULL;

	// iterate over the child elements and get references
	// to the class list and method implementation list,
	// and get the header

	for (i=0; i<dj_di_getListSize(di); i++)
	{
		element = dj_di_getListElement(di, i);
		switch (dj_di_element_getId(element))
		{
		case HEADER:
			ret->header = element;
			break;
		case CLASSLIST:
			ret->classList = element;
			break;
		case METHODIMPLLIST:
			ret->methodImplementationList = element;
			break;
		case STRINGTABLE:
			ret->stringTable = element;
			break;
		}

	}

	// Check if each of the required elements was found
	if (ret->stringTable==DJ_DI_NOT_SET||ret->classList==DJ_DI_NOT_SET||ret->methodImplementationList==DJ_DI_NOT_SET||ret->header==DJ_DI_NOT_SET)
		dj_panic(DJ_PANIC_MALFORMED_INFUSION);

	// iterate over the referenced infusion list and set the appropriate pointers
	for (i=0; i<dj_di_infusionList_getSize(infusionList); i++)
	{
		dj_di_pointer name = dj_di_infusionList_getChild(infusionList, i);
		dj_infusion *infusion = dj_vm_lookupInfusion(vm, name);

		if (infusion==NULL)
			dj_panic(DJ_PANIC_UNSATISFIED_LINK);

		ret->referencedInfusions[i] = infusion;
	}

	// add the new infusion to the VM
	dj_vm_addInfusion(vm, ret);

	return ret;
}


/**
 * Loads an infusion into the virtual machine, and marks it as the system infusion. Always load the
 * system infusion before anything else.
 * @param vm the virtual machine object to load the infusion into
 * @param di a di pointer to the infusion file in program space
 * @return a newly loaded infusion, or NULL in case of fail
 */
dj_infusion *dj_vm_loadSystemInfusion(dj_vm *vm, dj_di_pointer di)
{
	dj_infusion *ret = dj_vm_loadInfusion(vm, di);

	// if we run out of memory, let the caller handle it
	if (ret==NULL) return NULL;

	vm->systemInfusion = ret;
	return ret;
}

/**
 * Checks wheter it's safe to unload an infusion. Unloading an infusion is unsafe if
 * another loaded infusion imports it. If you want to unload the infusion you *must*
 * unload the infusions that depend on it first, or leave the VM in an undefined state.
 * @param vm the virtual machine context
 * @param unloadInfusion the infusion to unload
 */
bool dj_vm_safeToUnload(dj_vm *vm, dj_infusion * unloadInfusion)
{
	dj_infusion * infusion;

	printf("The infusion to unload is %d\n", dj_vm_getInfusionId(dj_exec_getVM(), unloadInfusion));
	infusion = vm->infusions;
	while (infusion!=NULL)
	{

		// dj_infusion_getReferencedInfusionIndex returns 0 is the unloadInfusion is equal to
		// infusion, a positive number if infusion imports unloadInfusion, and -1 otherwise.
		// Therefore this check is for a positive number, meaning that infusion imports
		// unloadInfusion, making it unsafe to unload that infusion.
		if (dj_infusion_getReferencedInfusionIndex(infusion, unloadInfusion)>0)
			return false;

		infusion = infusion->next;
	}

	return true;
}

/**
 * Unloads an infusion. Make sure you check whether it's safe to do so using dj_vm_safeToUnload! If you
 * unload an infusion that another infusion depends on you'll leave the VM in an undefined state. Also,
 * a kitten will die.
 * @param vm the virtual machine context
 * @param unloadInfusion the infusion to unload
 */
void dj_vm_unloadInfusion(dj_vm *vm, dj_infusion * unloadInfusion)
{
	dj_thread * thread;
	dj_frame * frame;
	dj_infusion * infusion;
	dj_infusion * prev;
	int index;

	// kill any thread that is currently executing any method in the infusion that we're unloading
	thread = vm->threads;
	while (thread!=NULL)
	{

		// check each frame
		frame = thread->frameStack;
		while (frame!=NULL)
		{
			if (frame->method.infusion==unloadInfusion)
			{
				// kill the thread
				thread->status = THREADSTATUS_FINISHED;

				break;
			}
			frame = frame->parent;
		}

		// next thread
		thread = thread->next;
	}

	// shift runtime IDs
	dj_mem_shiftRuntimeIDs(unloadInfusion->class_base, dj_di_parentElement_getListSize(unloadInfusion->classList));

	// update other infusions
	infusion = vm->infusions;
	while (infusion!=NULL)
	{
		if (infusion->class_base>unloadInfusion->class_base)
			infusion->class_base -= dj_di_parentElement_getListSize(unloadInfusion->classList);

		infusion = infusion->next;
	}

	prev = NULL;

	index = dj_vm_getInfusionId(vm, unloadInfusion);

	// remove the infusion from the list
	if (index==0)
	{
		vm->infusions = unloadInfusion->next;
	} else
	{
		prev = dj_vm_getInfusion(vm, index - 1);
		prev->next = unloadInfusion->next;
	}

}

/**
 * Gets the runtime class id for a class in the system infusion.
 * TODO: shouldn't this just return entity_id, since the system infusion is always the first loaded?
 * @param vm the virtual machine context
 * @param entity_id the entity_id of the class in question
 * @return the runtime Id of the given class
 */
uint8_t dj_vm_getSysLibClassRuntimeId(dj_vm *vm, uint8_t entity_id)
{
	dj_global_id class_id = (dj_global_id){vm->systemInfusion, entity_id};
	return dj_global_id_getRuntimeClassId(class_id);
}

/**
 * Creates an instance of a class in the system infusion.
 * @param vm the virtual machine context
 * @param entity_id the entity_id of the class to instantiate
 * @return a new class instance or NULL if fail (out of memory)
 */
dj_object * dj_vm_createSysLibObject(dj_vm *vm, uint8_t entity_id)
{
	dj_global_id class_id = (dj_global_id){vm->systemInfusion, entity_id};
	uint8_t runtime_id = dj_global_id_getRuntimeClassId(class_id);
	dj_di_pointer classDef = dj_vm_getRuntimeClassDefinition(vm, runtime_id);
	return dj_object_create(runtime_id,
			dj_di_classDefinition_getNrRefs(classDef),
			dj_di_classDefinition_getOffsetOfFirstReference(classDef)
			);
}

/**
 * Adds a thread to a virtual machine.
 * @param vm the virtual machine context
 * @param thread the thread to add to the vm
 */
void dj_vm_addThread(dj_vm *vm, dj_thread *thread)
{
	dj_thread *tail = vm->threads;
	if(thread->id == -1) // is a thread initializer
	{
		vm->threads = thread;
		thread->next = tail;
	}else {
		while ((tail!=NULL)&&(tail->next!=NULL))
			tail = tail->next;

		if (tail==NULL)
			// list is empty, add as first element
			vm->threads = thread;
		else
		{
			// add to the end of the list
			thread->id = tail->id + 1;
			tail->next = thread;
		}
		// the new infusion is the last element,
		// so its next should be NULL
		thread->next = NULL;
	}


}

/**
 * Removes a thread from a virtual machine.
 * @param vm the virtual machine context
 * @param thread the thread to add to the vm
 */
void dj_vm_removeThread(dj_vm *vm, dj_thread *thread)
{

	if (vm->threads==thread)
	{
		vm->threads = thread->next;
	} else
	{
		dj_thread *pre = vm->threads;
		while ( (pre!=NULL) && (pre->next!=thread) )
			pre = pre->next;

		pre->next = thread->next;
	}

}

/**
 * Counts the threads in a virtual machine.
 * @param vm the virtual machine context
 * @return the number of threads in the virtual machine
 */
int dj_vm_countThreads(dj_vm *vm)
{
	int ret = 0;
	dj_thread *tail = vm->threads;

	while (tail!=NULL)
	{
		tail = tail->next;
		ret++;
	}

	return ret;
}

/**
 * Wakes threads that are sleeping or blocked.
 * @param vm the virtual machine context
 */
long dj_vm_wakeThreads(dj_vm *vm)
{
	long nextScheduleTime = MAX_SCHEDULE_TIME;
	long ttmp = 0;
	dj_thread *thread = vm->threads;
	dj_monitor * monitor;

	long time = dj_timer_getTimeMillis();

	thread = vm->threads;
	while (thread!=NULL)
	{	
		long t = thread->scheduleTime;
		/* LELE: per determinare il prossimo schedule time al fine
		 * di mettere in sleep il processo cos� che contiki possa
		 * mettere la cpu in LPM.
		 * Se un thread ha time schedule  == 0 � in attesa di monitor
		 * che pu� essere rilasciato solo da un thread con scheduleTime > 0 ???
		 */
		if (thread->status==THREADSTATUS_SLEEPING) //LELE: in this way we dont take into account blocked threads which can have a prior old schedule time
			if((t < nextScheduleTime) &&( t > 0)){
				nextScheduleTime = t;
				//printf("new schedule time %ld at %ld from thread %u in state %u\n", nextScheduleTime, time, thread->id, thread->status);
			}

		// wake sleeping threads
		if (thread->status==THREADSTATUS_SLEEPING){

			if (thread->scheduleTime <= time)
				thread->status=THREADSTATUS_RUNNING;
		}

		// wake waiting threads that timed out
		if (thread->status==THREADSTATUS_WAITING_FOR_MONITOR)
			if ((thread->scheduleTime!=0)&&(thread->scheduleTime <= time))
			{
				thread->status=THREADSTATUS_RUNNING;
				thread->monitorObject=NULL;
			}

		// wake blocked threads
		if (thread->status==THREADSTATUS_BLOCKED_FOR_MONITOR)
		{
			monitor = dj_vm_getMonitor(vm, thread->monitorObject);
			if (monitor->count==0)
			{
				monitor->waiting_threads--;

				monitor->count = 1;
				monitor->owner = thread;

				thread->status = THREADSTATUS_RUNNING;
			}

		}

		thread = thread->next;
	}
	ttmp = MAX_SCHEDULE_TIME;
	//printf("Calculated nextScheduleTime %ld\n", nextScheduleTime);
	/*if(nextScheduleTime >= ttmp) // for  thread blocked on monitor (time == MAX_SCHEDULE_TIME)
		nextScheduleTime = 0;*/
	return nextScheduleTime;

}

/**
 * Notifes one or more threads waiting for a lock.
 * @param vm the virtual machine context
 * @param object the lock object
 * @param all whether or not to wake all threads waiting for this lock, or just one
 */
void dj_vm_notify(dj_vm *vm, dj_object *object, bool all)
{
	dj_thread *thread = vm->threads;
	dj_thread *next;

	while (thread != NULL)
	{
		next = thread->next;

		if ((thread->status == THREADSTATUS_WAITING_FOR_MONITOR)&&
			(thread->monitorObject==object))
		{
			thread->monitorObject = NULL;
			thread->status = THREADSTATUS_RUNNING;
			if (!all) return;
		}

		thread = next;
	}
}

/**
 * Checks for any threads that have reached the THREADSTATUS_FINISHED state, and removes them from
 * the virtual machine context.
 * @param vm the virtual machine context
 */
void dj_vm_checkFinishedThreads(dj_vm *vm)
{
	dj_thread *thread = vm->threads;
	dj_thread *next;

	while (thread != NULL) {
		next = thread->next;
		if (thread->status == THREADSTATUS_FINISHED)
		{
			if (vm->currentThread == thread)
				vm->currentThread = NULL;
			dj_vm_removeThread(vm, thread);
			dj_thread_destroy(thread);
		}

		thread = next;
	}

}

/**
 * Gets a thread from the virtual machine by index. The index is not equal to the thread ID.
 * TODO: NB: 8-10: this method does not return NULL if out of bounds, and might crash instead
 * @param vm the virtual machine context
 * @param index the index of the thread
 * @return the requested thread, or NULL if the index is out of bounds
 */
dj_thread * dj_vm_getThread(dj_vm * vm, int index)
{
	dj_thread *finger;
	int i=0;

	finger = vm->threads;
	while (index>i)
	{
		finger = finger->next;
		i++;
	}

	return finger;

}

/**
 * Gets a thread by its ID. Used by the native methods of the java.lang.Thread object to control threads and
 * call yield, sleep, wait, etc.
 * @param vm the virtual machine context
 * @param id the ID of the desired Thread
 */
dj_thread * dj_vm_getThreadById(dj_vm * vm, int id)
{
	dj_thread *finger;

	finger = vm->threads;
	while (finger!=NULL)
	{
		if (finger->id==id)
			return finger;

		finger = finger->next;
	}

	return finger;
}

dj_thread *dj_vm_getThreadByExecutionContext(dj_vm * vm, uint16_t id){
	dj_thread *finger;

		finger = vm->threads;
		while (finger!=NULL)
		{
			if (finger->executionContext==id)
				return finger;

			finger = finger->next;
		}

		return finger;
}

/**
 * Gets a thread by its waiting semaphore ID.
 * @param vm the virtual machine context
 * @param id the ID of the desired semaphore
 */
dj_thread * dj_vm_getThreadBySem(dj_vm * vm, int sem)
{
	dj_thread *finger;

	finger = vm->threads;
	while (finger!=NULL)
	{
		if (finger->sem_id==sem)
			return finger;

		finger = finger->next;
	}

	return finger;
}


/**
 * Counts the number of threads that are 'live', meaning that they are either running now or will
 * potentially start running in the future. This includes blocked and sleeping threads.
 * @param vm the virtual machine context
 */
int dj_vm_countLiveThreads(dj_vm *vm)
{
	int ret = 0;
	dj_thread *thread = vm->threads;

	while (thread!=NULL)
	{
		if ( (thread->status==THREADSTATUS_RUNNING) ||
				(thread->status==THREADSTATUS_BLOCKED) ||
				(thread->status==THREADSTATUS_BLOCKED_FOR_MONITOR) ||
				(thread->status==THREADSTATUS_WAITING_FOR_MONITOR) ||
				(thread->status==THREADSTATUS_SLEEPING) ||
				(thread->status==THREADSTATUS_BLOCKED_FOR_IO)
				) ret++;
		thread = thread->next;
	}
	return ret;
}

int32_t dj_vm_getVMSleepTime(dj_vm * vm)
{
	int32_t ret = -1, scheduleTime, time;
	dj_thread *thread = vm->threads;

	time = dj_timer_getTimeMillis();

	while (thread!=NULL)
	{
		if (thread->status==THREADSTATUS_RUNNING) ret = 0;

		if (thread->status==THREADSTATUS_SLEEPING)
		{
			scheduleTime = thread->scheduleTime - time;
			if (scheduleTime<0) scheduleTime = 0;
			if (ret==-1||((uint32_t)ret)>scheduleTime) ret = (int32_t)scheduleTime;
		}

		thread = thread->next;
	}

	return ret;
}


/**
 * Wakes up any threads that need to be woken up and chooses the next thread to be started and
 * schedules it for execution. A simple round-robin scheme is used for thread selection.
 * @param vm the virtual machine context
 */
long dj_vm_schedule(dj_vm *vm)
{
	int maxPriority;
	long nextScheduleTime = 0;


	dj_thread *thread, *selectedThread;

	// prune finished threads
	dj_vm_checkFinishedThreads(vm);

	// wake up any threads that need to be woken up
	nextScheduleTime = dj_vm_wakeThreads(vm);


	// Simple round-robin scheduling algo:
	// Select the thread with the highest priority and set its priority to zero, and
	// increase the priorities of all other threads by one.
	selectedThread = NULL;
	thread = vm->threads;
	maxPriority = -1;
	while (thread!=NULL)
	{
		//DEBUG_LOG("thread %d status %d  prio %d timeS %ld\n", thread->id, thread->status, thread->priority, thread->scheduleTime);
		if (thread->status==THREADSTATUS_RUNNING)
		{
			DEBUG_LOG("thread %d running \n", thread->id);
			thread->priority++;
			if(thread->need_resched){ // reschedule a thread which was waiting on I/O first!!!
				selectedThread=thread;
				thread->need_resched = 0;
				break;
			}
			if (maxPriority<thread->priority)
			{
				selectedThread=thread;
				maxPriority=thread->priority;
			}
		}
		thread = thread->next;
	}
	/*if(selectedThread != nullref)
		DEBUG_LOG("selected thread to run %d\n", selectedThread->id);
	else
		DEBUG_LOG("NOT selected thread to run\n");*/

	char ret = dj_vm_activateThread(vm, selectedThread);
	return nextScheduleTime; //ret
}

/**
 * Activates a thread, meaning that the current thread (if any) is switched out and the
 * new thread is switched in. Note that this method may be called with NULL, if there is
 * currently no active thread to run.
 * @param vm the virtual machine context
 * @param selectedThread the thread to activate
 */
char dj_vm_activateThread(dj_vm *vm, dj_thread *selectedThread)
{
	dj_thread *thread_iterator;
	// stop the current thread
	if (vm->currentThread != NULL && !(vm->currentThread->hibernated)){
		DEBUG_LOG("deactivating thread %d\n", vm->currentThread->id);
		dj_exec_deactivateThread(vm->currentThread);
		  /* added the ibernation control in order to reload the ibernated thread without
		   * resetting its pc state --> the deactivateThread function save the actual pc (now is 0) to the
		   * thread frame. If the thread is just ibernated the global variable pc == 0 and the ibernated thread
		   * will reset its execution */
	}
	if (vm->currentThread != NULL && (vm->currentThread->hibernated)){
		thread_iterator = vm->threads;
		while(thread_iterator != NULL){
			thread_iterator->hibernated = 0;
			thread_iterator = thread_iterator->next;
		}
	}

	vm->currentThread = selectedThread;

	// check if we found a thread we can activate
	if (selectedThread!=NULL)
	{
		selectedThread->priority=0;
		DEBUG_LOG("activating thread %d\n", selectedThread->id);
		dj_exec_activate_thread(selectedThread);
		return 1;
	} else
	{
		// no threads to schedule, return false
		return 0;
	}

}

/**
 * Marks the root set. The root set can be marked directly in one pass since reference and non-reference types
 * are separated.
 * @param vm the virtual machine context
 */
void dj_vm_markRootSet(dj_vm *vm)
{
	dj_thread * thread;
	dj_infusion * infusion;
	dj_monitor_block * monitorBlock;

	// Mark threads
	thread = vm->threads;
	while (thread!=NULL)
	{
		dj_thread_markRootSet(thread);
		thread = thread->next;
	}

    // Mark infusions
	infusion = vm->infusions;
	while (infusion!=NULL)
	{
		dj_infusion_markRootSet(infusion);
		infusion = infusion->next;
	}

    // Mark monitor blocks
	monitorBlock = vm->monitors;
	while (monitorBlock!=NULL)
	{
		dj_monitor_markRootSet(monitorBlock);
		monitorBlock = monitorBlock->next;
	}

}

/**
 * Updates pointers after the new offsets of objects have been calculated, and before the objects
 * are actually moved to their new locations.
 * @param vm the virtual machine context
 */
void dj_vm_updatePointers(dj_vm *vm)
{
	vm->currentThread = dj_mem_getUpdatedPointer(vm->currentThread);
	vm->infusions = dj_mem_getUpdatedPointer(vm->infusions);
	vm->monitors = dj_mem_getUpdatedPointer(vm->monitors);
	vm->systemInfusion = dj_mem_getUpdatedPointer(vm->systemInfusion);
	vm->threads = dj_mem_getUpdatedPointer(vm->threads);
}


/**
 * Finds a monitor for the given object. If no monitor exists, one is created.
 * @param vm virtual machine context
 * @param object object that acts as a lock
 */
dj_monitor * dj_vm_getMonitor(dj_vm *vm, dj_object * object)
{
	int i;
	dj_monitor *monitor;
	dj_monitor_block *block, *newBlock;

	// search for the monitor
	block = vm->monitors;
	for (i=0; i<vm->numMonitors; i++)
	{
		// check if the monitor applies to the given object
		monitor = &(block->monitors[i%MONITOR_BLOCK_SIZE]);

		if (monitor->object==object)
			return monitor;

		// if we have reached the end of a block, move to the next one
		if ((i%MONITOR_BLOCK_SIZE)==(MONITOR_BLOCK_SIZE-1))
			block = block->next;
	}

	// If the following code is reached, the monitor is not yet in the monitor list.
	// We add the monitor to the end of the list.

	// check if we need to add a new block
	if ((vm->numMonitors%MONITOR_BLOCK_SIZE)==0)
	{
		// tell the memory manager to update the pointer to the object should garbage collection
		// be triggered
		dj_mem_pushCompactionUpdateStack(VOIDP_TO_REF(object));

		// allocate a new monitor block
		newBlock = dj_monitor_block_create();

		// get the object pointer
		object = REF_TO_VOIDP(dj_mem_popCompactionUpdateStack());

		// check for out of memory and let the caller deal with it
		if (newBlock==NULL) return NULL;

		// find the last block (do this after the allocation to make sure everything goes right
		// in case the allocation triggers garbage collection)
		for (block=vm->monitors; (block!=NULL)&&(block->next!=NULL); block=block->next);

		// add the new monitor to the virtual machine
		if (vm->monitors==NULL)
			vm->monitors = newBlock;
		else
			block->next = newBlock;

		block = newBlock;
	} else
		// find last block
		for (block=vm->monitors; (block!=NULL)&&(block->next!=NULL); block=block->next);

	// Create a new monitor and add it to the list.
	// note that vm->monitors acts like a stack, so vm->monitors points to the
	// block that was added last
	monitor = &(block->monitors[vm->numMonitors%MONITOR_BLOCK_SIZE]);

	// reset the monitor to count=0, waiting_threads=0
	// NB: 8-10-08: This fixes the bug that caused the thread test to hang on the fleck
	monitor->count = 0;
	monitor->waiting_threads = 0;
	monitor->object = object;
	monitor->owner = NULL;
	vm->numMonitors++;
	block->count++;

	return monitor;
}

void dj_vm_removeMonitor(dj_vm *vm, dj_monitor * monitor)
{
	int i;
	dj_monitor_block *block, *lastBlock;

	// search for the monitor
	block = vm->monitors;
	for (i=0; i<vm->numMonitors; i++)
	{
		if (&(block->monitors[i%MONITOR_BLOCK_SIZE])==monitor) break;

		// if we have reached the end of a block, move to the next one
		if ((i%MONITOR_BLOCK_SIZE)==(MONITOR_BLOCK_SIZE-1))
			block = block->next;
	}

	// find the last block
	for (lastBlock=vm->monitors; (lastBlock!=NULL)&&(lastBlock->next!=NULL); lastBlock=lastBlock->next);

	// reduce the monitor count
	vm->numMonitors--;
	lastBlock->count--;

	if (i<vm->numMonitors)
	{
		// remove the monitor by replacing it with the last monitor in the linked list
		block->monitors[i%MONITOR_BLOCK_SIZE] = lastBlock->monitors[vm->numMonitors%MONITOR_BLOCK_SIZE];
	}

	// check if we need to dealloc the last block
	if (vm->numMonitors%MONITOR_BLOCK_SIZE==0)
	{
		if (vm->numMonitors>0)
		{
			// find the block that comes just before the last block
			for (block=vm->monitors; (block!=lastBlock)&&(block->next!=lastBlock); block=block->next);
			// set its next pointer to NULL
			block->next = NULL;
		} else
		{
			vm->monitors = NULL;
		}

		// free the last block
		dj_mem_free(lastBlock);
	}

}


inline dj_global_id dj_vm_getRuntimeClass(dj_vm *vm, runtime_id_t id)
{
	dj_global_id ret;
	dj_infusion *infusion = vm->infusions;
	runtime_id_t base = 0;

	// TODO: optimize this! (binary search?)
	// TODO: test for multiple loaded infusions
	while (infusion!=NULL)
	{
		base = infusion->class_base;
		DEBUG_LOG("Comparing with infusion %d having base id %d\n",dj_vm_getInfusionId(dj_exec_getVM(), infusion), infusion->class_base);
		if ((id>=base)&&(id<base + dj_di_parentElement_getListSize(infusion->classList)))
		{
			ret.infusion = infusion;
			ret.entity_id = id - base;

			return ret;
		}
		infusion = infusion->next;
	}

	// TODO raise error, class not found
	printf("error: class not found: %d\n", id);
    dj_panic(DJ_PANIC_ILLEGAL_INTERNAL_STATE);

    // dead code to make compiler happy
    ret.entity_id=255;
    ret.infusion=NULL;
    return ret;
}

dj_di_pointer dj_vm_getRuntimeClassDefinition(dj_vm *vm, runtime_id_t id)
{
	dj_global_id global_id = dj_vm_getRuntimeClass(vm, id);
	return dj_infusion_getClassDefinition(global_id.infusion, global_id.entity_id);
}

dj_infusion *dj_vm_getSystemInfusion(dj_vm *vm)
{
	return vm->systemInfusion;
}

/**
 * Runs the class initialisers of a given infusion. The class initialisers are the
 * methods with the name <cinit>, that are generated for code inside <code>static { ... }</code>
 * and for statements like <code>static int foo = 1;</code>.
 * @param vm the virtual machine context
 */
void dj_vm_runClassInitialisers(dj_vm *vm, dj_infusion *infusion)
{
	int i;
	dj_thread * thread;
	dj_frame * frame;
	dj_global_id methodImplId;

	// create a new thread object to run the <CLINIT> methods in
	thread = dj_thread_create();
	thread->id = -1;
	dj_vm_addThread(vm, thread);
	// the infusion for all of the <CINIT> methods we're executing is the same
	methodImplId.infusion = infusion;

	// iterate over the class list and execute any class initialisers that are encountered
	for (i=0; i<dj_di_parentElement_getListSize(infusion->classList); i++)
	{
		dj_di_pointer classDef = dj_di_parentElement_getChild(infusion->classList, i);
		methodImplId.entity_id = dj_di_classDefinition_getCLInit(classDef);

		if (methodImplId.entity_id!=255)
		{
			// create a frame to run the initialiser in
			frame = dj_frame_create(methodImplId);

			// if we're out of memory, panic
		    if (frame==NULL)
		    {
		        DEBUG_LOG("dj_vm_runClassInitialisers: could not create frame. Panicking\n");
		        dj_panic(DJ_PANIC_OUT_OF_MEMORY);

		    }

		    thread->frameStack = frame;
		    thread->status = THREADSTATUS_RUNNING;

			dj_exec_activate_thread(thread);

			// execute the method
			while (thread->status!=THREADSTATUS_FINISHED){
				dj_exec_run(RUNSIZE);
				DEBUG_LOG(" ***************** The thread initializer is running with id %d\n", thread->id);
			}
		}
	}

	// clean up the thread
	dj_vm_removeThread(vm, thread);
	dj_thread_destroy(thread);
	vm->currentThread = NULL;
}

//#undef DEBUG_LOG
