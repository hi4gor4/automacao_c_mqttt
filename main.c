#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include "mqtt.h"
#include "credentials.h"

#define TOUT_TO_PUBLISH 5000
#define PIN_BTN1 27
#define PIN_LUZ1 26

/* This program connects to https://www.maqiatto.com/
 * Periodically publishes test messages with your credentials.
 */

int main(int argc, char* argv[])
{
    MQTTBegin();

    MQTTSubscribe(TOPIC);
    wiringPiSetupGpio();
    char* json;
    pinMode(PIN_LUZ1, OUTPUT);
    digitalWrite(PIN_LUZ1, 1);
    delay(5000);
    pinMode(PIN_BTN1, INPUT);
    pullUpDnControl(PIN_BTN1, PUD_UP);
    
    int estado_luz1 = 0;

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
        // sprintf(json, "{ \"id\": %d, \"event\": \"%s\", \"data\": { \"state\": %s }};", 1, "OnChangeState", "true");
        // printf("batata %s", json);
        // printf(json)
        MQTTPublish(TOPIC,  "{ \"id\": 1, \"event\": \"Okdljwenhf\", \"data\": { \"state\": turedsdfvs }};");
        delay(3000);

    
    MQTTDisconnect();

    return 0;
}
