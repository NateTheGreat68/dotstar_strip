/*
 * Control a Dotstar (SK9822) RGB LED strip from a Raspberry Pi Pico.
 * Nathan Freeman (natethegreat68.dev@gmail.com)
 * Provided under the MIT License.
 */

#include <stdio.h>
#include "pico/stdlib.h"

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
const uint RELAY_PIN = 16; // GP16 is the pin used to control the main relay.
const uint SCLK_PIN = 18; // GP18 is the SCLK SPI pin for the dotstar strip.
const uint MOSI_PIN = 19; // GP19 is the MOSI SPI pin for the dotstar strip.

/*
 * Function prototypes.
 */
uint setup_relay(const uint relay_pin);
void relay_on(const uint relay_handle);
void relay_off(const uint relay_handle);

/*
 * Main loop.
 */
int main() {
	// Prepare stdio.
	stdio_init_all();
	// Prepare the relay.
	const uint relay_handle = setup_relay(RELAY_PIN);
	// Prepare the command.
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
			printf("Command: %s\n", command);
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
uint setup_relay(const uint relay_pin) {
	// Set it up.
	gpio_init(relay_pin);
	gpio_set_dir(relay_pin, GPIO_OUT);
	// Default it to off.
	gpio_put(relay_pin, 0);
	// Return the pin number for reference later.
	return relay_pin;
}

void relay_on(const uint relay_handle) {
	gpio_put(relay_handle, 1);
}

void relay_off(const uint relay_handle) {
	gpio_put(relay_handle, 0);
}
