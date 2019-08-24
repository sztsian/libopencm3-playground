#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

// Define a table of displaying 0 - F on the 
__CHAR16_TYPE__ table[] = {
	0x3f, 0x06, 0x5b, 0x4f, 0x66,
	0x6d, 0x7d, 0x07, 0x7f, 0x6f,
	0x77, 0x7c, 0x39, 0x5e, 0x79,
	0x71};
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

	// ---- First part, show on the 1st display only ----
	// Select the first display
	GPIOB_ODR = 0xFE80;
	// Clear display select bit
	gpio_clear(GPIOB, GPIO7);

	// A single digit can be displayed by
	// Segment selection bit
	//gpio_set(GPIOB,GPIO6);
	// Send segment data
	//GPIOB_ODR = 0x7940;
	// Clear the segment selection bit
	//gpio_clear(GPIOB,GPIO6);

	int digit = 0;
	for (digit = 0; digit < 16; digit++)
	{
		// This is char in GPIOB_8 to GPIOB_15 with segment selection bit on GPIOB_6 - 0x40
		GPIOB_ODR = (table[digit] << 8) | 0x40;
		for (i = 0; i < 800000; i++) /* Wait a bit. */
			__asm__("nop");
		// Blink the dot on the display
		gpio_toggle(GPIOB, GPIO15);
		for (i = 0; i < 800000; i++) /* Wait a bit. */
			__asm__("nop");
	}

	// ---- Second part, show the same on all display ----

	// Select all the 6 display
	GPIOB_ODR = 0xC080;
	// Clear display select bit
	gpio_clear(GPIOB, GPIO7);

	for (digit = 0; digit < 16; digit++)
	{
		GPIOB_ODR = (table[digit] << 8) | 0x40;
		for (i = 0; i < 800000; i++) /* Wait a bit. */
			__asm__("nop");
		// Blink the dot on the display
		gpio_toggle(GPIOB, GPIO15);
		for (i = 0; i < 800000; i++) /* Wait a bit. */
			__asm__("nop");
	}

	// ---- Third part, numbers in a series (1-6, 2-7 etc.) ----
	int firstchar = 0;
	for (firstchar = 0; firstchar < 11; firstchar++)
	{
		int inttime = 0;
		for (inttime = 0; inttime < 100; inttime++)
		{
			// Select display
			int intdisplay = 0;
			for (intdisplay = 0; intdisplay < 6; intdisplay++)
			{
				// High 8 bit: display selection data; 0x80: display selection bit
				GPIOB_ODR = (~(0x1 << intdisplay) << 8) | 0x80;
				gpio_clear(GPIOB, GPIO7);
				GPIOB_ODR = (table[firstchar + intdisplay] << 8) | 0x40;
				gpio_clear(GPIOB, GPIO6);
				// The wait is essential, otherwise 
				for (i = 0; i < 8000; i++)
					__asm__("nop");
			}

			// Clear display select bit
		}
	}
	
	// In the end, show E in case of messy display
	GPIOB_ODR = 0xFE80;
    GPIOB_ODR = (table[15] << 8) | 0x40;
	// Blink the dot to show the program is still running
	while (1)
	{
		gpio_toggle(GPIOB, GPIO15);
		for (i = 0; i < 800000; i++) /* Wait a bit. */
			__asm__("nop");
	}

	return 0;
}
