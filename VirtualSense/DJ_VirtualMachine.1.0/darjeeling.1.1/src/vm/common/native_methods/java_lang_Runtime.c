/*
 *	java_lang_Runtime.c
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
#include <stddef.h>

#include "common/execution/execution.h"
#include "common/global_id.h"
#include "common/debug.h"
#include "common/heap/heap.h"
#include "common/djtimer.h"

#include "pointerwidth.h"

// generated by the infuser
#include "base_definitions.h"


// void java.lang.Runtime.exitInternal(int)
void java_lang_Runtime_void_exit_int()
{
	// no exit implemented
}

// int java.lang.Runtime.freeMemory()
void java_lang_Runtime_int_freeMemory()
{
	dj_exec_stackPushInt(dj_mem_getFree());
}


// int java.lang.Runtime.totalMemory()
void java_lang_Runtime_int_totalMemory()
{
	dj_exec_stackPushInt(dj_mem_getSize());
}




