/* This is a very simple hello_world program.
 * It aims to demonstrate the co-existence of two processes:
 * One of them prints a hello world message and the other blinks the LEDs
 *
 * It is largely based on hello_world in $(CONTIKI)/examples/sensinode
 *
 * Author: George Oikonomou <G.Oikonomou@lboro.ac.uk>
 */

#include "contiki.h"
#include "dev/leds.h"
#include "dev/light-sensor.h"
#include "dev/button-sensor.h"
#include <stdio.h>


static int
get_light(void){
	return 10 * light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC) / 7;
}


/*---------------------------------------------------------------------------*/
PROCESS(sensing_process, "Light Sensing");

AUTOSTART_PROCESSES( &sensing_process );

/*---------------------------------------------------------------------------*/
/* Implementation of the second process */
PROCESS_THREAD(sensing_process, ev, data)
{
    static struct etimer timer;
    static int time = 0;

	PROCESS_BEGIN();
	
	SENSORS_ACTIVATE(light_sensor);
	SENSORS_ACTIVATE(button_sensor);
	
	printf("START4\n");

	while(1) {
        etimer_set(&timer, CLOCK_CONF_SECOND);
		
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

		time += 1;

		printf("%d, %d\n", time, get_light());
        
		
		/*PROCESS_WAIT_EVENT();

		if(ev == PROCESS_EVENT_TIMER){
		
			time += 1;

			printf("%d, %d\n", time, get_light());
		}
		else if(ev == sensors_event){
			
			while(1){
				
				PROCESS_WAIT_EVENT_UNTIL(ev==sensors_event);
				break;
			}
		}*/

    }
    PROCESS_END();
}

/*---------------------------------------------------------------------------*/
