/*
 *	gateway-shell.h
 *
 *  Copyright (c) 2012 DiSBeF, University of Urbino.
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
 * the command shell on the virtualsense gateway.
 *
 * @author Emanuele Lattanzi
 *
 */

#include "contiki.h"
#include "virtualsense-gateway-shell.h"
#include "dev/watchdog.h"
#include "net/rime.h"
#include "net/netstack.h"
#include "dev/leds.h"
#include "power-interface.h"
#include "platform-conf.h"
#include "../../DJ_VirtualMachine.1.0/darjeeling.1.1/src/vm/common/command_manager.h"


#include "node-id.h"
#include <string.h>


static int16_t app_id;
static uint8_t command_to_send;
static uint8_t id;
static struct broadcast_conn broadcast_command_channel;

PROCESS(command_manager_process, "Command manager");

/*---------------------------------------------------------------------------*/
PROCESS(shell_nodeid_process, "nodeid");
SHELL_COMMAND(nodeid_command,
	      "nodeid",
	      "nodeid: set node ID",
	      &shell_nodeid_process);

PROCESS(shell_apps_process, "apps");
SHELL_COMMAND(apps_command,
	      "apps",
	      "apps: <id> <command> manage installed applications\n command can be (LOAD, START, STOP, UNLOAD, PSAU)",
	      &shell_apps_process);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(shell_apps_process, ev, data)
{
  const char *newptr;
  char *command;
  char buf[60];
  PROCESS_BEGIN();

  uint16_t app = shell_strtolong(data, &newptr);
  if(newptr == data) {
     snprintf(buf, sizeof(buf), "apps: An application id is required");
  }else {
  command = strchr(data, ' ');
  if(strncmp (command," LOAD",5)== 0){
	  app_id = (int16_t )app;
	  command_to_send = COMMAND_APPS_LOAD;
	  process_poll(&command_manager_process);
	  snprintf(buf, sizeof(buf), "sendig a LOAD command for application %u\n",app);
  }else if (strncmp (command," START",5)== 0){
	  app_id = (int16_t )app;
	  command_to_send = COMMAND_APPS_START;
	  process_poll(&command_manager_process);
	  snprintf(buf, sizeof(buf), "sendig a START command for application %u\n",app);
  }else if (strncmp (command," STOP",5)== 0){
	  app_id = (int16_t )app;
	  command_to_send = COMMAND_APPS_STOP;
	  process_poll(&command_manager_process);
	  snprintf(buf, sizeof(buf), "sendig a STOP command for application %u\n",app);
  }else if (strncmp (command," UNLOAD",5)== 0){
	  app_id = (int16_t )app;
	  command_to_send = COMMAND_APPS_UNLOAD;
	  process_poll(&command_manager_process);
	  snprintf(buf, sizeof(buf), "sendig a UNLOAD command for application %u\n",app);
  }else if (strncmp (command," PSAU",5)== 0){
	  app_id = (int16_t )app;
	  command_to_send = COMMAND_APPS_PSAU;
	  process_poll(&command_manager_process);
	  snprintf(buf, sizeof(buf), "sendig a PSAU command for application %u\n",app);
  }else {
	  snprintf(buf, sizeof(buf), "apps: Command not found %s", command);
  }

  }
  shell_output_str(&apps_command, "", buf);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_nodeid_process, ev, data)
{
  uint16_t nodeid;
  char buf[20];
  const char *newptr;
  PROCESS_BEGIN();

  nodeid = shell_strtolong(data, &newptr);
  
  /* If no node ID was given on the command line, we print out the
     current channel. Else we burn the new node ID. */
  if(newptr == data) {
    nodeid = node_id;
  } else {
    nodeid = shell_strtolong(data, &newptr);
    watchdog_stop();
    leds_on(0);
    node_id_burn(nodeid);
    leds_on(1);
    node_id_restore();
    leds_off(0 + 1);
    watchdog_start();
  }

  snprintf(buf, sizeof(buf), "%d", nodeid);
  shell_output_str(&nodeid_command, "Node ID: ", buf);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void shell_gateway_init(struct broadcast_conn *c)
{
  broadcast_command_channel = *c;
  shell_register_command(&apps_command);
  shell_register_command(&nodeid_command);
  process_start(&command_manager_process, NULL);

}
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(command_manager_process, ev, data)
{
  struct virtualsense_apps_command_msg *msg;
  PROCESS_BEGIN();
  lock_MAC();
  app_id = -1;
  command_to_send = -1;
  id = 1;

  // open the broadcast command connection
  //broadcast_open(&broadcast, COMMAND_APPS_PORT, &broadcast_call);

  printf("gateway manager process started\n");
  while(1) {
	  if(app_id == -1){
		  PROCESS_YIELD();
	  }else {
		  // send command packet
		  //printf("sending command %u for app %d\n", command_to_send, app_id);
		  packetbuf_clear();
		  msg = (struct virtualsense_apps_command_msg *)packetbuf_dataptr();
		  packetbuf_set_datalen(sizeof(struct virtualsense_apps_command_msg));
		  memcpy(msg->header, COMMAND_APPS_HEADER, sizeof(COMMAND_APPS_HEADER));
		  msg->id = id;
		  msg->app_id = app_id;
		  msg->command = command_to_send;
		  //printf("packet prepared\n");
	      lock_RF();
	      broadcast_send(&broadcast_command_channel);
	      release_RF();
	     // printf("done\n");
	      app_id = -1;
	      id++;
	  }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
