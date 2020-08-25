/* main.c - Application main entry point */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <zephyr/types.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/pwm.h>
#include <drivers/gpio.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <bluetooth/services/bas.h>

#define SIGNAL_SEND_INTERVAL 1

double t = 0.0;

double a1 = 1.0,      f1 = 1.0 / 30.0;
double a2 = 1.0 / 4,  f2 = 3.0 / 30.0;
double a3 = 1.0 / 16, f3 = 5.0 / 30.0;
double a4 = 1.0 / 32, f4 = 7.0 / 30.0;

#define PI 3.14159265359

double calc_signal() {
  double result =
    a1 * sin(2 * PI * f1 * t) +
    a2 * sin(2 * PI * f2 * t) +
    a3 * sin(2 * PI * f3 * t) +
    a4 * sin(2 * PI * f4 * t);
  t += SIGNAL_SEND_INTERVAL;
  return result;
}

// Current state.
bool button_listening = true;
bool pwm_on = true;
bool signal_on = true;


// DT node for PWM LED.
#define PWM_LED0_NODE DT_ALIAS(pwm_led0)

// PWM frequency is 40 kHz => period is 25 us
#define PWM_PERIOD_NSEC 25000

#define FLAGS_OR_ZERO(node)                       \
  COND_CODE_1(DT_PHA_HAS_CELL(node, pwms, flags), \
              (DT_PWMS_FLAGS(node)),              \
              (0))

#if DT_NODE_HAS_STATUS(PWM_LED0_NODE, okay)
#define PWM_LABEL DT_PWMS_LABEL(PWM_LED0_NODE)
#define PWM_CHANNEL DT_PWMS_CHANNEL(PWM_LED0_NODE)
#define PWM_FLAGS FLAGS_OR_ZERO(PWM_LED0_NODE)
#else
#error "Unsupported board: pwm-led0 devicetree alias is not defined"
#define PWM_LABEL ""
#define PWM_CHANNEL 0
#define PWM_FLAGS 0
#endif

#define SW0_NODE DT_ALIAS(sw0)

#if DT_NODE_HAS_STATUS(SW0_NODE, okay)
#define SW0_GPIO_LABEL DT_GPIO_LABEL(SW0_NODE, gpios)
#define SW0_GPIO_PIN DT_GPIO_PIN(SW0_NODE, gpios)
#define SW0_GPIO_FLAGS (GPIO_INPUT | FLAGS_OR_ZERO(SW0_NODE))
#else
#error "Unsupported board: sw0 devicetree alias is not defined"
#define SW0_GPIO_LABEL ""
#define SW0_GPIO_PIN 0
#define SW0_GPIO_FLAGS 0
#endif


struct device *pwm = NULL;
struct device *button = NULL;
double signal_val = 0.0;
uint8_t duty_cycle = 0;


static ssize_t duty_cycle_read
  (struct bt_conn *conn, const struct bt_gatt_attr *attr,
   void *buf, u16_t len, u16_t offset);
static ssize_t duty_cycle_write
  (struct bt_conn *conn, const struct bt_gatt_attr *attr,
   const void *buf, u16_t len, u16_t offset, u8_t flags);

static struct bt_uuid_128 bt_uuid_pwm =
  BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x305F1AC8, 0x460B, 0x427E, 0x8CA1, 0x52B9FF1FC05F));

static struct bt_uuid_128 bt_uuid_duty_cycle =
  BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x315F1AC8, 0x460B, 0x427E, 0x8CA1, 0x52B9FF1FC05F));

static struct bt_uuid_128 bt_uuid_signal =
  BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x325F1AC8, 0x460B, 0x427E, 0x8CA1, 0x52B9FF1FC05F));

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)
#define ADV_LEN 12

static bool signal_notify = false;

static void signal_ccc_cfg_changed(const struct bt_gatt_attr *attr, u16_t value)
{
  signal_notify = value == BT_GATT_CCC_NOTIFY;
}

BT_GATT_SERVICE_DEFINE(pwm_svc,
                       BT_GATT_PRIMARY_SERVICE(&bt_uuid_pwm.uuid),

                       /* Synthetic signal */
                       BT_GATT_CHARACTERISTIC(&bt_uuid_signal.uuid,
                                              BT_GATT_CHRC_NOTIFY, 0x00, NULL, NULL, NULL),
                       BT_GATT_CCC(signal_ccc_cfg_changed,
                                   BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

                       /* Duty cycle */
                       BT_GATT_CHARACTERISTIC(&bt_uuid_duty_cycle.uuid,
                                              BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                                              BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                                              duty_cycle_read, duty_cycle_write, &duty_cycle)
                       );

static const struct bt_data ad[] = {
  BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
  BT_DATA_BYTES(BT_DATA_UUID128_ALL,
                0x30, 0x5F, 0x1A, 0xC8, 0x46, 0x0B, 0x42, 0x7E,
                0x8C, 0xA1, 0x52, 0xB9, 0xFF, 0x1F, 0xC0, 0x5F)
};

static ssize_t duty_cycle_read
  (struct bt_conn *conn, const struct bt_gatt_attr *attr,
   void *buf, u16_t len, u16_t offset)
{
  printk("duty_cycle_read: %d\n", duty_cycle);
  const char *value = attr->user_data;
  return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(duty_cycle));
}

static ssize_t duty_cycle_write
  (struct bt_conn *conn, const struct bt_gatt_attr *attr,
   const void *buf, u16_t len, u16_t offset, u8_t flags)
{
  printk("duty_cycle_write: %d\n", duty_cycle);
  u8_t *value = attr->user_data;
  if (offset + len > sizeof(duty_cycle)) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
  }
  memcpy(value + offset, buf, len);
  if (duty_cycle > 100) duty_cycle = 100;
  printk("  new value: %d\n", duty_cycle);
  if (pwm_on) {
    int on_nsec = PWM_PERIOD_NSEC * duty_cycle / 100;
    int ret = pwm_pin_set_nsec(pwm, PWM_CHANNEL, PWM_PERIOD_NSEC, on_nsec, PWM_FLAGS);
    if (ret) {
      printk("Error %d: failed to set pulse width\n", ret);
    }
  }
  return len;
}

static void connected(struct bt_conn *conn, u8_t err)
{
  if (err) {
    printk("Connection failed (err 0x%02x)\n", err);
  } else {
    printk("Connected\n");
  }
}

static void disconnected(struct bt_conn *conn, u8_t reason)
{
  printk("Disconnected (reason 0x%02x)\n", reason);
}

static struct bt_conn_cb conn_callbacks = {
  .connected = connected,
  .disconnected = disconnected,
};

static void bt_ready(void)
{
  printk("Bluetooth initialized\n");
  int err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
  if (err) {
    printk("Advertising failed to start (err %d)\n", err);
    return;
  }
  printk("Advertising successfully started\n");
}

static void signal_thread_fn() {
  signal_val = calc_signal();
  while (true) {
    k_sleep(K_SECONDS(SIGNAL_SEND_INTERVAL));
    if (signal_on) {
      signal_val = calc_signal();
      if (signal_notify)
        bt_gatt_notify(NULL, &pwm_svc.attrs[1], &signal_val, sizeof(signal_val));
    }
  }
}

// UI feedback semaphore.
K_SEM_DEFINE(feedback_sem, 0, 1);

#define FEEDBACK_BLINK_DELAY K_MSEC(250)

static void feedback_thread_fn() {
  while (true) {
    k_sem_take(&feedback_sem, K_FOREVER);
    for (int cnt = 0; cnt < 3; ++cnt) {
      pwm_pin_set_nsec(pwm, PWM_CHANNEL, PWM_PERIOD_NSEC, 0, PWM_FLAGS);
      k_sleep(FEEDBACK_BLINK_DELAY);
      pwm_pin_set_nsec(pwm, PWM_CHANNEL, PWM_PERIOD_NSEC, PWM_PERIOD_NSEC, PWM_FLAGS);
      k_sleep(FEEDBACK_BLINK_DELAY);
    }
    pwm_pin_set_nsec(pwm, PWM_CHANNEL, PWM_PERIOD_NSEC, 0, PWM_FLAGS);
    k_sleep(FEEDBACK_BLINK_DELAY);
    pwm_on = true;
    button_listening = true;
    int on_nsec = PWM_PERIOD_NSEC * duty_cycle / 100;
    int ret = pwm_pin_set_nsec(pwm, PWM_CHANNEL, PWM_PERIOD_NSEC, on_nsec, PWM_FLAGS);
    if (ret) {
      printk("Error %d: failed to set pulse width\n", ret);
    }
  }
}


#define SIGNAL_THREAD_STACK_SIZE 500
#define SIGNAL_THREAD_PRIORITY 5

#define FEEDBACK_THREAD_STACK_SIZE 500
#define FEEDBACK_THREAD_PRIORITY 5

// Signal calculation thread.
K_THREAD_DEFINE(signal_thread, SIGNAL_THREAD_STACK_SIZE,
                signal_thread_fn, NULL, NULL, NULL,
                SIGNAL_THREAD_PRIORITY, 0, 1000);

// UI feedback thread and semaphore.
K_THREAD_DEFINE(feedback_thread, FEEDBACK_THREAD_STACK_SIZE,
                feedback_thread_fn, NULL, NULL, NULL,
                FEEDBACK_THREAD_PRIORITY, 0, 0);

static struct gpio_callback button_cb_data;

#define BUTTON_DEBOUNCE_DELAY_MS 250

static u32_t time, last_time;

void button_pressed(struct device *dev, struct gpio_callback *cb, u32_t pins)
{
  if (button_listening) {
    time = k_uptime_get_32();
    if (time < last_time + BUTTON_DEBOUNCE_DELAY_MS) {
      last_time = time;
      return;
    }

    printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
    last_time = time;

    signal_on = !signal_on;
    pwm_on = false;
    button_listening = false;
    k_sem_give(&feedback_sem);
  }
}

void main(void)
{
  pwm = device_get_binding(PWM_LABEL);
  if (!pwm) {
    printk("Error: didn't find %s device\n", PWM_LABEL);
    return;
  }

  button = device_get_binding(SW0_GPIO_LABEL);
  if (!button) {
    printk("Error: didn't find %s device\n", SW0_GPIO_LABEL);
    return;
  }

  int err = gpio_pin_configure(button, SW0_GPIO_PIN, SW0_GPIO_FLAGS | GPIO_INT_DEBOUNCE);
  if (err != 0) {
    printk("Error %d: failed to configure %s pin %d\n",
           err, SW0_GPIO_LABEL, SW0_GPIO_PIN);
    return;
  }

  err = gpio_pin_interrupt_configure(button, SW0_GPIO_PIN, GPIO_INT_EDGE_TO_ACTIVE);
  if (err != 0) {
    printk("Error %d: failed to configure interrupt on %s pin %d\n",
           err, SW0_GPIO_LABEL, SW0_GPIO_PIN);
    return;
  }

  gpio_init_callback(&button_cb_data, button_pressed, BIT(SW0_GPIO_PIN));
  gpio_add_callback(button, &button_cb_data);
  printk("Set up button at %s pin %d\n", SW0_GPIO_LABEL, SW0_GPIO_PIN);

  err = bt_enable(NULL);
  if (err) {
    printk("Bluetooth init failed (err %d)\n", err);
    return;
  }

  bt_ready();

  bt_conn_cb_register(&conn_callbacks);

  // Set up GPIO button interrupt & callback.

  while (1) { k_sleep(K_SECONDS(1));}
}
