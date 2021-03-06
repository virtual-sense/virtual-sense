/*
 *	djtimer.h
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
#ifndef __dj_timer__
#define __dj_timer__

#include "types.h"

int32_t dj_timer_getTimeMillis();
void dj_timer_init();

/************************************************************************/
/*  multiplies the timer interrupt period per factor
 *
 ************************************************************************/
void dj_timer_slowDownClockByFactor(uint16_t factor);

void dj_timer_setSystemClockMillis(uint32_t millis);


#endif
