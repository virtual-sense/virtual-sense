/*
 *	config.h
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

#include "platform-conf.h"
#ifndef __config_h
#define __config_h

// define heap size
#define RUNSIZE 128
#define HEAPSIZE 6*1024
//#define TEMP_DI_SIZE 4*1024
//#define DI_FILES_BASE = 0x10000

// define wether to pack structs (this is fine on all AVR targets)
// don't pack structs on MSP430 targets
// #define PACK_STRUCTS
#define ALIGN_16

#define WITH_MANAGE_INFUSION
#define SERIAL_INPUT 1
//#define HAS_USART

/* Please see common/debug.h */
//#define DARJEELING_DEBUG
//#define DARJEELING_DEBUG_TRACE
//#define DARJEELING_DEBUG_CHECK_HEAP_SANITY
//#define DARJEELING_DEBUG_PERFILE
#define DARJEELING_PRINTF printf
#define DARJEELING_PGMSPACE_MACRO //PSTR
//#define printf_P printf

#ifndef PLATFORM_HAS_UART
#define printf(...)
#define PRINTF(...)
#endif

#endif
