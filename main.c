#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "credentials.h"
#include "mqtt.h"
#include "credentials.h"
#include <wiringPi.h>
#include "MQTTClient.h"

typedef enum {false, true} bool;

#define TOUT_TO_PUBLISH 5000

#define PIN_BTN1 27
#define PIN_LUZ1 26


#define TOPICLAMPADA1 "hiago23rangel@gmail.com/luz1"
#define TOPICLAMPADA2 "hiago23rangel@gmail.com/luz2"

//Variaveis globais
bool luz1 = false;
bool luz2 = false;

MQTTClient client;

/* Subscribed MQTT topic listener function. */
bool chartobool(char* msg){
    if(msg == "true"){
        return true;
    }else{
        return false;
    }
}
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
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
        printf(strcmp("hiago23rangel@gmail.com/luz1", topicName));
        if(strcmp("hiago23rangel@gmail.com/luz1", topicName) == 1){
            printf("entraste");
            luz1 = chartobool(payload);
    
    }
    }
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}
void connlost(void *context, char *cause)
{
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

void MQTTPublish(const char* topic, char* message)
{
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

void MQTTDisconnect()
{
    MQTTClient_disconnect(client, TIMEOUT);
    MQTTClient_destroy(&client);
}

void MQTTBegin()
{
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

int main(int argc, char* argv[])
{
    MQTTBegin();

    MQTTSubscribe(TOPICLAMPADA1);
    MQTTSubscribe(TOPICLAMPADA2);
    wiringPiSetupGpio();
    pinMode(PIN_LUZ1, OUTPUT);
    digitalWrite(PIN_LUZ1, 1);
    pinMode(PIN_BTN1, INPUT);
    pullUpDnControl(PIN_BTN1, PUD_UP);
    while(1) 
    {
        if(digitalRead(PIN_BTN1) == LOW){
            if(luz1){
                MQTTPublish(TOPICLAMPADA1, "false");
            }else{
                MQTTPublish(TOPICLAMPADA1, "true");
            
            }
            while(digitalRead(PIN_BTN1) == LOW); // aguarda enquato chave ainda esta pressionada           
            delay(1000);
            
        }

        //Verifica estados de pinos
        if(luz1){
            digitalWrite(PIN_LUZ1, LOW);

        }else{
            digitalWrite(PIN_LUZ1, HIGH);
        }

    
    
    
    };

    MQTTDisconnect();

    return 0;
}