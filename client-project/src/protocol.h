/*
 * protocol.h
 *
 * Server header file
 * Definitions, constants and function prototypes for the server
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

// Shared application parameters
#define DEFAULT_SERVER "127.0.0.1" //Default address
#define SERVER_PORT 56700  // Server port (change if needed)
#define BUFFER_SIZE 512    // Buffer size for messages
#define QUEUE_SIZE 5       // Size of pending connections queue


#define TEMPERATURE 't'
#define HUMIDITY 'h'
#define WIND_SPEED 'w'
#define PRESSURE 'p'

#define STATUS_SUCCESS 0
#define STATUS_CITY_NOT_FOUND 1
#define STATUS_INVALID_REQUEST 2

typedef struct {
    char type;        // Weather data type: 't', 'h', 'w', 'p'
    char city[64];    // City name (null-terminated string)
} weather_request_t;

typedef struct {
    unsigned int status;  // Response status code
    char type;            // Echo of request type
    float value;          // Weather data value
} weather_response_t;


// Function prototypes
// Add here the signatures of the functions implemented by students
float get_temperature();
float get_humidity();
float get_wind();
float get_pressure();

/*
 * Example function to implement:
 * int handle_client(int client_socket);
 */

#endif /* PROTOCOL_H_ */
