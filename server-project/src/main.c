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
#include <ctype.h>
#include "protocol.h"

#define NO_ERROR 0

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

int main(int argc, char *argv[]) {

	// TODO: Implement server logic

#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif

	int my_socket;

	// TODO: Create socket
	// my_socket = socket(...);

	my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (my_socket < 0) {
			printf("socket creation failed.\n");
			return -1;
		}


	// TODO: Configure server address
	// struct sockaddr_in server_addr;
	// server_addr.sin_family = AF_INET;
	// server_addr.sin_port = htons(SERVER_PORT);
	// server_addr.sin_addr.s_addr = INADDR_ANY;

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_address.sin_port = htons(SERVER_PORT);


	// TODO: Bind socket
	// bind(...);

	if (bind(my_socket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
		printf("bind() failed.\n");
		closesocket(my_socket);
		return -1;
	}

	// TODO: Set socket to listen
	// listen(...);

	int qlen = 6;
	if (listen(my_socket, qlen) < 0) {
		printf("listen() failed.\n");
		closesocket(my_socket);
		return -1;
	}

	// TODO: Implement connection acceptance loop
	// while (1) {
	//     int client_socket = accept(...);
	//     // Handle client communication
	//     closesocket(client_socket);
	// }

	struct sockaddr_in client_address;
	int client_socket;
	int client_len;
	printf("wait for client");
	char* cities[] = {"bari", "roma", "milano", "napoli", "torino", "palermo", "genova",
		"bologna", "firenze", "venezia"};

	while (1) {
		int client_len = sizeof(client_address);
		if ((client_socket = accept(my_socket, (struct sockaddr*) &client_address,
				&client_len)) < 0) {
			printf("accept() failed.\n");
			closesocket(client_socket);
			clearwinsock();
			return 0;
		}
		//printf("handling client %s\n", inet_ntoa(client_address.sin_addr));
		weather_request_t request;
		if (recv(client_socket, &request, sizeof(weather_request_t), 0) <= 0) {
			printf("recv() failed or connection closed prematurely");
			closesocket(client_socket);
			clearwinsock();
			return -1;
		}

		printf("Richiesta '%c %s' dal client %s", request.type, request.city, inet_ntoa(client_address.sin_addr));

		weather_response_t response;

		response.status = STATUS_CITY_NOT_FOUND;
		response.type = '\0';
		response.value = 0.0;

		for (int i = 0; request.city[i]; i++) {
			request.city[i] = tolower(request.city[i]);
		}

		for (int i = 0; i < 10; i++) {
			if (!strcmp(request.city, cities[i])) {
				response.status = STATUS_SUCCESS;
				break;
			}
		}

		switch (request.type) {
			case TEMPERATURE:
				response.type = TEMPERATURE;
				response.value = get_temperature();
				break;
			case HUMIDITY:
				response.type = HUMIDITY;
				response.value = get_humidity();
				break;
			case WIND_SPEED:
				response.type = WIND_SPEED;
				response.value = get_wind();
				break;
			case PRESSURE:
				response.type = PRESSURE;
				response.value = get_pressure();
				break;
			default:
				response.status = STATUS_INVALID_REQUEST;
				break;
		}

		if (send(client_socket, &response, sizeof(weather_response_t), 0) != sizeof(weather_response_t)) {
			printf("send() sent a different number of bytes than expected");
			closesocket(client_socket);
			clearwinsock();
			return -1;
		}

		closesocket(client_socket);

	}

	printf("Server terminated.\n");

	closesocket(my_socket);
	clearwinsock();
	return 0;
} // main end

float get_temperature() {
	return (((float)rand()/RAND_MAX)*50.0)-10.0;
}

float get_humidity() {
	return (((float)rand()/RAND_MAX)*80.0)+20.0;
}

float get_wind() {
	return ((float)rand()/RAND_MAX)*100.0;
}

float get_pressure() {
	return (((float)rand()/RAND_MAX)*100.0)+950.0;
}
