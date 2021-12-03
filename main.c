#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "credentials.h"
#include "mqtt.h"
#include "credentials.h"
#include <wiringPi.h>
#include "MQTTClient.h"
#include <time.h>

#define TOPICLAMPADA1 "hiago23rangel@gmail.com/luz1"
#define TOPICLAMPADA2 "hiago23rangel@gmail.com/luz2"
#define TOPICMAX "hiago23rangel@gmail.com/max"
#define TOPICMIN "hiago23rangel@gmail.com/min"
#define TOPICTEMP "hiago23rangel@gmail.com/temp"
#define TOPICACTIVATE "hiago23rangel@gmail.com/alarm1"
#define TOPICALARM "hiago23rangel@gmail.com/alarm2"

#define TOUT_TO_PUBLISH 5000

#define PIN_BTN1 27
#define PIN_BTN2 17
#define PIN_BTN3 16
#define PIN_BTN4 5
#define PIN_BTN5 23
#define PIN_LUZ1 26
#define LED1 22
#define LED2 6
#define LEDAR 24

//Variaveis globais
int luz1 = 0;
int luz2 = 0;
int max = 28;
int min = 10;
int temp = 25;
int seguranca = 0;
int atual_luz1 = 0;
int atual_luz2 = 0;
int atual_max = 28;
int atual_min = 10;
int autal_seguraca = 25;

MQTTClient client;

FILE *arquivo;

time_t rawtime;
struct tm *ptm;
int initday;
int initmon;

/* Subscribed MQTT topic listener function. */
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    if (message)
    {
        printf("Message arrived\n");
        printf("  topic: %s\n", topicName);
        printf("  message: ");
        printf("%s\n", (char *)message->payload);
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

int verify_topics(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    if (message)
    {
        printf("Message arrived\n");
        printf("  topic: %s\n", topicName);
        printf("  message: ");
        printf("%s\n", (char *)message->payload);
        char *payload = message->payload;

        if (!strcmp(TOPICLAMPADA1, topicName))
        {
            luz1 = atoi(payload);
            printf("%d", min);
        }
        else if (!strcmp(TOPICLAMPADA2, topicName))
        {
            luz2 = atoi(payload);
        }
        else if (!strcmp(TOPICMIN, topicName))
        {
            min = atoi(payload);
        }
        else if (!strcmp(TOPICMAX, topicName))
        {
            max = atoi(payload);
        }
        else if (!strcmp(TOPICACTIVATE, topicName))
        {
            seguranca = atoi(payload);
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

void MQTTSubscribe(const char *topic)
{
    printf("Subscribing to topic %s for client %s using QoS%d\n\n",
           topic, CLIENTID, QOS);
    MQTTClient_subscribe(client, topic, QOS);
}

void MQTTPublish(const char *topic, char *message)
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

void tempo()
{
    int day, mon;
    rawtime = time(NULL);
    ptm = localtime(&rawtime);
    day = ptm->tm_mday;
    mon = ptm->tm_mon;

    if (ptm == NULL)
    {
        printf("Não foi possivel pegar o localtime");
    }

    if (initday - day < 0)
    {
        fclose(arquivo);
        if (remove("old.txt") == 0)
        {
            printf("Deletou Escrevelog antigo");
        }
        if (rename("Escrevelog.txt", "old.txt") == 0)
        {
            arquivo = fopen("log.txt", "a");
            printf("Escrevelog novo inciado");
        }
        initday = ptm->tm_mday;
    }
    else if (initday - day > 0)
    {
        //Verifica se é um outro mês
        if (initmon - mon < 0)
        {
            fclose(arquivo);
            if (remove("old.txt") == 0)
            {
                printf("Deletou Escrevelog antigo");
            }
            if (rename("Escrevelog.txt", "old.txt") == 0)
            {
                arquivo = fopen("log.txt", "a");
                printf("Escrevelog novo inciado");
            }
            initmon = ptm->tm_mon;
            initday = ptm->tm_mday;
        }
        else
        {
            //Verifica se é um novo ano
            if (initmon - ptm->tm_mon > 0)
            {
                fclose(arquivo);
                if (remove("old.txt") == 0)
                {
                    printf("Deletou Escrevelog antigo");
                }
                if (rename("Escrevelog.txt", "old.txt") == 0)
                {
                    arquivo = fopen("log.txt", "a");
                    printf("Escrevelog novo inciado");
                }
                initmon = ptm->tm_mon;
                initday = ptm->tm_mday;
            }
        }
    }

    /* This program connects to https://www.maqiatto.com/
 * Periodically publishes test messages with your credentials.
 */
}

int main(int argc, char *argv[])
{

    arquivo = fopen("log.txt", "a");
    //Inicia o horario
    rawtime = time(NULL);
    ptm = localtime(&rawtime);
    initday = ptm->tm_mday;
    initmon = ptm->tm_mon;

    //Inicialização do MQTT e fazendo subriscribe nos topicos necessarios
    MQTTBegin();
    MQTTSubscribe(TOPICLAMPADA1);
    MQTTSubscribe(TOPICLAMPADA2);
    MQTTSubscribe(TOPICMAX);
    MQTTSubscribe(TOPICMIN);
    MQTTSubscribe(TOPICTEMP);
    MQTTSubscribe(TOPICALARM);
    MQTTSubscribe(TOPICACTIVATE);

    char str[10];

    //Configurando os pinos da raspberry
    wiringPiSetupGpio();
    pinMode(PIN_LUZ1, OUTPUT);
    digitalWrite(PIN_LUZ1, 1);
    pinMode(LED1, OUTPUT);
    digitalWrite(LED1, 1);
    pinMode(LED2, OUTPUT);
    digitalWrite(LED2, 1);
    digitalWrite(LEDAR, OUTPUT);
    digitalWrite(LEDAR, HIGH);
    pinMode(PIN_BTN1, INPUT);
    pullUpDnControl(PIN_BTN1, PUD_UP);
    pinMode(PIN_BTN2, INPUT);
    pullUpDnControl(PIN_BTN2, PUD_UP);
    pinMode(PIN_BTN3, INPUT);
    pullUpDnControl(PIN_BTN3, PUD_UP);
    pinMode(PIN_BTN4, INPUT);
    pullUpDnControl(PIN_BTN4, PUD_UP);
    pinMode(PIN_BTN5, INPUT);
    pullUpDnControl(PIN_BTN5, PUD_UP);

    if (arquivo == NULL)
    {
        printf("Não foi possivel criar Escrevelog");
    }

    while (1)
    {

        tempo();
        if (luz1 != atual_luz1)
        {
            fprintf(arquivo, "%d %d %d Estado da luz 1 alterado par: %d\n", ptm->tm_hour, ptm->tm_min, ptm->tm_sec, luz1);
            atual_luz1 = luz1;
        }
        if (luz2 != atual_luz2)
        {
            fprintf(arquivo, "%d %d %d Estado da luz 2 alterado para: %d\n", ptm->tm_hour, ptm->tm_min, ptm->tm_sec, luz2);
            atual_luz2 = luz2;
        }
        if (max != atual_max)
        {
            fprintf(arquivo, "%d %d %d Temperatura maxima atualizada para: %d\n", ptm->tm_hour, ptm->tm_min, ptm->tm_sec, max);
            atual_max = max;
        }
        if (min != atual_min)
        {
            fprintf(arquivo, "%d %d %d Temperatura minima atualizada para: %d\n", ptm->tm_hour, ptm->tm_min, ptm->tm_sec, min);
            atual_min = min;
        }

        if (seguranca != seguranca)
        {
            fprintf(arquivo, "%d %d %d Estado de segurança alterado para: %d\n", ptm->tm_hour, ptm->tm_min, ptm->tm_sec, seguranca);
            autal_seguraca = seguranca;
        }

        if (digitalRead(PIN_BTN1) == LOW)
        {
            if (luz1)
            {
                MQTTPublish(TOPICLAMPADA1, "0");
                printf("Apertou botão");
                fprintf(arquivo, "%d %d %d Estado da luz 1 alterado par: %d\n", ptm->tm_hour, ptm->tm_min, ptm->tm_sec, luz1);
            }
            else
            {
                MQTTPublish(TOPICLAMPADA1, "1");
                printf("Apertou botão");
                fprintf(arquivo, "%d %d %d Estado da luz 1 alterado par: %d\n", ptm->tm_hour, ptm->tm_min, ptm->tm_sec, luz1);
            }
    
            while (digitalRead(PIN_BTN1) == LOW); // aguarda enquato chave ainda esta pressionada
            delay(1000);
        }
        if (digitalRead(PIN_BTN2) == LOW)
        {
            if (luz2)
            {
                MQTTPublish(TOPICLAMPADA2, "0");
            }
            else
            {
                MQTTPublish(TOPICLAMPADA2, "1");
            }
            while (digitalRead(PIN_BTN2) == LOW)
                ; // aguarda enquato chave ainda esta pressionada
            delay(1000);

            fprintf(arquivo, "%d %d %d Estado da luz 2 alterado para: %d\n", ptm->tm_hour, ptm->tm_min, ptm->tm_sec, luz2);
        }

        //Verifica estados de pinos
        if (luz1)
        {

            digitalWrite(PIN_LUZ1, LOW);
        }
        else
        {
            digitalWrite(PIN_LUZ1, HIGH);
        }
        if (luz2)
        {
            digitalWrite(LED1, HIGH);
        }
        else
        {
            digitalWrite(LED1, LOW);
        }

        //Controle do sistema de segurança
        if (digitalRead(PIN_BTN3) == LOW)
        {
            if (seguranca)
            {
                MQTTPublish(TOPICALARM, "1");
                digitalWrite(LED2, HIGH);
                if (arquivo != NULL)
                {
                    fprintf(arquivo, "Intruso detectado \n");
                }
            }
            while (digitalRead(PIN_BTN3) == LOW)
                ; // aguarda enquato chave ainda esta pressionada
            delay(1000);

            fprintf(arquivo, "%d %d %d Estado de segurança alterado para: %d\n", ptm->tm_hour, ptm->tm_min, ptm->tm_sec, seguranca);
        }
        if (!seguranca)
        {
            digitalWrite(LED2, LOW);
        }

        if (digitalRead(PIN_BTN4) == LOW)
        {
            temp += 1;
            int templ = temp;
            sprintf(str, "%d", templ);
            MQTTPublish(TOPICTEMP, str);
            while (digitalRead(PIN_BTN4) == LOW)
                ; // aguarda enquato chave ainda esta pressionada
            delay(1000);
            if (temp >= max)
            {
                digitalWrite(LEDAR, HIGH);
            }

            fprintf(arquivo, "%d %d %d Temperatura maxima atualizada para: %d\n", ptm->tm_hour, ptm->tm_min, ptm->tm_sec, max);
        }

        if (digitalRead(PIN_BTN5) == LOW)
        {
            temp -= 1;
            int templ = temp;
            sprintf(str, "%d", templ);
            MQTTPublish(TOPICTEMP, str);
            while (digitalRead(PIN_BTN5) == LOW)
                ; // aguarda enquato chave ainda esta pressionada
            delay(1000);
            if (temp < min)
            {
                digitalWrite(LEDAR, LOW);
            }
            fprintf(arquivo, "%d %d %d Temperatura minima atualizada para: %d\n", ptm->tm_hour, ptm->tm_min, ptm->tm_sec, min);
        }
    };
    fclose(arquivo);
    MQTTDisconnect();
    return 0;
}
