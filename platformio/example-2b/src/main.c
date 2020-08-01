#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/pwm.h>

// DT node for PWM LED.
#define PWM_LED0_NODE	DT_ALIAS(pwm_led0)

// PWM frequency is 40 kHz => period is 25 us
#define PWM_PERIOD_NSEC 25000

// Every 10ms, ...
#define DUTY_STEP_TIME_MS 10

// ... step the duty cycle by 1%.
#define DUTY_STEP_NS 250


#define FLAGS_OR_ZERO(node)						\
	COND_CODE_1(DT_PHA_HAS_CELL(node, pwms, flags),		\
		    (DT_PWMS_FLAGS(node)),				\
		    (0))

#if DT_NODE_HAS_STATUS(PWM_LED0_NODE, okay)
#define PWM_LABEL	DT_PWMS_LABEL(PWM_LED0_NODE)
#define PWM_CHANNEL	DT_PWMS_CHANNEL(PWM_LED0_NODE)
#define PWM_FLAGS	FLAGS_OR_ZERO(PWM_LED0_NODE)
#else
#error "Unsupported board: pwm-led0 devicetree alias is not defined"
#define PWM_LABEL	""
#define PWM_CHANNEL	0
#define PWM_FLAGS	0
#endif


void main(void)
{
  printk("Starting PWM blinky...\n");

  struct device *pwm = device_get_binding(PWM_LABEL);
	if (!pwm) {
		printk("Error: didn't find %s device\n", PWM_LABEL);
		return;
	}

  int on_nsec = 0;
  int dir = 1;
	while (1) {
		int ret = pwm_pin_set_nsec(pwm, PWM_CHANNEL, PWM_PERIOD_NSEC, on_nsec, PWM_FLAGS);
		if (ret) {
			printk("Error %d: failed to set pulse width\n", ret);
			return;
		}

    on_nsec += dir * DUTY_STEP_NS;
    if (on_nsec < 0 || on_nsec > PWM_PERIOD_NSEC) {
      dir = -dir;
      if (on_nsec < 0) on_nsec = 0;
      if (on_nsec > PWM_PERIOD_NSEC) on_nsec = PWM_PERIOD_NSEC;
    }
    
		k_msleep(DUTY_STEP_TIME_MS);
	}
}
