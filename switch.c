#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
// MQTTライブラリのヘッダファイルをインクルードする
#include <mosquitto.h>

#define MAX_PAYLOAD_SIZE 256

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

char *topic   = "openHAB/bot/switch1";
char *message = NULL;
int state     = 0;
int connect_desire = TRUE;


//Brokerとの接続成功時に実行されるcallback関数
void on_connect(struct mosquitto *mosq, void *obj, int result)
{
    mosquitto_subscribe(mosq, NULL, topic, 0);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
    if(message->payloadlen){
        printf("%s ", message->topic);
        if (strcmp(message->payload,"ON")){
            state = 0;
        }else{
            state = 1;
        }
        fwrite(message->payload, 1, message->payloadlen, stdout);
        printf(" state:%d\n",state);
    }else{
        printf("%s (null)\n", message->topic);
    }

    fflush(stdout);
}

int main() {
    char *id            = "bot/switch";
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