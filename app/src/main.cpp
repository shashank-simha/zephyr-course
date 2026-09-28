#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#include "led_sensor.h"

#define SLEEP_TIME_MS CONFIG_APP_HEARTBEAT_PERIOD_MS
#define LED_SENSOR_NODE DT_ALIAS(led_sensor)

static const struct device *const led_sensor = DEVICE_DT_GET(LED_SENSOR_NODE);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

static int sensor_shell_fetch(const struct shell *shell, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    if (sensor_sample_fetch(led_sensor) < 0) {
        shell_error(shell, "LED sensor sample fetch failed");
        return -EIO;
    }

    shell_print(shell, "LED sensor sample fetched");
    return 0;
}

static int sensor_shell_read(const struct shell *shell, size_t argc, char **argv)
{
    struct sensor_value value;

    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    if (sensor_channel_get(led_sensor, SENSOR_CHAN_ALL, &value) < 0) {
        shell_error(shell, "LED sensor channel get failed");
        return -EIO;
    }

    shell_print(shell, "LED sensor value: %d.%06d", value.val1, value.val2);
    return 0;
}

static int sensor_shell_info(const struct shell *shell, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    shell_print(shell, "device: %s", led_sensor->name);
    shell_print(shell, "ready: %s", device_is_ready(led_sensor) ? "yes" : "no");
    return 0;
}

static int sensor_shell_set(const struct shell *shell, size_t argc, char **argv)
{
    char *end;
    long value;

    ARG_UNUSED(argc);

    errno = 0;
    value = strtol(argv[1], &end, 10);
    if (end == argv[1] || *end != '\0' || errno == ERANGE ||
        value < 0 || value > 100 || value > INT_MAX) {
        shell_error(shell, "value must be an integer from 0 to 100");
        return -EINVAL;
    }

    if (led_sensor_set_runtime_parameter(led_sensor, (int)value) < 0) {
        shell_error(shell, "failed to set LED sensor runtime parameter");
        return -EIO;
    }

    shell_print(shell, "LED sensor runtime parameter set to %d", (int)value);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sensor_shell_commands,
    SHELL_CMD(fetch, NULL, "Fetch a sample from the LED sensor", sensor_shell_fetch),
    SHELL_CMD(read, NULL, "Read and print the LED sensor value", sensor_shell_read),
    SHELL_CMD(info, NULL, "Print the LED sensor device information", sensor_shell_info),
    SHELL_CMD_ARG(set, NULL, "Set the LED sensor runtime parameter", sensor_shell_set, 2, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &sensor_shell_commands, "LED sensor commands", NULL);

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
