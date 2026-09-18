#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#if defined(CONFIG_APP_LED_SUBSYSTEM) && CONFIG_APP_LED_SUBSYSTEM
#define SLEEP_TIME_MS CONFIG_APP_HEARTBEAT_PERIOD_MS

#define LED_NODE DT_ALIAS(app_led)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);
#endif

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
#if defined(CONFIG_APP_LED_SUBSYSTEM) && CONFIG_APP_LED_SUBSYSTEM
    bool led_state = true;

    if (!gpio_is_ready_dt(&led)) return 0;

    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) return 0;

    while (1) {
        if (gpio_pin_toggle_dt(&led) < 0) return 0;

        led_state = !led_state;
#if defined(CONFIG_APP_LED_DEBUG) && CONFIG_APP_LED_DEBUG
        LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
#endif
        k_msleep(SLEEP_TIME_MS);
    }
#endif
    return 0;
}
