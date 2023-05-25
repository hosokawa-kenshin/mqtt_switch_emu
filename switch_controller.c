#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <mosquitto.h>

#define MAX_PAYLOAD_SIZE 256
//QUALITY_OF_SERVICE メッセージに使用されるサービス品質を示す整数値 0，1，2
#define QUALITY_OF_SERVICE 0
//RETAIN メッセージを保持に関するbool
#define RETAIN false

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

char *pub_topic   = "bot/switch1";
int state         = 0;

void change_state(struct mosquitto *mosq){
  char *pub_message = (char *)malloc(sizeof(char) * MAX_PAYLOAD_SIZE);
  char answer[10];
  printf("Change the state? y/n\n>");
  fgets(answer,10,stdin);
  if (strncmp(answer,"y",1) == 0){
    strncpy(pub_message,"CHANGE",strlen("CHANGE") + 1);
    mosquitto_publish(mosq, NULL, pub_topic, strlen(pub_message), pub_message, QUALITY_OF_SERVICE, RETAIN);
    printf("SUCCESS Change!\n");
  } else {
    printf("There is no change.\n");
  }
  free(pub_message);
  sleep(1);
}

int main() {
  char *id            = "controller";
  char *host          = "localhost";
  int   port          = 1883;
  int   keepalive     = 60;
  bool clean_session  = true;
  struct mosquitto *mosq = NULL;
  
  mosquitto_lib_init();
  mosq = mosquitto_new(id, clean_session, NULL);

  if (!mosq) {
    fprintf(stderr, "Cannot create mosquitto object\n");
    mosquitto_lib_cleanup();
    return EXIT_FAILURE;
  }

  if (mosquitto_connect_bind(mosq, host, port, keepalive, NULL)) {
    fprintf(stderr, "failed to connect broker.\n");
    mosquitto_lib_cleanup();
    return EXIT_FAILURE;
  }
  while(1){
    change_state(mosq);
  }
  return EXIT_SUCCESS;
}