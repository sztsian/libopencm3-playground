#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

__CHAR16_TYPE__ table[]={
	0x3f, 0x06, 0x5b, 0x4f, 0x66,
	0x6d, 0x7d, 0x07, 0x7f, 0x6f,
	0x77, 0x7c, 0x39, 0x5e, 0x79,
	0x71
};
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
	// This is char E in GPIOB_8 to GPIOB_15 with GPIOB_6 (segment selection bit, 0x40)
	GPIOB_ODR = 0x7940;
	//gpio_clear(GPIOB,GPIO6);
    int digit=0;
	while (1) {
		digit = digit % 16;
		GPIOB_ODR = ( table[digit]<<8 ) | 0x40;
		digit = digit + 1;
		for (i = 0; i < 800000; i++)	/* Wait a bit. */
			__asm__("nop");
		// Blink the dot on the display
		gpio_toggle(GPIOB,GPIO15);
		for (i = 0; i < 800000; i++)	/* Wait a bit. */
			__asm__("nop");
	}

	return 0;
}
