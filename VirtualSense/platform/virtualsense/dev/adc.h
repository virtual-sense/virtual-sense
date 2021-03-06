/*
 *	adc.h
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

/**
 * ADC access definitions.
 *
 * @author Emanuele Lattanzi
 *
 */
#include <msp430.h>

#ifndef __ADC_H__
#define __ADC_H__

#define NUM_CHANNEL 7

#define CHANNEL_0		0
#define CHANNEL_1		1
#define CHANNEL_2		2
#define CHANNEL_3		3
#define CHANNEL_4		4
#define CHANNEL_6		6
#define CHANNEL_7		7

#define REF_1_5V		0
#define REF_2V			1
#define REF_2_5V		2

#define CHANNEL_VREFP		8
#define CHANNEL_VREFN		9

#define BOARD_TEMP_CHANNEL	10
#define LIGHT_CHANNEL		1
#define VBAT_CHANNEL		11



void adc_init();

uint16_t read_adc_channel(int channel, short ref);

//uint16_t get_adc_extref(int channel, int vrefp, int vrefn);



#endif /* __ADC_H__ */
