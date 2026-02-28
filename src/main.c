#include <esp_timer.h>

#include <driver/gpio.h>
#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "pins.h"

typedef struct {
    short state;
    int64_t last_switch_time;
} LEDState;

typedef struct {
    LEDState leds_states[3];
} AppState;

int64_t millisa() {
    return esp_timer_get_time() / 1000;
}

void setup() {
    vTaskPrioritySet(NULL, 0);    

    gpio_config_t gpio_leds_config = {
        .pin_bit_mask = (1 << LED_PIN_1) | (1 << LED_PIN_2) | (1 << LED_PIN_3),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&gpio_leds_config);
}

void ledHandler(
    gpio_num_t pin,
    uint32_t switchPeriod,
    LEDState *led
) {
    int64_t currentTime = millisa();

    if (currentTime - led->last_switch_time > switchPeriod) {
        led->last_switch_time = currentTime;

        led->state = led->state == 1 ? 0 : 1;

        gpio_set_level(pin, led->state);
    }
}

void app_main() {
    setup();

    AppState state = {
        .leds_states = {
            { 0, 0 },
            { 0, 0 },
            { 0, 0 },
        }
    };

    while(1) {
        ledHandler(LED_PIN_1, LED_SPEED_1, &state.leds_states[0]);
        ledHandler(LED_PIN_2, LED_SPEED_2, &state.leds_states[1]);
        ledHandler(LED_PIN_3, LED_SPEED_3, &state.leds_states[2]);

        taskYIELD();
    }
}