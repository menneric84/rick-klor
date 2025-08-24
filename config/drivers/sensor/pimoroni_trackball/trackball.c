/*
 * Copyright (c) 2023-2025 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT pimoroni_trackball

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(pimoroni_trackball, CONFIG_SENSOR_LOG_LEVEL);

/* Register addresses */
#define TRACKBALL_REG_LEFT   0x04
#define TRACKBALL_REG_RIGHT  0x05
#define TRACKBALL_REG_UP     0x06
#define TRACKBALL_REG_DOWN   0x07

struct pimoroni_trackball_config {
    struct i2c_dt_spec i2c;
};

struct pimoroni_trackball_data {
    int16_t x;
    int16_t y;
};

static int pimoroni_trackball_sample_fetch(const struct device *dev,
                                           enum sensor_channel chan)
{
    const struct pimoroni_trackball_config *config = dev->config;
    struct pimoroni_trackball_data *data = dev->data;
    uint8_t left, right, up, down;
    int ret;

    /* Read movement registers */
    ret = i2c_reg_read_byte_dt(&config->i2c, TRACKBALL_REG_LEFT, &left);
    if (ret < 0) {
        return ret;
    }

    ret = i2c_reg_read_byte_dt(&config->i2c, TRACKBALL_REG_RIGHT, &right);
    if (ret < 0) {
        return ret;
    }

    ret = i2c_reg_read_byte_dt(&config->i2c, TRACKBALL_REG_UP, &up);
    if (ret < 0) {
        return ret;
    }

    ret = i2c_reg_read_byte_dt(&config->i2c, TRACKBALL_REG_DOWN, &down);
    if (ret < 0) {
        return ret;
    }

    /* Calculate deltas */
    data->x = (int16_t)right - (int16_t)left;
    data->y = (int16_t)down - (int16_t)up;

    return 0;
}

static int pimoroni_trackball_channel_get(const struct device *dev,
                                          enum sensor_channel chan,
                                          struct sensor_value *val)
{
    struct pimoroni_trackball_data *data = dev->data;

    switch (chan) {
    case SENSOR_CHAN_POS_DX:
        val->val1 = data->x;
        val->val2 = 0;
        break;
    case SENSOR_CHAN_POS_DY:
        val->val1 = data->y;
        val->val2 = 0;
        break;
    default:
        return -ENOTSUP;
    }

    return 0;
}

static int pimoroni_trackball_init(const struct device *dev)
{
    const struct pimoroni_trackball_config *config = dev->config;

    if (!device_is_ready(config->i2c.bus)) {
        LOG_ERR("I2C device not ready");
        return -ENODEV;
    }

    LOG_INF("Pimoroni trackball initialized");
    return 0;
}

static const struct sensor_driver_api pimoroni_trackball_api = {
    .sample_fetch = pimoroni_trackball_sample_fetch,
    .channel_get = pimoroni_trackball_channel_get,
};

#define PIMORONI_TRACKBALL_INIT(inst)                                          \
    static struct pimoroni_trackball_data pimoroni_trackball_data_##inst;      \
                                                                               \
    static const struct pimoroni_trackball_config                             \
        pimoroni_trackball_config_##inst = {                                  \
            .i2c = I2C_DT_SPEC_INST_GET(inst),                                \
        };                                                                     \
                                                                               \
    DEVICE_DT_INST_DEFINE(inst, pimoroni_trackball_init, NULL,                \
                          &pimoroni_trackball_data_##inst,                     \
                          &pimoroni_trackball_config_##inst, POST_KERNEL,      \
                          CONFIG_SENSOR_INIT_PRIORITY,                         \
                          &pimoroni_trackball_api);

DT_INST_FOREACH_STATUS_OKAY(PIMORONI_TRACKBALL_INIT)