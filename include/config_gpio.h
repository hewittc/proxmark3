//-----------------------------------------------------------------------------
// This code is licensed to you under the terms of the GNU GPL, version 2 or,
// at your option, any later version. See the LICENSE.txt file for the text of
// the license.
//-----------------------------------------------------------------------------
// GPIO pin mapping for the Proxmark3
//-----------------------------------------------------------------------------

#ifndef __CONFIG_GPIO_H
#define __CONFIG_GPIO_H

// LCD backlight control
#define GPIO_LCD_BL			AT91C_PIO_PA0
#define GPIO_LED_A			GPIO_LCD_BL
#define GPIO_LED_B			GPIO_LCD_BL
#define GPIO_LED_C			GPIO_LCD_BL
#define GPIO_LED_D			GPIO_LCD_BL

// LCD chip select
#define GPIO_LCD_CS			AT91C_PIO_PA10

// LDO control
#define GPIO_NVDD_ON			AT91C_PIO_PA26

// USB pull-up
#define GPIO_USB_PU			AT91C_PIO_PA24

// Clock to FPGA
#define GPIO_PCK0			AT91C_PA6_PCK0

// SSP lines for talking to peripherals
#define GPIO_NCS0			AT91C_PA11_NPCS0
#define GPIO_NCS1			AT91C_PA9_NPCS1
#define GPIO_NCS2			AT91C_PA10_NPCS2
#define GPIO_NCS3			AT91C_PA5_NPCS3
#define GPIO_MISO			AT91C_PA12_MISO
#define GPIO_MOSI			AT91C_PA13_MOSI
#define GPIO_SPCK			AT91C_PA14_SPCK

// FPGA serial bus
#define GPIO_SSC_FRAME			AT91C_PA15_TF
#define GPIO_SSC_CLK			AT91C_PA16_TK
#define GPIO_SSC_DOUT			AT91C_PA17_TD
#define GPIO_SSC_DIN			AT91C_PA18_RD

// FPGA configuration lines
#define GPIO_FPGA_NINIT			AT91C_PIO_PA25
#define GPIO_FPGA_ON			AT91C_PIO_PA26
#define GPIO_FPGA_DONE			AT91C_PIO_PA27
#define GPIO_FPGA_NPROGRAM		AT91C_PIO_PA28
#define GPIO_FPGA_CCLK			AT91C_PIO_PA29
#define GPIO_FPGA_DIN			AT91C_PIO_PA30
#define GPIO_FPGA_DOUT			AT91C_PIO_PA31

#define GPIO_MUXSEL_HIPKD		AT91C_PIO_PA8
#define GPIO_MUXSEL_LOPKD		AT91C_PIO_PA8
#define GPIO_MUXSEL_HIRAW		AT91C_PIO_PA8
#define GPIO_MUXSEL_LORAW		AT91C_PIO_PA8
#define GPIO_RELAY			AT91C_PIO_PA8

// Joystick buttons
#define GPIO_BUTTON_D			AT91C_PIO_PA19
#define GPIO_BUTTON_L			AT91C_PIO_PA20
#define GPIO_BUTTON_R			AT91C_PIO_PA21
#define GPIO_BUTTON_U			AT91C_PIO_PA22
#define GPIO_BUTTON			AT91C_PIO_PA23

#endif
