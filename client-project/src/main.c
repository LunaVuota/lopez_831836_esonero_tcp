/*
 * main.c
 *
 * TCP Server - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a TCP server
 * portable across Windows, Linux and macOS.
 */

#if defined WIN32
#include <winsock.h>
#else
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <stdio.h>
#include <stdlib.h>
#include "protocol.h"
#include <ctype.h>

#define NO_ERROR 0

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

void printCorrectUse() {
	printf("Uso corretto: [-s server] [-p port] -r \"type city\"\n");
	printf("-s server: optional, default %s\n", DEFAULT_SERVER);
	printf("-p port: optional, default %d\n", SERVER_PORT);
	printf("-r request: obbligatorio.\n");
}


int main(int argc, char *argv[]) {

#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif
	//initialize socket and set default args
	int my_socket;
	unsigned long int server_address_int = inet_addr(DEFAULT_SERVER);
	unsigned int port = SERVER_PORT;

	weather_request_t request;
	weather_response_t response;

	char* r_string = NULL;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-s") == 0) {
			if (++i < argc) {
				server_address_int = inet_addr(argv[i]);
			}
			else {
				printCorrectUse();
				return -1;
			}
		}
		else if (strcmp(argv[i], "-p") == 0) {
			if(++i < argc) {
				port = atoi(argv[i]);
			}
			else {
				printCorrectUse();
				return -1;
			}
		}
		else if (strcmp(argv[i], "-r") == 0) {
			if (++i < argc) {
				//request.type = argv[++i][0];
				//strcpy(request.city, argv[i]+2);
				r_string = argv[i];
			}
		}
		else {
			printf("Operazione sconosciuta.\n");
			printCorrectUse();
			return -1;
		}
	}

	if(r_string == NULL) {
		printf("Parametro -r obbligatorio");
		return -1;
	}
	else {
		if (strlen(r_string) < 3) {
			printCorrectUse();
			return -1;
		}
	}
	request.type = r_string[0];
	strcpy(request.city, r_string+2);

	//creazione socket
	my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (my_socket < 0) {
		printf("socket creation failed.\n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}


	//server address

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = server_address_int;
	server_address.sin_port = htons(port);

	//connect to server

	if (connect(my_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		printf("failed to connect.\n");
		return -1;
	}

	//send request

	if (send(my_socket, &request, sizeof(weather_request_t), 0) != sizeof(weather_request_t)) {
		printf("send() different amount of bytes");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	//receive

	if (recv(my_socket, &response, sizeof(weather_response_t), 0) <= 0) {
		printf("recv() failed or connection closed prematurely\n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	//printf("%d %c %.1f\n", response.status, response.type, response.value);

	char received_message_string[128];
	for (int i = 0; request.city[i]; i++) {
		request.city[i] = tolower(request.city[i]);
	}

	request.city[0] = toupper(request.city[0]);

	switch (response.status) {
		case 0:
			switch (response.type) {
				case TEMPERATURE:
					sprintf(received_message_string, "%s: Temperatura = %.1f°C", request.city, response.value);
					break;
				case HUMIDITY:
					sprintf(received_message_string, "%s: Umidità = %.1f%%", request.city, response.value);
					break;
				case WIND_SPEED:
					sprintf(received_message_string, "%s: Vento = %.1f km/h", request.city, response.value);
					break;
				case PRESSURE:
					sprintf(received_message_string, "%s: Pressione = %.1f hPa", request.city, response.value);
					break;
			}
			break;
		case 1:
			sprintf(received_message_string, "Città non disponibile");
			break;
		case 2:
			sprintf(received_message_string, "Richiesta non valida");
			break;
	}
	printf("Ricevuto risultato dal server ip %s. %s\n", inet_ntoa(server_address.sin_addr), received_message_string);

	//close socket

	printf("Client terminated.\n");
	closesocket(my_socket);
	clearwinsock();
	return 0;
} // main end

