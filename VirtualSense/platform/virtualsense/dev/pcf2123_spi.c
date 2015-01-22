/*
 *  pcf2321_spi.h
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
 * The SPI communication system with PCF2123 RTC.
 *
 * @author Emanuele Lattanzi
 *
 */

#include "contiki.h"
#include "dev/button-sensor.h"
#include "dev/spi_sw.h"
#include "dev/ioc.h"
#include "pcf2123_spi.h"
#include "contiki-conf.h"
#include "power-interface.h"

#include <stdio.h>
#include <stdint.h>

//#include "lib/_cpu.h"



uint8_t RTC_is_up(void){ //TODO: trovare un modo pulito per farlo
	uint8_t res = 0;
	if(RTC_get_year() == 11)
		res = 1;
	return res;
}


/* initilize the RTC module */
void RTC_init(void){
	printf("RTC init\n");

	// Set port PB4 as input with internal pullup
	/* Software controlled */
	GPIO_SOFTWARE_CONTROL(RTC_INT_PORT, (1 << RTC_INT_PIN));
	/* Set pin to input */
	GPIO_SET_INPUT(RTC_INT_PORT, (1 << RTC_INT_PIN));
	ioc_set_over(RTC_INT_PORT_NO, RTC_INT_PIN, IOC_OVERRIDE_PUE);

	spi_init();

	//printf("b2bcdbdvjbsdjvbsdjkvbjksdv");
	//printf("b2bcdbdvjbsdjvbsdjkvbjksdv");
	//printf("b2bcdbdvjbsdjvbsdjkvbjksdv\n");

	//RTC_write_register(PCF2123_REG_CTRL1, PCF2123_RESET);

	//RTC_write_register(PCF2123_REG_HR, bin2bcd(0x02));

	//uint8_t val = RTC_read_register(PCF2123_REG_HR);

	//printf("ora: %d\n", bcd2bin(val));

	//printf("b2bcdbdvjbsdjvbsdjkvbjksdv: %d", RTC_get_hours());

	//while(1);

	//printf("Init rtc\n");
	lock_SPI(); //NON ci sarebbe bisogno ma in questo modo impedisco che PM spenga
	// la SPI durante l'inizializzazione. Non servirebbe perch� ogni write acquisisce il lock
	// in questo modo ho performace maggiori
	uint8_t cout_value = RTC_read_register(PCF2123_REG_T_CLOKOUT);

	if(cout_value != 0x71){ // the rtc should be initialized because it has been just powerd-up

		/* reset RTC */
		RTC_write_register(PCF2123_REG_CTRL1, PCF2123_RESET);

		// setting low power mode by sourcing countdown timer with 1/60 Hz,
		// setting clockout frequency at 1Hz and disable countdown timer
		RTC_write_register(PCF2123_REG_T_CLOKOUT, PCF2123_COUT_F_1 | PCF2123_CDT_SF_1_64 | PCF2123_TIMER_DI);

		//RTC_write_register(PCF2123_REG_CTRL2, PCF2123_MI_INT);
		// enable seconds interrupt as pulse DEMO

		//Set time after power up to Giulia Lattanzi's birth day Sunday 27/11/11 14:24
		RTC_write_register(PCF2123_REG_HR, bin2bcd(0x0E));
		RTC_write_register(PCF2123_REG_MN, bin2bcd(0x18));
		RTC_write_register(PCF2123_REG_SC, bin2bcd(0x01));

		RTC_write_register(PCF2123_REG_MO, bin2bcd(0x0B));
		RTC_write_register(PCF2123_REG_DM, bin2bcd(0x1B));
		RTC_write_register(PCF2123_REG_DW, bin2bcd(0x00));
		RTC_write_register(PCF2123_REG_YR, bin2bcd(0x0B));

		uint8_t min = RTC_read_register(PCF2123_REG_MN);
		printf("RTC initialized with minutes %d\n", bcd2bin(min));
	}
	release_SPI();

}

void RTC_stop(void){
	RTC_write_register(PCF2123_REG_CTRL1, PCF2123_STOP);
}

void RTC_schedule_interrupt_at_minutes(uint8_t minutes){
	RTC_write_register(PCF2123_REG_CTRL2, PCF2123_AL_INT); // Enable alarm interrupt
	RTC_write_register(PCF2123_REG_MN_ALARM, bin2bcd(minutes)); //set alarm minutes
	release_SPI();
}
void RTC_schedule_interrupt_at_hours(uint8_t hours){
	RTC_write_register(PCF2123_REG_CTRL2, PCF2123_AL_INT); // Enable alarm interrupt
	RTC_write_register(PCF2123_REG_HR_ALARM, bin2bcd(hours)); //set alarm hours
}
void RTC_schedule_interrupt_at_days(uint8_t days){
	RTC_write_register(PCF2123_REG_CTRL2, PCF2123_AL_INT); // Enable alarm interrupt
	RTC_write_register(PCF2123_REG_DA_ALARM, bin2bcd(days)); //set alarm minutes
}


void RTC_clear_interrupt(void){
	uint8_t actual_value = RTC_read_register(PCF2123_REG_CTRL2);
	RTC_write_register(PCF2123_REG_CTRL2, (actual_value & 0xD3)); //clear BIT 5,3,2 --> 11010011*/
}
void RTC_disable_all_interrupts(void){
	RTC_write_register(PCF2123_REG_CTRL2, 0x00);
}
uint8_t RTC_get_year(void){
	uint8_t val = RTC_read_register(PCF2123_REG_YR);
	return bcd2bin(val);
}

uint8_t RTC_get_month(void){
	uint8_t val = RTC_read_register(PCF2123_REG_MO);
		return bcd2bin(val);
}

uint8_t RTC_get_day_of_month(void){
	uint8_t val = RTC_read_register(PCF2123_REG_DM);
		return bcd2bin(val);
}

uint8_t RTC_get_day_of_week(void){
	uint8_t val = RTC_read_register(PCF2123_REG_DW);
		return bcd2bin(val);
}

uint8_t RTC_get_hours(void){
	uint8_t val = RTC_read_register(PCF2123_REG_HR);
		return bcd2bin(val);
}

uint8_t RTC_get_minutes(void){
	uint8_t val = RTC_read_register(PCF2123_REG_MN);
		return bcd2bin(val);
}

uint8_t RTC_get_seconds(void){
	uint8_t val = RTC_read_register(PCF2123_REG_SC);
	uint8_t sec = bcd2bin(val);
	if (sec > 60)
		sec = sec -80;

	return sec; //bcd2bin(RTC_read_register(PCF2123_REG_SC));
}

/*
 *	transmit a byte
 */
/*void B1_tx(uint8_t val){
#if 0 //TODO to implement
	/// wait until a tx operation end
	while( UCB1STAT & UCBUSY );
	UCB1TXBUF = val;
	/// wait until a tx operation end
	while( UCB1STAT & UCBUSY );
#endif

}
*/
/*
 * read a byte
 */
/*uint8_t B1_rx(void){


	uint8_t read;
#if 0 //TODO to implement
	/// wait until a tx operation end
	while( UCB1STAT & UCBUSY );
	UCB1TXBUF = 0x00;
	/// wait until a tx operation end
	while( UCB1STAT & UCBUSY );
	/// when write operation is finished, received data is ready.
	read = UCB1RXBUF;
	/// close connection
#endif
	return read;
}*/


uint8_t RTC_read_register(uint8_t aReg)
{
	uint8_t casted = 0;
	uint32_t result = 0;

	//lock_SPI();
	ce_set();//<<<<<<<<<<<<<<<<<<SPI_CE_SET();
	asm("   NOP");

	// Write "command"
	spi_write(aReg|PCF2123_READ);//SPI_WRITE(aReg|PCF2123_READ);//B1_tx(aReg|PCF2123_READ);

	// Read back "data"
	result = spi_read();//SPI_READ(result);// = B1_rx();

	//release_SPI();
	//printf("read reg %d: %x\n", aReg, result);
	asm("   NOP");
	ce_clr();//<<<<<<<<<<<<<<<<<<SPI_CE_CLR();

	casted = (uint8_t)result;
	spi_delay(0xAA);

	return casted;
}


void RTC_write_register(uint8_t aReg, uint8_t aValue)
{
	//lock_SPI();

	ce_set();//<<<<<<<<<<<<<<<<<<SPI_CE_SET();
	asm("   NOP");
	spi_write(aReg|PCF2123_WRITE);//SPI_WRITE(aReg|PCF2123_WRITE);//B1_tx(aReg|PCF2123_WRITE);
	spi_write(aValue);//SPI_WRITE(aValue);//B1_tx(aValue);
    asm("   NOP");

    ce_clr();//<<<<<<<<<<<<<<<<<<SPI_CE_CLR();//SPI_CS_CLR(GPIO_B_NUM, 5);

	//release_SPI();
    spi_delay(0xAA);

	//printf("write: %d to reg %d\n", aValue, aReg);
}
