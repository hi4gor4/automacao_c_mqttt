#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "credentials.h"
#include "mqtt.h"
#include "credentials.h"
#include <wiringPi.h>
#include "MQTTClient.h"


#define TOUT_TO_PUBLISH 5000

#define PIN_BTN1 27
#define PIN_BTN2 17
#define PIN_BTN3 16
#define PIN_LUZ1 26
#define LED1 22
#define LED2 6
#define DHT 23


#define TOPICLAMPADA1 "hiago23rangel@gmail.com/luz1"
#define TOPICLAMPADA2 "hiago23rangel@gmail.com/luz2"
#define TOPICMAX "hiago23rangel@gmail.com/max"
#define TOPICMIN "hiago23rangel@gmail.com/min"
#define TOPICTEMP "hiago23rangel@gmail.com/temp"
#define TOPICACTIVATE "hiago23rangel@gmail.com/alarm1"
#define TOPICALARM "hiago23rangel@gmail.com/alarm2"

//Variaveis globais
int luz1 = 0;
int luz2 = 0;
int max = 0;
int min = 0;
int seguranca = 0;

MQTTClient client;

/* Subscribed MQTT topic listener function. */

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message){
    if(message) {
        printf("Message arrived\n");
        printf("  topic: %s\n", topicName);
        printf("  message: ");
        printf("%s\n", (char*)message->payload);
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);

    return 1;
}

int verify_topics(void *context, char *topicName, int topicLen, MQTTClient_message *message){
    if(message){
        printf("Message arrived\n");
        printf("  topic: %s\n", topicName);
        printf("  message: ");
        printf("%s\n", (char*)message->payload);
        char* payload = message->payload;
        if(!strcmp(TOPICLAMPADA1, topicName)){
            luz1 =atoi(payload);
        }else if (!strcmp(TOPICLAMPADA2,topicName)){
            luz2 = atoi(payload);
        }else if(!strcmp(TOPICMIN, topicName)){
            min = atoi(payload);
        }else if(!strcmp(TOPICMAX, topicName)){
            max = atoi(payload);
        }else if(!strcmp(TOPICACTIVATE, topicName)){
            seguranca = atoi(payload);
        }
    }
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}
void connlost(void *context, char *cause){
    printf("Connection lost\n");
    if (cause)
        printf("Reason is : %s\n", cause);
    MQTTDisconnect();
    /* Force to reconnect! */
    MQTTBegin();
}

void MQTTSubscribe(const char* topic)
{
    printf("Subscribing to topic %s for client %s using QoS%d\n\n", 
        topic, CLIENTID, QOS);
    MQTTClient_subscribe(client, topic, QOS);
}

void MQTTPublish(const char* topic, char* message){
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    pubmsg.payload = message;
    pubmsg.payloadlen = (int)strlen(message);
    pubmsg.qos = QOS;
    pubmsg.retained = 1;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    /*printf("Waiting for publication of message: %s\n"
            "topic: %s\n client: %s\n",
            message, TOPIC, CLIENTID);*/
    int rc = MQTTClient_waitForCompletion(client, token, 1000);
    /*printf("Message with delivery token %d delivered\n", token);*/
}

void MQTTDisconnect(){
    MQTTClient_disconnect(client, TIMEOUT);
    MQTTClient_destroy(&client);
}

void MQTTBegin(){
    int rc = -1;
    printf("Initializing MQTT...\n");
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = KEEP_ALIVE;
    conn_opts.cleansession = 1;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;
    MQTTClient_create(&client, BROKER_ADDR, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);

    /* Set connection, subscribe and publish callbacks. */
    MQTTClient_setCallbacks(client, NULL, connlost, verify_topics, NULL);

    while ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        sleep(TIMEOUT / 1000); 
    }
}

/* This program connects to https://www.maqiatto.com/
 * Periodically publishes test messages with your credentials.
 */

int main(int argc, char* argv[]){
    MQTTBegin();

    MQTTSubscribe(TOPICLAMPADA1);
    MQTTSubscribe(TOPICLAMPADA2);
    MQTTSubscribe(TOPICMAX);
    MQTTSubscribe(TOPICMIN);
    MQTTSubscribe(TOPICALARM);
    wiringPiSetupGpio();
    pinMode(PIN_LUZ1, OUTPUT);
    digitalWrite(PIN_LUZ1, 1);
    pinMode(PIN_BTN1, INPUT);
    pullUpDnControl(PIN_BTN1, PUD_UP);
    pinMode(LED1, OUTPUT);
    digitalWrite(LED1, 1);
    pinMode(PIN_BTN2, INPUT);
    pullUpDnControl(PIN_BTN2, PUD_UP);
    pinMode(PIN_BTN3, INPUT);
    pullUpDnControl(PIN_BTN3, PUD_UP);
    pinMode(LED2, OUTPUT);

    while(1){
        if(digitalRead(PIN_BTN1) == LOW){
            if(luz1){
                MQTTPublish(TOPICLAMPADA1, "0");
            }else{
                MQTTPublish(TOPICLAMPADA1, "1");
            
            }
            while(digitalRead(PIN_BTN1) == LOW); // aguarda enquato chave ainda esta pressionada           
            delay(1000);
            
        }
        if(digitalRead(PIN_BTN2) == LOW){
            if(luz2){
                MQTTPublish(TOPICLAMPADA2, "0");
            }else{
                MQTTPublish(TOPICLAMPADA2, "1");
            }
            while(digitalRead(PIN_BTN2) == LOW); // aguarda enquato chave ainda esta pressionada           
            delay(1000);
        }

        //Verifica estados de pinos
        if(luz1){
            digitalWrite(PIN_LUZ1, LOW);
        }else{
            digitalWrite(PIN_LUZ1, HIGH);
        }
        if (luz2){
            digitalWrite(LED1, HIGH);
        }else{
            digitalWrite(LED1, LOW);
        }

        if(digitalRead(PIN_BTN3) == LOW){
            if(seguranca == '1'){
                MQTTPublish(TOPICALARM, "1");
                digitalWrite(LED2, HIGH);
            }else{
                MQTTPublish(TOPICALARM, "0");
                digitalWrite(LED2, LOW);
            }
            while(digitalRead(PIN_BTN3) == LOW); // aguarda enquato chave ainda esta pressionada           
            delay(1000);
        }
    };

    MQTTDisconnect();

    return 0;
}