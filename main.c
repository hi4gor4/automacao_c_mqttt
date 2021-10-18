// Hiago


#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include "mqtt.h"
#include "credentials.h"
#include "MQTTClient.h"

#define TOUT_TO_PUBLISH 5000
#define PIN_BTN1 27
#define PIN_LUZ1 26



bool estado_luz1 =  false;
bool estado_luz1 =  false;

MQTTClient client;

/* Subscribed MQTT topic listener function. */
void trataSubscribe(char* mensagem){
    if (mensagem[0] == 1){
        //Define a alteração do estado de funcionamento das lampadas
        if (estado_luz1){
            estado_luz1 = false;
        }else{
            estado_luz1 = true;
        }

         if (estado_luz2){
            estado_luz2 = false;
        }else{
            estado_luz2 = true;
        }

    }

    //aqui eu vou tratar o recebimento da mensaagem do topico
    /*
    1- Acionamento ou desacionamento da lampada

    */


}
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    if(message) {
        printf("Message arrived\n");
        printf("  topic: %s\n", topicName);
        printf("  message: ");
        printf("%s\n", (char*)message->payload);
    }
    trataSubscribe(message)
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
    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
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
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, NULL);

    while ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        sleep(TIMEOUT / 1000); 
    }
}




int main(int argc, char* argv[])
{
    MQTTBegin();

    MQTTSubscribe(TOPIC);
    wiringPiSetupGpio();
    // pinMode(PIN_LUZ1, OUTPUT);
    // digitalWrite(PIN_LUZ1, 1);
    // delay(5000);
    // pinMode(PIN_BTN1, INPUT);
    // pullUpDnControl(PIN_BTN1, PUD_UP);
    

    /*while(1){
        if(digitalRead(PIN_BTN1) == LOW){
            if(estado_luz1 ==0){
                estado_luz1 =1;
                digitalWrite(PIN_LUZ1, HIGH);
                delay(1000);
                sprintf(json, " { \"id\": %d, \"event\": \"%s\", \"data\": { \"state\": %s });", 1, "OnChangeState", "true");
                MQTTPublish(TOPIC, json);
            }else{
                estado_luz1 = 0;
                digitalWrite(PIN_LUZ1, LOW);
                delay(500);
                sprintf(json, " { \"id\": %d, \"event\": \"%s\", \"data\": { \"state\": %s });", 1, "OnChangeState", "false");
                MQTTPublish(TOPIC, json);
            }
            while(digitalRead(PIN_BTN1) == LOW); // aguarda enquato chave ainda esta pressionada           
            delay(2000);
            
        }*/
        // printf("batata %s", json);
        // printf(json)
    while (1)
    {
        printf(estado_luz1)        
        delay(3000);
    
    }
        

    
    MQTTDisconnect();

    return 0;
}
