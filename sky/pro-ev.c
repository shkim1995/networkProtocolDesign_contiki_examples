#include "contiki.h"
#include <stdio.h>

PROCESS(example_process,"Example process");
PROCESS(sync_process, "Sync process");

AUTOSTART_PROCESSES(&example_process);

static char msg1[] = "Sync";
static char msg2[] = "Async";
static char msg3[] = "start";

static void
example_function(void)
{
  printf("3. Start sync_process in example_func\n");
  process_start(&sync_process, msg3);

  printf("5. Sync process will be posted\n");
  process_post_synch(&sync_process, PROCESS_EVENT_CONTINUE, msg1);
  printf("7. Sync_post is done\n\n");

  printf("8. Async proccess will be posted\n");
  process_post(&example_process, PROCESS_EVENT_CONTINUE, msg2);
  printf("9. Async_post is done\n\n");
}

PROCESS_THREAD(example_process, ev, data)
{
  char *line;

  PROCESS_BEGIN();

  printf("1. Example process begins\n");

  printf("2. Call example_function\n\n");
  example_function();
  printf("10. example_function done\n\n");

  line = (char*) data;
  printf("11. PROCESS_WAIT_EVENT() will be operated, ev: %d, data: %s\n", ev, line);
  PROCESS_WAIT_EVENT();

  line = (char*) data;
  printf("12. Example process got ev: %d, data: %s\n", ev, data);

  PROCESS_END();
}

PROCESS_THREAD(sync_process, ev, data)
{
  char *line;

  PROCESS_BEGIN();

  line = (char*) data;
  printf("4. Sync process begins, ev: %d, data: %s\n\n", ev, line);

  PROCESS_WAIT_EVENT();

  line = (char*) data;
  printf("6. Sync process got ev: %d, data: %s\n", ev, line);

  PROCESS_END();
}






