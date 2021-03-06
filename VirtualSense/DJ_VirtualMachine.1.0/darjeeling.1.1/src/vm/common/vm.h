/*
 *	vm.h
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
#ifndef __vm__
#define __vm__

#include "types.h"
//#include "contiki.h"


dj_vm * dj_vm_create();
void dj_vm_destroy(dj_vm * vm);

void dj_vm_addInfusion(dj_vm * vm, dj_infusion * infusion);
int dj_vm_countInfusions(dj_vm * vm);
uint8_t dj_vm_getInfusionId(dj_vm * vm, dj_infusion * infusion);
dj_infusion * dj_vm_getInfusion(dj_vm * vm, int index);
dj_infusion * dj_vm_lookupInfusion(dj_vm * vm, dj_di_pointer name);
dj_infusion *dj_vm_loadSystemInfusion(dj_vm * vm, dj_di_pointer di);
bool dj_vm_safeToUnload(dj_vm *vm, dj_infusion * infusion);
void dj_vm_unloadInfusion(dj_vm *vm, dj_infusion * infusion);
dj_infusion * dj_vm_loadInfusion(dj_vm * vm, dj_di_pointer di);
dj_infusion * dj_vm_getSystemInfusion(dj_vm * vm);
void dj_vm_runClassInitialisers(dj_vm * vm, dj_infusion * infusion);

void dj_vm_addThread(dj_vm * vm, dj_thread * thread);
int dj_vm_countThreads(dj_vm * vm);
int dj_vm_countLiveThreads(dj_vm * vm);
void dj_vm_checkFinishedThreads(dj_vm * vm);
void dj_vm_notify(dj_vm *vm, dj_object *object, bool all);
dj_thread *dj_vm_getThread(dj_vm * vm, int index);
dj_thread *dj_vm_getThreadById(dj_vm * vm, int id);
dj_thread * dj_vm_getThreadBySem(dj_vm * vm, int sem);
dj_thread *dj_vm_getThreadByExecutionContext(dj_vm * vm, uint16_t id);
void dj_vm_removeThread(dj_vm * vm, dj_thread * thread);
char dj_vm_activateThread(dj_vm * vm, dj_thread * selectedThread);
int32_t dj_vm_getVMSleepTime(dj_vm * vm);

dj_monitor * dj_vm_getMonitor(dj_vm * vm, dj_object * object);
void dj_vm_removeMonitor(dj_vm * vm, dj_monitor * monitor);
void dj_monitor_updatePointers(dj_monitor * monitor);

void dj_vm_markRootSet(dj_vm * vm);
void dj_vm_updatePointers(dj_vm * vm);

long dj_vm_schedule(dj_vm * vm);

dj_di_pointer dj_vm_getRuntimeClassDefinition(dj_vm * vm, runtime_id_t id);
dj_global_id dj_vm_getRuntimeClass(dj_vm * vm, runtime_id_t id);

uint8_t dj_vm_getSysLibClassRuntimeId(dj_vm * vm, uint8_t entity_id);
dj_object * dj_vm_createSysLibObject(dj_vm * vm, uint8_t entity_id);

#endif
