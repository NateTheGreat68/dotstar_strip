/*
 * Control a Dotstar (SK9822) RGB LED strip from a Raspberry Pi Pico.
 * Nathan Freeman (natethegreat68.dev@gmail.com)
 * Provided under the MIT License.
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#ifndef SET_LED_ORDER
#define LED_ORDER_BGR
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
const uint LED_COUNT = 67; // Number of LEDs for the on command.

/*
 * Structs.
 */
typedef struct LED_t {
	char global;
	char red;
	char green;
	char blue;
} LED;
typedef struct COLOR_DEF_t {
	char name[7];
	LED led_data;
} COLOR_DEF;

COLOR_DEF color_defs[8] = {
	{"blue", {31,0,0,255}},
	{"full", {31,255,255,255}},
	{"green", {31,0,255,0}},
	{"orange", {31,255,100,0}},
	{"purple", {31,200,0,255}},
	{"red", {31,255,0,0}},
	{"white", {31,255,240,240}},
	{"yellow", {31,255,180,0}},
};

/*
 * Function prototypes.
 */
void setup_relay();
void relay_on();
void relay_off();
void setup_spi();
void spi_write(char *data, uint data_count);
void process_command(char *command);
void write_data(LED *data, uint data_count);
static int compare_color_defs(const void *va, const void *vb);
LED *get_color(char *name);

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
			printf("\n");
			command[command_index] = '\0';
			process_command(command);
			command_index = 0;
		} else if (command_result != PICO_ERROR_TIMEOUT) { // Normal character.
			// Append to the string and increment the index.
			printf("%c", (char) command_result);
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
		printf("Turning on relay.\n");
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

void spi_write(char *data, uint data_count) {
	// Send the data.
	spi_write_blocking(SPI_NUMBER, data, data_count);
}

/*
 * Command processing and related functions.
 */
void process_command(char *command) {
	char color[COMMAND_LENGTH-3];
	uint led_count;
	LED *led_data;
	LED *single_led;

	// Determine which command was sent.
	if (strncmp(command, "on ", 3) == 0) { // "on" command
		relay_on();
		strcpy(color, command+3);
		led_count = LED_COUNT;
		led_data = (LED *)malloc(sizeof(LED)*led_count);
		single_led = (LED *)malloc(sizeof(LED));

		if (color[0] == '(' &&
				color[strlen(color)-1] == ')') { // (RRR,GGG,BBB)
			single_led->global = 31;
			single_led->red = (char)strtoul(color+1, NULL, 10);
			single_led->green = (char)strtoul(color+5, NULL, 10);
			single_led->blue = (char)strtoul(color+9, NULL, 10);
			for (int i=0; i<led_count; i++) {
				led_data[i] = *single_led;
			}
			write_data(led_data, led_count);
			printf("Relay on; color %s.\n", color);
		} else {
			single_led = get_color(color);
			if (single_led) {
				for (int i=0; i<led_count; i++) {
					led_data[i] = *single_led;
				}
				write_data(led_data, led_count);
				printf("Relay on; color %s.\n", color);
			} else {
				printf("Color %s not found.\n", color);
			}
			
		}

		free(led_data);
		free(single_led);
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
		printf("  custom: (RRR,GGG,BBB) 000-255 each\n");
	} else {
		printf("Command not recognized; try \"help\"\n");
	}
}

void write_data(LED *data, uint data_count) {
	char buffer[4*data_count]; // each LED requires 4 bytes (4 chars)
	uint half_count = (data_count >> 1) + 1;

	// Start frame.
	for (int i=0; i<4; i++) {
		buffer[i] = 0;
	}
	spi_write(buffer, 4);
	// Data frames.
	for (int i=0; i<data_count; i++) {
		parse_led(&data[i], &buffer[4*i]);
	}
	spi_write(buffer, data_count*4);
	// End frame.
	for (int i=0; i<half_count; i++) {
		buffer[i] = 0;
	}
	spi_write(buffer, half_count);
}

void parse_led(LED *led_data, char *string_data) {
	uint8_t prefix = 224;
	string_data[0] = prefix | led_data->global;
#ifdef LED_ORDER_BGR
	string_data[1] = led_data->blue;
	string_data[2] = led_data->green;
	string_data[3] = led_data->red;
#endif
}

static int compare_color_defs(const void *va, const void *vb) {
	return strcmp(((COLOR_DEF *)va)->name, ((COLOR_DEF *)vb)->name);
}

LED *get_color(char *name) {
	COLOR_DEF key[1];
	strcpy(key[0].name, name);
	COLOR_DEF *result = bsearch(key, color_defs,
			sizeof(color_defs)/sizeof(color_defs[0]), sizeof(color_defs[0]),
			compare_color_defs);
	if (result) {
		return &(result->led_data);
	} else {
		return NULL;
	}
}
