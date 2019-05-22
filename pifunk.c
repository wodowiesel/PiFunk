/* PiFunk (C) 2018-2019 silicator a.k.a wiesel

OS: raspbian (stretch) incl. desktop & recommended software v4.14 (8. April 2019) based on debian
SHA-256:a3ced697ca0481bb0ab3b1bd42c93eb24de6264f4b70ea0f7b6ecd74b33d83eb
-> get it here https://www.raspberrypi.org/downloads/raspbian/
-> or direct link https://downloads.raspberrypi.org/raspbian_full_latest => 2019-04-08-raspbian-stretch-full.zip
gcc 5.4.1 compiler or g++ 5.4.1 c++11/14
gdb 7.11.1 debugger

->get project:
 git clone https://github.com/silicator/PiFunk
->instructions:
 You will need alsa library for this:

 sudo apt-get install libsndfile1-dev
 cd PiFunk // goto path

->lm flag for math lib (obligatory), -g for debugger, -c for filesource for library, -c compile without linking
=>compile with admin/root permissions!!
 gcc -g -std=c99 -lm -Iinclude -Llib -c -fPIC pifunk.c -shared -o pifunk.o pifunk.out pifunk.so pifunk.a
 gcc -g -std=c99 -lm -Iinclude -Llib -lsndfile -fPIC pifunk.c -shared -o pifunk
 make
 make install

//compile & run with admin/root permissions!!
 sudo pifunk sound.wav 100.000 22050 fm callsign

-> real gpio hardware can't be simulated by c or py code! must be executed and compiled on linux
virtual maschine possible with qemu or alternative with everpad: nor sure about this, rather not using it
 wget -o -http://beta.etherpad.org/p/pihackfm/export/txt >/dev/null | gcc -lm -std=c99 -g -x c - && ./a.out sound.wav

LICENSE: GPLv2/3 !!
Credits: PiFM scripts from http://www.icrobotics.co.uk/wiki/index.php/Turning_the_Raspberry_Pi_Into_an_FM_Transmitter
!!!!!!! program needs more testing on real pi -> See Disclaimer!!!!!

-----Disclaimer-----
Rewritten for own purposes!
no guarantee, warranty for anything! Usage at own risk!
you should ground your antenna, eventually diode or 10uF-caps
at least use dummyloads 50 ohm @ max. 4 watts (S = 0-level) and compare signals with swr/pwr-meter!
do not shortout or do overstress it with more than 3.3V! it may cause damages
more infs about GPIO electronics https://de.scribd.com/doc/101830961/GPIO-Pads-Control2
Access on ARM-System !!! Running Linux, mostly on Raspberry Pi (me B+ rev.2)
used python 3.7.x on orig. Raspbian
don't forget to apt-get upgrade and update

1) Pi-FM version - frequency modulation direction left/right ← , →
2) Pi-AM version - amplitude modulation direction up/down ↑ , ↓
--> 700 MHz system clock of the pi1 -> please use heatsink (if you want with fan)

todo:
more definitions for memory-stuff
pointer & adress corrections
playlist func
tone generator for ctss (sin?)
make compatible arguments/funcs for py/shell scripts
*/

//std includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h> // for c99
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>

// functionality includes
#include <iso646.h> //c95 - back-compatible
#include <argp.h>
#include <getopt.h>
#include <time.h>
#include <sched.h>
#include <float.h>
#include <locale.h>
#include <errno.h>
#include <ctype.h>
#include <wchar.h>
#include <wctype.h>
#include <fcntl.h>
#include <malloc.h>
#include <dirent.h>
#include <signal.h>
#include <assert.h>
#include <setjmp.h>
#include <limits.h>
#include <termios.h>
#include <pthread.h>
#include <inttypes.h>
#include <math.h>
#include <tgmath.h>
#include <complex.h>
#include <features.h>
#include <fenv.h>
#include <grp.h>
#include <pwd.h>
#include <poll.h>
#include <argp.h>
// on posix linux
#include <sys/cdefs.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/file.h>
#include <sys/sysmacros.h>
#include <linux/spi/spidev.h>
//#include <missing.h>

// ip host socket
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <netdb.h>
#include <ifaddrs.h>

//-- c11
#include <stdalign.h>
#include <stdnoreturn.h>
#include <stdatomic.h>
#include <uchar.h>

//for c++14/17
/*
#include <iostream.h>
#include <threads.h>
#include <cstdlib.h>
#include <csignal.h>
using namespace std;
*/

// windows (10) if needed for maybe rpi3
/*
#include <windows.h>
#include <win.h>
#include <windef.h>
#include <winnt.h>
#include <winbase.h>
#include <conio.h> // dos-header
*/

// broadcom arm processor for mapping phys. addresses
#include "bcm2835/src/bcm2835.h"

//GPIO includes here 0.6.5 used
//#include "RPI.GPIO/source/c_gpio.h"
#include "RPI.GPIO/source/event_gpio.h"
//#include "RPI.GPIO/source/py_pwm.h"
#include "RPI.GPIO/source/soft_pwm.h"
//#include "RPI.GPIO/source/constants.h"
#include "RPI.GPIO/source/common.h"
#include "RPI.GPIO/source/cpuinfo.h"

// see http://www.mega-nerd.com/libsndfile/api.html for API needed for am -> ALSA sound
// download from mainpage http://www.alsa-project.org/main/index.php/Main_Page
#include "include/sndfile.h" // has problems with @typedef sf_count somehow -> set as int

//extra library https://github.com/libusb/libusb
//for usb soundcards for mic and alsa usage
//#include "libusb/libusb.h"
#include "libusb/libusb/libusb.h"
//#include "libusb/libusb/libusbi.h"
//#include "libusb/libusb/hotplug.h"
//#include "libusb/libusb/version.h"
//#include "libusb/libusb/version_name.h"
//#include "libusb/libusb/os/linux_usbfs.h"
#include "libusb/libusb/os/poll_posix.h"
#include "libusb/libusb/os/threads_posix.h"

//custom header for pifunk (dummy for now)
#include "include/pifunk.h"

//------------------------------------------------------------------------------
//preproccessor definitions
#ifdef __linux__ // ||__unix__
  //printf ("\nProgram runs under UNIX/LINUX \n");
	//#pragma GCC dependency "pifunk.h"
#elif __arm__
  printf ("\nProgram runs under ARM-Architecture! \n");
#else
   //#error
   printf ("Unknnown OS! or not Linux! \n");
#endif

#ifdef __GNUC__ && __STDC_VERSION__ >= 199901L
   //printf ("\nUsing Gnu C with ANSI C99!!\n");
#elif __GNUC__
   #warning "Using Gnu C without C99 standard!! Please compile with flag -std=c99 \n"
#else
   //#error
   printf ("Program was not compiled with Gnu and C99 standard! \n");
   exit (0)
#endif
//------------------------------------------------------------------------------
// Definitions & Makros
#define VERSION "0.1.6.9"
#define VERSION_MAJOR        (0)
#define VERSION_MINOR        (1)
#define VERSION_BUILD        (6)
#define VERSION_PATCHLEVEL   (9)
#define VERSION_STATUS "e"

//---- PI specific stuff
#define IN                    (0)
#define OUT                   (1)
#define FALSE                 (0)
#define TRUE                  (1)
/*
predefine if needed when not using bcm header
#define LOW (0)
#define HIGH (1)
*/
//-------buffers
#define PAGE_SIZE             (4*1024)
#define BLOCK_SIZE            (4*1024)
#define BUFFER_LEN            (8*1024)
#define BUFFERINSTRUCTIONS    (65536) // [1024];

//#define sleep [1000]
//#define usleep [1000]

//--------I-O access via GPIO
volatile unsigned *gpio;
volatile unsigned *allof7e;

// GPIO setup macros: Always use INP_GPIO (x) before using OUT_GPIO (x) or SET_GPIO_ALT(x, y)
#define PIN17 RPI_GPIO_P1_11 // which is the GPIO pin 17 for led
#define INP_GPIO(g)                   *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g)                   *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g, a)            *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // setsbits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0
#define GPIO_GET *(gpio+13) // sets bits which are 1 ignores bits which are 0
//-----
#ifdef  RASPI == 1                       // Original Raspberry Pi 1
#define PERIPH_VIRT_BASE               (0x20000000) // base=GPIO_offset dec: 2 virtual base
#define DRAM_PHYS_BASE                 (0x40000000) //dec: 1073741824
#define MEM_FLAG                       (0x0C) // alternative
#define CURBLOCK                       (0x0C) //dec: 12

#elif   RASPI >= 2                     // Raspberry Pi 2 & 3
#define PERIPH_VIRT_BASE               (0x3F000000) //dec: 1056964608
#define BCM2836_PERI_BASE              (0x3F000000) // register physical address dec: 1056964608
#define DRAM_PHYS_BASE                 (0xC0000000) //dec: 3221225472
#define MEM_FLAG                       (0x04) // dec: 4
#define CURBLOCK                       (0x04) // dec: 4 memflag?
#else
#define PERIPH_VIRT_BASE               (0x20000000)
#endif

//---
#define LENGTH                         (0x01000000) // dec: 1
#define GPIO_BASE (BCM2836_PERI_BASE + PERIPH_VIRT_BASE) // hex: 0x5F000000 dec: 1593835520
#define PWMCLK_CNTL                    (0x5A000016) // dec: 1509949462
//#define PWMCLK_DIV                   (0x5A002800) // dec: 1509959680
#define ADR                            (0x7E000000) // dec: 2113929216 phys base
#define CM_GP0CTL                      (0x7E101070) // p.107 dec: 2114982000
#define CM_GP0DIV                      (0x7E101074) // p.108 dec: 2114982004
#define DMABASE                        (0x7E007000) // dec: 2113957888
#define CLKBASE                        (0x7E101000) // dec: 2114981888
#define GPFSEL3                        (0x7E200000) // p.90 dec: 2116026368
#define PWMBASE                        (0x7E20C000) // controller dec: 2116075520
#define FIFO                           (0x18)   // dec: 24
#define CARRIER                        (0x5A)   // dec: 90
#define DMAREF                         (0x7F)   // dec: 127 dma base reference
#define MODULATE                       (0x4D72) // dec: 19826
#define DMAC                           (0x0707) // dec: 1799

#define ACCESS(PERIPH_VIRT_BASE)       (volatile int*)(PERIPH_VIRT_BASE + (volatile int) allof7e-ADR)
#define SETBIT(PERIPH_VIRT_BASE, bit)  ACCESS(PERIPH_VIRT_BASE) || 1<<bit // |=
#define CLRBIT(PERIPH_VIRT_BASE, bit)  ACCESS(PERIPH_VIRT_BASE) && ~(1<<bit) // &=

// possibility to give argv 0-4 an specific address or pointer
// addresses -> at least on my system-tests
#define CALLSIGN1_ADR                   (0x6052C0)       // dec: 6312640
#define NAME_ADR                        (0x7FFFFFFEC08)  // dec: 8796093017096
#define FILE_ADR                        (0x7FFFFFFFEC10) // dec: 140737488350224
#define CALLSIGN3_ADR                   (0x7FFFFFFFEAE8) // dec: 140737488349928
#define CALLSIGN2_ADR                   (0x7FFFFFFFEAEF) // dec: 140737488349935
#define FREQ_ADR                        (0x7FFFFFFFEC18) // dec: 140737488350232
#define SAMPLERATE_ADR                  (0x7FFFFFFFEC20) // dec: 140737488350240
#define MODULATION_ADR                  (0x7FFFFFFFEC28) // dec: 140737488350248
#define ARGC_ADR                        (0x7FFFFFFFEB0C) // dec: 140737488349964

// Pointers
#define FLOOR                           (0x0) // dec: 0
#define PWMADD1                         (0x4) // dec: 4
#define ARGC_PTR                        (0x5) // dec: 5
#define PWMADD2                         (0x8) // dec: 8
#define NAME_PTR                        (0x2F) // dec: 47
#define FREQ_PTR                        (0x31) //$ means is in RDS data dec: 49
#define SAMPLERATE_PTR                  (0x32) //$ in RDS data  dec: 50
#define MODULATION_PTR                  (0x66) //$ means isin RDS data // dec: 102
#define CALLSIGN_PTR                    (0x6D) // dec: 109
#define FILE_PTR                        (0x73) // dec: 115

//mathematical stuff
#define ln(x)                           (log(x)/log(2.718281828459045235f)) //log e(euler) = 0.4342944819
#define PI                              (3.14159265358979323846)
#define PHASE                           (2*PI) // 6.28318530718
#define HALF_PERIOD                     (1/PI) // 0.31830988618
#define PERIOD                          (1/PHASE) // 0.15915494309


//----------from pifmadv -> helps to understand the things the normal fm-script didnt specified
#define DMA_BASE_OFFSET                 (0x00007000) // dec: 28672
#define PWM_BASE_OFFSET                 (0x0020C000)// dec: 2146304
#define PWM_LEN                         (0x28) // dec: 40
#define CLK_BASE_OFFSET                 (0x00101000) // dec:1052672
#define CLK_LEN                         (0x1300) // dec: 4864
#define GPIO_BASE_OFFSET                (0x00200000) // dec: 2097152
#define GPIO_LEN                        (0x100) // dec: 256
#define PCM_BASE_OFFSET                 (0x00203000) // dec: 2109440
#define PCM_LEN                         (0x24) // dec: 36
#define PAD_BASE_OFFSET                 (0x00100000) // dec: 1048576
#define PAD_LEN                         (0x40/4) // 0x10 = dec: 16  //0x64 = dec: 100
#define PADGPIO                         (0x5A000018) // dec: 1509949464

#define DMA_VIRT_BASE                   (PERIPH_VIRT_BASE + DMA_BASE_OFFSET)
#define PWM_VIRT_BASE                   (PERIPH_VIRT_BASE + PWM_BASE_OFFSET)
#define CLK_VIRT_BASE                   (PERIPH_VIRT_BASE + CLK_BASE_OFFSET)
#define GPIO_VIRT_BASE                  (PERIPH_VIRT_BASE + GPIO_BASE_OFFSET)
#define PAD_VIRT_BASE                   (PERIPH_VIRT_BASE + PAD_BASE_OFFSET)
#define PCM_VIRT_BASE                   (PERIPH_VIRT_BASE + PCM_BASE_OFFSET)

#define PWM_PHYS_BASE                   (PERIPH_PHYS_BASE + PWM_BASE_OFFSET)
#define PCM_PHYS_BASE                   (PERIPH_PHYS_BASE + PCM_BASE_OFFSET)
#define GPIO_PHYS_BASE                  (PERIPH_PHYS_BASE + GPIO_BASE_OFFSET)

// GPIO
#define GPFSEL0                         (0x00/4) // p.90 dec: 0
#define GPFSEL1                         (0x04/4) // 1
#define GPFSEL2                         (0x08/4) // 2
#define GPPUD                           (0x94/4) // 37
#define GPPUDCLK0                       (0x98/4) // 38
#define GPPUDCLK1                       (0x9C/4) // 39

#define CORECLK_CNTL                    (0x08/4) //2
#define CORECLK_DIV                     (0x0C/4) //3
#define GPCLK_CNTL                      (0x70/4) //28
#define GPCLK_DIV                       (0x74/4) //29
#define EMMCCLK_CNTL                    (0x1C0/4) //112
#define EMMCCLK_DIV                     (0x1C4/4) //113

#define CM_LOCK                         (0x114/4) //69
#define CM_LOCK_FLOCKA                  (1<<8)
#define CM_LOCK_FLOCKB                  (1<<9)
#define CM_LOCK_FLOCKC                  (1<<10)
#define CM_LOCK_FLOCKD                  (1<<11)
#define CM_LOCK_FLOCKH                  (1<<12)

#define CM_PLLA                         (0x104/4) //65
#define CM_PLLC                         (0x108/4) //66
#define CM_PLLD                         (0x10C/4) //67
#define CM_PLLH                         (0x110/4) //68
#define CM_PLLB                         (0x170/4) //92

#define A2W_PLLA_ANA0                   (0x1010/4) //1028
#define A2W_PLLC_ANA0                   (0x1030/4) //1036
#define A2W_PLLD_ANA0                   (0x1050/4) //1044
#define A2W_PLLH_ANA0                   (0x1070/4) //1052
#define A2W_PLLB_ANA0                   (0x10F0/4) //1084
#define A2W_PLL_KA_SHIFT                (7)
#define A2W_PLL_KI_SHIFT                (19)
#define A2W_PLL_KP_SHIFT                (15)

#define PLLA_CTRL                       (0x1100/4) //1088
#define PLLA_FRAC                       (0x1200/4) //1152
#define PLLA_DSI0                       (0x1300/4) //1216
#define PLLA_CORE                       (0x1400/4) //1280
#define PLLA_PER                        (0x1500/4) //1344
#define PLLA_CCP2                       (0x1600/4) //1408

#define PLLB_CTRL                       (0x11E0/4) //1144
#define PLLB_FRAC                       (0x12E0/4) //1208
#define PLLB_ARM                        (0x13E0/4) //1272
#define PLLB_SP0                        (0x14E0/4) //1336
#define PLLB_SP1                        (0x15E0/4) //1400
#define PLLB_SP2                        (0x16E0/4) //1464

#define PLLC_CTRL                       (0x1120/4) //1196
#define PLLC_FRAC                       (0x1220/4) //1160
#define PLLC_CORE2                      (0x1320/4) //1224
#define PLLC_CORE1                      (0x1420/4) //1288
#define PLLC_PER                        (0x1520/4) //1352
#define PLLC_CORE0                      (0x1620/4) //1416

#define PLLD_CTRL                       (0x1140/4) //1104
#define PLLD_FRAC                       (0x1240/4) //1168
#define PLLD_DSI0                       (0x1340/4) //1232
#define PLLD_CORE                       (0x1440/4) //1296
#define PLLD_PER                        (0x1540/4) //1360
#define PLLD_DSI1                       (0x1640/4) //1424

#define PLLH_CTRL                       (0x1160/4) //1112
#define PLLH_FRAC                       (0x1260/4) //1176
#define PLLH_AUX                        (0x1360/4) //1240
#define PLLH_RCAL                       (0x1460/4) //1304
#define PLLH_PIX                        (0x1560/4) //1368
#define PLLH_STS                        (0x1660/4) //1432

// PWM
#define PWM_CTL                         (0x00/4) //0
#define PWM_DMAC                        (0x08/4) //2
#define PWM_RNG1                        (0x10/4) //4
#define PWM_RNG2                        (0x20/4) //8
#define PWM_FIFO                        (0x18/4) //6

#define PWMCLK_CNTL                     (40)
#define PWMCLK_DIV                      (41)

#define PWMCTL_PWEN1                    (1<<0)
#define PWMCTL_MODE1                    (1<<1)
#define PWMCTL_RPTL1                    (1<<2)
#define PWMCTL_POLA1                    (1<<4)
#define PWMCTL_USEF1                    (1<<5)
#define PWMCTL_CLRF                     (1<<6)
#define PWMCTL_MSEN1                    (1<<7)
#define PWMCTL_PWEN2                    (1<<8)
#define PWMCTL_MODE2                    (1<<9)
#define PWMCTL_RPTL2                    (1<<10)
#define PWMCTL_USEF2                    (1<<13)
#define PWMCTL_MSEN2                    (1<<15)

#define PWMDMAC_ENAB                    (1<<31)
#define PWMDMAC_THRSHLD                 ((15<<8)|(15<<0))

// PCM
#define PCM_CS_A                        (0x00/4) //0
#define PCM_FIFO_A                      (0x04/4) //1
#define PCM_MODE_A                      (0x08/4) //2
#define PCM_RXC_A                       (0x0C/4) //3
#define PCM_TXC_A                       (0x10/4) //4
#define PCM_DREQ_A                      (0x14/4) //5
#define PCM_INTEN_A                     (0x18/4) //6
#define PCM_INT_STC_A                   (0x1C/4) //7
#define PCM_GRAY                        (0x20/4) //8

#define PCMCLK_CNTL                     (38)
#define PCMCLK_DIV                      (39)

// PAD
#define GPIO_PAD_0_27                   (0x2C/4)  //11
#define GPIO_PAD_28_45                  (0x30/4)  //12
#define GPIO_PAD_46_52                  (0x34/4)  //13

// DMA
#define DMA_CHANNEL                     (14)
#define DMA_CHANNEL_MAX                 (14)
#define DMA_CHANNEL_SIZE                (0x100) //256

#define BCM2708_DMA_ACTIVE              (1<<0)
#define BCM2708_DMA_END                 (1<<1)
#define BCM2708_DMA_INT                 (1<<2)
#define BCM2708_DMA_WAIT_RESP           (1<<3)
#define BCM2708_DMA_D_DREQ              (1<<6)
#define BCM2708_DMA_DST_IGNOR           (1<<7)
#define BCM2708_DMA_SRC_INC             (1<<8)
#define BCM2708_DMA_SRC_IGNOR           (1<<11)
#define BCM2708_DMA_NO_WIDE_BURSTS      (1<<26)
#define BCM2708_DMA_DISDEBUG            (1<<28)
#define BCM2708_DMA_ABORT               (1<<30)
#define BCM2708_DMA_RESET               (1<<31)
#define BCM2708_DMA_PER_MAP(x)          ((x)<<16)
#define BCM2708_DMA_PRIORITY(x)         ((x)&0xF << 16)
#define BCM2708_DMA_PANIC_PRIORITY(x)   ((x)&0xF << 20)

#define DMA_CS                          (0x00/4) //0
#define DMA_CONBLK_AD                   (0x04/4) //1
#define DMA_DEBUG                       (0x20/4) //8

#define DMA_CS_RESET		                (1<<31)
#define DMA_CS_ABORT			              (1<<30)
#define DMA_CS_DISDEBUG		              (1<<29)
#define DMA_CS_WAIT_FOR_WRITES          (1<<28)
#define DMA_CS_INT			                (1<<2)
#define DMA_CS_END			                (1<<1)
#define DMA_CS_ACTIVE			              (1<<0)
#define DMA_CS_PRIORITY(x)		          ((x)&0xF << 16) //0xF=15
#define DMA_CS_PANIC_PRIORITY(x)	      ((x)&0xF << 20)

#define DREQ_PCM_TX                     (2)
#define DREQ_PCM_RX                     (3)
#define DREQ_SMI                        (4)
#define DREQ_PWM                        (5)
#define DREQ_SPI_TX                     (6)
#define DREQ_SPI_RX                     (7)
#define DREQ_SPI_SLAVE_TX               (8)
#define DREQ_SPI_SLAVE_RX               (9)

#define MEM_FLAG_DISCARDABLE            (1 << 0) /* can be resized to 0 at any time. Use for cached data */
#define MEM_FLAG_NORMAL                 (0 << 2) /* normal allocating alias. Don't use from ARM */
#define MEM_FLAG_DIRECT                 (1 << 2) /* 0xC alias uncached */
#define MEM_FLAG_COHERENT               (2 << 2) /* 0x8 alias. Non-allocating in L2 but coherent */
#define MEM_FLAG_L1_NONALLOCATING       (MEM_FLAG_DIRECT | MEM_FLAG_COHERENT) /* Allocating in L2 */
#define MEM_FLAG_ZERO                   (1 << 4)  /* initialise buffer to all zeros */
#define MEM_FLAG_NO_INIT                (1 << 5) /* don't initialise (default is initialise to all ones */
#define MEM_FLAG_HINT_PERMALOCK         (1 << 6) /* Likely to be locked for long periods of time. */

#define BUS_TO_PHYS(x)                  ((x)&~0xC0000000) //3221225472

#define PAGE_SHIFT                      (12)
#define NUM_PAGES                       ((sizeof (struct control_data_s) + PAGE_SIZE - 1) >> PAGE_SHIFT)

#define NUM_SAMPLES                     (64000)
#define NUM_CBS                         (NUM_SAMPLES * 2)

#define SUBSIZE                         (1)
#define DATA_SIZE                       (1000)

//----------------------------------
/* try a modprobe of i2C-BUS*/

//if (system ("/sbin/modprobe i2c_dev") == -1) {/* ignore errors */}
//if (system ("/sbin/modprobe i2c_bcm2835") == -1) {/* ignore errors */}
//-----------------------------------
//iterators for loops
int a;
int i;
int k;
float x;
int l;

//pi variables:
int  mem_fd;
char *gpio_mem;
char *gpio_map;
char *spi0_mem;
char *spi0_map;

//-----------------------------------------
char *description = "(experimental)"; // version-stage
static char *device = "default"; // playback device
const char *short_opt = "n:f:s:m:c::p:g:a::h::";
int opt;
char *filename;
double freq;
const double ctss_freq;
unsigned int samplerate;
double shift_ppm = 0;
//double I = sin ((PERIOD*freq) + shift_ppm);
//double Q = cos ((PERIOD*freq) + shift_ppm);
//double RF_SUM = (I+Q);

//samples max. 10 kHz resolution for am / 14.5 kHz FM radio can be recorded
unsigned int channels = 1;
char *mod = "fm";
char *fm = "fm";
char *am = "am";
char *callsign;
float volume = 1.0f;
unsigned int power = 7;
uint16_t pis = (0x1234); // 4660
uint32_t carrier_freq = 87600000;
float A = 87.6f; // compression parameter (stauchung)
//-> this might be the carrier too

unsigned int channelnumbercb;
unsigned int channelnumberpmr;
unsigned int channelmode;
unsigned int freqmode;
unsigned int modeselect;
unsigned int callnameselect;

//--network sockets for later
// here custom port via tcp/ip or udp
socklen_t addressLength;
unsigned int port = 8080;

// program variables
time_t rawtime;
char buffer [80];
char data_name [1024];

// we generate complex I-Q samples
float data [2*BUFFER_LEN];
float data_filtered [2*BUFFER_LEN];
float FactAmplitude = 2.0; //maybe here amp-modulator type input?
// logarithmic modulation

// fm vars
FILE infiles;
FILE outfiles;
int fp = STDIN_FILENO;
int filebit;
int readBytes;
float datanew, dataold = 0;

SF_INFO sfinfo;
//snd_output_t *output = NULL;
SNDFILE *infile, *outfile;
char *infilename, *outfilename;
float ampf;
float ampf2;
float factorizer;
float sampler;
unsigned int readcount, nb_samples;
unsigned int excursion = 6000; //32767 found another value but dont know on what this is based on

//audio control
//volume in dB 0db = unity gain, no attenuation, full amplitude signal
//-20db = 10x attenuation, significantly more quiet
float volumeLevelDb = -6.f; //cut amplitude in half
//float volbuffer [SAMPLES_PER_BUFFER];
float VOLUME_REFERENCE = 1.f;
//float volumeMultiplier = VOLUME_REFERENCE * pow (10, (volumeLevelDb/20.f) );

// instructor for access
unsigned long frameinfo;
int FileFreqTiming;
int instrs [BUFFERINSTRUCTIONS]; // [1024];
int bufPtr = 0;
int instrCnt = 0;
int instrPage;
int constPage;

int reg //= gpio / 10;
int shift //= (gpio % 10) * 3;

//--------------------------------------------------
// Structs
struct tm *info;
struct sockaddr_in localAddress;
//struct client_addr.sin_addr;
//struct local.sin_addr;

struct PAGEINFO // should use here bcm intern funcs-> repair
{
		void *p; // physical address BCM2836_PERI_BASE
		void *v; // virtual address
		int instrPage;
		int constPage;
		int instrs [BUFFERINSTRUCTIONS]; // [1024];
};

struct GPCTL
{
		char SRC         : 4;
		char ENAB        : 1;
		char KILL        : 1;
		char IDKW        : 1; // what is the blank char? gave it a dummyname for now
		char BUSY        : 1;
		char FLIP        : 1;
		char MASH        : 2;
		unsigned int IDK : 13; // what is the blank int?
		char PASSWD      : 8;
};

struct CB
{
		volatile unsigned int TI;
		volatile unsigned int SOURCE_AD;
		volatile unsigned int DEST_AD;
		volatile unsigned int TXFR_LEN;
		volatile unsigned int STRIDE;
		volatile unsigned int NEXTCONBK;
		volatile unsigned int RES1;
		volatile unsigned int RES2;
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

//programm flag options
struct option long_opt [] =
{
		{"filename",required_argument, NULL, 'n'},
		{"freq",   	required_argument, NULL, 'f'},
    {"samp",   	required_argument, NULL, 's'},
    {"mod",	    required_argument, NULL, 'm'},
    {"call",	  required_argument, NULL, 'c'},
    {"power", 	required_argument, NULL, 'p'},
    {"gpio",	  required_argument, NULL, 'g'},
    {"assist",	no_argument,       NULL, 'a'},
    {"help",	  no_argument,       NULL, 'h'}
};

/*
RTC (DS3231/1307 driver as bcm) stuff here if needed
*/

// basic function then specified one after another
static char timer ()
{
   char *newtime;
   time (&rawtime);
   info = localtime (&rawtime);
   *newtime = asctime (info);
   printf ("\nCurrent local time and date: %s \n", *newtime);
   return *newtime;
}

char filenamepath ()  // expected int?
{
  printf ("\nPlease enter the full path including name of the *.wav-file you want to use: \n");
  scanf ("%s", *filename);

  if (*filename == NULL)
	{
	    printf ("\nFile %s not found! \n", filename);
	    return 1;
	}
	else
	{
	   printf ("\nTrying to play default sound.wav ... \n");
	   *filename = open ("sound/sound.wav", "r"); // sounds/sound.wav directory should be testet
	   return *filename;
	}
	return *filename;
}

double freqselect () // gets freq by typing in
{
	printf ("\nYou selected 1 for Frequency-Mode \n");
	printf ("Type in Frequency (0.1-1200.00000 MHz): "); // 1b+ for 700Mhz chip, pi3 1.2ghz
	scanf  ("%f", freq);
	printf ("\nYou chose: %f MHz \n", freq);
  return freq;
}
//--------------------------------------------------
// Channel-mode
unsigned int channelmodepmr () //PMR
{
	printf ("\nChoose PMR-Channel 1-17 (18 to exit): ");
	scanf ("%d", &channelnumberpmr);
	switch (channelnumberpmr)
	{
	 //---- Analog & digital
	 //case 0: return freq=446.00625; printf ("\nDUMMY all-chan: Chan 0-> default Chan 1 %f ", freq); break;	// Scan all Chan till active , now chan1
	 case 1: return freq=446.00625; break;	// Standard
	 case 2: return freq=446.01875; break; // Geocaching
	 case 3: return freq=446.03125; break; // Standard
	 case 4: return freq=446.04375; break; // at 3-chan-PMR-devices its ch. 2
	 case 5: return freq=446.05625; break; // Contest
	 case 6: return freq=446.06875; break; // Events
	 case 7: return freq=446.08125; break; // at 3-channel-PMR-devices it's ch. 3
	 case 8: return freq=446.09375; break; // Standard
  //-----Digital only
	// dpmr digital new since 28.09.2016
	// extra 8 chan
	// 12.5 kHz steps
	 case 9:  return freq=446.10312; break; // 6.25 kHz steps
	 case 10: return freq=446.10625; break;
	 case 11: return freq=446.11875; break;
	 case 12: return freq=446.13125; break;
	 case 13: return freq=446.14375; break;
	 case 14: return freq=446.15625; break;
	 case 15: return freq=446.16875; break;
	 case 16: return freq=446.18125; break;
	 case 17: return freq=446.19375; break;
	 case 18: exit (0); break;
	 default: printf ("\nDefault chan = 1 %f \n", freq);  return freq=446.00625;
	}
  printf ("\nUsing Freq: %f", freq);
	return channelnumberpmr, freq;
}

unsigned int channelmodecb () // CB
{
	printf ("\nChoose CB-Channel 0-80 (81 to exit): ");
	scanf ("%d", &channelnumbercb);
	switch (channelnumbercb)
	{
		// --> translation of infos in english in future updates!
       case 0:   return freq=27.0450; break; //first digital channel
			 case 1:   return freq=26.9650; break; //empfohlener Anrufkanal (FM)
			 case 2:   return freq=26.9750; break; //inoffizieller Berg-DX-Kanal (FM)
			 case 3:   return freq=26.9850; break;
			 case 4:   return freq=27.0050; break; //empfohlener Anrufkanal (AM)/Anrufkanal Feststationen (AM)
			 case 5:   return freq=27.0150; break; //Kanal wird von italienischen Fernfahrern in Deutschland und Italien benutzt.
			 case 6:   return freq=27.0250; break; //Datenkanal (D)
		   case 7:   return freq=27.0350; break; //Datenkanal (D)
			 case 8:   return freq=27.0550; break;
			 case 9:   return freq=27.0650; break; //Fernfahrerkanal (AM)/weltweiter Notrufkanal EMG
			 case 10:  return freq=27.0750; break; //Antennen-Abgleich - halbe Channel-Anzahl!! ansonsten Chan 20 oder 40
/*		 Unterschied der Nachbarkanaele nicht um 10 kHz, sondern um 20 kHz
			 Diese Kanaele sind in den meisten Laendern nicht fuer CB-Funk zugelassen.
			 Zwecke wie z. B. Funkfernsteuerungen, Babyphones, kabellose Tastaturen und Maeuse verwendet */
			 case 11:  return freq=27.0850; break;  //freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland
			 case 1111: return freq=27.0950; break; //Eurobalise-Energieversorgung
			 case 12:  return freq=27.1050; break;
			 case 13:  return freq=27.1150; break;
			 case 14:  return freq=27.1250; break; //oft verwendet fuer Spielzeug-Fernsteuerungen (mittels Selektivton)
			 case 15:  return freq=27.1350; break; //inoffizieller Anrufkanal SSB (USB)
			 case 1515: return freq=27.1450; break;
			 case 16:  return freq=27.1550; break; //Funkverkehr mit und zwischen Wasserfahrzeugen
			 case 17:  return freq=27.1650; break; //Kanal wird von daenischen Schwertransportfahrern in Deutschland und Daenemark benutzt
			 case 18:  return freq=27.1750; break;
			 case 19:  return freq=27.1850; break; //empfohlener Fernfahrerkanal (FM)/oft von Walkie-Talkies genutzt/teilweise auch als Notrufkanal angegeben/auch von Babyfonen genutzt
			 case 1919: return freq=27.1950; break;
			 case 20:  return freq=27.2050; break; //zum Antennenabgleich genutzte Mitte bei 40-Kanal-Geraeten, wird in oesterreich sehr oft fuer Schwertransportfahrten benutzt

		 	//## 40 chan devices
			 case 21:  return freq=27.2150; break; //tuerkischer Anrufkanal in Deutschland und Europa (FM)
			 case 22:  return freq=27.2250; break; //oft von Walkie-Talkies genutzt, auch von Babyfonen genutzt, wird auch als Anrufkanal fuer rumaenische Fernlastfahrer verwendet
			 case 23:  return freq=27.2550; break; //Die Kanaele 23, 24, 25 sind sog. Dreher, sie folgen nicht dem aufsteigenden 10-kHz-Raster
			 case 24:  return freq=27.2350; break; //Datenkanal (D)
			 case 25:  return freq=27.2450; break; //Datenkanal (D), USB ROS Intern
			 case 26:  return freq=27.2650; break;
			 case 27:  return freq=27.2750; break;
			 case 28:  return freq=27.2850; break; //Kanal wird von polnischen Fernfahrern in Deutschland benutzt, Anrufkanal in Polen, wobei allgemein die CB-Kanalfrequenz in Polen um 5 kHz niedriger ist
			 case 29:  return freq=27.2950; break; //Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ber eine Internetverbindung in Deutschland
			 case 30:  return freq=27.3050; break; //inoffizieller DX-Kanal (FM), Anrufkanal fuer Funker aus dem ehemaligen Jugoslawien
			 case 31:  return freq=27.3150; break; //inoffizieller DX-Kanal (FM)
			 case 32:  return freq=27.3250; break;
			 case 33:  return freq=27.3350; break;
			 case 34:  return freq=27.3450; break; //freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland
			 case 35:  return freq=27.3550; break; //oeffentlicher Kanal
			 case 36:  return freq=27.3650; break; //Datenkanal USB ROS international
			 case 37:  return freq=27.3750; break; //Gateway-Kanal oesterreich, FM
			 case 38:  return freq=27.3850; break; //inoffizieller internationaler DX-Kanal (LSB)
			 case 39:  return freq=27.3950; break; //Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland
			 case 40:  return freq=27.4050; break; //ab Maerz 2016 freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete
			//ueber eine Internetverbindung in Deutschland (FM/AM/SSB in D)
			/* 80 chan devices
			 Auf den nationalen Zusatzkanaelen 41 bis 80 ist nur die Modulationsart FM erlaubt
			 Nachfolgend sind die Frequenzen der nationalen Zusatzkanaele, die im CB-Funk benutzt werden duerfen, aufgelistet: */
			case 41:  return freq=27.5650; break; //Ab Maerz 2016 Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland (FM), inoffizieller DX-Kanal (FM)
			case 42:  return freq=27.5750; break; //inoffizieller DX-Kanal (FM)
			case 43:  return freq=27.5850; break;
			case 44:  return freq=27.5950; break;
			case 45:  return freq=27.6050; break;
			case 46:  return freq=27.6150; break;
			case 47:  return freq=27.6250; break;
			case 48:  return freq=27.6350; break;
			case 49:  return freq=27.6450; break;
			case 50:  return freq=27.6550; break;
			case 51:  return freq=27.6650; break;
			case 52:  return freq=27.6750; break; //Datenkanal (D)(FM)
			case 53:  return freq=27.6850; break; //Datenkanal (D)(FM)
			case 54:  return freq=27.6950; break;
			case 55:  return freq=27.7050; break;
			case 56:  return freq=27.7150; break;
			case 57:  return freq=27.7250; break;
			case 58:  return freq=27.7350; break;
			case 59:  return freq=27.7450; break;
			case 60:  return freq=27.7550; break;

      case 61:  return freq=26.7650; break; //Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland
			case 62:  return freq=26.7750; break;
			case 63:  return freq=26.7850; break;
			case 64:  return freq=26.7950; break;
			case 65:  return freq=26.8050; break;
			case 66:  return freq=26.8150; break;
			case 67:  return freq=26.8250; break;
			case 68:  return freq=26.8350; break;
			case 69:  return freq=26.8450; break;
			case 70:  return freq=26.8550; break;
			case 71:  return freq=26.8650; break; //Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland
			case 72:  return freq=26.8750; break;
			case 73:  return freq=26.8850; break;
			case 74:  return freq=26.8950; break;
			case 75:  return freq=26.9050; break;
			case 76:  return freq=26.9150; break; //Datenkanal (D)(FM)
			case 77:  return freq=26.9250; break; //Datenkanal (D)(FM)
			case 78:  return freq=26.9350; break;
			case 79:  return freq=26.9450; break;
			case 80:  return freq=26.9550; break; //Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland */
			case 81:  exit (-1);
			default:  printf ("\nDefault: CB chan = 1 %f \n", freq); return freq=26.9650;

	}
  printf ("\n Using Freq: %f ", freq);
	return channelnumbercb, freq;
}

unsigned int modulationselect ()
{
	printf ("\nChoose your Modulation [1] FM // [2] AM // [3] Exit : ");
	scanf ("%d", &freqmode);
	switch (freqmode)
	{
		case 1: printf ("\nYou selected 1 for FM! \n");
				    //volaudio ();
		        unsigned int modulationfm ();
		        break;

		case 2: printf ("\nYou selected 2 for AM! \n");
			    	//volaudio ();
		        unsigned int modulationam (int argc, char **argv);
		        break;

		case 3: printf ("\nExiting... \n"); exit (-1); break;
		//default: printf ("\n Default = 1 \n"); break;
}

unsigned int channelselect ()
{
	printf ("\nYou selected 1 for Channel-Mode\n");
	printf ("Choose your Band [1] PMR // [2] CB // [3] Menu or [4] Exit : ");
  scanf  ("%d", &channelmode);

    switch (channelmode) // from here collecting infos and run it step by step, same for freq-mode
        {
         case 1: printf ("\nPMR CHAN-MODE in FM \n");
					channelmodepmr (); // gets freq from pmr list
					filenamepath ();
					printf ("\nChecking volume... \n");
					audiovol ();
					unsigned int modulationfm ();
					break;

		     case 2: printf ("\nCB CHAN-MODE SELECT \n");
				  channelmodecb (); // gets freq for chan
					filenamepath (); //gets file
					audiovol ();
					modulationselect (); //selects modulation
					break;

         case 3: printf ("\nReturning to Menu... \n");
          GetUserInput ();
          break;

         case 4:  printf ("\nExiting... \n");
					 exit (-1);

         default: printf ("\nDefault: Returning to Menu... \n"); GetUserInput (); break;
		    }
return channelmode;
}
/*
float audiovol ()
{
	for (int i = 0; i < SAMPLES_PER_BUFFER; ++i)
	{
     volbuffer [i] *= volumeMultiplier;
     printf ("\nValues: i: %d , volbuffer: %f , volumeMultiplier: %f \n", i, volbuffer [i], volumeMultiplier);
		 printf ("\nAdresses: i: %p , volbuffer: %p , volumeMultiplier: %p \n", &i, &volbuffer [i], &volumeMultiplier);
     return volbuffer [i], volumeMultiplier;
	}
return volbuffer [i], volumeMultiplier;
}
*/
//--------------- Voids
void handSig () // exit func
{
		exit (0);
}

void clearscreen ()
{
  print("\033[H\033[J");
  //fflush (stdin);
  clsscr ();
  //system("clear")
}

//--------------LED stuff
//controlling via py possible but c stuff can be useful too by bcm funcs!
//turn on LED (with 100 kOhm pullup resistor while transmitting
int ledactive ()
{
		//check if transmitting
		while (!play_wav ())
		{
		cm2835_gpio_write (PIN17, LOW);
		printf ("\nLED OFF - No Transmission!\n");
		}
    return 0;
}

int led ()
{
//simulation of gpio for debug
  //bcm2835_set_debug (1);
  if (!bcm2835_init ())
	{
	printf ("\nBCM 2835 init failed! \n");
  return 1;
	}
	else if (1)
	{
    // Set the pin to be an outputannels
    bcm2835_gpio_fsel (PIN17, BCM2835_GPIO_FSEL_OUTP);
  	printf ("\nBCM 2835 init done and PIN 4 activated \n");
    // LED is active during transmission
		while (play_wav ()) // (ledactive != 0)
		{
	// Turn it on
		bcm2835_gpio_write (PIN17, HIGH);
		printf ("\nLED ON - Transmission...! \n");
	// wait a bit
		bcm2835_delay (500);
		}
	}
	else // if no trans than turn it off
  {
		cm2835_gpio_write (PIN17, LOW);
		printf ("\nLED OFF - No Transmission \n");
	}
   bcm2835_close ();
	 printf ("\nBCM 2835 closing \n");
   return 0;
}

// FM ones
void infos () //warnings and infos
{
    printf ("\033[1;4;35m"); //red-yellow -> color:1 for "bright" / 4 for "underlined" and \0XX ansi colorcode //35 for Magenta, 33 red
    printf ("\nWelcome to the Pi-Funk! v%s-%s for Raspian ARM! \n\a", VERSION, *description);
    printf ("\033[0m"); //collor escape command for resetting
   	printf ("Radio works with *.wav-file with 16-bit @ 22050 [Hz] Mono / 1-700.00000 MHz Frequency \nUse '. dot' as decimal-comma seperator! \n");
    printf ("Pi oparates with square-waves (²/^2) PWM on GPIO 4 (Pin 7 @ ~500 mA & max. 3.3 V). \nUse power supply with enough specs only! \n=> Use Low-/Highpassfilters and/or ~10 uF-cap, isolators orresistors if needed! \nYou can smooth it out with 1:1 baloon. Do NOT shortcut if dummyload is used! \nCheck laws of your country! \n");
    printf ("\nFor testing (default setting) run: sudo sound.wav 100.0000 22050 fm callsign\n");
	 //printf ("\nclient ip+port: %s:%d \n", inet_ntoa (client_addr.sin_addr), (int) ntohs (client_addr.sin_port));
   //printf ("local ip+port: %s:%d \n", inet_ntoa (local.sin_addr), ntohs (local.sin_port));
}

void modulate (int l)
{
		ACCESS (CM_GP0DIV) == (CARRIER << 24) + MODULATE + l;  //
}

void getRealMemPage (void** vAddr, void** pAddr) // should work through bcm header!
{
		void* a = valloc (4096);

		((int*) a) [0] = 1; // use page to force allocation

		mlock (a, 4096); // lock into ram

		*vAddr = a; // we know the virtual address now

		int fp = open ("/proc/self/pagemap", 'w');
		lseek (fp, ((int) a)/4096*8, SEEK_SET);
		read (fp, &frameinfo, sizeof (frameinfo));

		*pAddr = (void*) ((int) (frameinfo*4096));
}

void freeRealMemPage (void** vAddr)
{
		munlock (vAddr, 4096); // unlock ram
		free    (vAddr); // free the ram
}

void carrierhigh () // enables it
{
/* Added functions to enable and disable carrier */
// Set CM_GP0CTL.ENABLE to 1 HIGH (2nd number) // 0x5A dec: 90
struct GPCTL setupword = {6, 1, 0, 0, 0, 1, 0x5A};// set it to ! = LOW
ACCESS (CM_GP0CTL) == *((int*) &setupword); //setting cm
}

void carrierlow () // disables it
{
struct GPCTL setupword = {6, 0, 0, 0, 0, 1, 0x5A};// set it to 0 = LOW
ACCESS (CM_GP0CTL) == *((int*) &setupword);
}

void setupfm ()
{
    printf ("\nSetting up FM... \n");
    // open /dev/mem
    if ((mem_fd = open ("/dev/mem", O_RDWR|O_SYNC) ) < 0)
	{
        printf ("\nCan't open /dev/mem! \n"); // via bcm possible
        exit (-1);
  }
    allof7e = (unsigned*) mmap (
								NULL,
								LENGTH, // length
								PROT_READ|PROT_WRITE,
								MAP_SHARED,
								mem_fd,
								PERIPH_VIRT_BASE); // base

   if ((int) allof7e == -1) exit (-1);

   SETBIT (GPFSEL0, 14);
   CLRBIT (GPFSEL0, 13);
   CLRBIT (GPFSEL0, 12);
	 carrierhigh ();
}
///------------------------------------
//relevant for transmitting stuff
void play_list () // exit func
{
		printf ("\nPlaying music from playlist-folder \n"); // in sounds/playlist

}

void play_wav (char *filename, double freq, unsigned int samplerate)
{
	/* wiki https://en.wikipedia.org/wiki/WAV https://en.wikipedia.org/wiki/44,100_Hz
    NTSC: 44056 Hz
    245 × 60 × 3 = 44100
    245 active lines/field × 60 fields/second × 3 samples/line = 44100 samples/second
    (490 active lines per frame, out of 525 lines total)
    PAL:
    294 × 50 × 3 = 44100
    294 active lines/field × 50 fields/second × 3 samples/line = 44100 samples/second
    (588 active lines per frame, out of 625 lines total)
	*/
    // after getting filename insert then open
	int sz = lseek (fp, 0L, SEEK_END);

  short* data = (short*) malloc (sz);

  for (int i = 0; i < 22; i++) // why i less then 22?
    {
        read (fp, &data, 2); // read past header (or sz instead on 2 ?)
        printf ("\nFor i=0: read fp \n");
    }

  while (readBytes = read (fp, &data, 1024))
    {
        float value = data [i] * 4 * volume; // modulation index (AKA volume) logar. hearing of human
        float fmconstant = (samplerate*50.0E-6); //1.1025 for pre-emphisis filter, 50us time constant
        unsigned int clocksPerSample = (22050/samplerate*1400); // for timing if 22050 then 1400 (why this?)
        // if samplerate > 15.75 then clocks per sample is negetive !! not good
        datanew = ((float) (*data)/excursion); //some constant for modulation unsigned int excursion

        float sample = datanew + (dataold-datanew)/(1-fmconstant); // fir of 1 + s tau
        float dval = sample*15.0; // actual transmitted sample, 15 is standard bandwidth (about 75 kHz) better 14.5

        int intval = (int) (round (dval)); // integer component
        float frac = ((dval - (float) intval)/2 + 0.5);
        unsigned int fracval = (frac*clocksPerSample);

        bufPtr++;
        //problem still with .v & .p endings for struct!!
        //while (ACCESS (DMABASE + CURBLOCK & ~ DMAREF) == (int) (instrs [bufPtr].p) ); // CURBLOCK of struct PageInfo
        //usleep (1000);

        //Create DMA command to set clock controller to output FM signal for PWM "LOW" time
        //(struct CB*) (instrs [bufPtr].v))->SOURCE_AD = ((int) constPage.p + 2048 + intval*4 - 4);

        bufPtr++;
        //while (ACCESS (DMABASE + 0x04) == (int) (instrs [bufPtr].p));
        //usleep (1000);

        //Create DMA command to delay using serializer module for suitable time
        //((struct CB*) (instrs [bufPtr].v))->TXFR_LEN = clocksPerSample-fracval;

        bufPtr++;
        //while (ACCESS (DMABASE + 0x04) == (int) (instrs [bufPtr].p));
        //usleep (1000);

        //Create DMA command to set clock controller to output FM signal for PWM "HIGH" time.
        //((struct CB*) (instrs [bufPtr].v))->SOURCE_AD = ((int) constPage.p + 2048 + intval*4+4);

        //while (ACCESS (DMABASE + 0x04) == (int) (instrs [bufPtr].p));
        //usleep (1000);

        //Create DMA command for more delay.
        //((struct CB*) (instrs [bufPtr].v))->TXFR_LEN = fracval;

        bufPtr = (bufPtr+1) % (BUFFERINSTRUCTIONS); // [1024] for buffer

        dataold = datanew;
        //ss->consume (data, readBytes);// ss-> for stereo
        printf ("\nReading bytes from fp ... \n");
    }

   close (fp);
   close (*filename);
   printf ("\nClosing file \n");
}

void unSetupDMA ()
{
	struct DMAREGS* DMA0 = (struct DMAREGS*) (ACCESS (DMABASE));
	DMA0->CS == 1<<31; // reset dma controller
	printf ("\nUnsetting DMA done \n");
	exit (-1);
}

void setupDMA (double freq)
{
	printf ("\nSetup of DMA starting... \n");
	atexit (unSetupDMA);
	signal (SIGINT, handSig);
	signal (SIGTERM, handSig);
	signal (SIGHUP, handSig);
	signal (SIGQUIT, handSig);

	// allocate a few pages of ram
  //getRealMemPage (&constPage.v, &constPage.p);
	int centerFreqDivider = (int) ((500.0/freq) * (float) (1<<12) + 0.5);
	// make data page contents - it s essientially 1024 different commands for the
	// DMA controller to send to the clock module at the correct time
	for (int i=0; i<1024; i++)
	{
	   // ((int*) (constPage.v))[i] = (CARRIER << 24) + centerFreqDivider - 512 + i;
	}

	while (instrCnt < 1024) //BUFFERINSTRUCTIONS
	{
     //getRealMemPage (&instrPage.v, &instrPage.p);

     // make copy instructions
  	 //struct CB* instr0 = (struct CB*)instrPage.v;

     for (int i=0; i<4096/sizeof (struct CB); i++)
     {
         /*
         instrs[instrCnt].v = (void*) ((int) instrPage.v + sizeof (struct CB)*i);
         instrs[instrCnt].p = (void*) ((int) instrPage.p + sizeof (struct CB)*i);
         instr0->SOURCE_AD = (unsigned int) constPage.p + 2048;

         instr0->DEST_AD = PWMBASE + (FIFO); //fifo
         instr0->TXFR_LEN = 4;
         instr0->STRIDE = 0;
         instr0->NEXTCONBK = (int) instrPage.p + sizeof (struct CB)*(i+1);

	      // DREQ then PWM then no-wide
         instr0->TI = (1<<6) | (5<<16) |  (1<<26);
         instr0->RES1 = 0;
         instr0->RES2 = 0;

        if (i%2)
	      {
         instr0->DEST_AD = CM_GP0DIV;
         instr0->STRIDE = 4;
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
   ACCESS (CLKBASE + 40*4) == (PWMCLK_CNTL); // (dec: 1509949478)
   //usleep (1000);

   ACCESS (CLKBASE + 41*4) == (PWMCLK_DIV); //
   //usleep (1000);

   // set up pwm
   ACCESS (PWMBASE + 0x0) == 0;
   //usleep (1000);

   ACCESS (PWMBASE + 0x4) == -1; // status: clear errors (0x4 in dec: 4)
   //usleep (1000);

   // Use fifo, repeat, serializer, enable ch
   ACCESS (PWMBASE + 0x0) == -1 | (1<<13) | (1<<10) | (1<<9) | (1<<8);
   //usleep (1000);

   // DMAC then DMA enable in 0x8 dec:8 / pwmbase+8 = 7E20C008 (dec:2116075528) /
   ACCESS (PWMBASE + 0x8) == (1<<31) | (DMAC);

  //activate dma
   struct DMAREGS* DMA0 = (struct DMAregs*) (ACCESS (DMABASE));
   DMA0->CS = 1<<31; // reset
   DMA0->CONBLK_AD = 0;
   DMA0->TI = 0;

   //DMA0->CONBLK_AD = (unsigned int) (instrPage.p);
   DMA0->CS = (1<<0) | (255 <<16);  // enable bit = 0, clear end flag = 1, prio=19-16
   printf ("\nSetup DMA done! \n");
}

// AM ones
void WriteTone (double freq, uint32_t Timing)
{
	typedef struct
	{
	  const double Frequency;
		uint32_t WaitForThisSample;
	}
	samplerf_t;
	samplerf_t RfSample;
	RfSample.Frequency; //= Frequency;

	RfSample.WaitForThisSample = Timing; //in 100 of nanoseconds
	printf ("Freq: %f , Timing: %d \n", RfSample.Frequency, RfSample.WaitForThisSample);

	if (write (FileFreqTiming, &RfSample, sizeof (samplerf_t)) != sizeof (samplerf_t))
	{
		fprintf (stderr, "\nUnable to write sample! \n");
	}
}
//---------------------//
// main progs
int tx ()
{

  // Drive Strength (power 7 standard): 0 = 2mA, 7 = 16mA. Ref: https://www.scribd.com/doc/101830961/GPIO-Pads-Control2
  //pad_reg[GPIO_PAD_0_27] = PADGPIO + power;
  //pad_reg[GPIO_PAD_28_45] = PADGPIO+ power;

	// GPIO needs to be ALT FUNC 0 to output the clock
	//gpio_reg[reg] = (gpio_reg[reg] & ~(7 << shift));

return 0;
}

//FM
unsigned int modulationfm (int argc, char **argv)
{
    printf ("\nPreparing for FM... \n");

    if (argc>0)
 	{
	  printf ("\nChecking Path... \n");
    setupfm (); // gets filename & path or done by filmename() func

	  printf ("\nSetting up DMA... \n");

    setupDMA (argc>2 ? atof (argv [2]):100.00000); // default freq, maybe do input here?

	  printf ("\nTesting Samplerate... \n"); //normally in 15 Hz bandwidth
    play_wav (argv [1], argc>3 ? atof (argv [3]):22050, *short_opt); // <-- in 22.05 kHz, should be same as AM!!

	  printf ("\nChecking & Setting LED for Transmission \n");
	  led ();
	  timer ();
	  printf ("\nNow transmitting... \n");
    }
	else
	{
		 fprintf (stderr, "\nUse Parameters to run: [filename] [freq] [samplerate] [mod (fm/am)] \nWhere wav-file is 16-bit @ 22050 Hz Mono.\nSet wavfile to '-' to use stdin.\nFrequency is between 1-700.0000 [MHz] (default 100.0000)\nYou can play an empty file to transmit silence. \n");
	}
	return 0;
}

//AM --- not yet adapted, needs revision for freq
unsigned int modulationam (int argc, char **argv)
{
	    /*
              {IQ (FileInput is a Mono Wav contains I on left Channel, Q on right channel)}
              {IQFLOAT (FileInput is a Raw float interlaced I, Q)}
              {RF (FileInput is a (float) Frequency,Time in nanoseconds}
              {RFA (FileInput is a (float) Frequency, (int) Time in nanoseconds, (float) Amplitude}
              {VFO (constant frequency)}
    */
	nb_samples = (readcount/channels);
	printf ("\n nb_samples: %f \n", nb_samples);
	printf ("Compression prameter A: %f \n", A); // was defined as global var above

	if (argc>=4)
	{
		printf ("filefreq timing opener test");
		FileFreqTiming = open (outfilename, O_CREAT | O_WRONLY | O_TRUNC, 0644); // O_RDWR
		//return FileFreqTiming, outfilename;
	}
	else
	{
		outfilename = (char *) malloc (128);// allocatong memory for filename
		sprintf (outfilename, "%s", "out.ft");
	}
//-------
    if (!(filename = open (filename, SFM_READ, &sfinfo)))
    {   // Open failed so print an error message.
        printf ("\nNot able to open input file %s \n", filename);
        // Print the error message from libsndfile.
        return 1;
    }
//-----------------
	if (sfinfo.samplerate == 22050) //44 or 48 khz needs testing
	{
		return samplerate;
	}
	else if (sfinfo.samplerate == 14500)
	{
			printf ("Samplerate is 14500 !");
			return samplerate;
	}
	else
  {
	printf ("\nInput samplerate must be at least 22.05 [kHz] AM/ or 14.5 kHz FM (mono)! \n");
	return 1;
	}
//--------------------
	if (filebit != 16)
	{
		printf ("\nInput must be 16 bit (mono)! \n");
		return 1;
	}
	// While there are frames in the input file, read them,
	//process them and write them to the output file
//----------------------
  while (readcount = read (filename, data, BUFFER_LEN))
  {
	 // where to input the freq like in fm?
	  for (k = 0 ; k < nb_samples ; k++)
	  {
		  char b = data [k*channels];
			printf ("\nChannel buffer b= %s \n", b);
			if (channels == 0)
			{
				printf ("File is NOT mono -> 0 Channels: Error!) \n"); // >1 in stereo or dual mono with half samplerate
			}
			else if (channels == 1)
			{
				// stereo file, avg left + right --> should be mono at 22.05kHz
				b += data [k*channels+1];
				b /= 2; // maybe *2 to make a dual mono and not doing stereo in half!
				return b;
			}
			else if (channels >= 2){ printf ("Error: File has 2 or more Channels!) \n");} // >1 in stereo or dual mono with half samplerate

			//maybe here am option for amplitude factor input!?
			printf ("Factamplitude: %f \n", FactAmplitude);

			ampf = (x/32767.0f);
			printf ("ampf1: %f \n", ampf);

      ampf2 = (fabs (ampf) < 1.0f/A) ? A * fabs (ampf)/(1.0f+ln (A)) : (1.0f+ln (A*fabs (ampf)))/(1.0f+ln (A)); //compand
			printf ("compand ampf2: %f \n", ampf2);

			x = (int) (round (ampf2*32767.0f));
			printf ("new x: %f \n", x);

		  factorizer = (x*32767.0f*FactAmplitude);
			printf ("factorizer: %f \n", factorizer);

			sampler = (1E9/samplerate); //44.00
			printf ("sampler: %f \n", sampler);

			timer ();
			printf ("\nNow writing tone in AM... \n");
			WriteTone (factorizer, sampler); // somehow input freq here ?!?

      return channels, ampf, ampf2, x, factorizer, sampler;
	  }
  	led ();
    }
    printf ("Reading file: %s \n", filename);
    printf ("Freq: %f \n", freq);
  	printf ("Sample Rate: %u \n", samplerate);
  	printf ("Channels: %d \n", channels);
    printf ("Writing file: %s \n", outfilename);

    // Close input and output files
    fclose (FileFreqTiming);
    fclose (filename);
    printf ("\nFile saved! \n");
    return FileFreqTiming, filename;
}

return freqmode;
}
// all subch. -> base/default case 0 -> channel 0
// if subchannels is 0 = all ch. then check special stuff -> maybe scan func ?
// squelch/treshhold to build in maybe -> scan function till signal?

// read / import csv for pmr

char csvreader()
{
    printf ("\nChecking for CSV-file... \n");
    char *sfp, *dfp;
    char c;

    *sfp = fopen ("csvpmr.csv", "r");// readonly!
    *dfp = fopen ("csvwriter.csv", "w+"); // with + it updates , if exists overwrites
    while (!feof (*sfp))
    {
    //here check for semicolon or comma delimiter (default)
    c = fgetc (*sfp);
    fputc (c,*dfp);
    }

    fclose (*sfp);
    fclose (*dfp);
    printf ("\n%s\n", c);
    printf ("\nCSV-Import of ctss-List finished! \n");

    return c, *sfp, *dfp;
}

char callname (char *callsign)
{
    //if (*callsign == NULL){
		switch (callnameselect)
	  {
		printf ("\nYou don't have specified a callsign yet!\n Do you want to customize it? press (1) or use (2) default 'callsign': \n");
		case 1: printf ("\nType in your callsign: ");
				scanf  ("%s", *callsign);
				printf ("\nYour callsign is: %s \n", *callsign);
        return callsign, &callsign, *callsign;
				break;

		case 2: *callsign = "callsign"; //default callsign
				printf ("\nUsing default callsign: %s \n", *callsign);
        printf ("Adress %p , Pointer %p \n", &callsign, *callsign);
				break;
    }
  return callsign, &callsign, *callsign;
}

int GetUserInput () //my menu-assistent
{
    timer ();
    infos ();
    printf ("Press Enter to Continue... \n");
    while (getchar () != "\n");
	  printf ("Choose a Mode [1] Channel-Mode // [2] Frequency-Mode // [3] CSV-Reader // [4] CMD // [5] Exit : ");
	  scanf ("%d", &modeselect);

	  switch (modeselect)
    {
      case '1': channelselect (); //undefined reference
							filenamepath ();
							callname (*callsign);
					    break;

		  case '2': filenamepath ();
		          freqselect ();
		          modulationselect ();
					    break;

			case '3': printf ("\nReading CSV for PMR: \n");
				    	csvreader ();
					    break;
			case '4': printf ("\nShell - Commandline: \n");
			        int main (int argc, char **argv []); // go back to cmd if you want
				    	break;

			case '5': printf ("\nExiting... \n");
				    	exit (-1);
					    break;

			default: printf ("\nDefault = 1 \n"); break;

    }
    return 0;
}

int main (int argc, char **argv [], char *short_opt) // arguments for global use must! be in main
{
   argv [0] = "pifunk"; // for custom  programname, default is the filename itself
   printf ("%s \n", argv [0]);
   printf ("\nProgram was proccessed on %s at %s \n", __DATE__, __TIME__);
	 printf ("\nProgram name is %s \n", __FILE__);

   char *filename = argv [1];
   // atoll () is meant for integers & it stops parsing when it finds the first non-digit
   // atof () or strtof () is for floats. Note that strtof () requires C99 or C++11
   double freq = strtof (argv [2], NULL); //float only accurate to .4 digits idk why, from 5 it will round ?!
   unsigned int samplerate = atof (argv [3]); //maybe check here on != 22050 on 16 bits as fixed value (eventually allow 48k)

   char *mod = argv [4];
   char *callsign = argv [5];
   //char volume = argv [6]; // argc>4 ? atoi(argv[6]):4
   //unsigned int gain = atoi (argv [6]); // => (atoi gives the value of a string) in play_wav possible
   int options;
   //---
   headertest ();
   infos (); //information, disclaimer
   timer (); //local time

   //-- for debugging or information :)
   /*
   printf ("\nArguments(argc): %d /Programm(0): %s / File(1): %s \nFreq(2): %s / Samplerate(3): %s / Modulation(4): %s / Callsign(5): %s / Volume(6): %s / Gain(6): %d \n", argc, argv [0], argv [1], argv [2], argv [3], argv [4], argv [5], argv [6], gain);
   printf ("&Adresses-> Arguments: %p / Name: %p \nFile: %p / Freq: %p \nSamplerate: %p / Modulation: %p / Callsign: %p/ Volume: %p / Gain: %p \n", &argc, &argv [0], &argv [1], &argv [2], &argv [3], &argv [4], &argv [5], &argv [6], &gain);
   printf ("*Pointers-> argc: %p / Name: %p / File: %p / Freq: %p / Samplerate: %p / Modulation: %p / Callsign: %p/ Volume: %p \n", argc, *argv [0], *argv [1], *argv [2], *argv [3], *argv [4], *argv [5], *argv [6]);
   //printf ("\nHostname: %s , WAN+LAN-IP: %s , Port: %d \n", host, ip, port);
   //---
   */

   if (argc=0||NULL)
   {
     fprintf (stderr, "\nArgument-Error! Use Parameters to run: [-n <filename>] [-f <freq>] [-s <samplerate>] [-m <mod (fm/am)>] [-c <yourcallsign (optional)>] [-p <power (0-7>]!\nThere is also an assistent [-a] or for help [-h]! The *.wav-file must be 16-bit @ 22050 [Hz] Mono \n");
     return -1;
   }

   while (options = getopt (argc, argv, "n:f:s:m:c::p:a::h::") != -1)
   {
   switch (options)
   {
   //assistent
   case 'a':
   if (argc=1)
   {
      printf ("\nAssistent activated! \n");
      GetUserInput (); //  to menu -> must be refactored later
   }
   break;

   // help
   case 'h':
   if (argc=1)
   {
     infos ();
     printf ("\nHELP: Use Parameters to run: [-n <filename>] [-f <freq>] [-s <samplerate>] [-m <mod (fm/am)>] [-c <yourcallsign (optional)>] [-p <power (0-7>]!\nThere is also an assistent [-a] \n");
   }
   break;

   //filename
   case 'n':
      return filename;
      printf ("\nFilename is %s \n", filename);
      break;

   case 'f':

      printf ("\nFrequency is %f \n", freq);
      return samplerate;
      break;

   case 's':
      printf ("\nSamplerate is %f \n", samplerate);
      return samplerate;
      break;

// modulation
   case 'm':

     if (mod != NULL)
     {
                if (!strcmp (mod, "fm"))
                {
                printf ("\nPushing args to fm Modulator... \n");
                unsigned int modulationfm (int argc, char **argv); // idk if here to jump to the modulator or just parse it?!
                }
                else if (!strcmp (mod, "am"))
                {
                printf ("\nPushing args to am Modulator... \n");
                unsigned int modulationam (int argc, char **argv);
                }
      }
      else
      {
        printf ("\nNo Modulation specified! Using standard modulation fm \n");
        mod = "fm";
        return mod;
      }
      break;

//callsign
   case 'c':
   if (callsign != NULL)
   {
     printf ("\nCallsign is %s \n", callsign);
     return callsign;
    }
    else
    {
     callsign = "callsign";
     return callsign;
    }
    break;

//power managment
   case 'p':
   if (power != NULL)
   {

     printf ("\nPowerlevel is %d \n", power);
     return power;
    }
    else
    {
      printf ("\nNo Powerlevel given, using maximum output %d \n", power);
      return power = 7;
    }
    break;

   default: fprintf (stderr, "\nArgument-Error! Use Parameters to run: [-n <filename>] [-f <freq>] [-s <samplerate>] [-m <mod (fm/am)>] [-c <yourcallsign (optional)>] [-p <power (0-7>]!\n There is also an assistent [-a] or for help [-h]! The *.wav-file must be 16-bit @ 22050 [Hz] Mono \n");
   }

   /*
   printf ("Checking File: %s \n", argv [1]);
   printf ("String-Conversion to Freq: %f [MHz] @ Samplerate: %u [Hz] \n", freq, samplerate);
   printf ("Checking Channels: %s \n", channels);
   printf ("Checking Modulation: %s \n", mod);
   printf ("Checking Callsign: %s \n", *callsign);
   printf ("Checking Volume/Gain: %s / %d \n", volume, gain);
   */
printf ("End of main \n");
return 0;
}
