/*
 *	types.h
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
#ifndef __types_h
#define __types_h

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

// platform-specific header files
#include "config.h"
#include "program_mem.h"
#include "pointerwidth.h"

// the "ref_t"  type and  the "null" constant  are now defined  in the
// platform-specific file "pointerwidth.h", but in both cases they are
// 16-bits wide.

// common types
typedef uint16_t runtime_id_t;

// keep this a multiple of 2, to keep the size of the blocks a multiple of 2 (for 16-bit architectures)
#define MONITOR_BLOCK_SIZE 8

typedef struct _dj_local_id dj_local_id;
typedef struct _dj_global_id dj_global_id;

typedef struct _dj_object dj_object;

typedef struct _dj_thread dj_thread;
typedef struct _dj_frame dj_frame;
typedef struct _dj_monitor dj_monitor;
typedef struct _dj_monitor_block dj_monitor_block;

typedef struct _dj_infusion dj_infusion;
typedef struct _dj_vm dj_vm;
typedef struct _dj_semaphore dj_semaphore;
typedef struct _dj_app_table_node dj_app_table_node;



struct _dj_app_table_node
{
	/** the app_unique_id **/
	int16_t app_id;

	/** the memory app pointer **/
	dj_di_pointer app_pointer;
}
#ifdef PACK_STRUCTS
__attribute__ ((__packed__))
#endif
;

/**
 * A two-byte tuple that references entities. The infusion_id points to an infusion and the entity_id indexes
 * a certain entity within that infusion. The local ID is called 'local' because it only makes sense within the context of
 * the infusion in which it is found. This is because the infusion_id indexes into an import list in the infusion. Local IDs
 * can be resolved into global IDs.
 */
struct _dj_local_id
{
	/** Infusion ID */
	uint8_t infusion_id;

	/** Entity ID */
	uint8_t entity_id;
}
#ifdef PACK_STRUCTS
__attribute__ ((__packed__))
#endif
;

/**
 * A pointer/byte tuple that holds a pointer to an infusion and a byte that indexes a certain entity within
 * that infusion.
 */
struct _dj_global_id
{
	dj_infusion *infusion;
	uint8_t entity_id;
}
#ifdef PACK_STRUCTS
__attribute__ ((__packed__))
#endif
;

typedef void (*dj_native_handler)(dj_global_id);

struct _dj_object
{
	// nothing here, the Object struct is more syntacting than functional
}
#ifdef PACK_STRUCTS
__attribute__ ((__packed__))
#endif
;

struct _dj_frame
{
	dj_frame * parent;							// stack implemented as a linked list
//	dj_infusion * infusion;						// for resolving references
//	dj_di_pointer method;						// method that is executing in this frame
	dj_global_id method;

	uint16_t pc;								// program counter, return adress
	uint8_t nr_int_stack;						// the number of values on the integer stack
	uint8_t nr_ref_stack;						// the number of values on the reference stack
}
#ifdef PACK_STRUCTS
__attribute__ ((__packed__))
#endif
;

struct _dj_thread
{
	//int32_t scheduleTime;
	long scheduleTime;
	int16_t id;								// unique thread id
	uint8_t status;
	uint8_t priority;
	uint8_t hibernated;	//flag to identify the hibernation
	uint16_t sem_id;
	uint16_t executionContext;	//flag to identify the execution context belonging to
	uint8_t need_resched;

	dj_frame * frameStack;

	// runnable object
	dj_object * runnable;

	// monitor object in case of blocked
	dj_object * monitorObject;

	// threads are stored as a linked list
	dj_thread * next;

}
#ifdef PACK_STRUCTS
__attribute__ ((__packed__))
#endif
;

struct _dj_monitor
{
	dj_object * object;
	dj_thread * owner;
	uint8_t count;
	uint8_t waiting_threads;
}
#ifdef PACK_STRUCTS
__attribute__ ((__packed__))
#endif
;

struct _dj_monitor_block
{
	dj_monitor_block *next;
	dj_monitor monitors[MONITOR_BLOCK_SIZE];
	uint8_t count;
#ifdef ALIGN_16
	uint8_t PADDING;
#endif
}
#ifdef PACK_STRUCTS
__attribute__ ((__packed__))
#endif
;

struct _dj_infusion
{

	// DI file elements
	dj_di_pointer header;
	dj_di_pointer classList;
	dj_di_pointer methodImplementationList;
	dj_di_pointer stringTable;

	// pointer to native method handler
	dj_native_handler native_handler;

	// for dynamic adress translation
	runtime_id_t class_base;

	// gc bookkeeping
	uint8_t nr_static_refs;

	// nr of referenced infusions
	uint8_t nr_referenced_infusions;

	// Infusions are stored as a linked list
	dj_infusion * next;

	// static fields
	ref_t * staticReferenceFields;
	uint8_t * staticByteFields;
	uint16_t * staticShortFields;
	uint32_t * staticIntFields;

	// infusion mapping
	dj_infusion ** referencedInfusions;

}
#ifdef PACK_STRUCTS
__attribute__ ((__packed__))
#endif
;

struct _dj_vm
{

	dj_thread *currentThread;

	uint16_t threadNr;
	uint16_t numMonitors;

	dj_infusion *systemInfusion;

	dj_infusion *infusions;
	dj_thread *threads;
	dj_monitor_block *monitors;

};

/*struct _dj_semaphore
{

	dj_object *semaphore;
	dj_thread *currentThread;
	dj_semaphore *next;

}*/


#ifdef PACK_STRUCTS
__attribute__ ((__packed__))
#endif
;

#endif
