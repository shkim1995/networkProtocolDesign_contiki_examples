#include "sys/node-id.h"
	

struct message{
	uint16_t src;
	uint16_t dst;
	uint16_t seqno;
	uint16_t ttl;
};

