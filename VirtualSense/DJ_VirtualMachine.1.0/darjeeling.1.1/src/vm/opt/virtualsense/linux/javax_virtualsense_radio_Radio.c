/*
 *	javax_radio_Radio.c
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
#include <stdint.h>

#include "common/execution/execution.h"
#include "common/array.h"


#include "base_definitions.h"

//static struct abc_conn uc;
static short receiveThreadId = -1;



//static const struct abc_callbacks abc_callbacks = {recv_uc};

void javax_virtualsense_radio_Radio_void__waitForMessage()
{
	// wait for radio
	dj_thread * currentThread = dj_exec_getCurrentThread();
	receiveThreadId = currentThread->id;
	currentThread->status = THREADSTATUS_BLOCKED_FOR_IO;
	dj_exec_breakExecution();
}

// byte[] javax.virtualsense.radio.Radio._readBytes()
void javax_virtualsense_radio_Radio_byte____readBytes()
{
	dj_int_array * ret;
	int length = 0;// packetbuf_datalen(); //rimebuf_datalen() prima versione

	ret = dj_int_array_create(T_BYTE, length);

	// copy data from the rimebuf to the return array
	//packetbuf_copyto(ret->data.bytes);

	dj_exec_stackPushRef(VOIDP_TO_REF(ret));
}

// void javax.virtualsense.radio.Radio._init()
void javax_virtualsense_radio_Radio_void__init()
{
	//abc_open(&uc, 9345, &abc_callbacks);
	/*lock =*/ dj_exec_stackPopRef();
}

// byte javax.virtualsense.radio.Radio._getNumMessages()
void javax_virtualsense_radio_Radio_byte__getNumMessages()
{

}

// void javax.virtualsense.radio.Radio.setChannel(short)
void javax_virtualsense_radio_Radio_void_setChannel_short()
{
	int16_t channel = dj_exec_stackPopShort();
	// not implemented
}

// short javax.virtualsense.radio.Radio.getMaxMessageLength()
void javax_virtualsense_radio_Radio_short_getMaxMessageLength()
{

}

// void javax.virtualsense.radio.Radio._broadcast(byte[])
void javax_virtualsense_radio_Radio_void__broadcast_byte__()
{
    //leds_on(LEDS_GREEN);

	//rimeaddr_t addr;

	dj_int_array * byteArray = dj_exec_stackPopRef();

	// check null
	if (byteArray==nullref)
		dj_exec_createAndThrow(BASE_CDEF_java_lang_NullPointerException);

	// copy bytes to the rime buffer
    //packetbuf_copyfrom(byteArray->data.bytes, byteArray->array.length);
    //packetbuf_set_datalen(byteArray->array.length);

    // abc
    //abc_send(&uc);

    //leds_off(LEDS_GREEN);
}

// boolean javax.virtualsense.radio.Radio._send(short, byte[])
void javax_virtualsense_radio_Radio_boolean__send_short_byte__()
{
    //leds_on(LEDS_GREEN);

	//rimeaddr_t addr;

	dj_int_array * byteArray = dj_exec_stackPopRef();
	int16_t id = dj_exec_stackPopShort();

	// check null
	if (byteArray==nullref)
		dj_exec_createAndThrow(BASE_CDEF_java_lang_NullPointerException);

	// copy bytes to the rime buffer
    //packetbuf_copyfrom(byteArray->data.bytes, byteArray->array.length);
    //packetbuf_set_datalen(byteArray->array.length);

    // abc
    //abc_send(&uc);

    //leds_off(LEDS_GREEN);
}

// short javax.virtualsense.radio.Radio.getDestId()
void javax_virtualsense_radio_Radio_short_getDestId(){
	// Destination should be broadcast or node id because otherwise
	// contiki network stack does not call the callback
	dj_exec_stackPushShort(0);
}

void javax_virtualsense_radio_Radio_short_getSenderId(){
	dj_exec_stackPushShort(1);
}
