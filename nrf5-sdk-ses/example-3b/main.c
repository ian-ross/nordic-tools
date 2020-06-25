#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"
#include "app_pwm.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "app_timer.h"
#include "ble_nus.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


// Tag identifying SoftDevice BLE configuration.
#define APP_BLE_CONN_CFG_TAG 1

// Name of device, included in advertising data.
#define DEVICE_NAME "PWM_Blinky"

// UUID type for Nordic UART Service (vendor specific).
#define NUS_SERVICE_UUID_TYPE BLE_UUID_TYPE_VENDOR_BEGIN

// Application's BLE observer priority. Shouldn't need to modify this
// value.
#define APP_BLE_OBSERVER_PRIO 3

// Advertising interval (in units of 0.625 ms. This value corresponds
// to 40 ms).
#define APP_ADV_INTERVAL 64

// Advertising duration (180 seconds) in units of 10 milliseconds.
#define APP_ADV_DURATION  18000

// Minimum and maximum acceptable connection interval (20 ms),
// Connection intervals use 1.25 ms units.
#define MIN_CONN_INTERVAL MSEC_TO_UNITS(20, UNIT_1_25_MS)
#define MAX_CONN_INTERVAL MSEC_TO_UNITS(75, UNIT_1_25_MS)

// Slave latency.
#define SLAVE_LATENCY 0

// Connection supervisory timeout (4 seconds), Supervision Timeout
// uses 10 ms units.
#define CONN_SUP_TIMEOUT MSEC_TO_UNITS(4000, UNIT_10_MS)

// Time from initiating event (connect or start of notification) to
// first time sd_ble_gap_conn_param_update is called (5 seconds).
#define FIRST_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(5000)

// Time between each call to sd_ble_gap_conn_param_update after the
// first call (30 seconds).
#define NEXT_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(30000)

// Number of attempts before giving up the connection parameter
// negotiation.
#define MAX_CONN_PARAMS_UPDATE_COUNT 3



APP_PWM_INSTANCE(PWM, 1);                         // PWM instance using TIMER1.
BLE_NUS_DEF(m_nus, NRF_SDH_BLE_TOTAL_LINK_COUNT); // BLE NUS service instance.
NRF_BLE_GATT_DEF(m_gatt);                         // GATT module instance.
NRF_BLE_QWR_DEF(m_qwr);                           // Context for Queued Write module.
BLE_ADVERTISING_DEF(m_advertising);               // Advertising module instance.

// Handle of the current connection.
static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;

// Maximum length of data (in bytes) that can be transmitted to the
// peer by the Nordic UART service module.
static uint16_t m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - 3;

// Universally unique service identifier.
static ble_uuid_t m_adv_uuids[] = { {BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE} };


void pwm_ready_callback(uint32_t pwm_id) // PWM callback function
{ }

// Function for putting the chip into sleep mode.
static void sleep_mode_enter(void)
{
  // Go to system-off mode (this function will not return; wakeup will cause a reset).
  APP_ERROR_CHECK(sd_power_system_off());
}

// Advertising event handler.
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
  switch (ble_adv_evt) {
  case BLE_ADV_EVT_IDLE:
    sleep_mode_enter();
    break;
  default:
    break;
  }
}

// BLE event handler.
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
  switch (p_ble_evt->header.evt_id) {
  case BLE_GAP_EVT_CONNECTED:
    NRF_LOG_INFO("Connected");
    m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
    APP_ERROR_CHECK(nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle));
    break;

  case BLE_GAP_EVT_DISCONNECTED:
    NRF_LOG_INFO("Disconnected");
    // LED indication will be changed when advertising starts.
    m_conn_handle = BLE_CONN_HANDLE_INVALID;
    break;

  case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
    {
      NRF_LOG_INFO("PHY update request.");
      ble_gap_phys_t const phys =
        {
          .rx_phys = BLE_GAP_PHY_AUTO,
          .tx_phys = BLE_GAP_PHY_AUTO,
        };
      APP_ERROR_CHECK(sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys));
    } break;

  case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
    // Pairing not supported
    APP_ERROR_CHECK(sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL));
    break;

  case BLE_GATTS_EVT_SYS_ATTR_MISSING:
    // No system attributes have been stored.
    APP_ERROR_CHECK(sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0));
    break;

  case BLE_GATTC_EVT_TIMEOUT:
    // Disconnect on GATT Client timeout event.
    APP_ERROR_CHECK(sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                          BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION));
    break;

  case BLE_GATTS_EVT_TIMEOUT:
    // Disconnect on GATT Server timeout event.
    APP_ERROR_CHECK(sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                          BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION));
    break;

  default:
    // No implementation needed.
    break;
  }
}

static void ble_stack_init(void)
{
  ret_code_t err_code;

  APP_ERROR_CHECK(nrf_sdh_enable_request());

  // Configure the BLE stack using the default settings.
  // Fetch the start address of the application RAM.
  uint32_t ram_start = 0;
  APP_ERROR_CHECK(nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start));

  // Enable BLE stack.
  APP_ERROR_CHECK(nrf_sdh_ble_enable(&ram_start));

  // Register a handler for BLE events.
  NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

// Set up all the necessary GAP (Generic Access Profile) parameters of
// the device. Also sets the permissions and appearance.
static void gap_params_init(void)
{
  ble_gap_conn_sec_mode_t sec_mode;
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

  APP_ERROR_CHECK(sd_ble_gap_device_name_set
                  (&sec_mode, (const uint8_t *) DEVICE_NAME, strlen(DEVICE_NAME)));

  ble_gap_conn_params_t gap_conn_params;
  memset(&gap_conn_params, 0, sizeof(gap_conn_params));
  gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
  gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
  gap_conn_params.slave_latency     = SLAVE_LATENCY;
  gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;
  APP_ERROR_CHECK(sd_ble_gap_ppcp_set(&gap_conn_params));
}

// GATT library event handler.
void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{
  if ((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED)) {
    m_ble_nus_max_data_len = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
    NRF_LOG_INFO("Data len is set to 0x%X(%d)", m_ble_nus_max_data_len, m_ble_nus_max_data_len);
  }
  NRF_LOG_INFO("ATT MTU exchange completed. central 0x%x peripheral 0x%x",
               p_gatt->att_mtu_desired_central,
               p_gatt->att_mtu_desired_periph);
}

// Initialize GATT library.
void gatt_init(void)
{
  APP_ERROR_CHECK(nrf_ble_gatt_init(&m_gatt, gatt_evt_handler));
  APP_ERROR_CHECK(nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE));
}

// Queued Write Module error handler.
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
  APP_ERROR_HANDLER(nrf_error);
}


// Nordic UART Service data handler.
static void nus_data_handler(ble_nus_evt_t * p_evt)
{
  NRF_LOG_INFO("NUS event: %d", p_evt->type);

  if (p_evt->type == BLE_NUS_EVT_RX_DATA) {
    NRF_LOG_INFO("Received data from BLE NUS. Setting PWM duty cycle.");
    NRF_LOG_HEXDUMP_INFO(p_evt->params.rx_data.p_data, p_evt->params.rx_data.length);

    // TODO: PARSE DATA AND SET PWM DUTY CYCLE HERE
  }
}

// Initialize services used by the application.
static void services_init(void)
{
  // Queued Write Module.
  nrf_ble_qwr_init_t qwr_init = {0};
  qwr_init.error_handler = nrf_qwr_error_handler;
  APP_ERROR_CHECK(nrf_ble_qwr_init(&m_qwr, &qwr_init));

  // NUS.
  // TODO: REPLACE WITH CUSTOM RX-ONLY SERVICE.
  ble_nus_init_t nus_init;
  memset(&nus_init, 0, sizeof(nus_init));
  nus_init.data_handler = nus_data_handler;
  APP_ERROR_CHECK(ble_nus_init(&m_nus, &nus_init));
}

// Initialize advertising functionality.
static void advertising_init(void)
{
  ble_advertising_init_t init;
  memset(&init, 0, sizeof(init));

  init.advdata.name_type          = BLE_ADVDATA_FULL_NAME;
  init.advdata.include_appearance = false;
  init.advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;

  init.srdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
  init.srdata.uuids_complete.p_uuids  = m_adv_uuids;

  init.config.ble_adv_fast_enabled  = true;
  init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
  init.config.ble_adv_fast_timeout  = APP_ADV_DURATION;
  init.evt_handler = on_adv_evt;

  APP_ERROR_CHECK(ble_advertising_init(&m_advertising, &init));
  ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

// Connection parameters event handler.
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
  if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED) {
    APP_ERROR_CHECK(sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE));
  }
}

// Connection parameters error handler.
static void conn_params_error_handler(uint32_t nrf_error)
{
  APP_ERROR_HANDLER(nrf_error);
}

// Initialize connection parameters module.
static void conn_params_init(void)
{
  ble_conn_params_init_t cp_init;
  memset(&cp_init, 0, sizeof(cp_init));

  cp_init.p_conn_params                  = NULL;
  cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
  cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
  cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
  cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
  cp_init.disconnect_on_fail             = false;
  cp_init.evt_handler                    = on_conn_params_evt;
  cp_init.error_handler                  = conn_params_error_handler;

  APP_ERROR_CHECK(ble_conn_params_init(&cp_init));
}

int main(void)
{
  // Initialisation based on ble_app_uart example.

  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();

  APP_ERROR_CHECK(app_timer_init());

  APP_ERROR_CHECK(nrf_pwr_mgmt_init());

  ble_stack_init();
  gap_params_init();
  gatt_init();
  services_init();
  advertising_init();
  conn_params_init();

  // 1-channel PWM, 5 kHz, output on DK LED pins.
  app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_1CH(200L, BSP_LED_0);

  // Initialize and enable PWM.
  APP_ERROR_CHECK(app_pwm_init(&PWM, &pwm1_cfg, pwm_ready_callback));
  app_pwm_enable(&PWM);

  // Make duty cycle range from 0 to 100% and back down again repeatedly.
  while (true) {
    for (uint8_t i = 0; i < 100; ++i) {
      // Duty cycle ramps up and down between 0 and 100%.
      uint32_t duty_cycle;
      if (i < 50) {
        duty_cycle = i * 2; // Up
      } else {
        duty_cycle = (100 - i) * 2;  // Down
      }

      // Set the duty cycle -- keep trying until PWM is ready.
      while (app_pwm_channel_duty_set(&PWM, 0, duty_cycle) == NRF_ERROR_BUSY) ;
      nrf_delay_ms(10);
    }
  }
}
