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

#include <stdio.h>

int x = 0;
int y = 0;
int z = 0;

/*---------------------------------------------------------------------------*/
PROCESS(blink_process, "LED blink process");

AUTOSTART_PROCESSES( &blink_process);

/*---------------------------------------------------------------------------*/
/* Implementation of the second process */
PROCESS_THREAD(blink_process, ev, data)
{
    static struct etimer timer;
    PROCESS_BEGIN();
	printf("START3\n");
	leds_toggle(LEDS_GREEN);
	leds_toggle(LEDS_BLUE);
	leds_toggle(LEDS_RED);
    while(1) {
        etimer_set(&timer, 2*CLOCK_CONF_SECOND);
		
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
		
        printf("Blink... (state %0.2X).\r\n", leds_get());
		printf("%d\n", x);
		
		leds_toggle(LEDS_RED);
		
		if(x==2 || x==4 || x==6 || x==0){
			printf("green\n");
			leds_toggle(LEDS_GREEN);
		}
		if(x==4 || x==0){
			printf("blue\n");
			leds_toggle(LEDS_BLUE);
		}
		x = (x+1)%8;
    }
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
