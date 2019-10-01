#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
//#include <libopencm3/cm3/nvic.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

ssize_t _write      (int file, const char *ptr, ssize_t len);

// Note: Need to use GNU ARM Embedded toolchain from ARM.com

static void clock_setup(void)
{
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	/* Enable GPIO clock and USART clock. */
	rcc_periph_clock_enable(RCC_GPIOC); // for LED
	rcc_periph_clock_enable(RCC_GPIOA); // for USART1
	rcc_periph_clock_enable(RCC_AFIO); // USART is alternative function
	rcc_periph_clock_enable(RCC_USART1);

}

static void usart_setup(void)
{
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);

	/* Setup UART parameters. */
	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART1, USART_MODE_TX);

	/* Finally enable the USART. */
	usart_enable(USART1);
}

static void gpio_setup(void)
{
	gpio_set(GPIOC, GPIO13);

	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
}

ssize_t _write (int file, const char *ptr, ssize_t len)
{
	int i;

	if (file == STDOUT_FILENO || file == STDERR_FILENO) {
		for (i = 0; i < len; i++) {
			if (ptr[i] == '\n') {
				usart_send_blocking(USART1, '\r');
			}
			usart_send_blocking(USART1, ptr[i]);
		}
		return i;
	}
	errno = EIO;
	return -1;
}

int main(void)
{

	int counter = 0;
	float fcounter = 0.0;
	double dcounter = 0.0;
	volatile int i, j = 0, c = 0;
	int isleep;
	clock_setup();
	usart_setup();
	gpio_setup();


	/* Blink the LED (PC13) on the board. */
	for (i=0; i<10; i++) {
		gpio_toggle(GPIOC, GPIO13);
    	for (isleep = 0; isleep < 800000; isleep++)	/* Wait a bit. */
        	__asm__("nop");
		usart_send_blocking(USART1, c + '0');	/* USART2: Send byte. */
		c = (c == 9) ? 0 : c + 1;	/* Increment c. */
		if ((j++ % 10) == 9) {	/* Newline after line full. */
			usart_send_blocking(USART1, '\r');
			usart_send_blocking(USART1, '\n');
		}
	}

	while (1) {
		gpio_toggle(GPIOC, GPIO13);
    	for (isleep = 0; isleep < 800000; isleep++)	/* Wait a bit. */
        	__asm__("nop");
		//printf("Hello World!");
		printf("Hello World! %i %f %f\r\n", counter, fcounter, dcounter);
		//fflush(stdout);
		counter++;
		fcounter += 0.01;
		dcounter += 0.01;
	}

	return 0;
}
