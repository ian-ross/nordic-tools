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

#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <bluetooth/services/bas.h>


static ssize_t duty_cycle_read
  (struct bt_conn *conn, const struct bt_gatt_attr *attr,
   void *buf, u16_t len, u16_t offset);
static ssize_t duty_cycle_write
  (struct bt_conn *conn, const struct bt_gatt_attr *attr,
   const void *buf, u16_t len, u16_t offset, u8_t flags);

static struct bt_uuid_128 bt_uuid_pwm =
  BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x3D5F1AC8, 0x460B, 0x427E, 0x8CA1, 0x52B9FF1FC05F));

static struct bt_uuid_128 bt_uuid_duty_cycle =
  BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x2E0B1FF1, 0x1984, 0x496F, 0xBDF0, 0x29896AEC6302));

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)
#define ADV_LEN 12

uint16_t duty_cycle = 0;

BT_GATT_SERVICE_DEFINE(pwm_svc,
	BT_GATT_PRIMARY_SERVICE(&bt_uuid_pwm.uuid),

	/* Duty cycle */
	BT_GATT_CHARACTERISTIC(&bt_uuid_duty_cycle.uuid,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY |
                         BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
			       duty_cycle_read, duty_cycle_write, &duty_cycle),
);

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN)
};

static ssize_t duty_cycle_read
  (struct bt_conn *conn, const struct bt_gatt_attr *attr,
   void *buf, u16_t len, u16_t offset)
{
  printk("duty_cycle_read\n");
	return 0;
}

static ssize_t duty_cycle_write
  (struct bt_conn *conn, const struct bt_gatt_attr *attr,
   const void *buf, u16_t len, u16_t offset, u8_t flags)
{
  printk("duty_cycle_write\n");
	return 0;
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
	int err;

	printk("Bluetooth initialized\n");

	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}

	printk("Advertising successfully started\n");
}


void main(void)
{
	int err;

	err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	bt_ready();

	bt_conn_cb_register(&conn_callbacks);

	while (1) { k_sleep(K_SECONDS(1));}
}
