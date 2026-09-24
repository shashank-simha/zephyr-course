#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "led_sensor.h"

#define SLEEP_TIME_MS CONFIG_APP_HEARTBEAT_PERIOD_MS
#define LED_SENSOR_NODE DT_ALIAS(led_sensor)

static const struct device *const led_sensor = DEVICE_DT_GET(LED_SENSOR_NODE);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    struct sensor_value value;

    if (!device_is_ready(led_sensor)) {
        LOG_ERR("LED sensor is not ready");
        return 0;
    }

    if (led_sensor_set_runtime_parameter(led_sensor, 84) < 0) {
        LOG_ERR("Failed to set LED sensor runtime parameter");
        return 0;
    }

    while (true) {
        if (sensor_sample_fetch(led_sensor) < 0) {
            LOG_ERR("LED sensor sample fetch failed");
            return 0;
        }

        k_msleep(SLEEP_TIME_MS);

        if (sensor_channel_get(led_sensor, SENSOR_CHAN_ALL, &value) < 0) {
            LOG_ERR("LED sensor channel get failed");
            return 0;
        }

        LOG_INF("LED sensor value: %d", value.val1);
        k_msleep(SLEEP_TIME_MS);
    }
}
