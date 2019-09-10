#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

// Define a table of displaying 0 - F on the 
__CHAR16_TYPE__ table[] = {
	0x3f, 0x06, 0x5b, 0x4f, 0x66,
	0x6d, 0x7d, 0x07, 0x7f, 0x6f,
	0x77, 0x7c, 0x39, 0x5e, 0x79,
	0x71};

int digit = 0; // The digit to show on 7 segment display

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

static void nvic_setup(void)
{
	/* Without this the timer interrupt routine will never be called. */
	nvic_enable_irq(NVIC_TIM2_IRQ);
	nvic_set_priority(NVIC_TIM2_IRQ, 1);
}
void show_1digit(int d)
{
	// A single digit can be displayed by
	// Segment selection bit
	//gpio_set(GPIOB,GPIO6);
	// Send segment data
	//GPIOB_ODR = 0x7940;
	// Clear the segment selection bit
	//gpio_clear(GPIOB,GPIO6);

	// This is char in GPIOB_8 to GPIOB_15 with segment selection bit on GPIOB_6 - 0x40
	GPIOB_ODR = (table[d] << 8) | 0x40;
	// for (i = 0; i < 800000; i++) /* Wait a bit. */
	// 	__asm__("nop");
	// // Blink the dot on the display
	// gpio_toggle(GPIOB, GPIO15);
	// for (i = 0; i < 800000; i++) /* Wait a bit. */
	// 	__asm__("nop");
}

void clock_setup(void)
{
	rcc_clock_setup_in_hse_16mhz_out_72mhz();
}

void timer_setup(void)
{
	rcc_periph_clock_enable(RCC_TIM2);

	/* Set timer start value. */
	TIM_CNT(TIM2) = 1;

	/* 72MHz/1440 => 50000 counts per second. */
	// However, ABP1 can be 36MHz maximum, when perpheral clock is 72MHz 
	// in order to met the 36MHz limit, the clock is divided when offering
	// into TIM. And then TIM multiple it by 2 so that it can go back to 72MHz.
	// So, in this way, PSC should be 1440/2 to make it 1 second.
	TIM_PSC(TIM2) = 1440/2;

	/* End timer value. If this is reached an interrupt is generated. */
	TIM_ARR(TIM2) = 50000;

	/* Update interrupt enable. */
	TIM_DIER(TIM2) |= TIM_DIER_UIE;

	/* Start timer. */
	TIM_CR1(TIM2) |= TIM_CR1_CEN;

}

void tim2_isr(void)
{
	digit++;
	digit = digit % 10;
	show_1digit(digit);
	TIM_SR(TIM2) &= ~TIM_SR_UIF; /* Clear interrrupt flag. */
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

	clock_setup();
    nvic_setup();
    timer_setup();
	while (1);

	return 0;
}
