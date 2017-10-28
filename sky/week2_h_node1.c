
#include "string.h"
#include "contiki.h"
#include "net/rime.h"
#include "random.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include <stdio.h>
#include <stdlib.h>

#define WAKEUP_TIME 10
#define WAIT_TIME 5

int seqToSend = 1;
char buf[16];

/*---------------------------------------------------------------------------*/

PROCESS(broadcast_process, "Broadcast");
AUTOSTART_PROCESSES(&broadcast_process);

/*---------------------------------------------------------------------------*/	

static void
broadcast_recv(struct broadcast_conn *c, const rimeaddr_t *from)
{
	char toRecieve[16];
	char* recieved = (char *)packetbuf_dataptr();
	
	itoa(seqToSend+1, toRecieve, 10);

	//drop if replicated
	if(strcmp(toRecieve, recieved)!=0){
		printf("broadcast duplicated at node 2: %s\n", recieved);
	}

	//continue the process if not replicated
	else{
	    printf("broadcast received at node 2: %s\n", recieved);
		process_post_synch(&broadcast_process, PROCESS_EVENT_CONTINUE, NULL);
	}
}

static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;


/*---------------------------------------------------------------------------*/

PROCESS_THREAD(broadcast_process, ev, data)
{
    static struct etimer et;

    PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
    PROCESS_BEGIN();

    broadcast_open(&broadcast, 129, &broadcast_call);
	
	//send the initial packet
	itoa(seqToSend, buf, 10);
    packetbuf_copyfrom(buf, 16);
	//sprintf(buf, "%d\0", seqToSend);
    broadcast_send(&broadcast);
	printf("Broadcast sent from node 1 : seq = %s\n", buf);

    while(1) {
		
		//wait until timeout event
        etimer_set(&et, CLOCK_SECOND * WAKEUP_TIME);

        PROCESS_WAIT_EVENT();
		
		//if timeout, retransmit the packet
		if(ev == PROCESS_EVENT_TIMER){
			
			itoa(seqToSend, buf, 10);
    		packetbuf_copyfrom(buf, 16);
			broadcast_send(&broadcast);
			printf("Broadcast re-sent from node 1 : seq = %s\n", buf);
		}

		//if recieved correctly, send the next packet
		if(ev == PROCESS_EVENT_CONTINUE){
			
        	etimer_set(&et, CLOCK_SECOND * WAIT_TIME);
			PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
			
			//update seq
			seqToSend += 2;
			
			itoa(seqToSend, buf, 10);
    		packetbuf_copyfrom(buf, 16);
    		broadcast_send(&broadcast);
			printf("Broadcast sent from node 1 : seq = %s\n", buf);
		}

    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
