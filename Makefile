all: main.c mqtt.c
	gcc -o mqttConnect main.c  -I. -lcurl -lpaho-mqtt3c -lpthread -lwiringPi