/* PiFunk lite----License-------GPL v3.0->see LICENSE.md !!------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include <float.h>
#include <iso646.h>
#include <time.h>
#include <malloc.h>
#include <signal.h>
#include <ctype.h>
#include <inttypes.h>
#include <gnumake.h>
#include <pthread.h>
#include <sys/mman.h>
/*
#include <stdalign.h>
#include <stdnoreturn.h>
#include <stdatomic.h>
#include <argp.h>
#include <utime.h>
#include <sched.h>
#include <locale.h>
#include <errno.h>
#include <wchar.h>
#include <wctype.h>
#include <fcntl.h>
#include <dirent.h>
#include <assert.h>
#include <setjmp.h>
#include <termios.h>
#include <tgmath.h>
#include <complex.h>
#include <features.h>
#include <fenv.h>
#include <grp.h>
#include <pwd.h>
#include <poll.h>
#include <uchar.h>
//#include <limits.h>
#include <sys/cdefs.h>
#include <sys/time.h>
#include <sys/timex.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/file.h>
#include <sys/sysmacros.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <sys/poll.h>
*/
// I2C
//#include <linux/i2c.h>
//#include <linux/i2c-dev.h>
// FW
//#include <drm/drm_fb_cma_helper.h>
// broadcom arm processor for mapping phys. addresses
//#include <bcm_host.h>
//#include "opt/vc/include/bcm_host.h" // firmware stuff
//#include "opt/vc/include/interface/vcos/vcos.h" // Video Core OS Abstraction Layer
// activate for your specific system
//#include <bcm2835.h> // -lbcm2835
//#include "bcm2835/src/bcm2835.h" // pi 0/1 v1.3
// RPi.GPIO lib, 0.7.0 used with pi4 support
//#include "RPI.GPIO/source/i2c.h"
//#include "RPI.GPIO/source/c_gpio.h"
//#include "RPI.GPIO/source/event_gpio.h"
//#include "RPI.GPIO/source/py_pwm.h"
//#include "RPI.GPIO/source/soft_pwm.h"
//#include "RPI.GPIO/source/constants.h"
//#include "RPI.GPIO/source/common.h"
//#include "RPI.GPIO/source/cpuinfo.h"
// custom header for pifunk (definitions)
//#include "include/pifunk.h"
// preproccessor definitions
#ifndef PIFUNK_C
	#define PIFUNK_C
#endif
#ifdef __LINUX__
	#warning Program runs under LINUX!
	#pragma GCC dependency "pifunk.h"
#endif
#ifdef __UNIX__
	#warning Program runs under UNIX!
	#pragma GCC dependency "pifunk.h"
#endif
#ifdef __ARM__
  #warning Program runs under ARM-Architecture!
  #pragma ARM
	// same as -CODE32
#endif
#ifdef __ARM64__
  #warning Program runs under ARM64-Architecture!
  #pragma ARM64
#endif
#ifdef __GNUC__ // gcc
  #warning Using GNU C with ANSI ISO C99 as GNU99!
  //#pragma GCC system_header
	//#pragma GCC push(visibility)
  //#pragma GCC visibility pop
  #define EXPORT __attribute__((visibility("default")))
  #define EXPORT_HIDDEN __attribute__((visibility("hidden")))
  #define IMPORT
#endif
#ifdef _GNU_SOURCE
		#warning Using GNU Source Macro!
   //#define basename __basename_gnu
#endif
#ifdef _POSIX
  #warning Using POSIX standard!
  #define _POSIX_C_SOURCE   		(200809L) // or 199309L
  //#define _USE_MATH_DEFINES 1 // for math lm lib
#endif
#ifdef __STDC_VERSION__
  #warning Using GNU C with C99 standard!
  //#define _STDC_VERSION (199901L)  // -std=c99
#endif
// definitions & macros
//#define NDEBUG
#define VERSION 											"0.1.7.6" // my version
#define VERSION_MAJOR        					(0) //
#define VERSION_MINOR        					(1) //
#define VERSION_BUILD        					(7) //
#define VERSION_PATCH        					(6) //
#define VERSION_STATUS 			 					"lite" // reduced, only neccessary stuf
#define IN                    				(0) //
#define OUT                   				(1) //
#define FALSE               					(0) //
#define TRUE                 					(1) //
// predefine if needed when not using bcm header
#define USLEEP 							  				[1000] // sleep timer
// mathematical stuff
#define EULER                         (2.718281828459045235360287471352f) // log e(EULER) = 0.4342944819
//#define log(EULER)                    (0.4342944819)
#define lg(EULER)                     (1.44269504089)
#define ln(x)                         (log(x)/log(EULER))
#define PI                            (3.14159265358979323846) // radial constant
#define PHASE                         (2*PI) // 6.28318530718
#define PERIOD                        (1/PHASE) // 0.15915494309
#define HALF_PERIOD                   (1/PI) // 0.31830988618
#define AMPLITUDE                     (1) // for sampling I/Q must be constant
// buffers
#define BLOCK_SIZE                    (4*1024) // 4096
#define BUFFER_LEN                    (8*1024) // 8192
#define BUFFERINSTRUCTIONS            (65536) // [1024]
#define F_XTAL    						 		  	(19200000.0)
// I-O access via GPIO
volatile unsigned 										(*gpio); //
volatile unsigned 										(*allof7e); // shouuld be null in the begining
// GPIO setup macros: Always use INP_GPIO (x) before using OUT_GPIO (x) or SET_GPIO_ALT (x, y)
#define ALLOF7EB											(*allof7e-SUB_BASE)
#define GPIO_SET 											*(gpio+7)  // setsbits which are 1 ignores bits which are 0
#define GPIO_CLR 											*(gpio+10) // clears bits which are 1 ignores bits which are 0
#define GPIO_GET 											*(gpio+13) // sets bits which are 1 ignores bits which are 0
#define INP_GPIO(g)                   *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3)) // % means here Modulo-operation for remainder
#define OUT_GPIO(g)                   *(gpio+((g)/10)) |=  (1<<(((g)%10)*3)) //
#define SET_GPIO_ALT(g, a)            *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))
// specific pi addresses & definitions + alternative old/different versions
#ifdef  RPI // old RPIs meant here
#define PERIPH_VIRT_BASE               (0x20000000) // dec:536870912
#define PERIPH_PHYS_BASE               (0x7E000000) // dec:536870912
#define BCM2835_VIRT_BASE              (0x20000000) // dec:536870912
#define DRAM_PHYS_BASE                 (0x40000000) // dec: 1073741824
#define GPIO_BASE_OFFSET               (0x00200000) // dec: 2097152
#define MEM_FLAG                       (0x0C) // alternative
#define CURBLOCK                       (0x04) // dec: 4
#define CLOCK_BASE									   (19.2E6) //
#define XTAL_CLOCK                     (54.0E6) // = 54000000
#define DMA_CHANNEL										 (14) //
#define PLLD_FREQ 										 (500000000) //
#define F_PLLD_CLK 										 (500000000.0)
#define BUFFER_TIME 									 (1000000) //
#define PWM_WRITES_PER_SAMPLE 				 (10) //
#define PWM_CHANNEL_RANGE 						 (32) //
#endif
// pi 0 zero & w
#ifdef 	RASPI0 //== 0
#define PERIPH_VIRT_BASE               (0x20000000) // base=GPIO_offset dec: 2 virtual base
#define PERIPH_PHYS_BASE               (0x7E000000) // dec: 2113929216
#define BCM2835_VIRT_BASE              (0x20000000) // dec:536870912
#define DRAM_PHYS_BASE                 (0x40000000) // dec: 1073741824
#define GPIO_BASE_OFFSET               (0x00200000) // dec: 2097152
#define MEM_FLAG                       (0x0C) // alternative
#define CURBLOCK                       (0x0C) // dec: 12
#define CLOCK_BASE										 (19.2E6) // = 19200000
#define XTAL_CLOCK                     (54.0E6) // = 54000000
#define DMA_CHANNEL										 (14) //
#define PLLD_FREQ											 (500000000) //
#define F_PLLD_CLK 										 (500000000.0)
#define BUFFER_TIME 									 (1000000) //
#define PWM_WRITES_PER_SAMPLE 				 (10) //
#define PWM_CHANNEL_RANGE 						 (32) //
#endif
// pi 1 - BCM2835 -> my version
#ifdef  RASPI1 // == 1
#define PERIPH_VIRT_BASE               (0x20000000) // base=GPIO_offset dec: 2 virtual base
#define PERIPH_PHYS_BASE               (0x7E000000) // dec: 2113929216
#define BCM2835_PERI_BASE              (0x3F000000) //
#define BCM2835_VIRT_BASE              (0x20000000) // dec:536870912
#define DRAM_PHYS_BASE                 (0x40000000) // dec: 1073741824
#define GPIO_BASE_OFFSET               (0x00200000) // dec: 2097152
#define MEM_FLAG                       (0x0C) // alternative
#define CURBLOCK                       (0x0C) // dec: 12
#define CLOCK_BASE										 (19.2E6) //
#define XTAL_CLOCK                     (54.0E6) // = 54000000
#define PAGE_SIZE                      (1024) // 4096
#define DMA_CHANNEL										 (14) //
#define PLLD_FREQ											 (500000000.) //
#define F_PLLD_CLK 									   (500000000.0)
#define BUFFER_TIME 									 (1000000) //
#define PWM_WRITES_PER_SAMPLE 				 (10) //
#define PWM_CHANNEL_RANGE 						 (32) //
#endif
// pi 2 - BCM2836/7
#ifdef  RASPI2 //== 2
#define PERIPH_VIRT_BASE               (0x3F000000) // dec: 1056964608
#define PERIPH_PHYS_BASE               (0x7E000000) // dec: 2113929216
#define BCM2836_PERI_BASE              (0x3F000000) // register physical address dec: 1056964608 alternative name
#define BCM2837_PERI_BASE              (0x3F000000) // later version 2.1
#define DRAM_PHYS_BASE                 (0xC0000000) // dec: 3221225472
#define GPIO_BASE_OFFSET               (0x00200000) // dec: 2097152
#define MEM_FLAG                       (0x04) // dec: 4
#define CURBLOCK                       (0x04) // dec: 4 memflag
#define CLOCK_BASE									   (19.2E6) //
#define XTAL_CLOCK                     (54.0E6) // = 54000000
#define PAGE_SIZE                      (1024) // 4096
#define DMA_CHANNEL										 (14) //
#define PLLD_FREQ 										 (500000000) //
#define F_PLLD_CLK 									   (500000000.0)
#define BUFFER_TIME 									 (1000000) //
#define PWM_WRITES_PER_SAMPLE 				 (10) //
#define PWM_CHANNEL_RANGE 						 (32) //
#endif
// pi3 - BCM2837/B0
#ifdef 	RASPI3 //== 3
#define PERIPH_VIRT_BASE               (0x20000000) // dec: 536870912
#define PERIPH_PHYS_BASE               (0x7E000000) // dec: 2113929216
#define BCM2837_PERI_BASE              (0x3F000000) // register physical address dec: 1056964608 alternative name
#define BCM2837B0_PERI_BASE            (0x3F000000) // 3B+ and 3A+
#define DRAM_PHYS_BASE                 (0xC0000000) // dec: 3221225472
#define GPIO_BASE_OFFSET               (0x00200000) // dec: 2097152
#define MEM_FLAG                       (0x04) // dec: 4
#define CURBLOCK                       (0x04) // dec: 4 memflag
#define CLOCK_BASE									   (19.2E6) //
#define XTAL_CLOCK                     (54.0E6) // = 54000000
#define PAGE_SIZE                      (1024) // 4096
#define DMA_CHANNEL										 (14) //
#define PLLD_FREQ 										 (500000000) //
#define F_PLLD_CLK										 (500000000.0)
#define BUFFER_TIME 									 (1000000) //
#define PWM_WRITES_PER_SAMPLE 				 (10) //
#define PWM_CHANNEL_RANGE 						 (32) //
#endif
// pi 4 - BCM2838
#ifdef  RASPI4 //== 4
#define BCM2708_PERI_BASE 						 (0xFE000000) //
#define PERIPH_VIRT_BASE               (0xFE000000) // dec: 4261412864
#define PERIPH_PHYS_BASE               (0x7E000000) // dec: 2113929216
#define GPIO_BASE_OFFSET               (0x7E215000) // GPIO register base address
#define BCM2838_PERI_BASE              (0x3F000000) // dec: 1056964608
#define BCM2711_PERI_BASE              (0x3F000000) // coprocessor !!!
#define DRAM_PHYS_BASE                 (0xC0000000) // dec: 3221225472
#define MEM_FLAG                       (0x04) // dec: 4
#define CURBLOCK                       (0x04) // dec: 4 memflag
#define CLOCK_BASE									   (19.2E6) // = 19200000
#define XTAL_CLOCK                     (54.0E6) // = 54000000
#define F_XTAL 												 (54000000.0)
#define PAGE_SIZE 										 (4096) //
#define DMA_CHANNEL                    (14) // 4A
#define DMA_CHANNELB                   (7) // BCM2711 (Pi 4 B only)  chan=7
#define F_PLLD_CLK 										 (750000000.0)
#define PLLD_FREQ 										 (750000000) // has higher freq than pi 0-3
#define BUFFER_TIME 									 (1000000) //
#define PWM_WRITES_PER_SAMPLE 				 (10) //
#define PWM_CHANNEL_RANGE 						 (32) //
#define CLOCK_ALT 									   (0xFFFF8FFF) // dec: 4294938623
#define CLOCK_BACK 										 (0xFFFFFFC7) // dec: 4294967239
#endif
// standard & general definitions
#define PIN_7                           (4) // pin 4
#define GPIO_4                          (PIN_7)
#define PIN_40                          (40) // pin 40
#define GPIO_21                         (PIN_40)
#define GPIO_LEN                        (0x100) // dec: 256
#define LENGTH                          (0x01000000) // dec:	16777216 for peripherial size
#define SUB_BASE                        (0x7E000000) // dec: 2113929216 phys base
#define CLKBASE                         (0x7E101000) // dec: 2114981888
#define CLK_BASE_OFFSET                 (0x00101000) // dec: 1052672
#define CLK0_BASE_OFFSET 							  (0x00101070) // dec: 1052784
#define CLK1_BASE_OFFSET                (0x00101078) // dec: 1052792
#define CLK_LEN                         (0x1300) // dec: 4864
#define PWMBASE                         (0x7E20C000) // controller dec: 2116075520
#define MODULATE                        (0x4D72) // dec: 19826
#define FIFO                            (0x18)   // dec: 24
#define CARRIER                         (0x5A)   // dec: 90
#define DMABASE                         (0x7E007000) // dec: 2113957888
#define DMAREF                          (0x7F)   // dec: 127 dma base reference
#define DMAC                            (0x0707) // dec: 1799
#define DMA0_BASE_OFFSET                (0x00007000) // dec: 28672 -> dma7=0x700, dma14 = 0xE00
#define DMA15_BASE_OFFSET 						  (0x00E05000) // dec: 14700544 -> 0x00
#define PWMCLK_CNTL0                    (0x5A000016) // dec: 1509949462
#define PWMCLK_DIV0                     (0x5A002800) // dec: 1509959680
#define PWMCLK_BASE_OFFSET              (0x001010A0) // dec: 1052832
#define PWM_BASE_OFFSET                 (0x0020C000) // dec: 2146304
#define PWM_LEN                         (0x28) // dec: 40
#define TIMER_BASE_OFFSET 						  (0x00003000) // dec: 12288
#define PCM_BASE_OFFSET                 (0x00203000) // dec: 2109440
#define PCM_LEN                         (0x24) // dec: 36
// GPIO
#define GPFSEL0                         (0x00/4) // p.90, dec: 0
#define GPFSEL1                         (0x04/4) // 1
#define GPFSEL2                         (0x08/4) // 2
#define GPFSEL3                         (0x7E200000) // p.90 dec: 2116026368
#define CM_GP0CTL                       (0x7E101070) // p.107 dec: 2114982000
#define CM_GP0DIV                       (0x7E101074) // p.108 dec: 2114982004
#define CM_GP1CTL                       (0x7E101078) //
#define CM_GP1DIV                       (0x7E10107C) //
#define CM_GP2CTL                       (0x7E101080) //
#define CM_GP2DIV                       (0x7E101084) //
#define CM_UART_CTL                     (0x7E1010F0)
#define CM_UART_DIV                     (0x7E1010F4)
#define GPPUD                           (0x94/4) // 37
#define GPPUDCLK0                       (0x98/4) // 38
#define GPPUDCLK1                       (0x9C/4) // 39
// PADS
#define PAD_BASE_OFFSET                 (0x00100000) // dec: 1048576
#define PAD_LEN                         (0x40/4) // 0x10 = dec: 16,  0x64 = dec: 100
#define PADGPIO                         (0x5A000018) // dec: 1509949464
#define GPIO_PAD_0_27                   (0x2C/4)  // 11
#define GPIO_PAD_28_45                  (0x30/4)  // 12
#define GPIO_PAD_46_52                  (0x34/4)  // 13
#define GPCLK_CNTL                      (0x70/4) // 112 / 4 = 28 -> 0x5A = decimal(90)
#define GPCLK_DIV                       (0x74/4) // 29
#define CORECLK_CNTL                    (0x08/4) // 2
#define CORECLK_DIV                     (0x0C/4) // 3
#define EMMCCLK_CNTL                    (0x1C0/4) // 112
#define EMMCCLK_DIV                     (0x1C4/4) // 113
#define CM_PLLA                         (0x104/4) // 65
#define CM_PLLB                         (0x170/4) // 92
#define CM_PLLC                         (0x108/4) // 66
#define CM_PLLD                         (0x10C/4) // 67
#define CM_PLLH                         (0x110/4) // 68
#define CM_LOCK                         (0x114/4) // 69
#define CM_LOCK_FLOCKA                  (1<<8) //
#define CM_LOCK_FLOCKB                  (1<<9) //
#define CM_LOCK_FLOCKC                  (1<<10) //
#define CM_LOCK_FLOCKD                  (1<<11) //
#define CM_LOCK_FLOCKH                  (1<<12) //
#define A2W_PLLA_ANA0                   (0x1010/4) // 10287
#define A2W_PLLB_ANA0                   (0x10F0/4) // 1084
#define A2W_PLLC_ANA0                   (0x1030/4) // 1036
#define A2W_PLLD_ANA0                   (0x1050/4) // 1044
#define A2W_PLLH_ANA0                   (0x1070/4) // 1052
#define A2W_PLL_KA_SHIFT                (7) //
#define A2W_PLL_KI_SHIFT                (19) //
#define A2W_PLL_KP_SHIFT                (15) //
#define PLLA_CTRL                       (0x1100/4) // 1088
#define PLLA_FRAC                       (0x1200/4) // 1152
#define PLLA_DSI0                       (0x1300/4) // 1216
#define PLLA_CORE                       (0x1400/4) // 1280
#define PLLA_PER                        (0x1500/4) // 1344
#define PLLA_CCP2                       (0x1600/4) // 1408
#define PLLB_CTRL                       (0x11E0/4) // 1144
#define PLLB_FRAC                       (0x12E0/4) // 1208
#define PLLB_ARM                        (0x13E0/4) // 1272
#define PLLB_SP0                        (0x14E0/4) // 1336
#define PLLB_SP1                        (0x15E0/4) // 1400
#define PLLB_SP2                        (0x16E0/4) // 1464
#define PLLC_CTRL                       (0x1120/4) // 1196
#define PLLC_FRAC                       (0x1220/4) // 1160
#define PLLC_CORE0                      (0x1620/4) // 1416
#define PLLC_CORE2                      (0x1320/4) // 1224
#define PLLC_CORE1                      (0x1420/4) // 1288
#define PLLC_PER                        (0x1520/4) // 1352
#define PLLD_CTRL                       (0x1140/4) // 1104
#define PLLD_FRAC                       (0x1240/4) // 1168
#define PLLD_DSI0                       (0x1340/4) // 1232
#define PLLD_DSI1                       (0x1640/4) // 1424
#define PLLD_CORE                       (0x1440/4) // 1296
#define PLLD_PER                        (0x1540/4) // 1360
#define PLLH_CTRL                       (0x1160/4) // 1112
#define PLLH_FRAC                       (0x1260/4) // 1176
#define PLLH_AUX                        (0x1360/4) // 1240
#define PLLH_RCAL                       (0x1460/4) // 1304
#define PLLH_PIX                        (0x1560/4) // 1368
#define PLLH_STS                        (0x1660/4) // 1432
// PWM
#define PWM_CTL                         (0x00/4) // 0
#define PWM_FIFO                        (0x18/4) // 6
#define PWM_DMAC                        (0x08/4) // 2
#define PWM_RNG1                        (0x10/4) // 4
#define PWM_RNG2                        (0x20/4) // 8
#define PWMDMAC_ENAB                    (1<<31)  // shift bit to left
#define PWMDMAC_THRSHLD                 ((15<<8)|(15<<0)) // this means it requests as soon as there is one free slot in the FIFO
#define DEVIATION                       (12.50) // in kHz, a-pmr width normal analog
#define DEVIATION2                      (6.25)  // d-pmr width digital
#define DEVIATION3                      (10.00) // CB width
#define DEVIATION4                      (20.00) // dmr width
#define DEVIATION5                      (25.00) // dmr mixed
#define PWMCLK_CNTL                     (40) // offset 0A0
#define PWMCLK_DIV                      (41) // 0A4
#define PWMCTL_CLRF                     (1<<6) //
#define PWMCTL_PWEN1                    (1<<0) //
#define PWMCTL_PWEN2                    (1<<8) //
#define PWMCTL_MODE1                    (1<<1) //
#define PWMCTL_MODE2                    (1<<9) //
#define PWMCTL_RPTL1                    (1<<2) //
#define PWMCTL_RPTL2                    (1<<10) //
#define PWMCTL_POLA1                    (1<<4) //
#define PWMCTL_USEF1                    (1<<5) //
#define PWMCTL_USEF2                    (1<<13) //
#define PWMCTL_MSEN1                    (1<<7) //
#define PWMCTL_MSEN2                    (1<<15) //
// PCM
#define PCMCLK_CNTL                     (38) // 098
#define PCMCLK_DIV                      (39) // 09C
#define PCM_CS_A                        (0x00/4) // 0
#define PCM_FIFO_A                      (0x04/4) // 1
#define PCM_MODE_A                      (0x08/4) // 2
#define PCM_RXC_A                       (0x0C/4) // 3
#define PCM_TXC_A                       (0x10/4) // 4
#define PCM_DREQ_A                      (0x14/4) // 5
#define PCM_INTEN_A                     (0x18/4) // 6
#define PCM_INT_STC_A                   (0x1C/4) // 7
#define PCM_GRAY                        (0x20/4) // 8
// DMA
#define BCM_HOST_GET_PERIPHERAL_SIZE    (0x01000000)
#define BCM2708_DMA_ACTIVE              (1<<0) //
#define BCM2708_DMA_END                 (1<<1) //
#define BCM2708_DMA_INT                 (1<<2) //
#define BCM2708_DMA_WAIT_RESP           (1<<3) //
#define BCM2708_DMA_D_DREQ              (1<<6) //
#define BCM2708_DMA_DST_IGNOR           (1<<7) //
#define BCM2708_DMA_SRC_INC             (1<<8) //
#define BCM2708_DMA_SRC_IGNOR           (1<<11) //
#define BCM2708_DMA_NO_WIDE_BURSTS      (1<<26) //
#define BCM2708_DMA_DISDEBUG            (1<<28) //
#define BCM2708_DMA_ABORT               (1<<30) //
#define BCM2708_DMA_RESET               (1<<31) //
#define BCM2708_DMA_PER_MAP(x)          ((x)<<16) //
#define BCM2708_DMA_PRIORITY(x)         ((x)&0xF<<16) //
#define BCM2708_DMA_PANIC_PRIORITY(x)   ((x)&0xF<<20) //
#define DMA_CHANNEL                     (14) //
#define DMA_CHANNEL_MAX                 (14) //
#define DMA_CHANNEL_SIZE                (0x100) // 256
#define DMA_CONBLK_AD                   (0x04/4) // 1
#define DMA_DEBUG                       (0x20/4) // 8
#define DMA_CS                          (0x00/4) // 0
#define DMA_CS_RESET		                (1<<31) //
#define DMA_CS_ABORT			              (1<<30) //
#define DMA_CS_DISDEBUG		              (1<<29) //
#define DMA_CS_WAIT_FOR_WRITES          (1<<28) //
#define DMA_CS_INT			                (1<<2) //
#define DMA_CS_ACTIVE			              (1<<0) //
#define DMA_CS_END			                (1<<1) //
#define DMA_CS_PRIORITY(x)		          ((x)&0xF<<16) // 0xF=15
#define DMA_CS_PANIC_PRIORITY(x)	      ((x)&0xF<<20) //
// Requests
#define DREQ_PCM_TX                     (2) //
#define DREQ_PCM_RX                     (3) //
#define DREQ_SMI                        (4) //
#define DREQ_PWM                        (5) //
#define DREQ_SPI_TX                     (6) //
#define DREQ_SPI_RX                     (7) //
#define DREQ_SPI_SLAVE_TX               (8) //
#define DREQ_SPI_SLAVE_RX               (9) //
// memory
#define PROT_WRITE											(1) //
#define PROT_READ 											(0) //
#define MAP_SHARED 											(1) //
#define MAP_FIXED 											(0) //
#define MEM_FLAG_DISCARDABLE            (1<<0) // can be resized to 0 at any time. Use for cached data
#define MEM_FLAG_NORMAL                 (0<<2) // normal allocating alias. Don't use from ARM
#define MEM_FLAG_DIRECT                 (1<<2) // 0xC dec: 12 alias uncached
#define MEM_FLAG_COHERENT               (2<<2) // 0x8 dec: 8 alias. Non-allocating in L2 but coherent
#define MEM_FLAG_L1_NONALLOCATING       (MEM_FLAG_DIRECT|MEM_FLAG_COHERENT) // Allocating in L2
#define MEM_FLAG_ZERO                   (1<<4) // initialise buffer to all zeros
#define MEM_FLAG_NO_INIT                (1<<5) // don't initialise (default is initialise to all ones
#define MEM_FLAG_HINT_PERMALOCK         (1<<6) // Likely to be locked for long periods of time.
#define PAGE_SHIFT                      (12) //
#define NUM_PAGES                       ((sizeof(struct control_data_s)+PAGE_SIZE-1)>>PAGE_SHIFT) //
#define NUM_SAMPLES                     (64000) //
#define NUM_CBS                         (NUM_SAMPLES*2) // 128000
#define SUBSIZE                         (1) //
#define DATA_SIZE                       (1000) //
#define SAMPLES_PER_BUFFER 							(512) //
#define GPIO_BASE                       (BCM2835_PERI_BASE + PERIPH_VIRT_BASE) // hex: 0x5F000000 dec: 1593835520
#define GPIO_BASE1                      (BCM2836_PERI_BASE + PERIPH_VIRT_BASE) // hex: 0x5F000000 dec: 1593835520
#define GPIO_BASE2                      (BCM2837_PERI_BASE + PERIPH_VIRT_BASE) // hex: 0x5F000000 dec: 1593835520
#define GPIO_BASE3                      (BCM2837B0_PERI_BASE + PERIPH_VIRT_BASE) // hex: 0x5F000000 dec: 1593835520
#define GPIO_BASE4                      (BCM2838_PERI_BASE + PERIPH_VIRT_BASE) // hex: 0x5F000000 dec: 1593835520
#define GPIO_VIRT_BASE                  (PERIPH_VIRT_BASE + GPIO_BASE_OFFSET) //
#define GPIO_PHYS_BASE                  (PERIPH_PHYS_BASE + GPIO_BASE_OFFSET) //
#define DMA_VIRT_BASE                   (PERIPH_VIRT_BASE + DMA0_BASE_OFFSET) //
#define PWM_VIRT_BASE                   (PERIPH_VIRT_BASE + PWM_BASE_OFFSET) //
#define PWM_PHYS_BASE                   (PERIPH_PHYS_BASE + PWM_BASE_OFFSET) //
#define CLK_VIRT_BASE                   (PERIPH_VIRT_BASE + CLK_BASE_OFFSET) //
#define PAD_VIRT_BASE                   (PERIPH_VIRT_BASE + PAD_BASE_OFFSET) //
#define PCM_VIRT_BASE                   (PERIPH_VIRT_BASE + PCM_BASE_OFFSET) //
#define PCM_PHYS_BASE                   (PERIPH_PHYS_BASE + PCM_BASE_OFFSET) //
#define BUS_TO_PHYS(x)                  ((x)&~0xC0000000) // dec: 3221225472, ~means negotiation/complementary 8inversion)
#define ACCESS(PERIPH_VIRT_BASE)        (PERIPH_VIRT_BASE+ALLOF7EB) // volatile + int* volatile unsigned*
#define SETBIT(PERIPH_VIRT_BASE, bit)   ACCESS(PERIPH_VIRT_BASE) || 1<<bit // |=
#define CLRBIT(PERIPH_VIRT_BASE, bit)   ACCESS(PERIPH_VIRT_BASE) == ~(1<<bit) // &=
const char *description = "experimental - WIP"; // version-stage
const char *device = "default"; // playback device
int w = (0);
int m;
int n;
int i;
int j;
int k;
int l;
int r;
int x = 1; // for ampf calc later
int ex;
char c;
char *gpio_mem;
char *gpio_map;
char *spi0_mem;
char *spi0_map;
unsigned bcm_host_get_peripheral_address (); // // This returns the ARM-side physical address where peripherals are mapped.
unsigned bcm_host_get_peripheral_size (); // This returns the size of the peripheral's space, which is (0x01000000) for all models.
unsigned bcm_host_get_sdram_address (); // This returns the bus address of the SDRAM.
char *filename;
float freq;
float shift_ppm = (0.0);
float xtal_freq = (1.0/19.2E6); // LOCK_BASE
float subfreq;
float ctss_freq;
int samplerate;
int halfsamplerate = (22050/2);
int channels;
int Timing;
char *mod; // = "fm"
char *fm = "fm";
char *am = "am";
int power;
int powerlevel; // same as drive
int DRIVESTRENGTH; // drive 1-7
int HYSTERESIS = (1); // bits: 3, Fieldname: HYST, type: RW, reset 0x1, 0=disabled / 1=enabled
int gpiopin;
float bandwidth;
int dmachannel;
char *type; // a/d
char *mod_type; //
int loop;
bool repeat; // testing for loop
float b;
float divider = (10964912280.7); // PLLD_FREQ/(2000*228*(1.+shift_ppm/1.E6)); // 2000*228-> 500000000/ 456000*(1+0/10000000) -> 1E-7 ,=> Pll/ 0.0456= 10964912280.7, previously as int
//int idivider = (float) divider;
//int fdivider = (int) ((divider-idivider)*pow (2, 12));
int opt;
int menuoption;
int channelnumbercb;
int channelnumberpmr;
int subchannelnumberpmr;
int channelmode;
int freqmode;
int modeselect;
int num;
// files
FILE *rfp, *wfp;
FILE wavefile;
FILE FileFreqTiming;
int MEM_FD; // = open ("/dev/mem"); // , O_RDWR | O_SYNC | O_CREAT | O_TRUNC | O_NONBLOCK
//SNDFILE *infile;
//SNDFILE *outfile;
//snd_output_t *output = 0;
int fp; // = STDIN_FILENO;
int filebit = abs (16); // for now 16 until i can read the value from an audio file
int readcount;
int readBytes;
float datanew = (0);
float dataold = (0);
float data [2*BUFFER_LEN];
float data_filtered [2*BUFFER_LEN];
char data_name [1024];
char buffer [80];
//SF_INFO sfinfo;
float samp1 = (15.75);
float samp2 = (14.50);
int nb_samples;
float timeconst = (50.0E-6); // 0.00005 = => 50 us (microseconds) time constant
int excursion = (6000);
int excursion2 = (32767); // found another value but dont know on what this is based on
float A = (87.6f); // compression amplitude parameter < 90°
int carrier_freq = (87600000); // this might be the carrier too, why this value?
float FactAmplitude = (1.0f); // maybe here amp-modulator input?
float ampf;
float ampf2;
float factorizer;
float sampler;
unsigned long frameinfo;
int instrs [BUFFERINSTRUCTIONS]; // [1024]
int bufPtr;
int instrCnt;
//int instrPage;
//int constPage;
int reg; // = (gpio/10); // volatile unsigned
int shift; // = (gpio%10)*3; // volatile unsigned int shift
void *vAddr;
void *pAddr;
static volatile uint32_t *pwm_reg;
static volatile uint32_t *pcm_reg;
static volatile uint32_t *clk_reg;
static volatile uint32_t *gpio_reg;
static volatile uint32_t *dma_reg;
static volatile uint32_t *pad_reg;
static volatile uint32_t *pad_reg1;
static volatile uint32_t *pad_reg2;
static volatile uint32_t *pad_val;
struct PAGEINFO // should use here bcm intern funcs -> repair p/v
{
		void *p; // physical address BCMXXXX_PERI_BASE
		void *v; // virtual address
		int instrPage[BUFFERINSTRUCTIONS];
		int constPage[BUFFERINSTRUCTIONS];
		int instrs [BUFFERINSTRUCTIONS]; // [1024]
};
//struct PageInfo instrPage[BUFFERINSTRUCTIONS];
//struct PageInfo constPage[BUFFERINSTRUCTIONS];
//struct PageInfo instrs[BUFFERINSTRUCTIONS];
struct GPFSEL0_T
{
    char FSEL0 : 3;
    char FSEL1 : 3;
    char FSEL2 : 3;
    char FSEL3 : 3;
    char FSEL4 : 3;
    char FSEL5 : 3;
    char FSEL6 : 3;
    char FSEL7 : 3;
    char FSEL8 : 3;
    char FSEL9 : 3;
    char RESERVED : 2;
};
struct GPCTL // 9 parameters
{
		char SRC         : 4; // 4 = PLLA per
		char ENAB        : 1; // Enable the clock generator
		char KILL        : 1; // 0 = no action, 1 = stop and reset the clock generator
		char             : 1; // un-used, bit: 23-11, type: R, reset: 0
		char BUSY        : 1; //
		char FLIP        : 1; //
		char MASH        : 2; // 2 -> 3 source / ( DIVI - 1 ) source / ( DIVI + DIVF / 1024 ) source / ( DIVI + 2 )
		unsigned int     : 13; // un-used, type: R, reset: 0
		char PASSWD      : 8; // bits: 31:24, Must be 0x5A when writing: Accidental write protect password, type: W, reset: 0
};
struct CB // control blocks
{
		volatile unsigned int TI;
		volatile unsigned int SOURCE_AD;
		volatile unsigned int DEST_AD;
		volatile unsigned int TXFR_LEN;
		volatile unsigned int STRIDE;
		volatile unsigned int NEXTCONBK;
		volatile unsigned int RES1; // reserved set to 0
		volatile unsigned int RES2; // reserved set to 0
};
struct DMAREGS
{
		volatile unsigned int CS;
		volatile unsigned int CONBLK_AD;
		volatile unsigned int TI;
		volatile unsigned int SOURCE_AD;
		volatile unsigned int DEST_AD;
		volatile unsigned int TXFR_LEN;
		volatile unsigned int STRIDE;
		volatile unsigned int NEXTCONBK;
		volatile unsigned int DEBUG;
};
/*
class ClockOutput : public ClockDevice // pi4 pin fix for under 93 MHz
{
        public:
        #ifndef GPIO_21
        ClockOutput (unsigned divisor) : ClockDevice (CLK0_BASE_OFFSET, divisor)
        {
            output = reinterpret_cast<int *> (peripherals->GetVirtualAddress (GPIO_BASE_OFFSET));
            *output = (*output & 0xFFFF8FFF) | (0x04 << 12);
        #else
        ClockOutput (unsigned divisor) : ClockDevice (CLK1_BASE_OFFSET, divisor)
        {
            output = reinterpret_cast<int *> (peripherals->GetVirtualAddress (GPIO_BASE_OFFSET + 0x08));
            *output = (*output & 0xFFFFFFC7) | (0x02 << 3);
        #endif
        }
        virtual ~ClockOutput ()
        {
        #ifndef GPIO_21
            *output = (*output & 0xFFFF8FFF) | (0x01 << 12);
        #else
            *output = (*output & 0xFFFFFFC7) | (0x02 << 3);
        #endif
        }
}
*/
void clearscreen ()
{
  printf ("\n\033[H\033[J\n");
  //fflush (stdin); // alterntives
  //clsscr ();
  system ("clear"); // works for terminal
  return;
}
void infos () // warnings and infos
{
		/* red-yellow -> color: 1 for "bright" / 4 for "underlined" and \0XX ansi colorcode: 35 for Magenta, 33 red -> \033[14;35m   escape command for resetting \033[0m */
    printf ("\nWelcome to the PiFunk lite! v %s %s for Raspbian/Raspberry Pi OS on ARM! \n", VERSION, description);
   	printf ("\nRadio works with *.wav-file with 16-bit @ 22050 [Hz] Mono / 1-700.00000 MHz frequency \nUse '. dot' as decimal-comma seperator! \n");
    printf ("\nPi operates with square-waves (²/^2) PWM on GPIO 4 (PIN 7 @ ~500 mA & max. +3.3 V). \nUse power supply with enough specs only! \n=> Use Low-/Highpassfilters and/or ~10 uF-cap, isolators or resistors if needed! \nYou can smooth it out with 1:1 balun. Do NOT shortcut, use a dummyload instead! \nCheck laws of your country! \n");
		printf ("\nHELP: Use Parameters to run: \n[-n <filename (*.wav)>] [-f <freq (26.9650)>] [-s <samplerate (22050)>] [-m <mod (fm/am)>] [-t <type (a/d)>] \n[-b <bandwidth (12.5)>] [-p <power (1-7)>] [-g <gpiopin (4/21)>] [-d <dmachannel (7/14)>] [-l <loop (0/1)] \nThere is also an assistant [-a], menu [-u] or help [-h]! The *.wav-file must be 16-bit @ 22050 [Hz] Mono. \n");
		printf ("\nFor testing (default settings) run: sudo ./pifunk -n sound.wav -f 26.9650 -s 22050 -m fm -t a -b 12.5 -p 7 -g 4 -d 14 -l 0 \n");
		return;
}
int fileselect ()  // expected int for opening, char *filename
{
	printf ("\nPlease enter the full path including name of the *.wav-file you want to use: ");
	scanf ("%s", filename);
	printf ("\nOpening file %s ... \n", &filename);
	printf ("\nAllocating filename memory ... \n");
	char *soundname = (char *) malloc (sizeof(filename)); // (char *) allocating memory for filename 128
	//sprintf (soundname, "%s", "file.ft");
	char *stdfile = "sound.wav";
	if (filename != stdfile)
	{
		int fp = open (filename, 0644); // O_RDONLY | O_WRONLY | O_RDWR | O_CREAT | O_TRUNC | O_NONBLOCK,
	}
	else
	{
	  int fp = open ("sound.wav", 0644); // O_WRONLY | O_RDWR | , O_RDONLY | O_CREAT | O_TRUNC | O_NONBLOCK sounds/sound.wav directory should be tested
	}
		printf ("\nFile %s opened ... \n", &filename);
	return (fp);
}
float freqselect () // gets freq by typing in
{
	printf ("\nYou selected 1 for Frequency-Mode \n");
	printf ("\nType in Frequency (0.1-1200.00000 MHz): \n"); // 1B+ for 700 MHz chip, pi3 1.2 GHz pi4
	scanf  ("%f", &freq);
	printf ("\nYou chose: %f MHz \n", freq);
	return (freq);
}
float channelmodepmranalog ()
{
	printf ("\nChoose analog PMR-Channel 1-17 (18 to exit): ");
	scanf ("%d", &channelnumberpmr);
	switch (channelnumberpmr)
	{
	case 0: freq=446.00000; printf ("\naPMR-Chan 0 on %f \n", freq); break;	// base
	case 1: freq=446.00625; printf ("\naPMR-Chan 1 on %f \n", freq); break;	// Standard
	case 2: freq=446.01875; printf ("\naPMR-Chan 2 on %f \n", freq); break; // Geocaching
	case 3: freq=446.03125; printf ("\naPMR-Chan 3 on %f \n", freq); break; // Standard
	case 4: freq=446.04375; printf ("\naPMR-Chan 4 on %f \n", freq); break; // at 3-chan-PMR-devices its ch. 2
	case 5: freq=446.05625; printf ("\naPMR-Chan 5 on %f \n", freq); break; // Contest
	case 6: freq=446.06875; printf ("\naPMR-Chan 6 on %f \n", freq); break; // Events
	case 7: freq=446.08125; printf ("\naPMR-Chan 7 on %f \n", freq); break; // at 3-channel-PMR-devices it's ch. 3
	case 8: freq=446.09375; printf ("\naPMR-Chan 8 on %f \n", freq); break; // Standard
	case 9:  freq=446.10312; printf ("\nDMR-Chan 9 on %f \n", freq); break;
	case 10: freq=446.10625; printf ("\nDMR-Chan 10 on %f \n", freq); break;
	case 11: freq=446.11875; printf ("\nDMR-Chan 11 on %f \n", freq); break;
	case 12: freq=446.13125; printf ("\nDMR-Chan 12 on %f \n", freq); break;
	case 13: freq=446.14375; printf ("\nDMR-Chan 13 on %f \n", freq); break;
	case 14: freq=446.15625; printf ("\nDMR-Chan 14 on %f \n", freq); break;
	case 15: freq=446.16875; printf ("\nDMR-Chan 15 on %f \n", freq); break;
	case 16: freq=446.18125; printf ("\nDMR-Chan 16 on %f \n", freq); break;
	case 17: freq=446.19375; printf ("\nDMR-Chan 17 on %f \n", freq); break;
	case 18: printf ("\nExit ... \n"); exit (0);
	default:	freq=446.00625;
            printf ("\nDefault channelnumber = 1 on freq = %f \n", freq);
            break;
	}
	printf ("\nanalog-freq is %f \n", freq);
	return (freq);
}
float channelmodepmrdigital ()
{
	printf ("\nChoose ditigal PMR-Channel 1-32 (33 to exit): ");
	scanf ("%d", &channelnumberpmr);
	switch (channelnumberpmr)
	{
   case 0:	freq=446.000000; printf ("\ndPMR-Chan 0 on %f \n", freq); break;
	 case 1:	freq=446.003125; printf ("\ndPMR-Chan 1 on %f \n", freq); break;
	 case 2:	freq=446.009375; printf ("\ndPMR-Chan 2 on %f \n", freq); break;
	 case 3:	freq=446.015625; printf ("\ndPMR-Chan 3 on %f \n", freq); break;
	 case 4:	freq=446.021875; printf ("\ndPMR-Chan 4 on %f \n", freq); break;
	 case 5:	freq=446.028125; printf ("\ndPMR-Chan 5 on %f \n", freq); break;
	 case 6:	freq=446.034375; printf ("\ndPMR-Chan 6 on %f \n", freq); break;
	 case 7:	freq=446.040625; printf ("\ndPMR-Chan 7 on %f \n", freq); break;
	 case 8:	freq=446.046875; printf ("\ndPMR-Chan 8 on %f \n", freq); break;
	 case 9:	freq=446.053125; printf ("\ndPMR-Chan 9 on %f \n", freq); break;
	 case 10:	freq=446.059375; printf ("\ndPMR-Chan 10 on %f \n", freq); break;
	 case 11:	freq=446.065625; printf ("\ndPMR-Chan 11 on %f \n", freq); break;
	 case 12:	freq=446.071875; printf ("\ndPMR-Chan 12 on %f \n", freq); break;
	 case 13:	freq=446.078125; printf ("\ndPMR-Chan 13 on %f \n", freq); break;
	 case 14:	freq=446.084375; printf ("\ndPMR-Chan 14 on %f \n", freq); break;
	 case 15:	freq=446.090625; printf ("\ndPMR-Chan 15 on %f \n", freq); break;
	 case 16:	freq=446.096875; printf ("\ndPMR-Chan 16 on %f \n", freq); break;
	 case 17:	freq=446.103125; printf ("\ndPMR-Chan 17 on %f \n", freq); break;
	 case 18:	freq=446.109375; printf ("\ndPMR-Chan 18 on %f \n", freq); break;
	 case 19:	freq=446.115625; printf ("\ndPMR-Chan 19 on %f \n", freq); break;
	 case 20:	freq=446.121875; printf ("\ndPMR-Chan 20 on %f \n", freq); break;
	 case 21:	freq=446.128125; printf ("\ndPMR-Chan 21 on %f \n", freq); break;
	 case 22:	freq=446.134375; printf ("\ndPMR-Chan 22 on %f \n", freq); break;
	 case 23:	freq=446.140625; printf ("\ndPMR-Chan 23 on %f \n", freq); break;
	 case 24:	freq=446.146875; printf ("\ndPMR-Chan 24 on %f \n", freq); break;
	 case 25:	freq=446.153125; printf ("\ndPMR-Chan 25 on %f \n", freq); break;
	 case 26:	freq=446.159375; printf ("\ndPMR-Chan 26 on %f \n", freq); break;
	 case 27:	freq=446.165625; printf ("\ndPMR-Chan 27 on %f \n", freq); break;
	 case 28:	freq=446.171875; printf ("\ndPMR-Chan 28 on %f \n", freq); break;
	 case 29:	freq=446.178125; printf ("\ndPMR-Chan 29 on %f \n", freq); break;
	 case 30:	freq=446.184375; printf ("\ndPMR-Chan 30 on %f \n", freq); break;
	 case 31:	freq=446.190625; printf ("\ndPMR-Chan 31 on %f \n", freq); break;
	 case 32:	freq=446.196875; printf ("\ndPMR-Chan 32 on %f \n", freq); break;
	 case 33: 	printf ("\nExit ... \n"); exit (0);
	 default:		freq=446.003125;
							printf ("\nDefault channelnumber = 1 on freq = %f \n", freq);
							break;
	}
	printf ("\ndigital-freq is %f \n", freq);
	return (freq);
}
float subchannelmodepmr () // Pilot-tone
{
	printf ("\nChoose Standard Subchannel: 0-38 / non-typical 39-50 (51 to exit): ");
	scanf ("%d", &subchannelnumberpmr);
	switch (subchannelnumberpmr)
	{
	case 0:  subfreq=0.000;  printf ("\nCTSS-Chan 0, default base on %f (NoTone) \n", subfreq); break;	// chan 0 base -> no tone
	case 1:  subfreq=67.000; printf ("\nCTSS-Chan 1 on %f \n", subfreq); break;	// 4.90 Hz steps, normal. 67.000
	case 2:  subfreq=71.900; printf ("\nCTSS-Chan 2 on %f \n", subfreq); break;
	case 3:  subfreq=74.400; printf ("\nCTSS-Chan 3 on %f \n", subfreq); break;
	case 4:  subfreq=77.000; printf ("\nCTSS-Chan 4 on %f \n", subfreq); break; // at 3-chan-PMR-devices it's ch. 2
	case 5:  subfreq=79.700; printf ("\nCTSS-Chan 5 on %f \n", subfreq); break; // Contests
	case 6:  subfreq=82.500; printf ("\nCTSS-Chan 6 on %f \n", subfreq); break; // Events
	case 7:  subfreq=85.400; printf ("\nCTSS-Chan 7 on %f \n", subfreq); break; // at 3-channel-PMR-devices it's ch. 3
	case 8:  subfreq=88.500; printf ("\nCTSS-Chan 8 on %f \n", subfreq); break; // Standard opening chan
	case 9:  subfreq=91.500; printf ("\nCTSS-Chan 9 on %f \n", subfreq); break;
	case 10: subfreq=94.800; printf ("\nCTSS-Chan 10 on %f \n", subfreq); break;
	case 11: subfreq=97.400; printf ("\nCTSS-Chan 11 on %f \n", subfreq); break;
	case 12: subfreq=100.000; printf ("\nCTSS-Chan 12 on %f \n", subfreq); break;
	case 13: subfreq=103.500; printf ("\nCTSS-Chan 13 on %f \n", subfreq); break;
	case 14: subfreq=107.200; printf ("\nCTSS-Chan 14 on %f \n", subfreq); break;
	case 15: subfreq=110.900; printf ("\nCTSS-Chan 15 on %f \n", subfreq); break;
	case 16: subfreq=114.800; printf ("\nCTSS-Chan 16 on %f \n", subfreq); break;
	case 17: subfreq=118.800; printf ("\nCTSS-Chan 17 on %f \n", subfreq); break;
	case 18: subfreq=123.000; printf ("\nCTSS-Chan 18 on %f \n", subfreq); break;
	case 19: subfreq=127.300; printf ("\nCTSS-Chan 19 on %f \n", subfreq); break;
	case 20: subfreq=131.800; printf ("\nCTSS-Chan 20 on %f \n", subfreq); break;
	case 21: subfreq=136.500; printf ("\nCTSS-Chan 21 on %f \n", subfreq); break;
	case 22: subfreq=141.300; printf ("\nCTSS-Chan 22 on %f \n", subfreq); break;
	case 23: subfreq=146.200; printf ("\nCTSS-Chan 23 on %f \n", subfreq); break;
	case 24: subfreq=151.400; printf ("\nCTSS-Chan 24 on %f \n", subfreq); break;
	case 25: subfreq=156.700; printf ("\nCTSS-Chan 25 on %f \n", subfreq); break;
	case 26: subfreq=162.200; printf ("\nCTSS-Chan 26 on %f \n", subfreq); break;
	case 27: subfreq=167.900; printf ("\nCTSS-Chan 27 on %f \n", subfreq); break;
	case 28: subfreq=173.800; printf ("\nCTSS-Chan 28 on %f \n", subfreq); break;
	case 29: subfreq=179.900; printf ("\nCTSS-Chan 29 on %f \n", subfreq); break;
	case 30: subfreq=186.200; printf ("\nCTSS-Chan 30 on %f \n", subfreq); break;
	case 31: subfreq=192.800; printf ("\nCTSS-Chan 31 on %f \n", subfreq); break;
	case 32: subfreq=203.500; printf ("\nCTSS-Chan 32 on %f \n", subfreq); break;
	case 33: subfreq=210.700; printf ("\nCTSS-Chan 33 on %f \n", subfreq); break;
	case 34: subfreq=218.100; printf ("\nCTSS-Chan 34 on %f \n", subfreq); break;
	case 35: subfreq=225.700; printf ("\nCTSS-Chan 35 on %f \n", subfreq); break;
	case 36: subfreq=233.600; printf ("\nCTSS-Chan 36 on %f \n", subfreq); break;
	case 37: subfreq=241.800; printf ("\nCTSS-Chan 37 on %f \n", subfreq); break;
	case 38: subfreq=250.300; printf ("\nCTSS-Chan 38 on %f \n", subfreq); break;
	case 39: subfreq=69.300;  printf ("\nCTSS-Chan 39 on %f \n", subfreq); break;
	case 40: subfreq=159.800; printf ("\nCTSS-Chan 40 on %f \n", subfreq); break;
	case 41: subfreq=165.500; printf ("\nCTSS-Chan 41 on %f \n", subfreq); break;
	case 42: subfreq=171.300; printf ("\nCTSS-Chan 42 on %f \n", subfreq); break;
	case 43: subfreq=177.300; printf ("\nCTSS-Chan 43 on %f \n", subfreq); break;
	case 44: subfreq=183.500; printf ("\nCTSS-Chan 44 on %f \n", subfreq); break;
	case 45: subfreq=189.900; printf ("\nCTSS-Chan 45 on %f \n", subfreq); break;
	case 46: subfreq=196.600; printf ("\nCTSS-Chan 46 on %f \n", subfreq); break;
	case 47: subfreq=199.500; printf ("\nCTSS-Chan 47 on %f \n", subfreq); break;
	case 48: subfreq=206.500; printf ("\nCTSS-Chan 48 on %f \n", subfreq); break;
	case 49: subfreq=229.100; printf ("\nCTSS-Chan 49 on %f \n", subfreq); break;
	case 50: subfreq=254.100; printf ("\nCTSS-Chan 50 on %f \n", subfreq); break;
	case 51:  printf ("\nExit ... \n"); exit (0);
	default:  subfreq=67.000;
						printf ("\nDefault subchannel = 1 on subfreq = %f \n", subfreq);
						break;
	}
	printf ("\nSubchannelnumber = %d on subfreq = %f \n", subchannelnumberpmr, subfreq);
	return (subfreq);
}
// Channel-mode
char channelmodepmr () // PMR
{
	printf ("\nChoose PMR-Type a for analog / d for digital: ");
	scanf ("%s", type);
	if (strcmp (type, "a"))
	{
		printf ("\nYou chose type analog \n");
		channelmodepmranalog ();
	}
	else if (strcmp (type, "d"))
	{
		printf ("\nYou chose type digital \n");
		channelmodepmrdigital ();
	}
	else
	{
		type="a";
		printf ("\nNO type could be determined, wrong input! Using %s as standard \n", type);
	}
	printf ("\nOn type = %s with channelnumberpmr = %d on freq = %f \n", type, channelnumberpmr, freq);
	return (*type);
}
float channelmodecb () // CB
{
	printf ("\nChoose CB-Channel 0-80 (81 to exit): ");
	scanf ("%d", &channelnumbercb);
	switch (channelnumbercb)
	{
			 case 0:   freq=27.0450; break; // first digital channel / normal opening chan
			 case 1:   freq=26.9650; break; // recommended Startingchannel (FM)
			 case 2:   freq=26.9750; break; // inofficial Mountain-DX-Chan (FM)
			 case 3:   freq=26.9850; break; //
			 case 4:   freq=27.0050; break; // recommended Startingchannel & fix stations (AM)
			 case 5:   freq=27.0150; break; // italian/german truckers
			 case 6:   freq=27.0250; break; // Data channel (D)
			 case 7:   freq=27.0350; break; // Data channel (D)
			 case 8:   freq=27.0550; break;
			 case 9:   freq=27.0650; break; // Trucker channel (AM) / international emergency channel EMG
			 case 10:  freq=27.0750; break; // for antenna calibration / middle for 20-chan-devices
			 case 11:   freq=27.0850; break; // free released chan for hook-up with multiple CB-devices over internet in germany
			 case 1111: freq=27.0950; break; // Eurobalise-Energysuppliers
			 case 12:   freq=27.1050; break; //
			 case 13:   freq=27.1150; break; //
			 case 14:   freq=27.1250; break; // mostly for Toys-remotecontrol (through Selectivetone)
			 case 15:   freq=27.1350; break; // recommended Startingchannel SSB (USB)
			 case 1515: freq=27.1450; break; //
			 case 16:   freq=27.1550; break; // mostly for Watertransport vehicles / ships & boats
			 case 17:   freq=27.1650; break; // daenish/german heavy haulage transport
			 case 18:   freq=27.1750; break; //
			 case 19:   freq=27.1850; break; // recommended Truckerchannel (FM) / Walkie-Talkies sometimes EMG or Babyphones
			 case 1919: freq=27.1950; break; //
			 case 20:   freq=27.2050; break; // for antenna calibration / middle for 40-chan-devices, or autrian heavy hauglage transport
		 	 // 40 chan devices
			 case 21:   freq=27.2150; break; // turkish starting chan (FM)
			 case 22:   freq=27.2250; break; // Walkie-Talkies/mobile devises, Babyphones rumaenian truckers
			 case 23:   freq=27.2550; break; // chan 23, 24, 25 socalled Turners, NOT following 10-kHz-Raster
			 case 24:   freq=27.2350; break; // Data channel (D)
			 case 25:   freq=27.2450; break; // Data channel (D), USB ROS Intern
			 case 26:   freq=27.2650; break; //
			 case 27:   freq=27.2750; break; //
			 case 28:   freq=27.2850; break; // polish truckers sometimes 5 kHz less
			 case 29:   freq=27.2950; break; // free released chan for hook-up with multiple CB-devices over internet
			 case 30:   freq=27.3050; break; // inofficial DX-chan (FM), jugoslawic truckers
			 case 31:   freq=27.3150; break; // inofficial DX-chan (FM)
			 case 32:   freq=27.3250; break; //
			 case 33:   freq=27.3350; break; //
			 case 34:   freq=27.3450; break; // free released chan for hook-up with multiple CB-devices over internet
			 case 35:   freq=27.3550; break; // public Kanal
			 case 36:   freq=27.3650; break; // Datachannel USB ROS international
			 case 37:   freq=27.3750; break; // Gateway-chan austria, FM
			 case 38:   freq=27.3850; break; // inofficial internationaler DX-chan (LSB)
			 case 39:   freq=27.3950; break; // free released chan for hook-up with multiple CB-devices over internet
			 case 40:   freq=27.4050; break; // since march 2016 free released chan for hook-up with multiple CB-devices over internet (FM/AM/SSB in D)
			 // 80 chan devices, on national extra-chan 41 - 80 is only modulation-type FM permitted!
			case 41:   freq=27.5650; break; // since march 2016 free released chan for hook-up with multiple CB-devices over internet (FM), inofficial DX-chan (FM)
			case 42:   freq=27.5750; break; // inofficial DX-chan (FM)
			case 43:   freq=27.5850; break;
			case 44:   freq=27.5950; break;
			case 45:   freq=27.6050; break;
			case 46:   freq=27.6150; break;
			case 47:   freq=27.6250; break;
			case 48:   freq=27.6350; break;
			case 49:   freq=27.6450; break;
			case 50:   freq=27.6550; break;
			case 51:   freq=27.6650; break;
			case 52:   freq=27.6750; break; // Datachannel (D) (FM)
			case 53:   freq=27.6850; break; // Datachannel (D) (FM)
			case 54:   freq=27.6950; break;
			case 55:   freq=27.7050; break;
			case 56:   freq=27.7150; break;
			case 57:   freq=27.7250; break;
			case 58:   freq=27.7350; break;
			case 59:   freq=27.7450; break;
			case 60:   freq=27.7550; break;
			case 61:   freq=26.7650; break; // free released chan for hook-up with multiple CB-devices over internet
			case 62:   freq=26.7750; break;
			case 63:   freq=26.7850; break;
			case 64:   freq=26.7950; break;
			case 65:   freq=26.8050; break;
			case 66:   freq=26.8150; break;
			case 67:   freq=26.8250; break;
			case 68:   freq=26.8350; break;
			case 69:   freq=26.8450; break;
			case 70:   freq=26.8550; break;
			case 71:   freq=26.8650; break; // free released chan for hook-up with multiple CB-devices over internet
			case 72:   freq=26.8750; break;
			case 73:   freq=26.8850; break;
			case 74:   freq=26.8950; break;
			case 75:   freq=26.9050; break;
			case 76:   freq=26.9150; break; // Datachannel (D)(FM)
			case 77:   freq=26.9250; break; // Datachannel (D)(FM)
			case 78:   freq=26.9350; break;
			case 79:   freq=26.9450; break;
			case 80:   freq=26.9550; break; // free released chan for hook-up with multiple CB-devices over internet
			case 81:   printf ("\nExit ... \n");
					   		 exit (0);
			default:	freq=26.9650;
								printf ("\nDefault CB chan = 1 on %f MHz \n", freq);
								break;
	printf ("\nUsing channel = %d on freq = %f \n", channelnumbercb, freq);
	}
	return (freq);
}
int channelselect ()
{
	printf ("\nYou selected 1 for Channel-Mode \n");
	printf ("\nChoose your Band: [1] PMR // [2] CB ");
	scanf  ("%d", &channelmode);
	switch (channelmode) // from here collecting infos and run it step by step, same for freq-mode
	{
					case 1: 	printf ("\nPMR CHAN-MODE \n");
										channelmodepmr (); // gets freq from pmr list
										break;
					case 2: 	printf ("\nCB CHAN-MODE \n");
										channelmodecb ();
										break;
					default: 	printf ("\nDefault: PMR CHAN-MODE \n");
									 	channelmodepmr ();
									 	break;
	}
	return (channelmode);
}
int modetypeselect ()
{
	printf ("\nChoose Mode: [1] Channelmode // [2] Frequencymode ");
	scanf ("%d", &modeselect);
	switch (modeselect)
	{
		case 1:		printf ("\n[1] Channelmode: \n");
							channelselect ();
							break;
		case 2:		printf ("\n[2] Frequencymode: \n");
							freqselect ();
							break;
		default:	printf ("\nError! Using [1] (default) Channelmode! \n");
							channelselect ();
							break;
	}
	return (modeselect);
}
char modulationselect ()
{
	printf ("\nChoose your Modulation [1] FM // [2] AM // [3] Exit : ");
	scanf ("%d", &freqmode);
	switch (freqmode)
	{
		case 1: 	printf ("\nYou selected 1 for FM! \n");
							mod = "fm";
							break;
		case 2:		printf ("\nYou selected 2 for AM! \n");
							mod = "am";
							break;
		case 3:		printf ("\nExiting... \n");
							exit (0);
		default:	printf ("\n Default = 1 (FM) \n");
							mod = "fm";
							break;
	}
	return (*mod);
}
int samplerateselect ()
{
	printf ("\nPlease choose the samplerate [Hz]: 11025 / 22050 (default) / 44100 : ");
	scanf ("%d", &samplerate);
	printf ("\nThe samplerate is %d \n", samplerate);
	if (samplerate == (11025))
	{
		printf ("\nThe samplerate is low \n");
	}
	else if (samplerate == (22050))
	{
		printf ("\nThe samplerate is good!! \n");
		samplerate = (7); //DMA_CHANNEL = 7;
	}
	else if (samplerate == (44100))
	{
		printf ("\nThe samplerate is activated! \n");
	}
	else
	{
		printf ("\nThe DMA-Channel not recognized, using default 22050! \n");
		samplerate = (22050);
	}
	return (samplerate);
}
char typeselect ()
{
	if (!strcmp (type, "a"))
	{
		printf ("\nUsing analog mode \n");
		channelmodepmranalog ();
	}
	else if (!strcmp (type, "d"))
	{
		printf ("\nUsing digital mode \n");
		channelmodepmrdigital ();
	}
	else
	{
		printf ("\nError in -t type \n");
	}
	return (*type);
}
float bandwidthselect ()
{
	printf ("\nChoose Bandwidth-Steps: 6.25 / 10.00 / 12.50 (default) / 20.00 / 25.00 kHz: ");
	scanf ("%f", &bandwidth);
	if (bandwidth==6.25)
	{
	printf ("\nSteps are %f kHz \n", bandwidth);
	}
	else if (bandwidth==10.00)
	{
	printf ("\nSteps are %f kHz \n", bandwidth);
	}
	else if (bandwidth==12.50)
	{
	printf ("\nSteps are %f kHz \n", bandwidth);
	}
	else if (bandwidth==20.00)
	{
	printf ("\nSteps are %f kHz \n", bandwidth);
	}
	else if (bandwidth==25.00)
	{
	printf ("\nSteps are %f kHz \n", bandwidth);
	}
	else
	{
	printf ("\nNO steps could be determined, wrong input! Using standard 12.50 kHz \n");
	bandwidth = DEVIATION;
	}
	return (bandwidth);
}
int powerselect ()
{
	// low-output ~0.4 V -> 400 mV, high-output 2.4-2.9/3.3V -> 2900 mV => a) P=V*A=400mV*2mA=800mW b) 2900*16=mW=46.4W c) 400*16=6400mW=6.4W
	// 0.1024 W for all pins simultaniously -> *16 (all on 1 pin would be 1.6384 W) --> Don't drive capacitive loads btw
	printf ("\nType in powerlevel (DRIVE: 0=2mA, 1=4mA, 2=6mA, 3=8mA, 4=10mA, 5=12mA, 6=14mA, 7=16mA @0.4-3.3V): "); // bits: 2:0, Fieldname: drive, type: RW, reset 0x3
	scanf ("%d", &powerlevel);
	printf ("\nPowerlevel was set to: %d \n", powerlevel);
	power = abs (powerlevel);
	//printf ("\n Type in Hysteresis setting (0=disabled , 1= enabled ): %d \n", HYSTERESIS);
	//scanf ("%d", &HYSTERESIS);
	printf ("\n Hysteresis is %d \n", HYSTERESIS);
	return (power);
}
int gpioselect ()
{
	printf ("\nPlease choose GPIO-Pin (GPIO 4 = PIN 7 default) or GPIO 21 = PIN 40, alternatives: 20, 29, 32, 34, 38 (not recommended): ");
	scanf ("%d", &gpiopin);
	printf ("\nYour GPIO for transmission is %d \n", gpiopin);
	if (gpiopin == 4)
	{
		printf ("\nUsing default GPIO 4 \n");
	}
	else if (gpiopin == 21)
	{
		printf ("\nUsing GPIO 21, mostly used for Pi 4 \n");
	}
	else if (gpiopin == 20 || 29 || 32 || 34 || 38)
	{
		printf ("\nUsing alternative GPIO setup %d \n", gpiopin);
	}
	else
	{
		printf ("\nError: GPIOPin not recognized! Using default GPIO 4! \n");
		gpiopin = (4);
	}
	return (gpiopin);
}
int dmaselect ()
{
	printf ("\nPlease choose the DMA-Channel: 7 (Pi 4) / 14 (default) / 255 (off): ");
	scanf ("%d", &dmachannel);
	printf ("\nThe DMA-Channel is %d \n", dmachannel);
	if (dmachannel == (255))
	{
		printf ("\nThe DMA-Channel is deactivated! \n");
		dmachannel = (255); //DMA_CHANNEL = 255;
	}
	else if (dmachannel == (7))
	{
		printf ("\nThe DMA-Channel is activated! \n");
		dmachannel = (7); //DMA_CHANNEL = 7;
	}
	else if (dmachannel == (14))
	{
		printf ("\nThe DMA-Channel is activated! \n");
		dmachannel = (14); //DMA_CHANNEL = (14);
	}
	else
	{
		printf ("\nThe DMA-Channel not recognized, using default 14! \n");
		dmachannel = (14);
	}
	return (dmachannel);
}
int loopselect ()
{
	printf ("\nChecking loop/repeat \n");
	if (repeat == true)
	{
		loop = (1);
		printf ("\nTWith looping true soundfile infinitelly \n");
	}
	else
	{
		loop = (0);
		printf ("\nFalse loop, only playing once \n");
	}
	return (loop);
}
// memory managment, acual modulater etc.
void usleep2 (long us)
{
	nanosleep ((struct timespec []) { {0, us*1000} }, 0); //
	return;
}
void getRealMemPage (void *vAddr, void *pAddr) // should work through bcm header!
{
		void *m = valloc (4096);
		((int*) m) [0] = (1); // use page to force allocation
		mlock (m, 4096); // lock into ram
		//*vAddr = *m; // we know the virtual address now, error invalid use of void
		int fp = open ("/proc/self/pagemap"); // "w" , O_NONBLOCK | O_RDONLY |
		lseek (fp, ((int) m)/4096*8, SEEK_SET);
		//read (fp, &frameinfo, sizeof (frameinfo));
		//*pAddr = ((int) (frameinfo*4096)); // (void*) error invalid expr voird
		printf ("\nCould not map memory! \n");
		return;
}
void freeRealMemPage (void *vAddr)
{
		printf ("\nTrying to free vAddr ... \n");
		munlock (vAddr, 4096); // unlock ram
		free (vAddr); // free the ram
		printf ("\nvAddr is free now ... \n");
		return;
}
void carrierhigh () // enables it
{
	printf ("\nSetting carrier high ... \n");
	// Set CM_GP0CTL.ENABLE to 1 HIGH (2nd number) as 0x5A -> CARRIER dec: 90
	struct GPCTL setupword = {6, 1, 0, 0, 0, 1, 0x5A}; // set clock to 1 = HIGH
	//ACCESS (CM_GP0CTL) = *((int*) &setupword); // setting cm
	while (!(ACCESS(CM_GP0CTL) &0x80))
	{
		printf ("\nCarrier high yet and waiting ... \n");
	 }; // Wait for busy flag to turn on
	printf ("\nCarrier is high ... \n");
	return;
}
void carrierlow () // disables it
{
	printf ("\nSetting carrier low ... \n");
	struct GPCTL setupword = {6, 0, 0, 0, 0, 1, 0x5A}; // 6 = "SRC", set it to 0 = LOW
	//ACCESS (CM_GP0CTL) = *((int*) &setupword);
	while (ACCESS(CM_GP0CTL)&0x80) {  }; // wait
	printf ("\nCarrier is low ... \n");
	return;
}
void pad ()
{
/*
pad_val = (PADGPIO + power);
if ((pad_reg1 || pad_reg2) == pad_val) // check equality
{
  printf ("\npad_reg1 & pad_reg2 = pad_val -> %p / %p / %p \n", pad_reg1, pad_reg2, pad_val);
}
else
{
  printf ("\npad_reg are NOT the same -> %p / %p / %p \n", pad_reg1, pad_reg2, pad_val);
}
pad_reg = pad_reg [GPIO_PAD_0_27] = (PADGPIO) + power; // pi-gpio bank-row1
pad_reg2 = pad_reg [GPIO_PAD_28_45] = (PADGPIO) + power; // pi-gpio bank-row2
gpio_reg [reg] = (gpio_reg [reg] & ~(7 << shift)); // alternative regshifter
*/
return;
}
void handSig () // exit func
{
		printf ("\nExiting ... \n");
		exit (0);
}
void terminate () // static
{
	printf("\nStop outputting and generating the clock \n");
	if (clk_reg && gpio_reg && vAddr)
	{
        // Set GPIO4 to be an output (instead of ALT FUNC 0, which is the clock)
        gpio_reg [GPFSEL0] = (gpio_reg [GPFSEL0] & ~(7 << 12)) | (1 << 12);
        printf ("\ngpio_reg is %p \n", gpio_reg);
				//printf ("\n%PRIu32\n", gpio_reg);
        // Disable the clock generator
        clk_reg [GPCLK_CNTL] = (0x5A);
        printf ("\nclk_reg is %p \n", clk_reg); // u or lu?
	}
	if (dma_reg && vAddr)
	{
        dma_reg [DMA_CS] = BCM2708_DMA_RESET;
        printf ("\ndma_reg is %p \n", dma_reg);

        //udelay (10);
	}
     //fm_mpx_close ();
     //close_control_pipe ();
	if (vAddr != 0)
	{
      // unmapmem (vAddr, NUM_PAGES); // *4096
      // mem_unlock (mbox.handle, mbox.mem_ref);
      // mem_free (mbox.handle, mbox.mem_ref);
	}
    printf ("\nTerminating: cleanly deactivated the DMA engine and killed the carrier. Exiting \n");
    return;
}
void setupio ()
{
	printf ("\nSetting up FM ... \n");
	//struct sched_param sp[1024]; // error
	//memset (&sp, 0, sizeof (sp));
	//sp.sched_priority = sched_get_priority_max (SCHED_FIFO);
	//sched_setscheduler (0, SCHED_FIFO, &sp);
	//mlockall (MCL_CURRENT | MCL_FUTURE); //error
	// open /dev/mem
	if (MEM_FD < 0)
	{
	printf ("\nCan't open /dev/mem ! \n"); // via bcm possible
	return;
	}
	// Allocate MAP block
	if ((gpio_mem = (char *) malloc(BLOCK_SIZE + (PAGE_SIZE-1))) == 0)
	{
		printf ("\nAllocation error \n");
		exit (0);
	}
	// Make sure pointer is on 4K boundary
	if ((unsigned long) gpio_mem % PAGE_SIZE)
	{
		gpio_mem += PAGE_SIZE - ((unsigned long) gpio_mem % PAGE_SIZE);
		// Now map it
		gpio_map = (unsigned char *) mmap (
		gpio_mem,
		BLOCK_SIZE,
	  PROT_READ | PROT_WRITE, // error
		MAP_SHARED | MAP_FIXED, // error
		MEM_FD,
		GPIO_BASE);
	}
	if ((long) gpio_map < 0)
	{
	    printf ("\nmmap error %d \n", (int) gpio_map);
	    exit (0);
	}
	gpio = (volatile unsigned *) gpio_map;
	carrierhigh ();
	return;
}
void modulate (int l)
{
	printf ("\nModulate carrier ... \n");
	ACCESS(CM_GP0DIV) == ((CARRIER << 24) + (MODULATE + l));  //
	return;
}
void setupfm ()
{
  allof7e = (unsigned*) mmap (
                0,
                BCM_HOST_GET_PERIPHERAL_SIZE, // Peripherial LENGTH
                PROT_READ | PROT_WRITE, // error
                MAP_SHARED, // error
                MEM_FD, //
                PERIPH_VIRT_BASE); // PERIPH_VIRT_BASE, std = 0x20000000
	if ((int) allof7e == (-1))
	{
		exit (0);
	}
	SETBIT (GPFSEL0, 14);
	CLRBIT (GPFSEL0, 13);
	CLRBIT (GPFSEL0, 12);
	struct GPCTL setupword = {6, 1, 0, 0, 0, 1, 0x5A};
	//ACCESS (CM_GP0DIV) = (0x5A << 24) + divider;
	//ACCESS (CM_GP0CTL) = *((int*) &setupword);
  return;
}
/*
void sendByteAsk (unsigned char byte, int sleep)
{
	for (j = 0; j < 8; j++)
	{
	if ((byte&(1 << j)) > 0)
	{
			carrierlow ();
			delayMicrosecondsHard (sleep);
			//usleep2 (sleep);
			carrierhigh ();
			delayMicrosecondsHard (sleep);
			//usleep2 (sleep);
	}
		else if ((byte&(1 << j)) == 0)
	{
			carrierhigh ();
			delayMicrosecondsHard (sleep);
			//usleep2 (sleep);
			carrierlow ();
			delayMicrosecondsHard (sleep);
			//usleep2 (sleep);
		}
	}
	carrierlow ();
  return;
}
void sendStringAsk (char *string, int sleep)
{
	int length = strlen (string);
	for (j = 0; j < length; j++)
	{
		sendByteAsk (string [j], sleep);
	}
  return;
}
*/
void playfm (char *filename, int mod, float bandwidth) // char *filename, float freq, int samplerate
{
	printf ("\nAllocating file to memory for wave-data ... \n");
	/*
	dma_reg = map_peripheral(DMA_VIRT_BASE, (DMA_CHANNEL_SIZE * (DMA_CHANNEL_MAX + 1)));
	dma_reg = dma_reg + (DMA_CHANNEL_SIZE / (sizeof (int) * (DMA_CHANNEL)));
	pwm_reg = map_peripheral(PWM_VIRT_BASE, PWM_LEN);
	gpio_reg = map_peripheral(GPIO_VIRT_BASE, GPIO_LEN);
	pcm_reg = map_peripheral(PCM_VIRT_BASE, PCM_LEN);
	pad_reg = map_peripheral(PAD_VIRT_BASE, PAD_LEN);
	clk_reg = map_peripheral(CLK_VIRT_BASE, CLK_LEN);
	clk_reg[GPCLK_CNTL] = (0x5a<<24) | (1<<4) | (4);
	clk_reg[CM_PLLA] = (0x5A00022A); // Enable PLLA_PER
	pad ();
	*/
	int sz = lseek (fp, 0L, SEEK_END);
	short *data = (short*) malloc (sz);
	for (r = 0; r < 22; r++) // why i less then 22?
	{
        read (fp, &data, 2); // read past header (or sz instead on 2 ?)
        printf ("\nReading fp .. \n");
	}
	while (readBytes == read (fp, &data, 1024))
	{
        int clocksPerSample = (22050/samplerate*1400); // for timing if 22050 then 1400
				printf ("\nclocksPerSample: %d \n", clocksPerSample);
        // if samplerate > 15.75 then clocks per sample is < 1 !! not good
        datanew = ((float) (*data)/excursion); // some constant for unsigned int excursion
				printf ("\ndatanew: %f \n", datanew);
        float fmconstant = (samplerate*timeconst); // 1.1025 for pre-emphasis filter
				printf ("\nfmconstant: %f \n", fmconstant);
        float sample = datanew + (dataold-datanew)/(1-fmconstant); // fir of 1 + s tau
				printf ("\nsample: %f \n", sample);
        float dval = (sample*12.50); // actual transmitted sample
        // 15.75 Hz is standard bandwidth (about 75 kHz), maybe DEVIATION-input here later
				printf ("\ndval: %f \n", dval);
        int intval = (int) (round (dval)); // integer component
				printf ("\nintval: %d \n", intval);
				int div_val = (0x5A<<24)+((int)(divider*pow(2.0, 12))); // divisor
				printf ("\ndiv_val: %d \n", div_val);
        float frac = ((dval-intval)/2+0.5);
				printf ("\nfrac: %f \n", frac);
        int fracval = (frac*clocksPerSample);
				printf ("\nfracval: %d \n", fracval);
        bufPtr++;
        // problem still with .v & .p endings for struct!!
        while (ACCESS ((DMABASE + CURBLOCK) & (~DMAREF)) == (int) (instrs [bufPtr]) ) { }; // CURBLOCK of struct PageInfo, [bufPtr].p
        //usleep2 (1000); // leaving out sleep for faster process
        // Create DMA command to set clock controller to output FM signal for PWM "LOW" time
        //(struct CB*) (instrs [bufPtr].v))->SOURCE_AD = ((int) constPage.p + 2048 + intval*4 - 4);
        bufPtr++;
        //while (ACCESS (DMABASE + 0x04) == (int) (instrs [bufPtr].p));
        //usleep2 (1000);
        // Create DMA command to delay using serializer module for suitable time
        //((struct CB*) (instrs [bufPtr].v))->TXFR_LEN = clocksPerSample-fracval;
        bufPtr++;
        //while (ACCESS (DMABASE + 0x04) == (int) (instrs [bufPtr].p));
        //usleep2 (1000);
        // Create DMA command to set clock controller to output FM signal for PWM "HIGH" time.
        //((struct CB*) (instrs [bufPtr].v))->SOURCE_AD = ((int) constPage.p + 2048 + intval*4+4);
        //while (ACCESS (DMABASE + 0x04) == (int) (instrs [bufPtr].p));
        //usleep2 (1000);
        // Create DMA command for more delay.
        //((struct CB*) (instrs [bufPtr].v))->TXFR_LEN = fracval;
        bufPtr = (bufPtr+1) % (BUFFERINSTRUCTIONS); // [1024] for buffer
        dataold = datanew;
        //ster->consume (data, readBytes); // ster -> for stereo
        printf ("\nReading bytes from fp ... \n");
	}
	close (fp);
	printf ("\nClosing file \n");
	return;
}
void setupDMA ()
{
	printf ("\nSetup of DMA starting ... \n");
	printf ("\ndma_reg is %p \n", dma_reg);
	//atexit (unsetupDMA);
	signal (SIGINT,  handSig);
	signal (SIGTERM, handSig);
	signal (SIGHUP,  handSig);
	signal (SIGQUIT, handSig);
	// allocate a few pages of ram
	//getRealMemPage (&constPage.v, &constPage.p);
	int centerFreqDivider = (int) ((500.000/freq) * (float) (1<<12) + 0.5);
	printf ("\ncenterFreqDivider: %d \n", centerFreqDivider);
	for (i = 0; i < 1024; i++)
	{
	   // ((int*) (constPage.v))[i] = (CARRIER << 24) + centerFreqDivider - 512 + i;
	}
	while (instrCnt < 1024) // BUFFERINSTRUCTIONS
	{
     //getRealMemPage (&instrPage.v, &instrPage.p);
     // make copy instructions
  	 //struct CB* instr0 = (struct CB*)instrPage.v;
		for (i = 0; i < (4096/sizeof (struct CB)); i++)
		{
         /*
         instrs[instrCnt].v = (void*) ((int) instrPage.v + sizeof(struct CB)*i);
         instrs[instrCnt].p = (void*) ((int) instrPage.p + sizeof(struct CB)*i);
         instr0->SOURCE_AD = (unsigned int) constPage.p + 2048;
         instr0->DEST_AD = PWMBASE + (FIFO); // fifo
         instr0->TXFR_LEN = 4;
         instr0->STRIDE = 0;
         instr0->NEXTCONBK = (int) instrPage.p + sizeof(struct CB)*(i+1);
	      // DREQ then PWM then no-wide
         instr0->TI = (1<<6) | (5<<16) |  (1<<26);
         instr0->RES1 = 0;
         instr0->RES2 = 0;
        if (i%2)
	      {
         instr0->DEST_AD = CM_GP0DIV;
         instr0->STRIDE = (4);
         instr0->TI = (1<<26) ;
        }
       if (instrCnt!=0) ((struct CB*) (instrs [instrCnt-1].v))->NEXTCONBK = (int) instrs [instrCnt].p;
       instr0++;
       */
       instrCnt++;
		}
	}
   //((struct CB*) (instrs [1023].v))->NEXTCONBK = (int) instrs [0].p;
   // set up a clock for the base
   //ACCESS (CLKBASE + 40*4) == (PWMCLK_CNTL); // (dec: 1509949478)
   //usleep (1000);
   //ACCESS (CLKBASE + 41*4) == (PWMCLK_DIV); //
   //usleep (1000);
   // set up pwm
   //ACCESS (PWMBASE + 0x0) == 0;
   //usleep (1000);
   //ACCESS (PWMBASE + 0x4) == (-1); // status: clear errors (0x4 in dec: 4)
   //usleep (1000);
   // Use fifo, repeat, serializer, enable ch
   //ACCESS (PWMBASE + 0x0) == (-1 | (1<<13) | (1<<10) | (1<<9) | (1<<8));
   //usleep (1000);
   // DMAC then DMA enable in 0x8 dec: 8 / pwmbase+8 = 7E20C008 (dec:2116075528) /
   //ACCESS (PWMBASE + 0x8) == (1<<31) | (DMAC);
   // activate dma
   //struct DMAREGS* DMA0 = (struct DMAREGS*)(ACCESS (DMABASE));
   //DMA0->CS = 1<<31; // reset
   //DMA0->CONBLK_AD = 0;
   //DMA0->TI = 0;
   //DMA0->CONBLK_AD = (int) (instrPage.p);
   //DMA0->CS = (1<<0) | (255 <<16);  // enable bit = 0, clear end flag = 1, prio=19-16
   printf ("\nSetup DMA done! \n");
   return;
}
void unsetupDMA ()
{
	//struct DMAREGS* DMA0 = (struct DMAREGS* ACCESS(DMABASE) );
	//DMA0->CS = 1<<31; // reset dma controller
	printf ("\nUnsetting DMA done \n");
	return;
}
int sampleselect () // char *filename, int samplerate
{
	printf ("\nSamplerate/bit select starting \n");
	/*
	if (!(fp = open (filename, SFM_READ, &sfinfo))) // check what SFM and  sfinfo do!?
	{   // Open failed so print an error message.
        printf ("\nNot able to open input file for sampleselect %s \n", filename);
				printf ("\nNot able to open filepointer for sampleselct %d \n", fp);
        // Print the error message from libsndfile.
        return (-1);
	}
	//sfinfo.samplerate = samplerate;
	if (sfinfo.samplerate == 48000) // 44 or 48 kHz needs testing
	{
    printf ("\nSamplerate is 48000! (%d) \n", sfinfo.samplerate);
    return sfinfo.samplerate;
	}
	else if (sfinfo.samplerate == 44100)
	{
    printf ("\nSamplerate is 44100! (%d) \n", sfinfo.samplerate);
    return sfinfo.samplerate;
	}
	else if (sfinfo.samplerate == 22050)
	{
		printf ("\nSamplerate is 22050! (%d) \n", sfinfo.samplerate);
		return sfinfo.samplerate;
	}
	else if (sfinfo.samplerate == 14500)
	{
		printf ("\nSamplerate is 14500! (%d) \n", sfinfo.samplerate);
		return sfinfo.samplerate;
	}
	else if (sfinfo.samplerate == 11025)
	{
		printf ("\nSamplerate is 11025! (%d) \n", sfinfo.samplerate);
		return sfinfo.samplerate;
	}
	else
	{
	printf ("\nInput samplerate must be at least 22.050 kHz for FM or 14.500 kHz for AM! \n");
	return (-1);
	}
	*/
	// check filebits here somehow (via SFM)
	if (filebit != 16)
	{
		printf ("\nError: Input must be 16 bit! \n");
		return (-1);
	}
	else
	{
	printf ("\nInput is 16 bit! \n");
  nb_samples = (readcount/channels);
  printf ("\nnb_samples: %d \n", nb_samples);
	while (readcount == read (fp, data, BUFFER_LEN))
	{
	 // where to input the freq like in fm
	for (k = 0; k < nb_samples; k++)
	{
			b = data [k*channels];
			printf ("\nChannel buffer b = %f \n", b);
			if (channels == 0)
			{
				printf ("\nError! NO (0) channels \n"); // > 1 in stereo or dual mono with half samplerate
			return (-1);
			}
			else if (channels == 1)
			{
				printf ("\n File has %d channel (MONO)! \n", channels);
				// stereo file, avg left + right --> should be mono at 22.050 kHz
				b += data [k*channels+1];
				b /= 2.0; // maybe *2 to make a dual mono and not doing stereo in half?
				printf ("\nb = %f \n", b);
			}
			else if (channels == 2)
			{
			printf ("\nFile has 2 Channels (STEREO)! maybe supported later! \n"); // >1 in stereo or dual mono with half samplerate
			printf ("\nUsing half of the samplerate: %d \n", halfsamplerate);
			}
			else
			{
			printf ("\nError: File has %d Channels! Channels > 2 are not supported! \n", channels);
			return (-1);
			}
			// was defined as global var above
			printf ("\nCompression parameter A: %f \n", A);
			// maybe here am option for amplitude factor input!?
			printf ("\nFactamplitude: %f \n", FactAmplitude);
			ampf = (x/excursion2);
			printf ("\nampf: %f \n", ampf);
			ampf2 = (fabs (ampf) < 1.0f/A) ? A*fabs (ampf)/(1.0f+ln (A)):(1.0f+ln (A*fabs (ampf)))/(1.0f+ln (A)); // compand
			printf ("\ncompand ampf2: %f \n", ampf2);
			ex = (int) (round (ampf2*excursion2));
			printf ("\nnew e: %d \n", ex);
			factorizer = (ex*excursion2*FactAmplitude);
			printf ("\nfactorizer: %f \n", factorizer);
			sampler = (1E9/samplerate); // 44.100
			printf ("\nsampler: %f \n", sampler);
	} // for loop
		printf ("\nEnding readcount ... \n");
	} // while loop
  //fclose (FileFreqTiming);
	close (fp);
	printf ("\nFile closed! \n");
	}
	return (samplerate);
}
void playam ()
{
	float Frequencies = freq;
	typedef struct
	{
		float Frequency;
		int WaitForThisSample;
	} samplerf_t;
	samplerf_t RfSample;
	RfSample.Frequency = Frequencies;
	RfSample.WaitForThisSample = Timing; // in 100 of nanoseconds
	printf ("\nFreq = %f, Timing = %d \n", RfSample.Frequency, RfSample.WaitForThisSample);
	if (write (fp, &RfSample, sizeof (samplerf_t)) != sizeof (samplerf_t))
	{
		printf ("\nUnable to write sample! \n");
	}
	printf ("\nWriting tone in am \n");
	return;
}
void modulationfm () // int argc, char **argv
{
  	printf ("\nPreparing for fm ... \n");
    setupfm (); // gets filename & path or done by fileselect () func
		printf ("\nSetting up DMA ... \n");
		setupDMA (); // (argc>2 ? atof (argv [2]):100.00000); // default freq
    printf ("\nfm modulator starting ... \n");
    void playfm (char *filename, int mod, float bandwidth); // atof (argv [3]):22050)
		return;
}
void modulationam () //
{
		printf ("\nam modulator starting \n");
		playam (); // actual modulation stuff here for am -> write tone? maybe better name later
	  return;
}
void modselect () //
{
	printf ("\nOpening Modulator-Selection ... \n");
	if (strcmp (mod, "fm"))
	{
    printf ("\nYou selected fm! \n");
    printf ("\nPushing arguments to fm Modulator ... \n");
		modulationfm ();
	}
	else if (strcmp (mod, "am"))
	{
    printf ("\nYou selected am! \n");
    printf ("\nPushing arguments to am Modulator ... \n");
		modulationam ();
	}
	else
	{
    printf ("\nError in -m selecting modulation! Using default fm \n");
    modulationfm ();
	}
 	return;
}
int tx (char *filename, float freq, int samplerate, char *mod, char *type, float bandwidth, int power, int gpiopin, int dmachannel, int loop)
{
  printf ("\nPreparing for transmission ... \n");
	// here the apropiate transmission function
	//sampleselect (); // later for internal sample calculations
	printf ("\nBroadcasting now! ... \n");
	return (0);
}
void assistant () // assistant
{
		printf ("\nStarting assistant for setting parameters! \n");
		infos ();
		fileselect ();
		modetypeselect ();	// for freq identifications
		samplerateselect ();
		modulationselect ();
		typeselect ();
		bandwidthselect ();
		powerselect ();
		gpioselect ();
		dmaselect ();
		loopselect ();
		printf ("\nAll information gathered, parsing & going back to main! \n");
		return;
}
int menu ()
{
	printf ("\nChoose menu: [1] CMD // [2] Exit: ");
 	scanf ("%d", &menuoption);
	switch (menuoption)
	{
		case 1:  printf ("\nShell - Commandline (main) \n");
						 int main (int argc, char **argv); //, const char *short_opt); // go back to cmd if you want
						 break;
		case 2:  printf ("\nExiting ... \n");
					 	 exit (0);
		default: printf ("\nError: Returning back to Main (Default) \n");
             int main (int argc, char **argv);
		 				 break;
	}
	return (menuoption);
}
int main (int argc, char **argv) // *argv []=**argv, const char *short_opt
{
	clearscreen();
	printf ("\nStarting Program \n\nBeginning initializations ... \n");
	printf ("\nProgram was processed on %s at %s \n", __DATE__, __TIME__);
	printf ("\nFILE: %s \n", __FILE__); //
	infos (); // disclaimers
	bcm_host_get_peripheral_address ();
	bcm_host_get_peripheral_size ();
	bcm_host_get_sdram_address ();
	if (system ("/sbin/modprobe i2c_bcm2835") == -1)
	{
		printf ("\ni2c-modprobe-test BCM ailed \n");
	}
	else if (system ("/sbin/modprobe i2c_dev") == -1)
	{
		printf ("\ni2c-modprobe-test DEV failed \n");
	}
	else
	{
		printf ("\ni2c-modprobe-test BCM & DEV successful \n");
	}
	printf ("\nDevicename: %s \n", device);
	printf ("\nChecking arguments argc: %d, Address: %p \n", argc, &argc);
	int parametercount=((argc/2)); // max is 10 , + 3 extra assistent, menu &help
	printf ("\nChecking parametercount: %d \n", parametercount);
	const char *short_opt = "n:f:s:m:t:b:p:g:d:l:auh"; // program flags
	printf ("\nChecking short_opt: %s \n", short_opt);
	int options = getopt (argc, argv, short_opt); // short_opt must be constant
	printf ("\nChecking options: %d \n", options);
	if (argc <= 0) // || options
	{
	fprintf (stderr, "\nError! HELP: Use Parameters to run: \n[-n <filename (*.wav)>] [-f <freq (26.9650)>] [-s <samplerate (22050)>] [-m <mod (fm/am)>] [-t <type (a/d)>] \n[-b <bandwidth (12.5)>] [-p <power (1-7)>] [-g <gpiopin (4/21)>] [-d <dmachannel (7/14)>] [-l <loop (0/1)] \nThere is also an assistant [-a], menu [-u] or help [-h]! The *.wav-file must be 16-bit @ 22050 [Hz] Mono. \n");
	return (-1);
	}
	else if (argc > 0) // if -1 then all flags were read, if ? then unknown, parametercount=(argc/2)-1 =10
	{
		printf ("\nReading given arguments \n");
		while (options != -1) // options != -1
		{
		printf ("\nArgument switch loop \n");
		switch (options)
		{
		case 'n':
							 filename = optarg;
							 printf ("\nFilename is %s \n", filename);
							 break;
		case 'f':
							 freq = strtof (optarg, NULL); // string to float conversion
							 printf ("\nFrequency is %f \n", freq);
							 break;
		case 's':
							 samplerate = atoi (optarg);
							 printf ("\nSamplerate is %d \n", samplerate);
							 break;
		case 'm':
							 mod = optarg;
               printf ("\nMod is %s \n", mod);
               break;
		case 't':
					     type = optarg;
					     printf ("\nType is %s \n", type);
					   	 break;
		case 'b':
					 		 bandwidth = strtof (optarg, NULL);
					 		 printf ("\nBandwidth is %f \n", bandwidth);
					 		 break;
		case 'p':
         				power = atoi (optarg);
								if (power <= 0 || power > 7)
								{
								fprintf (stderr, "\nOutput power has to be set in range of 1-7, setting default 7! \n");
								power = (7);
							  }
								else
         				{
									printf ("\nPower is %d \n", power);
								}
								break;
		case 'g':
								gpiopin = atoi (optarg);
								printf ("\nGPIO-PIN is %d \n", gpiopin);
								break;
		case 'd':
								dmachannel = atoi (optarg);
								printf ("\nDMA-Channel is %d \n", dmachannel);
								break;
		case 'l':
								 loop = atoi (optarg); // true
								 printf ("\nLoop is %d (0=false, 1=true) \n", loop);
								 loopselect ();
								 break;
		case 'a':
							 if (argc == 2)
							 {
								printf ("\nAssistant activated \n");
								assistant ();
							 }
							 else
							 {
								printf ("\nError in -a assistant, no other arguments required \n");
							 }
							 return (-1);
		case 'u':
							 if (argc == 2)
							 {
         				printf ("\nOpening menu \n");
         				menu (); // extra menu for main
         			 }
         			 else
         			 {
         				printf ("\nError in -u menu \n");
         			 }
							 return (-1);
		case 'h':
							 if (argc == 2)
							 {
								printf ("\nHELP: Use Parameters to run: \n[-n <filename (*.wav)>] [-f <freq (26.9650)>] [-s <samplerate (22050)>] [-m <mod (fm/am)>] [-t <type (a/d)>] \n[-b <bandwidth (12.5)>] [-p <power (1-7)>] [-g <gpiopin (4/21)>] [-d <dmachannel (7/14)>] [-l <loop (0/1)] \nThere is also an assistant [-a], menu [-u] or help [-h]! The *.wav-file must be 16-bit @ 22050 [Hz] Mono. \n");
							 }
							 else
							 {
								printf ("\nError in -h help \n");
							 }
							 return (-1);
		case '?':
		 						//char *unknown = optopt;
                printf ("\nUnknown option: %c \n", optopt);
                return (-1);
		default:
								printf ("\nDefault-fallback: HELP: Use Parameters to run: \n[-n <filename (*.wav)>] [-f <freq (26.9650)>] [-s <samplerate (22050)>] [-m <mod (fm/am)>] [-t <type (a/d)>] \n[-b <bandwidth (12.5)>] [-p <power (1-7)>] [-g <gpiopin (4/21)>] [-d <dmachannel (7/14)>] [-l <loop (0/1)] \nThere is also an assistant [-a], menu [-u] or help [-h]! The *.wav-file must be 16-bit @ 22050 [Hz] Mono. \n");
								return (-1);
		} // end of switch
	  } // end of while
			printf ("\nEnd of switch \n");
	} // end of if
	else
	{
		printf ("\nArgument/Option Errors! \n");
	}
	printf ("\nEnd of argument check, printing debug \n");
	printf ("\nChecking File: %s \n", filename);
	printf ("\nChecking Freq: %f [MHz] \n", freq);
	printf ("\nChecking xtal_freq: %f \n", xtal_freq);
	printf ("\nChecking Samplerate: %d [Hz] \n", samplerate);
	printf ("\nChecking Modulation: %s \n", mod);
	printf ("\nChecking Type: %s \n", type);  // analog, digital:
	printf ("\nChecking Bandwidth: %f [Hz] \n", bandwidth);
	printf ("\nChecking Output-Power: %d \n", power);
	printf ("\nChecking GPIO-Pin: %d \n", gpiopin);
	printf ("\nChecking DMA-channel: %d \n", dmachannel);
	printf ("\nChecking Loop: %d \n", loop);
	printf ("\nChecking Repeat: %d \n", repeat);
  //printf ("\nChecking arg-Adresses: Name: %p / Dash -: %p, Filename: %p / Dash -: %p, Freq: %f \nDash -: %p, Samplerate: %p / Dash -: %p, Modulation: %p / Dash -: %p, Type: %p / Dash -: %p, Bandwidth: %p / Dash -: %p, Power: %p \nDash -: %p, GPIO: %p / Dash -: %p, DMA: %p / Dash -: %p, Loop: is %p \n",
	//argv [0], argv [1], argv [2], argv [3], argv [4], argv [5], argv [6], argv [7], argv [8], argv [9], argv [10], argv [11], argv [12], argv [13], argv [14], argv [15], argv [16], argv [17], argv [18], argv [19], argv [20]);
  //printf ("\nChecking values: Name: %s / Filename: %s / Freq: %f \nSamplerate: %d / Modulation: %s / Type: %s / Bandwidth: %f / Power: %d \nGPIO: %d / DMA: %d / Loop: is %d \n", argv [0], &filename, &freq, &samplerate, &mod, &type, &bandwidth, &power, &gpiopin, &dmachannel, &loop); // deref
	printf ("\nChecking values: Name: %s / Filename: %s / Freq: %f / Samplerate: %d \nModulation: %s / Type: %s / Bandwidth: %f / Power: %d \nGPIO: %d / DMA: %d / Loop: is %d \n", argv [0], argv [2], argv [4], argv [6], argv [8], argv [10], argv [12], argv [14], argv [16], argv [18], argv [20]); //
	printf ("\nTrying to start transmission ... \n");
	tx ((char *) filename, (float) freq, (int) samplerate, (char *) mod, (char *) type, (float) bandwidth, (int) power, (int) gpiopin, (int) dmachannel, (int) loop); // transmission
	printf ("\nTransmission ended! \n");
  terminate ();
	printf ("\nEnd of Program! Closing ... \n"); // EOF
	return (0);
}
