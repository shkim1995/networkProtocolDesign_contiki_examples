#include "contiki.h"
#include "net/rime.h"
#include "random.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "dev/light-sensor.h"
#include "dev/cc2420.h"
#include "sys/node-id.h"

#include <stdio.h>
#include <string.h>

#define DST 4
#define SRC 1
#define TTL 4

#define DEBUG_PATH 1
#define DEBUG_DUP_TTL 1

struct message{
	uint16_t src;
	uint16_t dst;
	uint16_t seqno;
	uint16_t ttl;
#if DEBUG_PATH
	uint16_t path[TTL+1];
	uint16_t head;
#endif
};

int seqBefore = 0;
struct message msg;
/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process3, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process3);
/*---------------------------------------------------------------------------*/

#if DEBUG_PATH
static void print_path(uint16_t *pt, uint16_t len){
	int i;
	printf("Path recieved : ");
	for(i=0; i<=len; i++){
		printf("%d ", *pt);
		pt++;
	}
	printf("\n");
}
#endif


static void
broadcast_recv(struct broadcast_conn *c, const rimeaddr_t *from)
{
	if(msg.src == node_id) return; // src node does nothing

	memcpy(&msg, packetbuf_dataptr(), sizeof(struct message));
	
	if(msg.seqno>seqBefore){ //no duplication
		
		seqBefore = msg.seqno;
		
		if(msg.ttl==0){ // ttl out : do nothing
#if DEBUG_DUP_TTL
			printf("DEBUG : ttl out of time \n");
#endif
		}
		
		else{
			
			if(msg.dst == node_id){ // destination node
				printf("broadcast message #%d recieved at %d\n", msg.seqno, node_id);				
#if DEBUG_PATH
				print_path(&(msg.path[0]), msg.head);
#endif
			}

			else{ // forwarding node
				printf("forwarding message #%d\n", msg.seqno);				
				msg.ttl = msg.ttl - 1;
#if DEBUG_PATH
				msg.head = msg.head + 1;
				msg.path[msg.head] = node_id;
#endif
				packetbuf_copyfrom(&msg, sizeof(struct message));
				process_post_synch(&example_broadcast_process3, PROCESS_EVENT_CONTINUE, NULL);
			}
		}
	}
	else{ //duplication : do nothing
#if DEBUG_DUP_TTL
		printf("DEBUG : duplication \n");
#endif
	}

}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_broadcast_process3, ev, data)
{
	static struct etimer et;
	static uint8_t seqno = 1;

	struct message msg;

	/* buiid a message */
	msg.src = SRC;
	msg.dst = DST;	
	msg.ttl = TTL;
	msg.seqno = seqno;
#if DEBUG_PATH
	msg.head = 0;
	msg.path[msg.head] = node_id;
#endif
	PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

	PROCESS_BEGIN();

	broadcast_open(&broadcast, 129, &broadcast_call);

	//first packet
	packetbuf_copyfrom(&msg, sizeof(struct message));
	broadcast_send(&broadcast);
	printf("broadcast message #%d sent from %d to %d\n", msg.seqno, msg.src, msg.dst);
	seqno++;
	
	while(1) {
		
		if(node_id == 1){ // if sender
			etimer_set(&et, CLOCK_SECOND * 4 + random_rand() % (CLOCK_SECOND * 4));
		}
		
		PROCESS_WAIT_EVENT();
		
		//rcver & forwarding nodes : continue event
		if(ev==PROCESS_EVENT_CONTINUE){
			broadcast_send(&broadcast);
		}
		
		//sender : sende next packet
		if(ev==PROCESS_EVENT_TIMER){
			packetbuf_copyfrom(&msg, sizeof(struct message));
			broadcast_send(&broadcast);
			printf("broadcast message #%d sent from %d to %d\n", msg.seqno, msg.src, msg.dst);
			seqno++;
		}
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
