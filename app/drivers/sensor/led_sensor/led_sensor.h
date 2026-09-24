#ifndef ZEPHYR_APP_LED_SENSOR_H_
#define ZEPHYR_APP_LED_SENSOR_H_

#include <zephyr/device.h>

#ifdef __cplusplus
extern "C" {
#endif

int led_sensor_set_runtime_parameter(const struct device *dev, int value);

#ifdef __cplusplus
}
#endif

#endif
