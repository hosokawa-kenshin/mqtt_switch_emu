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
#define DEC 10

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

char *sub_topic            = "cmd/pinot/meterlight";
char *sub_topic2            = "cmd/pinot/meter2light";
char *sub_topic3            = "cmd/pinot/meter3light";

char *pub_topic            = "dt/pinot/meter";
char *pub_topic2           = "dt/pinot/meter2";
char *pub_topic3           = "dt/pinot/meter3";

//Brokerとの接続成功時に実行されるcallback関数
void on_connect(struct mosquitto *mosq, void *obj, int result){
  mosquitto_subscribe(mosq, NULL, sub_topic, RETAIN);
  mosquitto_subscribe(mosq, NULL, sub_topic2, RETAIN);
  mosquitto_subscribe(mosq, NULL, sub_topic3, RETAIN);
}

void display_sub_log(const struct mosquitto_message *message){
  printf("\n\nsub_topic: %s\nsub_message: ", message->topic);
  fwrite(message->payload, 1, message->payloadlen, stdout);
  fflush(stdout);
}

void display_pub_log(char* publish_topic, char* pub_message){
  printf("\nSUCCESS Change!\n");
  printf("pub_topic: %s\npub_message: %s\n\n", publish_topic, pub_message);
  fflush(stdout);
}

//Brokerからのメッセージ受信時に実行されるcallback関数
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message){
  if(message->payloadlen){
    if (strcmp(message->payload,"ON") == 0) {
      display_sub_log(message);
    } else {
      printf("The command %p is not defined.\n", message->payload);
    }

  } else {
    printf("%s (null)\n", message->topic);
  }
}

int main() {
  char *id            = "switch1";
  char *host          = "localhost";
  int   port          = 1883;
  int   keepalive     = 60;
  bool clean_session  = true;
  struct mosquitto *mosq = NULL;

  int num = 0;
  int num2 = 26;
  int num3 = 20;
  bool decrease = false;
  bool decrease2 = false;
  bool decrease3 = false;

  char data[3];
  char data2[3];
  char data3[3];
  
  mosquitto_lib_init();
  mosq = mosquitto_new(id, clean_session, NULL);

  if(!mosq){
    fprintf(stderr, "Cannot create mosquitto object\n");
    mosquitto_lib_cleanup();
    return EXIT_FAILURE;
  }

  mosquitto_connect_callback_set(mosq, on_connect);
  mosquitto_message_callback_set(mosq, on_message);

  if(mosquitto_connect_bind(mosq, host, port, keepalive, NULL)){
    fprintf(stderr, "failed to connect broker.\n");
    mosquitto_lib_cleanup();
    return EXIT_FAILURE;
  }

  mosquitto_loop_start(mosq);

  while(1){
    if(decrease){
      num-=25;
      if(num == 0)decrease = false;
    }else{
      num+=10;
      if(num >= 200)decrease = true;
    } 

    if(decrease2){
      num2-=1;
      if(num2 == 25)decrease2 = false;
    }else{
      num2+=1;
      if(num2 >= 30)decrease2 = true;
    }

    if(decrease3){
      num3-=5;
      if(num3 == 0)decrease3 = false;
    }else{
      num3+=10;    
      if(num3 >= 100)decrease3 = true;
    }

    itoa(num,data,DEC);
    itoa(num2,data2,DEC);
    itoa(num3,data3,DEC);

    mosquitto_publish(mosq, NULL, pub_topic, strlen(data), data, QUALITY_OF_SERVICE, RETAIN);
    mosquitto_publish(mosq, NULL, pub_topic2, strlen(data2), data2, QUALITY_OF_SERVICE, RETAIN);
    mosquitto_publish(mosq, NULL, pub_topic3, strlen(data3), data3, QUALITY_OF_SERVICE, RETAIN);

    sleep(1);
  }

  return EXIT_SUCCESS;
}