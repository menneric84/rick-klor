/*
 * Copyright (c) 2023-2025 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT pimoroni_trackball

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/input/input.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/dt-bindings/input/input-event-codes.h>

LOG_MODULE_REGISTER(pimoroni_trackball, CONFIG_INPUT_LOG_LEVEL);

/* Register addresses */
#define TRACKBALL_REG_LEFT   0x04
#define TRACKBALL_REG_RIGHT  0x05
#define TRACKBALL_REG_UP     0x06
#define TRACKBALL_REG_DOWN   0x07

struct pimoroni_trackball_config {
    struct i2c_dt_spec i2c;
    int poll_period_ms;
};

struct pimoroni_trackball_data {
    struct k_timer timer;
    struct k_work work;
    const struct device *dev;
};

static void pimoroni_trackball_work_handler(struct k_work *work)
{
    struct pimoroni_trackball_data *data = CONTAINER_OF(work, struct pimoroni_trackball_data, work);
    const struct pimoroni_trackball_config *config = data->dev->config;
    uint8_t left, right, up, down;
    int ret;

    /* Read movement registers */
    ret = i2c_reg_read_byte_dt(&config->i2c, TRACKBALL_REG_LEFT, &left);
    if (ret < 0) {
        LOG_ERR("Failed to read LEFT register: %d", ret);
        return;
    }

    ret = i2c_reg_read_byte_dt(&config->i2c, TRACKBALL_REG_RIGHT, &right);
    if (ret < 0) {
        LOG_ERR("Failed to read RIGHT register: %d", ret);
        return;
    }

    ret = i2c_reg_read_byte_dt(&config->i2c, TRACKBALL_REG_UP, &up);
    if (ret < 0) {
        LOG_ERR("Failed to read UP register: %d", ret);
        return;
    }

    ret = i2c_reg_read_byte_dt(&config->i2c, TRACKBALL_REG_DOWN, &down);
    if (ret < 0) {
        LOG_ERR("Failed to read DOWN register: %d", ret);
        return;
    }

    /* Calculate deltas */
    int16_t x = (int16_t)right - (int16_t)left;
    int16_t y = (int16_t)down - (int16_t)up;

    /* Only send events if there's movement */
    if (x != 0 || y != 0) {
        LOG_DBG("Movement: x=%d, y=%d", x, y);
        
        if (x != 0) {
            input_report_rel(data->dev, INPUT_REL_X, x, false, K_FOREVER);
        }
        if (y != 0) {
            input_report_rel(data->dev, INPUT_REL_Y, y, false, K_FOREVER);
        }
        
        /* Send sync to indicate end of movement data */
        input_report_rel(data->dev, INPUT_REL_X, 0, true, K_FOREVER);
    }
}

static void pimoroni_trackball_timer_handler(struct k_timer *timer)
{
    struct pimoroni_trackball_data *data = CONTAINER_OF(timer, struct pimoroni_trackball_data, timer);
    k_work_submit(&data->work);
}

static int pimoroni_trackball_init(const struct device *dev)
{
    const struct pimoroni_trackball_config *config = dev->config;
    struct pimoroni_trackball_data *data = dev->data;

    if (!device_is_ready(config->i2c.bus)) {
        LOG_ERR("I2C device not ready");
        return -ENODEV;
    }

    data->dev = dev;
    k_work_init(&data->work, pimoroni_trackball_work_handler);
    k_timer_init(&data->timer, pimoroni_trackball_timer_handler, NULL);
    
    /* Start polling timer */
    k_timer_start(&data->timer, K_MSEC(config->poll_period_ms), K_MSEC(config->poll_period_ms));

    LOG_INF("Pimoroni trackball initialized (poll period: %d ms)", config->poll_period_ms);
    return 0;
}

#define PIMORONI_TRACKBALL_INIT(inst)                                          \
    static struct pimoroni_trackball_data pimoroni_trackball_data_##inst;      \
                                                                               \
    static const struct pimoroni_trackball_config                             \
        pimoroni_trackball_config_##inst = {                                  \
            .i2c = I2C_DT_SPEC_INST_GET(inst),                                \
            .poll_period_ms = DT_INST_PROP_OR(inst, poll_period_ms, 10),      \
        };                                                                     \
                                                                               \
    DEVICE_DT_INST_DEFINE(inst, pimoroni_trackball_init, NULL,                \
                          &pimoroni_trackball_data_##inst,                     \
                          &pimoroni_trackball_config_##inst, POST_KERNEL,      \
                          CONFIG_INPUT_INIT_PRIORITY, NULL);

DT_INST_FOREACH_STATUS_OKAY(PIMORONI_TRACKBALL_INIT)