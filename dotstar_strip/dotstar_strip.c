/*
 * Control a Dotstar (SK9822) RGB LED strip from a Raspberry Pi Pico.
 * Nathan Freeman (natethegreat68.dev@gmail.com)
 * Provided under the MIT License.
 */

#include "pico/stdlib.h"

/*
 * Constants.
 */
const uint RELAY_PIN = 16; //GP16 is the pin used to control the main relay.
const uint SCLK_PIN = 18; //GP18 is the SCLK SPI pin for the dotstar strip.
const uint MOSI_PIN = 19; //GP19 is the MOSI SPI pin for the dotstar strip.

/*
 * Function prototypes.
 */
uint setup_relay(const uint relay_pin);
void relay_on(const uint relay_pin);
void relay_off(const uint relay_pin);

/*
 * Main loop.
 */
int main() {
	// Prepare the relay.
	const uint relay_pin = setup_relay(RELAY_PIN);
	// Loop until power off.
    while (true) {
    }
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

void relay_on(const uint relay_pin) {
	gpio_put(relay_pin, 1);
}

void relay_off(const uint relay_pin) {
	gpio_put(relay_pin, 0);
}
