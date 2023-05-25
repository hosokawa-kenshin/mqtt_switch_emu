#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <mosquitto.h>

#define MAX_PAYLOAD_SIZE 256
//QUALITY_OF_SERVICE メッセージに使用されるサービス品質を示す整数値 0，1，2
#define QUALITY_OF_SERVICE 1
//RETAIN メッセージを保持に関するbool
#define RETAIN true

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

char *sub_topic   = "bot/switch";
char *pub_topic   = "openhab/bot/switch";
int state         = 0;

//Brokerとの接続成功時に実行されるcallback関数
void on_connect(struct mosquitto *mosq, void *obj, int result){
  mosquitto_subscribe(mosq, NULL, sub_topic, RETAIN);
}

//Brokerからのメッセージ受信時に実行されるcallback関数
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message){
  char *pub_message = (char *)malloc(sizeof(char) * MAX_PAYLOAD_SIZE);
  if(message->payloadlen){
    if(strcmp(message->payload,"ON")){
      state = 0;
      strncpy(pub_message,"OFF",strlen("OFF") + 1);
      mosquitto_publish(mosq, NULL, pub_topic, strlen(pub_message), pub_message, QUALITY_OF_SERVICE, RETAIN);
    }else{
      state = 1;
      strncpy(pub_message,"ON",strlen("ON") + 1);
      mosquitto_publish(mosq, NULL, pub_topic, strlen(pub_message), pub_message, QUALITY_OF_SERVICE, RETAIN);
    }
    printf("sub_topic: %s\nsub_message: ", message->topic);
    fwrite(message->payload, 1, message->payloadlen, stdout);
    printf("\npub_topic: %s\npub_message: %s\nstate: %d\n\n", pub_topic, pub_message, state);
  }else{
    printf("%s (null)\n", message->topic);
  }
  fflush(stdout);
  free(pub_message);
}

int main() {
  char *id            = "switch1";
  char *host          = "localhost";
  int   port          = 1883;
  int   keepalive     = 60;
  bool clean_session  = true;
  struct mosquitto *mosq = NULL;
  
  mosquitto_lib_init();
  mosq = mosquitto_new(id, clean_session, NULL);

  if(!mosq){
    fprintf(stderr, "Cannot create mosquitto object\n");
    mosquitto_lib_cleanup();
    return(EXIT_FAILURE);
  }

  mosquitto_connect_callback_set(mosq, on_connect);
  mosquitto_message_callback_set(mosq, on_message);

  if(mosquitto_connect_bind(mosq, host, port, keepalive, NULL)){
    fprintf(stderr, "failed to connect broker.\n");
    mosquitto_lib_cleanup();
    return(EXIT_FAILURE);
  }

  mosquitto_loop_forever(mosq, -1, 1);

  return(EXIT_SUCCESS);
}