#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

static void gpio_setup(void)
{
	/* Enable GPIOC clock. */
	/* Manually: */
	// RCC_APB2ENR |= RCC_APB2ENR_IOPCEN;
	/* Using API functions: */
	rcc_periph_clock_enable(RCC_GPIOB);

	/* Manually set the whole GPIOB to GPIO_MODE_OUTPUT_2_MHZ 
	 * and GPIO_CNF_OUTPUT_PUSHPULL*/
	GPIOB_CRH = 0x22222222;
	/* Using API functions to set the others */
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, GPIO6);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, GPIO7);
}

int main(void)
{
	int i;

	gpio_setup();

	// Select the display
	//gpio_set(GPIOB, GPIO7);
	GPIOB_ODR = 0xFE80;
	gpio_clear(GPIOB, GPIO7);

	// Select the segment
	//gpio_set(GPIOB,GPIO6);
	// Send segment data
	// This is char E with GPIOB_6 (segment selection bit)
	GPIOB_ODR = 0x7940;
	//gpio_clear(GPIOB,GPIO6);

	while (1) {
		for (i = 0; i < 800000; i++)	/* Wait a bit. */
			__asm__("nop");
		// Blink the dot on the display
		gpio_toggle(GPIOB,GPIO15);
	}

	return 0;
}
