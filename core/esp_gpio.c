/* GPIO management functions
 *
 * Part of esp-open-rtos
 * Copyright (C) 2015 Angus Gratton
 * BSD Licensed as described in the file LICENSE
 */
#include <esp/gpio.h>
#include <esp/rtc_regs.h>

void gpio_enable(const uint8_t gpio_num, const gpio_direction_t direction)
{
    if (gpio_num == 16) {
        RTC.GPIO_CFG[3] = (RTC.GPIO_CFG[3] & 0xffffffbc) | 1;
        RTC.GPIO_CONF = (RTC.GPIO_CONF & 0xfffffffe) | 0;
        switch (direction) {
        case GPIO_INPUT:
            RTC.GPIO_ENABLE = (RTC.GPIO_OUT & 0xfffffffe);
            break;
        case GPIO_OUTPUT:
        case GPIO_OUT_OPEN_DRAIN:
            /* TODO open drain? */
            RTC.GPIO_ENABLE = (RTC.GPIO_OUT & 0xfffffffe) | 1;
            break;
        }
        return;
    }

    switch (direction) {
    case GPIO_INPUT:
        GPIO.ENABLE_OUT_CLEAR = BIT(gpio_num);
        iomux_set_gpio_function(gpio_num, false);
        break;
    case GPIO_OUTPUT:
        GPIO.CONF[gpio_num] &= ~GPIO_CONF_OPEN_DRAIN;
        GPIO.ENABLE_OUT_SET = BIT(gpio_num);
        iomux_set_gpio_function(gpio_num, true);
        break;
    case GPIO_OUT_OPEN_DRAIN:
        GPIO.CONF[gpio_num] |= GPIO_CONF_OPEN_DRAIN;
        GPIO.ENABLE_OUT_SET = BIT(gpio_num);
        iomux_set_gpio_function(gpio_num, true);
        break;
    }
}

void gpio_set_pullup(uint8_t gpio_num, bool enabled, bool enabled_during_sleep)
{
    uint32_t flags = 0;

    if (enabled) {
        flags |= IOMUX_PIN_PULLUP;
    }
    if (enabled_during_sleep) {
        flags |= IOMUX_PIN_PULLUP_SLEEP;
    }
    iomux_set_pullup_flags(gpio_to_iomux(gpio_num), flags);
}

