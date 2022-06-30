/*
 * Control a Dotstar (SK9822) RGB LED strip from a Raspberry Pi Pico.
 * Nathan Freeman (natethegreat68.dev@gmail.com)
 * Provided under the MIT License.
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#ifndef RGB_ORDER
	#define RGB_ORDER BGR
#endif
/*
 * Constants.
 */
#define COMMAND_LENGTH 250 // Maximum length of an input command string.
#define COMMAND_TIMEOUT 1e6 // Timeout in us for reading the input.
#define COMMAND_CHAR_END 13 // End of the command (CR).
#define COMMAND_CHAR_GROUP_SEPARATOR 59 // Separator between subcommands (;).
#define COMMAND_CHAR_RGB_SEPARATOR 44 // Separator between RGB values (,).
#define SPI_NUMBER spi0 // Hardware SPI instance.
#define SPI_BAUDRATE 100e3 // Baud rate for the LED SPI.
const uint RELAY_PIN = 16; // GP16 is the pin used to control the main relay.
const uint SCLK_PIN = 18; // GP18 is the SCLK SPI pin for the dotstar strip.
const uint MOSI_PIN = 19; // GP19 is the MOSI SPI pin for the dotstar strip.

/*
 * Function prototypes.
 */
void setup_relay();
void relay_on();
void relay_off();
void setup_spi();
void spi_write(char *data);
void process_command(char *command);

/*
 * Main loop.
 */
int main() {
	// Prepare stdio.
	stdio_init_all();
	// Prepare the relay.
	setup_relay();
	// Prepare SPI.
	setup_spi();
	// Prepare the command variables.
	char command[COMMAND_LENGTH];
	int command_result;
	int command_index = 0;
	// Loop until power off.
    while (true) {
		// Read and parse a character from input.
		command_result = getchar_timeout_us(COMMAND_TIMEOUT);
		if (command_result == COMMAND_CHAR_END) { // End of this command.
			// Null-terminate the string, perform action, restart the index.
			command[command_index] = '\0';
			process_command(command);
			command_index = 0;
		} else if (command_result != PICO_ERROR_TIMEOUT) { // Normal character.
			// Append to the string and increment the index.
			command[command_index] = (char)command_result;
			command_index++;
		}
		if (command_index == COMMAND_LENGTH) { // Check if too long.
			// Display error message and restart the index.
			printf("Command too long; max length %i\n", COMMAND_LENGTH);
			command_index = 0;
		}
    }
	return 0;
}

/*
 * Relay-related functions.
 */
void setup_relay() {
	// Set it up.
	gpio_init(RELAY_PIN);
	gpio_set_dir(RELAY_PIN, GPIO_OUT);
	// Default it to off.
	gpio_put(RELAY_PIN, 0);
}

void relay_on() {
	// Check the relay status. If it's already on, do nothing.
	// Otherwise, turn it on and wait 3 seconds for it to stabilize.
	if (!gpio_get(RELAY_PIN)) {
		gpio_put(RELAY_PIN, 1);
		sleep_ms(3e3);
	}
}

void relay_off() {
	// Turn the relay off and return immediately.
	gpio_put(RELAY_PIN, 0);
	sleep_ms(50);
}

/*
 * SPI-related functions.
 */
void setup_spi() {
	// Set it up.
	spi_init(SPI_NUMBER, SPI_BAUDRATE);
	gpio_set_function(SCLK_PIN, GPIO_FUNC_SPI);
	gpio_set_function(MOSI_PIN, GPIO_FUNC_SPI);
}

void spi_write(char *data) {
	// Send the data.
	spi_write_blocking(SPI_NUMBER, data, strlen(data));
}

void process_command(char *command) {
	char color[COMMAND_LENGTH-3];

	// Determine which command was sent.
	if (strncmp(command, "on ", 3) == 0) { // "on" command
		printf("Turning on relay.\n");
		relay_on();
		strcpy(color, command+3);
		printf("Relay on; color %s.\n", color);
	} else if (strncmp(command, "off", 3) == 0) { // "off" command
		relay_off();
		printf("Relay off.\n");
	} else if (strncmp(command, "help", 4) == 0) { // "help" command
		printf("Commands:\n");
		printf("  on <color>\n");
		printf("  off\n");
		printf("  colors\n");
	} else if (strncmp(command, "colors", 6) == 0) { // "colors" command
		printf("Available colors:\n");
		printf("  full\n  white\n  red\n  green\n  blue\n");
		printf("  orange\n  purple\n  yellow\n");
		printf("  custom: (<red>,<green>,<blue>) 0-255 each\n");
	} else {
		printf("Command not recognized; try \"help\"\n");
	}
}
