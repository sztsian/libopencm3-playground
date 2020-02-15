#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <libopencm3/stm32/usart.h>

#include "nrf24l01.h"

#define NRF_SPI_CSN_PORT	GPIOB
#define NRF_SPI_CSN_PIN	GPIO2
#define NRF_SPI_CE_PORT	GPIOB
#define NRF_SPI_CE_PIN	GPIO0
#define SPI_CS_HIGH		gpio_set(NRF_SPI_CSN_PORT, NRF_SPI_CSN_PIN)
#define SPI_CS_LOW		gpio_clear(NRF_SPI_CSN_PORT, NRF_SPI_CSN_PIN)

#ifdef WITH_CONIO
#include "conio.h"
#include "serial.h"
#endif

// Delay factor
#define DF 10

ssize_t _write      (int file, const char *ptr, ssize_t len);

static void usart_setup(void)
{
	AFIO_MAPR |= AFIO_MAPR_USART1_REMAP;
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_RE_TX);

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

static void spi_setup(void) {

	/* Configure GPIOs: SS=PA4, SCK=PA5, MISO=PA6 and MOSI=PA7 */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
			GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO4 |
							GPIO5 |
							GPIO7 );

	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO6);

	/* Configure CS pin on PB2. */
	gpio_set_mode(NRF_SPI_CSN_PORT, GPIO_MODE_OUTPUT_50_MHZ,
			GPIO_CNF_OUTPUT_PUSHPULL, NRF_SPI_CSN_PIN);
	gpio_set_mode(NRF_SPI_CE_PORT, GPIO_MODE_OUTPUT_50_MHZ,
			GPIO_CNF_OUTPUT_PUSHPULL, NRF_SPI_CE_PIN);
	/* Reset SPI, SPI_CR1 register cleared, SPI is disabled */
	spi_reset(SPI1);

	spi_set_unidirectional_mode(SPI1); /* We want to send only. */
	spi_set_dff_8bit(SPI1);
	spi_set_full_duplex_mode(SPI1);
	spi_send_msb_first(SPI1);

	/* Handle the CS signal in software. */
	spi_enable_software_slave_management(SPI1);
	spi_set_nss_high(SPI1);

	/* PCLOCK/8 as clock. */
	spi_set_baudrate_prescaler(SPI1, SPI_CR1_BR_FPCLK_DIV_8);

	/* We want to control everything and generate the clock -> master. */
	spi_set_master_mode(SPI1);
	spi_set_clock_polarity_0(SPI1); /* SCK idle state low. */

	/* Bit is taken on the second (falling edge) of SCK. */
	spi_set_clock_phase_0(SPI1);
	spi_enable_ss_output(SPI1);

	SPI_CS_HIGH;

	spi_enable(SPI1);
}

void nrf_init(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_AFIO);
	rcc_periph_clock_enable(RCC_GPIOC);


	/* Enable SPI1 Periph and gpio clocks */
	rcc_periph_clock_enable(RCC_SPI1);
	rcc_periph_clock_enable(RCC_USART1);

	spi_setup();
	usart_setup();
}

void nrf_spi_csh(void)
{
     SPI_CS_HIGH;
}

void nrf_spi_csl(void)
{
     SPI_CS_LOW;
}

unsigned char nrf_spi_xfer_byte(unsigned char data)
{
     return spi_xfer(SPI1, data);
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

static void gpio_setup(void)
{
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
	/* Enable GPIOC clock. */
	/* Manually: */
	// RCC_APB2ENR |= RCC_APB2ENR_IOPCEN;
	/* Using API functions: */
	//rcc_periph_clock_enable(RCC_GPIOC);

	/* Set GPIO13 (in GPIO port C) to 'output push-pull'. */
	/* Manually: */
	// GPIOC_CRH = (GPIO_CNF_OUTPUT_PUSHPULL << (((12 - 8) * 4) + 2));
	// GPIOC_CRH |= (GPIO_MODE_OUTPUT_2_MHZ << ((12 - 8) * 4));
	/* Using API functions: */

}

void delay(unsigned long n)
{
	unsigned long i;

	while(n--) {
		i = 2;
		while(i--) __asm__("nop");
	}
}


/**
 * Configure the NRF into ShockBurst without autoretry. Set device as PTX.
 */
void nrf_configure_esb_tx(void) {
	gpio_clear(NRF_SPI_CE_PORT, NRF_SPI_CE_PIN);
	// Set address for TX and receive on P0
 	nrf_reg_buf addr;

	addr.data[0] = 0xF0;
	addr.data[1] = 0xF0;
	addr.data[2] = 0xF0;
	addr.data[3] = 0xF0;
	addr.data[4] = 0xE1;

 	// set devicde into ESB mode as PTX
	nrf_preset_esb(NRF_MODE_PTX, 76, 32, 3, NRF_RT_DELAY_250, &addr);

	// Wait for radio to power up
	delay(10000 * DF);
	gpio_set(NRF_SPI_CE_PORT, NRF_SPI_CE_PIN);
}

#ifdef WITH_CONIO

void nrf_dump_regs(nrf_regs *r) {

	int i;
	int j;

	cio_print("\n\r** START nRF2401 Register DUMP **\n\r");

	nrf_reg_buf buf;

	for(i = 0; i < r->count; i++) {

		nrf_read_reg(i, &buf);

		if(r->data[i].size == 0) continue;

		cio_print(r->data[i].name);
		cio_print(": ");

		for(j = 0; j < buf.size; j++) {
			cio_printb(buf.data[j], 8);
			cio_printf(" (%x) ", buf.data[j]);
		}

		cio_print("\n\r - ");

		for(j = 0; j < r->data[i].fields->count; j++) {
			cio_printf("%u[%u]:%s=%u ", j,
				r->data[i].fields->data[j].size,
				r->data[i].fields->data[j].name,
				nrf_get_reg_field(i, j, &buf));
		}

		cio_print("-\n\r");
	}

	cio_print("** END nRF2401 Register DUMP **\n\r");
}

#endif

int main(void)
{
	gpio_setup();
	nrf_init();
	nrf_configure_esb_tx();
#ifdef WITH_CONIO
	nrf_dump_regs(&nrf_reg_def);
#endif
    usart_setup();
	static nrf_payload   p;
	printf("Config done");
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, GPIO1);
	int s;

	p.size = 1;
	p.data[0] = 0;

	while (1) {
		printf("Sending payload: %x ", p.data[0]);

		s = nrf_send(&p);

		if(s == NRF_ERR_MAX_RT) {
			printf("- Unable to send message (MAX_RT)\n\r");
		}
		else if(s == NRF_ERR_TX_FULL) {
			printf("- Unable to send message (TX_FULL)\n\r");
		}
		else {
			printf(" - done; bytes send: %u\n\r", s);
		}

		delay(50000 * DF);

		p.data[0]++;
	}


	return 0;
}
