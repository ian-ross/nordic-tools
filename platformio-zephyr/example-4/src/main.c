/* main.c - Application main entry point */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/pwm.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <bluetooth/services/bas.h>

#define PI_SEND_INTERVAL K_SECONDS(1)

typedef struct {
  int q, r, t, k, n, l;
} PiCalcState;

void calc_pi_init(PiCalcState *s) {
  s->q = 1;
  s->r = 0;
  s->t = 1;
  s->k = 1;
  s->n = 3;
  s->l = 3;
}

int calc_pi(PiCalcState *s) {
  while (true) {
    if (4*s->q + s->r - s->t < s->n * s->t) {
      int result = s->n;
      int nr = 10 * (s->r - s->n * s->t);
      s->n  = ((10*(3*s->q + s->r)) / s->t) - 10*s->n;
      s->q *= 10;
      s->r = nr;
      return result;
    } else {
      int nr = (2*s->q + s->r) * s->l;
      int nn = (s->q * (7*s->k) + 2 + (s->r*s->l)) / (s->t * s->l);
      s->q *= s->k;
      s->t *= s->l;
      s->l += 2;
      s->k += 1;
      s->n = nn;
      s->r = nr;
    }
  }
}

// Current state.

/* struct State { */
/*   bool pwm_on; */
/*   bool pi_gen_on; */
/* }; */

// DT node for PWM LED.
#define PWM_LED0_NODE DT_ALIAS(pwm_led0)

// PWM frequency is 40 kHz => period is 25 us
#define PWM_PERIOD_NSEC 25000

#define FLAGS_OR_ZERO(node) \
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


struct device *pwm = NULL;
uint8_t pi_digit = 0;
uint8_t duty_cycle = 0;


static ssize_t pi_digit_read
  (struct bt_conn *conn, const struct bt_gatt_attr *attr,
   void *buf, u16_t len, u16_t offset);
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

static struct bt_uuid_128 bt_uuid_pi_digit =
  BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x325F1AC8, 0x460B, 0x427E, 0x8CA1, 0x52B9FF1FC05F));

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)
#define ADV_LEN 12

BT_GATT_SERVICE_DEFINE(pwm_svc,
  BT_GATT_PRIMARY_SERVICE(&bt_uuid_pwm.uuid),

  /* Duty cycle */
  BT_GATT_CHARACTERISTIC(&bt_uuid_duty_cycle.uuid,
             BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY |
                         BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
             BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
             duty_cycle_read, duty_cycle_write, &duty_cycle),

  /* Digit of pi */
  BT_GATT_CHARACTERISTIC(&bt_uuid_pi_digit.uuid,
             BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
             BT_GATT_PERM_READ,
             pi_digit_read, NULL, &pi_digit),
);

static const struct bt_data ad[] = {
  BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
  BT_DATA_BYTES(BT_DATA_UUID128_ALL,
    0x30, 0x5F, 0x1A, 0xC8, 0x46, 0x0B, 0x42, 0x7E,
    0x8C, 0xA1, 0x52, 0xB9, 0xFF, 0x1F, 0xC0, 0x5F)
};

static ssize_t pi_digit_read
  (struct bt_conn *conn, const struct bt_gatt_attr *attr,
   void *buf, u16_t len, u16_t offset)
{
  printk("pi_digit_read: %d\n", pi_digit);
  const char *value = attr->user_data;
  return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(pi_digit));
}

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
  int on_nsec = PWM_PERIOD_NSEC * duty_cycle / 100;
  int ret = pwm_pin_set_nsec(pwm, PWM_CHANNEL, PWM_PERIOD_NSEC, on_nsec, PWM_FLAGS);
  if (ret) {
    printk("Error %d: failed to set pulse width\n", ret);
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

void pi_calc_thread() {
  PiCalcState st;
  calc_pi_init(&st);
  while (true) {
    int digit = calc_pi(&st);
    pi_digit = digit;
    k_sleep(PI_SEND_INTERVAL);
  }
}

#define PI_THREAD_STACK_SIZE 500
#define PI_THREAD_PRIORITY 5

// Start pi calculation thread.
K_THREAD_DEFINE(pi_thread, PI_THREAD_STACK_SIZE,
                pi_calc_thread, NULL, NULL, NULL,
                PI_THREAD_PRIORITY, 0, 1000);

void main(void)
{
  pwm = device_get_binding(PWM_LABEL);
  if (!pwm) {
    printk("Error: didn't find %s device\n", PWM_LABEL);
    return;
  }

  int err = bt_enable(NULL);
  if (err) {
    printk("Bluetooth init failed (err %d)\n", err);
    return;
  }

  bt_ready();

  bt_conn_cb_register(&conn_callbacks);

  // Set up GPIO button interrupt & callback.

  while (1) { k_sleep(K_SECONDS(1));}
}
