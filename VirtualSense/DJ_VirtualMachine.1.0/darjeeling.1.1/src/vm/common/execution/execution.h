/*
 *	execution.h
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

#ifndef __execution__
#define __execution__

#include "common/types.h"
#include "common/vmthread.h"
#include "common/vm.h"
#include "common/object.h"

#include "config.h"

int dj_exec_run();
void dj_exec_breakExecution();
void dj_exec_activate_thread(dj_thread *thread);
void dj_exec_deactivateThread(dj_thread *thread);

void dj_exec_throw(dj_object *obj, uint16_t throw_pc);
void dj_exec_throwHere(dj_object *obj);
void dj_exec_createAndThrow(int exceptionId);

void dj_exec_stackPushShort(int16_t value);
void dj_exec_stackPushInt(int32_t value);
void dj_exec_stackPushRef(ref_t value);

int16_t dj_exec_stackPopShort();
int32_t dj_exec_stackPopInt();
ref_t dj_exec_stackPopRef();

int16_t dj_exec_stackPeekShort();
int32_t dj_exec_stackPeekInt();
ref_t dj_exec_stackPeekRef();

dj_thread *dj_exec_getCurrentThread();
dj_infusion *dj_exec_getCurrentInfusion();

void dj_exec_setVM(dj_vm *_vm);
dj_vm *dj_exec_getVM();

void dj_exec_updatePointers();

#endif
