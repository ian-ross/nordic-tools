#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"
#include "app_pwm.h"


APP_PWM_INSTANCE(PWM, 1);  // Create instance "PWM" using TIMER1.

void pwm_ready_callback(uint32_t pwm_id) // PWM callback function
{ }

int main(void)
{
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
