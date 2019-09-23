/* PiFunk (C) 2018-2019 silicator a.k.a Wiesel
version=0.1.7.7e
OS: raspbian (stretch) incl. desktop & recommended software v4.14+ (8. April 2019) based on debian
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

sudo apt-get install libraspberrypi-dev raspberrypi-kernel-headers
 -lbcm_host //firmware v1.20190718 located in /opt/vc/include/

cd PiFunk // goto path

->lm flag for math lib (obligatory), -g3 for debugger, -c for not linkin to library
=>compile with admin/root permissions!!
 sudo gcc -g3 -std=c99 -lm -Iinclude -Llib -lsndfile -D_USE_MATH_DEFINES -D_GNU_C_SOURCE -D_POSIX_C_SOURCE=199309L -L/opt/vc/lib -lbcm_host -fPIC pifunk.c -shared  -O3 -o include/pifunk.i lib/pifunk.s lib/pifunk.o lib/pifunk.a lib/pifunk.so lib/pifunk.lib
 sudo gcc -g3 -std=c99 -lm -Iinclude -Llib -lsndfile -D_USE_MATH_DEFINES -D_GNU_SOURCE -D_POSIX_C_SOURCE=199309L -L/opt/vc/lib -lbcm_host -fPIC pifunk.c -shared  -O3 -o bin/pifunk bin/pifunk.out
 or do make (compile flags in make included)
 -std=c99 is the same as -std=iso9899:1999 or =gnu99 or -std=c++17 (11/14)
 -E tells to stop after preprocessing stage
 -v verbose

-> real gpio hardware can't be simulated by c or py code! must be executed and compiled on linux
virtual maschine possible with qemu or alternative with everpad: nor sure about this, rather not using it
 wget -o -http://beta.etherpad.org/p/pihackfm/export/txt >/dev/null | gcc -std=c99 -g -lm -x c && ./pifunk.out sound.wav

LICENSE: GPLv2/3 !!
!!!!!!! program needs more testing on real pi !!!!!!!

-----Disclaimer-----
Rewritten for own purposes!
no guarantee, warranty for anything! Usage at own risk!
you should ground your antenna, eventually diode or 10uF-caps
at least use dummyloads 50 ohm @ max. 4 watts (S = 0-level) and compare signals with swr/pwr-meter!
do not shortout or do overstress it with more than 3.3V! it may cause damages
more infs about GPIO electronics https://de.scribd.com/doc/101830961/GPIO-Pads-Control2
Access on ARM-System !!! Running Linux, mostly on Raspberry Pi (me B+ rev.2)
used python 3.7.x on original Raspbian
-----------------------
don't forget to apt-get upgrade and update

1) Pi-FM version - frequency modulation direction left/right ← , →
2) Pi-AM version - amplitude modulation direction up/down ↑ , ↓
--> 700 MHz system clock of the pi1 -> please use heatsink + fan

todo:
memory-stuff
pointer & address corrections
make compatible arguments/funcs for py/shell scripts
tone generator for ctss (sin?)
*/

//std includes
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> // for c99
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdalign.h>
#include <stdnoreturn.h>
#include <stdatomic.h>
#include <unistd.h>

// functionality includes
#include <iso646.h> //c95 back-compatible  -std=iso9899:199409
#include <argp.h>
#include <string.h>
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
#include <uchar.h>
#include <metrics.h>
//#include <config.h>
//#include <missing.h>

// on posix linux
#include <sys/cdefs.h>
#include <sys/time.h>
#include <sys/timex.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/file.h>
#include <sys/sysmacros.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

//linux includes
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>		//Needed for I2C port
#include <linux/bcd.h>
#include <linux/spi/spidev.h>
#include <linux/rtc.h>
//#include <linux/rtc/ds1307.h>
#include <linux/rtc/ds3231.h>
#include <ds3231.h> // rtc/

// ip host socket
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <netdb.h>
#include <ifaddrs.h>

//for c++11/14/17
/*
#include <iostream.h>
#include <sstream.h>
#include <threads.h>
#include <cstdlib.h>
#include <csignal.h>
#include <cmath.h>
#include <cstdint.h>
#include <iomanip.h>
#include <algorithm.h>
#include <vector.h>
#include <stdalign.h>
#include <stdnoreturn.h>
#include <stdatomic.h>
#include <uchar.h>
#include <cstring.h>
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

//broadcom arm processor for mapping phys. addresses
#include "opt/vc/include/bcm_host.h" // firmware stuff
#include "opt/vc/include/interface/vcos/vcos.h"
#include "bcm2835/src/bcm2835.h"

//RPI.GPIO includes here, 0.6.5 used
#include "RPI.GPIO/source/i2c.h"
#include "RPI.GPIO/source/c_gpio.h"
#include "RPI.GPIO/source/event_gpio.h"
//#include "RPI.GPIO/source/py_pwm.h"
#include "RPI.GPIO/source/soft_pwm.h"
//#include "RPI.GPIO/source/constants.h"
#include "RPI.GPIO/source/common.h"
#include "RPI.GPIO/source/cpuinfo.h"

//see http://www.mega-nerd.com/libsndfile/api.html for API needed for am -> ALSA sound
//download from mainpage http://www.alsa-project.org/main/index.php/Main_Page
//#include "include/sndfile.h" // has problems with @typedef sf_count somehow -> set as int

//extra library https://github.com/libusb/libusb
//for usb soundcards for mic and alsa usage
#include "libusb/libusb/libusb.h"
//#include "libusb/libusb.h"
//#include "libusb/libusb/libusbi.h"
//#include "libusb/libusb/hotplug.h"
#include "libusb/libusb/version.h"
#include "libusb/libusb/version_nano.h"
//#include "libusb/libusb/os/linux_usbfs.h"
#include "libusb/libusb/os/poll_posix.h"
#include "libusb/libusb/os/threads_posix.h"

//custom header for pifunk (dummy for now)
#include "include/pifunk.h"

//------------------------------------------------------------------------------
//preproccessor definitions
#ifdef __LINUX__ // || __UNIX__
  printf ("\nProgram runs under UNIX/LINUX\n");
	#pragma GCC dependency "pifunk.h"
#endif

#ifdef __UNIX__
  printf ("\nProgram runs under UNIX\n");
	#pragma GCC dependency "pifunk.h"
#endif

#ifdef __ARM__
  printf ("\nProgram runs under ARM-Architecture!\n");
  //#pragma ARM
  //same as -CODE32
  //#error NOT ARM
#endif

#ifdef __GNUC__
   //printf ("\nUsing GNU C with ANSI C99!!\n");
   //#pragma GCC system_header
#endif

#ifdef __STDC_VERSION__ //>= 199901L
   /*#warning "\nPlease compile with flag -std=c99\n" string */
   //printf ("\nUsing GNU C with C99 standard!!\n");
#endif
//------------------------------------------------------------------------------
// Definitions & Makros
#define VERSION 						 "0.1.7.7e" // my version
#define VERSION_MAJOR        (0) //
#define VERSION_MINOR        (1) //
#define VERSION_BUILD        (7) //
#define VERSION_PATCHLEVEL   (7) //
#define VERSION_STATUS 			 "experimental" // WIP work in progress

#define _GNU_SOURCE // for using gcc compiler
#define _POSIX_C_SOURCE = 		(200809L) //or 199309L
#define _USE_MATH_DEFINES // for math lm lib needed

// simple operators
#define IN                    (0) //
#define OUT                   (1) //
#define FALSE                 (0) //
#define TRUE                  (1) //
/*
predefine if needed when not using bcm header
#define LOW 									(0) //
#define HIGH 									(1) //
*/

//mathematical stuff
#define ln(x)                           (log (x)/log (2.718281828459045235f)) //log e(euler) = 0.4342944819
#define PI                              (3.14159265358979323846) // radial constant
#define PHASE                           (2*PI) // 6.28318530718
#define HALF_PERIOD                     (1/PI) // 0.31830988618
#define PERIOD                          (1/PHASE) // 0.15915494309

// buffers
#define PAGE_SIZE             (4*1024) //4096
#define BLOCK_SIZE            (4*1024) //4096
#define BUFFER_LEN            (8*1024) //8192
#define BUFFERINSTRUCTIONS    (65536) //[1024]
//#define sleep 								[1000] // for waiting between functions & tasks
//#define usleep 								[1000] //

// I-O access via GPIO
volatile unsigned 										*gpio; //
volatile unsigned 										*allof7e; //

// GPIO setup macros: Always use INP_GPIO (x) before using OUT_GPIO (x) or SET_GPIO_ALT(x, y)
#define ALLOF7ED											(*allof7e - SUB_BASE)
#define PIN_GND                       (9) // which is the GPIO pin 17 for led
#define PIN17                         RPI_GPIO_P17 // which is the GPIO pin 17 for led
#define INP_GPIO(g)                   *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g)                   *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g, a)            *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET 											*(gpio+7)  // setsbits which are 1 ignores bits which are 0
#define GPIO_CLR 											*(gpio+10) // clears bits which are 1 ignores bits which are 0
#define GPIO_GET 											*(gpio+13) // sets bits which are 1 ignores bits which are 0

//----- specific pi adresses & definitions
#ifdef  RPI // alternative BCM2711B0
#define PERIPH_VIRT_BASE               (0x20000000) // dec:536870912
#define DRAM_PHYS_BASE                 (0x40000000) //dec: 1073741824
#define MEM_FLAG                       (0x0C) // alternative
#define CURBLOCK                       (0x04) //dec: 4
#define PLLD_FREQ 										 (500000000.) //

#ifdef 	RASPI == 0 // pi zero & w
#define PERIPH_VIRT_BASE               (0x20000000) // base=GPIO_offset dec: 2 virtual base
#define PERIPH_PHYS_BASE               (0x7E000000) // dec: 2113929216
#define DRAM_PHYS_BASE                 (0x40000000) //dec: 1073741824
#define MEM_FLAG                       (0x0C) // alternative
#define CURBLOCK                       (0x0C) // dec: 12
#define PLLD_FREQ											 (500000000.) //
#endif

#ifdef  RASPI == 1 // pi 1 my version
#define PERIPH_VIRT_BASE               (0x20000000) // base=GPIO_offset dec: 2 virtual base
#define PERIPH_PHYS_BASE               (0x7E000000) // dec: 2113929216
#define DRAM_PHYS_BASE                 (0x40000000) //dec: 1073741824
#define MEM_FLAG                       (0x0C) // alternative
#define CURBLOCK                       (0x0C) //dec: 12
#define CLOCK_BASE										 19.2E6 //
#define DMA_CHANNEL										 (14) //
#define PLLD_FREQ											 (500000000.) //
#endif

#ifdef  (RASPI) == 2 // pi2
#define PERIPH_VIRT_BASE               (0x3F000000) // dec: 1056964608
#define PERIPH_PHYS_BASE               (0x7E000000) // dec: 2113929216
#define BCM2836_PERI_BASE              (0x3F000000) // register physical address dec: 1056964608 alternative name
#define DRAM_PHYS_BASE                 (0xC0000000) //dec: 3221225472
#define MEM_FLAG                       (0x04) // dec: 4
#define CURBLOCK                       (0x04) // dec: 4 memflag
#define CLOCK_BASE									   (19.2E6) //
#define DMA_CHANNEL										 (14) //
#define PLLD_FREQ 										 (500000000.) //
#endif

#ifdef 	(RASPI) == 3 // BCM2835
#define PERIPH_VIRT_BASE               (0x20000000) // dec: 536870912
#define PERIPH_PHYS_BASE               (0x7E000000) // dec: 2113929216
#define BCM2836_PERI_BASE              (0x3F000000) // register physical address dec: 1056964608 alternative name
#define DRAM_PHYS_BASE                 (0xC0000000) //dec: 3221225472
#define MEM_FLAG                       (0x04) // dec: 4
#define CURBLOCK                       (0x04) // dec: 4 memflag
#define CLOCK_BASE									   (19.2E6) //
#define DMA_CHANNEL										 (14) //
#define PLLD_FREQ 										 (500000000.) //
#endif

#ifdef  (RASPI) == 4 //pi4 - BCM2838
#define PERIPH_VIRT_BASE               (0xFE000000) // dec: 4261412864
#define PERIPH_PHYS_BASE               (0x7E000000) // dec: 2113929216
#define DRAM_PHYS_BASE                 (0xC0000000) // dec: 3221225472
#define MEM_FLAG                       (0x04) // dec: 4
#define PAGE_SIZE 										 (4096) //
#define CLOCK_BASE									   (19.2E6) //
#define XTAL_CLOCK                     (54.0E6) //
#define DMA_CHANNEL                    (6) //
#define BUFFER_TIME 									 (1000000) //
#define PWM_WRITES_PER_SAMPLE 				 (10) //
#define PWM_CHANNEL_RANGE 						 (32) //
#define PLLD_FREQ 										 (750000000.) // has higher freq than pi0-3
#endif

// standard & general definitions
#define GPIO_BASE (BCM2836_PERI_BASE + PERIPH_VIRT_BASE) // hex: 0x5F000000 dec: 1593835520
#define LENGTH                         (0x01000000) // dec: 1
#define SUB_BASE                       (0x7E000000) // dec: 2113929216 phys base
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
#define PWMCLK_CNTL0                   (0x5A000016) // dec: 1509949462
#define PWMCLK_DIV0                    (0x5A002800) // dec: 1509959680

// possibility to give argv 0-4 a specific address or pointer
// addresses
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

//---the normal fm-script didn't specified that
#define DMA_BASE_OFFSET                 (0x00007000) // dec: 28672
#define DMA15_BASE_OFFSET 						  (0x00E05000) // dec: 14700544
#define TIMER_BASE_OFFSET 						  (0x00003000) // dec: 12288
#define PWM_BASE_OFFSET                 (0x0020C000) // dec: 2146304
#define PWM_LEN                         (0x28) // dec: 40
#define CLK_BASE_OFFSET                 (0x00101000) // dec: 1052672
#define CLK0_BASE_OFFSET 							  (0x00101070) // dec: 1052784
#define CLK_LEN                         (0x1300) // dec: 4864
#define GPIO_BASE_OFFSET                (0x00200000) // dec: 2097152
#define GPIO_LEN                        (0x100) // dec: 256
#define PCM_BASE_OFFSET                 (0x00203000) // dec: 2109440
#define PCM_LEN                         (0x24) // dec: 36
#define PAD_BASE_OFFSET                 (0x00100000) // dec: 1048576
#define PAD_LEN                         (0x40/4) // 0x10 = dec: 16  //0x64 = dec: 100
#define PADGPIO                         (0x5A000018) // dec: 1509949464

#define DMA_VIRT_BASE                   (PERIPH_VIRT_BASE + DMA_BASE_OFFSET) //
#define PWM_VIRT_BASE                   (PERIPH_VIRT_BASE + PWM_BASE_OFFSET) //
#define PWM_PHYS_BASE                   (PERIPH_PHYS_BASE + PWM_BASE_OFFSET) //
#define CLK_VIRT_BASE                   (PERIPH_VIRT_BASE + CLK_BASE_OFFSET) //
#define GPIO_VIRT_BASE                  (PERIPH_VIRT_BASE + GPIO_BASE_OFFSET) //
#define GPIO_PHYS_BASE                  (PERIPH_PHYS_BASE + GPIO_BASE_OFFSET) //
#define PAD_VIRT_BASE                   (PERIPH_VIRT_BASE + PAD_BASE_OFFSET) //
#define PCM_VIRT_BASE                   (PERIPH_VIRT_BASE + PCM_BASE_OFFSET) //
#define PCM_PHYS_BASE                   (PERIPH_PHYS_BASE + PCM_BASE_OFFSET) //

// GPIO
#define GPFSEL0                         (0x00/4) //p.90 dec: 0
#define GPFSEL1                         (0x04/4) //1
#define GPFSEL2                         (0x08/4) //2
#define GPPUD                           (0x94/4) //37
#define GPPUDCLK0                       (0x98/4) //38
#define GPPUDCLK1                       (0x9C/4) //39

#define CORECLK_CNTL                    (0x08/4) //2
#define CORECLK_DIV                     (0x0C/4) //3
#define GPCLK_CNTL                      (0x70/4) //28
#define GPCLK_DIV                       (0x74/4) //29
#define EMMCCLK_CNTL                    (0x1C0/4) //112
#define EMMCCLK_DIV                     (0x1C4/4) //113

#define CM_LOCK                         (0x114/4) //69
#define CM_LOCK_FLOCKA                  (1<<8) //
#define CM_LOCK_FLOCKB                  (1<<9) //
#define CM_LOCK_FLOCKC                  (1<<10) //
#define CM_LOCK_FLOCKD                  (1<<11) //
#define CM_LOCK_FLOCKH                  (1<<12) //

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
#define A2W_PLL_KA_SHIFT                (7) //
#define A2W_PLL_KI_SHIFT                (19) //
#define A2W_PLL_KP_SHIFT                (15) //

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

#define PWMCLK_CNTL                     (40) //
#define PWMCLK_DIV                      (41) //

#define PWMCTL_PWEN1                    (1<<0) //
#define PWMCTL_MODE1                    (1<<1) //
#define PWMCTL_RPTL1                    (1<<2) //
#define PWMCTL_POLA1                    (1<<4) //
#define PWMCTL_USEF1                    (1<<5) //
#define PWMCTL_CLRF                     (1<<6) //
#define PWMCTL_MSEN1                    (1<<7) //
#define PWMCTL_PWEN2                    (1<<8) //
#define PWMCTL_MODE2                    (1<<9) //
#define PWMCTL_RPTL2                    (1<<10) //
#define PWMCTL_USEF2                    (1<<13) //
#define PWMCTL_MSEN2                    (1<<15) //

#define PWMDMAC_ENAB                    (1<<31)  //
#define PWMDMAC_THRSHLD                 ((15<<8)|(15<<0)) //

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

#define PCMCLK_CNTL                     (38) //
#define PCMCLK_DIV                      (39) //

// PAD
#define GPIO_PAD_0_27                   (0x2C/4)  //11
#define GPIO_PAD_28_45                  (0x30/4)  //12
#define GPIO_PAD_46_52                  (0x34/4)  //13

// DMA
#define DMA_CHANNEL                     (14) //
#define DMA_CHANNEL_MAX                 (14) //
#define DMA_CHANNEL_SIZE                (0x100) //256

#define BCM2708_DMA_ACTIVE              (1<<0) //why bcm 2708?
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
#define BCM2708_DMA_PRIORITY(x)         ((x)&0xF << 16) //
#define BCM2708_DMA_PANIC_PRIORITY(x)   ((x)&0xF << 20) //

#define DMA_CS                          (0x00/4) //0
#define DMA_CONBLK_AD                   (0x04/4) //1
#define DMA_DEBUG                       (0x20/4) //8

#define DMA_CS_RESET		                (1<<31) //
#define DMA_CS_ABORT			              (1<<30) //
#define DMA_CS_DISDEBUG		              (1<<29) //
#define DMA_CS_WAIT_FOR_WRITES          (1<<28) //
#define DMA_CS_INT			                (1<<2) //
#define DMA_CS_ACTIVE			              (1<<0) //
#define DMA_CS_END			                (1<<1) //
#define DMA_CS_PRIORITY(x)		          ((x)&0xF << 16) //0xF=15
#define DMA_CS_PANIC_PRIORITY(x)	      ((x)&0xF << 20) //

#define DREQ_PCM_TX                     (2) //
#define DREQ_PCM_RX                     (3) //
#define DREQ_SMI                        (4) //
#define DREQ_PWM                        (5) //
#define DREQ_SPI_TX                     (6) //
#define DREQ_SPI_RX                     (7) //
#define DREQ_SPI_SLAVE_TX               (8) //
#define DREQ_SPI_SLAVE_RX               (9) //

#define MEM_FLAG_DISCARDABLE            (1 << 0) /* can be resized to 0 at any time. Use for cached data */
#define MEM_FLAG_NORMAL                 (0 << 2) /* normal allocating alias. Don't use from ARM */
#define MEM_FLAG_DIRECT                 (1 << 2) /* 0xC dec: 12 alias uncached */
#define MEM_FLAG_COHERENT               (2 << 2) /* 0x8 dec: 8 alias. Non-allocating in L2 but coherent */
#define MEM_FLAG_L1_NONALLOCATING       (MEM_FLAG_DIRECT | MEM_FLAG_COHERENT) /* Allocating in L2 */
#define MEM_FLAG_ZERO                   (1 << 4)  /* initialise buffer to all zeros */
#define MEM_FLAG_NO_INIT                (1 << 5) /* don't initialise (default is initialise to all ones */
#define MEM_FLAG_HINT_PERMALOCK         (1 << 6) /* Likely to be locked for long periods of time. */

#define PAGE_SHIFT                      (12) //
#define NUM_PAGES                       ((sizeof (struct control_data_s) + PAGE_SIZE - 1) >> PAGE_SHIFT)
#define NUM_SAMPLES                     (64000) //
#define NUM_CBS                         (NUM_SAMPLES * 2) //

#define SUBSIZE                         (1) //
#define DATA_SIZE                       (1000) //
#define SAMPLES_PER_BUFFER 							(512) //

#define BUS_TO_PHYS(x)                 ((x)&~0xC0000000) // dec: 3221225472
#define ACCESS(PERIPH_VIRT_BASE)       (PERIPH_VIRT_BASE + ALLOF7ED) //volatile + int* volatile unsigned*
#define SETBIT(PERIPH_VIRT_BASE, bit)  ACCESS(PERIPH_VIRT_BASE) || 1<<bit// |=
#define CLRBIT(PERIPH_VIRT_BASE, bit)  ACCESS(PERIPH_VIRT_BASE) == ~(1<<bit) // &=

//RTC (DS3231/DS1307 driver as bcm)
#define RTC_I2C_ADDRESS                  (0x68) // dec: 104
#define DS3231_TEMPERATURE_MSB           (0x11)
#define DS3231_TEMPERATURE_LSB           (0x12)
#define SLAVE_ADDR_WRITE                 b(11010000)
#define SLAVE_ADDR_READ                  b(11010001)
#define DS1307_I2C_INPUT_ADDR            (0xD0)            // Adresse ueber die auf den DS1307 geschr. wird
#define DS1307_I2C_OUTPUT_ADDR           (0xD1)            // Adresse ueber die auf den DS1307 geschr. wird
#else
//#error Unknown Raspberry Pi version (variable RASPI)
#endif

/* try a modprobe of i2C-BUS*/
//if (system ("/sbin/modprobe i2c_dev") == -1) {/* ignore errors */}
//if (system ("/sbin/modprobe i2c_bcm2835") == -1) {/* ignore errors */}

//----------------------------------
//program version status and default device
const char *description = "(experimental)"; // version-stage
static char *device = "default"; // playback device

//iterators for loops
int a;
int i;
int k;
int l;
char j;
float x;

//pi memory-map:
int  mem_fd;
char *gpio_mem;
char *gpio_map;
char *spi0_mem;
char *spi0_map;

float xtal_freq=1.0/19.2E6; //LOCK_BASE

//-----------------------------------------
//arguments
int opt;
char *filename = "sound.wav";
float freq = fabs (446.006250);
float subfreq = 67.0;
float ctss_freq = 67.0;
uint32_t Timing;
char *mod;
char *fm = "fm";
char *am = "am";
char *callsign = "callsign";
int power = abs (7);
int powerlevel = abs (7);
int samplerate = abs (22050);
int channels = 1;
double shift_ppm = 0.0;

//float divider = (500000000/(2000*228*(1.+shift_ppm/1.E6) ) ); //PLLD_FREQ = 500000000.
//uint32_t idivider = (float) divider;
//uint32_t fdivider = (uint32_t) ((divider - idivider)*pow(2, 12));

//menu variables
int menuoption;
int channelnumbercb;
int channelnumberpmr;
int subchannelnumberpmr;
int channelmode;
int freqmode;
int modeselect;
int callnameselect;
time_t t;

//IQ & carrier
uint16_t pis = (0x1234); // dec: 4660
//float I = sin ((PERIOD*freq) + shift_ppm);
//float Q = cos ((PERIOD*freq) + shift_ppm);
//float RF_SUM = (I+Q);

//files
FILE *rfp, *wfp;
FILE FileFreqTiming;
FILE wavefile;
//SNDFILE *infile;
//SNDFILE *outfile;
//snd_output_t *output = NULL;
int fp = STDIN_FILENO;
int filebit = abs (16); // for now 16 until i can read the value from an audio file
int readcount;
int readBytes;
float datanew = 0;
float dataold = 0;
float data [2*BUFFER_LEN];
float data_filtered [2*BUFFER_LEN];
char data_name [1024];
char buffer [80];

//audio & sample control
//logarithmic modulation
//volume in dB 0db = unity gain, no attenuation, full amplitude signal
//-20db = 10x attenuation, significantly more quiet
float volume = 1.1f;
const int volume_reference =	1;
float volbuffer [512];
float volumeLevelDb = -6.f; //cut amplitude in half
float volumeMultiplier = 10E-1; //volumeLevelDb

//samples max. 15 kHz resolution for AM / 14.5 kHz FM radio can be recorded
//SF_INFO sfinfo;
int nb_samples;
int excursion = 6000; // 32767 found another value but dont know on what this is based on
float A = 87.6f; // compression parameter
uint32_t carrier_freq = 87600000; // -> this might be the carrier too, why this value?
float FactAmplitude = 2.0; //maybe here amp-modulator input?
float ampf;
float ampf2;
float factorizer;
float sampler;

//instructor for access
unsigned long frameinfo;
int instrs [BUFFERINSTRUCTIONS]; // [1024];
int bufPtr 		= 0;
int instrCnt 	= 0;
int instrPage;
int constPage;
int reg 			= 0; //= gpio / 10;
int shift 		= 0; //= (gpio % 10) * 3;

//network sockets
//custom port via tcp/ip or udp
socklen_t addressLength;
char *localip = "127.0.0.1";
char *host 		= "localhost";
int 	port 		= 8080;

// GPS-coordinates
//default Frankfurt in decimal °grad (centigrade)
float longitude = 8.682127; // E
float latitude 	= 50.110924; // N
float altitude	= fabs (100.00); // elevation in meter above see level (u.N.N.)

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

//program flag options
struct option long_opt [] =
{
		{"filename",		required_argument, NULL, 'n'},
		{"freq",   			required_argument, NULL, 'f'},
    {"samplerate", 	required_argument, NULL, 's'},
    {"mod",	    		required_argument, NULL, 'm'},
    {"callsign",	  required_argument, NULL, 'c'},
    {"power", 			required_argument, NULL, 'p'},
    {"gpio",	  		required_argument, NULL, 'g'},
		{"dma",	  			required_argument, NULL, 'd'},
		{"bandwidth",	  required_argument, NULL, 'b'},
    {"assistent",		no_argument,       NULL, 'a'},
    {"help",	  		no_argument,       NULL, 'h'},
		{"menu",	  		no_argument,       NULL, 'u'}
};


//--------basic functions specified one after another
void infos () //warnings and infos
{
		printf ("\n");
		/*red-yellow -> color:1 for "bright" / 4 for "underlined" and \0XX ansi colorcode //35 for Magenta, 33 red */
    printf ("\033[1;4;35mWelcome to the Pi-Funk! v%s %s for Raspian ARM!\033[0m", VERSION, description); //color escape command for resetting
   	printf ("\nRadio works with *.wav-file with 16-bit @ 22050 [Hz] Mono / 1-700.00000 MHz Frequency \nUse '. dot' as decimal-comma seperator! \n");
    printf ("\nPi oparates with square-waves (²/^2) PWM on GPIO 4 (Pin 7 @ ~500 mA & max. 3.3 V). \nUse power supply with enough specs only! \n=> Use Low-/Highpassfilters and/or ~10 uF-cap, isolators orresistors if needed! \nYou can smooth it out with 1:1 baloon. Do NOT shortcut if dummyload is used! \nCheck laws of your country! \n");
    printf ("\nFor testing (default settings) run: sudo ./pifunk -n sound.wav -f 100.0000 -s 22050 -m fm -c callsign -p 7\n");
		printf ("\nDevicename: %s \n", device);
    printf ("\nxtal_freq: %f \n", xtal_freq);
 		return;
}

int timer (time_t t)
{

	 time (&t);
   //info = localtime (&rawtime);
	 //strftime (buffer, 80, "%x - %I:%M%p", info);
   printf ("\nCurrent formated date & time : %s \n", ctime (&t));
   return 0;
}
//----- programm functions
int gpioselect (int gpiopin)
{
	printf ("\nPlease choose GPIO-Pin (GPIO4=Pin7 default) or 20, 32, 34 \n");
  scanf ("%d", &gpiopin);
	printf ("\nYour GPIO for Transmission is %d ... \n", gpiopin);
	return gpiopin;
}

int dmaselect (int dmachannel)
{
	printf ("\nPlease choose the DMA-Channel \n");
  scanf ("%d", &dmachannel);
	printf ("\nYour DMA-Channel is %d ... \n", dmachannel);
	return dmachannel;
}

float bandwidthselect (float bandwidth)
{
	printf ("\nPlease choose the bandwidth (default=15.00 kHz) \n");
  scanf ("%f", &bandwidth);
	printf ("\nYour bandwidth is %f \n", bandwidth);
	return bandwidth;
}

int filecheck (char *filename, FILE wavefile)  // expected int?
{
  printf ("\nPlease enter the full path including name of the *.wav-file you want to use: \n");
  scanf ("%s", &filename);
	printf ("\nTrying to play %s ... \n", filename);
	printf ("\nOpening file ... \n");
	printf ("\nallocating filename mem... \n");
	filename = (char *) malloc (128);// allocating memory for filename
	sprintf (filename, "\n%s \n", "file.ft");
	char *stdfile= "sound.wav";
  if (filename != stdfile)
	{
     fp = open (filename, O_RDONLY | O_CREAT | O_WRONLY | O_TRUNC, 0644); // O_RDWR
	   return fp;
	}
	else
	{
	   fp = open ("sound.wav", O_RDONLY | O_CREAT | O_WRONLY | O_TRUNC, 0644); // sounds/sound.wav directory should be tested
	   return fp;
	}
	return fp;
}

float freqselect () // gets freq by typing in
{
	printf ("\nYou selected 1 for Frequency-Mode \n");
	printf ("\nType in Frequency (0.1-1200.00000 MHz): \n"); // 1B+ for 700 MHz chip, pi3 1.2 GHz
	scanf  ("%f", &freq);
	printf ("\nYou chose: %f MHz \n", freq);
  return freq;
}
//--------------------------------------------------
float step ()
{
	float steps;
	printf ("\nChoose PMR-Steps 6.25 / 12.5 kHz: \n");
	scanf ("%f", &steps);
	if (steps==6.25)
	{
	printf ("\nnSteps are %f kHz \n", steps);
  return steps;
	}
	else if (steps==12.5)
	{
	printf ("\nSteps are %f kHz \n", steps);
  return steps;
	}
  else
  {
  printf ("\nNO steps could be determined, wrong input! Using Standard 12.5\n");
  steps = 12.5;
  return steps;
  }
}

// Channel-mode
float channelmodepmr () //PMR
{
	char *type;
  char *analog = "a";
	char *digital = "d";
	printf ("\nChoose PMR-Type (a)nalog / (d)igital: \n");
	scanf ("%s", &type);

	if (type==analog)
	{
	printf ("\nChoose aPMR-Channel 1-16 (18 to exit): \n");
	scanf ("%d", &channelnumberpmr);
	switch (channelnumberpmr)
	 {
	 //---- Analog & DMR
	 case 1: freq=446.00625; break;	// Standard
	 case 2: freq=446.01875; break; // Geocaching
	 case 3: freq=446.03125; break; // Standard
	 case 4: freq=446.04375; break; // at 3-chan-PMR-devices its ch. 2
	 case 5: freq=446.05625; break; // Contest
	 case 6: freq=446.06875; break; // Events
	 case 7: freq=446.08125; break; // at 3-channel-PMR-devices it's ch. 3
	 case 8: freq=446.09375; break; // Standard

  //-----Digital only
	// dmr (tier 1) digital new since 28.09.2016
	// extra 8 chan
	// 12.5 kHz steps
	 case 9:  freq=446.10312; break;
	 case 10: freq=446.10625; break;
	 case 11: freq=446.11875; break;
	 case 12: freq=446.13125; break;
	 case 13: freq=446.14375; break;
	 case 14: freq=446.15625; break;
	 case 15: freq=446.16875; break;
	 case 16: freq=446.18125; break;
	 case 17: freq=446.19375; break;

	 case 18: exit (0);
	 default:	freq=446.00625;
	 					printf ("\nDefault channelnumber = 1 on freq = %f \n", freq);
						break;
   }
  }
	else if (type==digital)
	{
	printf ("\nChoose dPMR-Channel 1-32 (33 to exit): \n");
	scanf ("%d", &channelnumberpmr);
	switch (channelnumberpmr)
	 {
   //FD-PMR 6.25 kHz steps  & for DCDM devices: CC1 TG99 TS1 = Kontakt, CC1 TG9112 TS1 = EmCOM
	 case 1:	freq=446.003125; break;
	 case 2:	freq=446.009375; break;
	 case 3:	freq=446.015625; break;
	 case 4:	freq=446.021875; break;
	 case 5:	freq=446.028125; break;
	 case 6:	freq=446.034375; break;
	 case 7:	freq=446.040625; break;
	 case 8:	freq=446.046875; break;
	 case 9:	freq=446.053125; break;
	 case 10:	freq=446.059375; break;
	 case 11:	freq=446.065625; break;
	 case 12:	freq=446.071875; break;
	 case 13:	freq=446.078125; break;
	 case 14:	freq=446.084375; break;
	 case 15:	freq=446.090625; break;
	 case 16:	freq=446.096875; break;
	 case 17:	freq=446.103125; break;
	 case 18:	freq=446.109375; break;
	 case 19:	freq=446.115625; break;
	 case 20:	freq=446.121875; break;
	 case 21:	freq=446.128125; break;
	 case 22:	freq=446.134375; break;
	 case 23:	freq=446.140625; break;
	 case 24:	freq=446.146875; break;
	 case 25:	freq=446.153125; break;
	 case 26:	freq=446.159375; break;
	 case 27:	freq=446.165625; break;
	 case 28:	freq=446.171875; break;
	 case 29:	freq=446.178125; break;
	 case 30:	freq=446.184375; break;
	 case 31:	freq=446.190625; break;
	 case 32:	freq=446.196875; break;
	 //normaly up to 32 chan in dpmr
	 case 33: 		exit (0);
	 default:			freq=446.003125;
	 							printf ("\nDefault channelnumber = 1 on freq = %f \n", freq);
								break;
	 }
	}
  else
	{
		printf ("\nNO type could be determined, wrong input! Using analog as standard \n");
		type=analog;
	}
  printf ("\nChannelnumber = %d on freq = %f \n", channelnumberpmr, freq);
	return freq;
}

float subchannelmodepmr () //Pilot-tone
{
	printf ("\nChoose Sub-Channel 0-38 (39 to exit): \n");
	scanf ("%d", &subchannelnumberpmr);
	switch (subchannelnumberpmr)
	{
		// FYI 19 (38)-kHz-Pilottone on UKW
	 //---- Analog & digital
	 case 0:	subfreq=67.000; printf ("\nChannels (all) = 0, default CTSS-Chan 1 %f \n", subfreq); break;	// Scan all Chan till active , now chan1
	 case 1:  subfreq=67.900; break;	//4.9 Hz step
	 case 2: 	subfreq=71.900; break;
	 case 3: 	subfreq=74.400; break;
	 case 4: 	subfreq=77.000; break; // at 3-chan-PMR-devices its ch. 2
	 case 5: 	subfreq=79.700; break; // Contest
	 case 6: 	subfreq=82.500; break; // Events
	 case 7: 	subfreq=85.400; break; // at 3-channel-PMR-devices it's ch. 3
	 case 8: 	subfreq=88.500; break; // Standard
	 case 9:  subfreq=91.500; break;
	 case 10: subfreq=94.800; break;
	 case 11: subfreq=97.400; break;
	 case 12: subfreq=100.000; break;
	 case 13: subfreq=103.500; break;
	 case 14: subfreq=107.200; break;
	 case 15: subfreq=110.900; break;
	 case 16: subfreq=114.800; break;
	 case 17: subfreq=118.800; break;
	 case 18: subfreq=123.000; break;
	 case 19: subfreq=127.300; break;
	 case 20: subfreq=131.800; break;
	 case 21: subfreq=136.500; break;
	 case 22: subfreq=141.300; break;
	 case 23: subfreq=146.200; break;
	 case 24: subfreq=151.400; break;
	 case 25: subfreq=156.700; break;
	 case 26: subfreq=162.200; break;
	 case 27: subfreq=167.900; break;
	 case 28: subfreq=173.800; break;
	 case 29: subfreq=179.900; break;
	 case 30: subfreq=186.200; break;
	 case 31: subfreq=192.800; break;
	 case 32: subfreq=203.500; break;
	 case 33: subfreq=210.700; break;
	 case 34: subfreq=218.100; break;
	 case 35: subfreq=225.700; break;
	 case 36: subfreq=233.600; break;
	 case 37: subfreq=241.800; break;
	 case 38: subfreq=250.300; break;
	 case 39: exit (0);
	 default: subfreq=67.000;
						printf ("\nDefault subchannel = 1 on subfreq = %f \n", subfreq);
						break;
	}
  printf ("\nSubchannelnumber = %d on subfreq = %f \n", subchannelnumberpmr, subfreq);
	return freq;
}

float channelmodecb () // CB
{
	printf ("\nChoose CB-Channel 0-80 (81 to exit): \n");
	scanf ("%d", &channelnumbercb);
	switch (channelnumbercb)
	{
			// --> translation of infos in english in future updates!
       case 0:   freq=27.0450; break; //first digital channel
			 case 1:   freq=26.9650; break; //empfohlener Anrufkanal (FM)
			 case 2:   freq=26.9750; break; //inoffizieller Berg-DX-Kanal (FM)
			 case 3:   freq=26.9850; break;
			 case 4:   freq=27.0050; break; //empfohlener Anrufkanal (AM)/Anrufkanal Feststationen (AM)
			 case 5:   freq=27.0150; break; //Kanal wird von italienischen Fernfahrern in Deutschland und Italien benutzt.
			 case 6:   freq=27.0250; break; //Datenkanal (D)
		   case 7:   freq=27.0350; break; //Datenkanal (D)
			 case 8:   freq=27.0550; break;
			 case 9:   freq=27.0650; break; //Fernfahrerkanal (AM)/weltweiter Notrufkanal EMG
			 case 10:  freq=27.0750; break; //Antennen-Abgleich - halbe Channel-Anzahl!! ansonsten Chan 20 oder 40
			 /*		 Unterschied der Nachbarkanaele nicht um 10 kHz, sondern um 20 kHz
			 Diese Kanaele sind in den meisten Laendern nicht fuer CB-Funk zugelassen.
			 Zwecke wie z. B. Funkfernsteuerungen, Babyphones, kabellose Tastaturen und Maeuse verwendet */
			 case 11:   freq=27.0850; break;  //freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland
			 case 1111: freq=27.0950; break; //Eurobalise-Energieversorgung
			 case 12:   freq=27.1050; break;
			 case 13:   freq=27.1150; break;
			 case 14:   freq=27.1250; break; //oft verwendet fuer Spielzeug-Fernsteuerungen (mittels Selektivton)
			 case 15:   freq=27.1350; break; //inoffizieller Anrufkanal SSB (USB)
			 case 1515: freq=27.1450; break;
			 case 16:   freq=27.1550; break; //Funkverkehr mit und zwischen Wasserfahrzeugen
			 case 17:   freq=27.1650; break; //Kanal wird von daenischen Schwertransportfahrern in Deutschland und Daenemark benutzt
			 case 18:   freq=27.1750; break;
			 case 19:   freq=27.1850; break; //empfohlener Fernfahrerkanal (FM)/oft von Walkie-Talkies genutzt/teilweise auch als Notrufkanal angegeben/auch von Babyfonen genutzt
			 case 1919: freq=27.1950; break;
			 case 20:   freq=27.2050; break; //zum Antennenabgleich genutzte Mitte bei 40-Kanal-Geraeten, wird in oesterreich sehr oft fuer Schwertransportfahrten benutzt

		 	 // 40 chan devices
			 case 21:   freq=27.2150; break; //tuerkischer Anrufkanal in Deutschland und Europa (FM)
			 case 22:   freq=27.2250; break; //oft von Walkie-Talkies genutzt, auch von Babyfonen genutzt, wird auch als Anrufkanal fuer rumaenische Fernlastfahrer verwendet
			 case 23:   freq=27.2550; break; //Die Kanaele 23, 24, 25 sind sog. Dreher, sie folgen nicht dem aufsteigenden 10-kHz-Raster
			 case 24:   freq=27.2350; break; //Datenkanal (D)
			 case 25:   freq=27.2450; break; //Datenkanal (D), USB ROS Intern
			 case 26:   freq=27.2650; break;
			 case 27:   freq=27.2750; break;
			 case 28:   freq=27.2850; break; //Kanal wird von polnischen Fernfahrern in Deutschland benutzt, Anrufkanal in Polen, wobei allgemein die CB-Kanalfrequenz in Polen um 5 kHz niedriger ist
			 case 29:   freq=27.2950; break; //Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ber eine Internetverbindung in Deutschland
			 case 30:   freq=27.3050; break; //inoffizieller DX-Kanal (FM), Anrufkanal fuer Funker aus dem ehemaligen Jugoslawien
			 case 31:   freq=27.3150; break; //inoffizieller DX-Kanal (FM)
			 case 32:   freq=27.3250; break;
			 case 33:   freq=27.3350; break;
			 case 34:   freq=27.3450; break; //freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland
			 case 35:   freq=27.3550; break; //oeffentlicher Kanal
			 case 36:   freq=27.3650; break; //Datenkanal USB ROS international
			 case 37:   freq=27.3750; break; //Gateway-Kanal oesterreich, FM
			 case 38:   freq=27.3850; break; //inoffizieller internationaler DX-Kanal (LSB)
			 case 39:   freq=27.3950; break; //Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland
			 case 40:   freq=27.4050; break; //ab Maerz 2016 freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete
		  	//ueber eine Internetverbindung in Deutschland (FM/AM/SSB in D)
			 /* 80 chan devices
			 Auf den nationalen Zusatzkanaelen 41 bis 80 ist nur die Modulationsart FM erlaubt
			 Nachfolgend sind die Frequenzen der nationalen Zusatzkanaele, die im CB-Funk benutzt werden duerfen, aufgelistet: */
			case 41:   freq=27.5650; break; //Ab Maerz 2016 Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland (FM), inoffizieller DX-Kanal (FM)
			case 42:   freq=27.5750; break; //inoffizieller DX-Kanal (FM)
			case 43:   freq=27.5850; break;
			case 44:   freq=27.5950; break;
			case 45:   freq=27.6050; break;
			case 46:   freq=27.6150; break;
			case 47:   freq=27.6250; break;
			case 48:   freq=27.6350; break;
			case 49:   freq=27.6450; break;
			case 50:   freq=27.6550; break;
			case 51:   freq=27.6650; break;
			case 52:   freq=27.6750; break; //Datenkanal (D)(FM)
			case 53:   freq=27.6850; break; //Datenkanal (D)(FM)
			case 54:   freq=27.6950; break;
			case 55:   freq=27.7050; break;
			case 56:   freq=27.7150; break;
			case 57:   freq=27.7250; break;
			case 58:   freq=27.7350; break;
			case 59:   freq=27.7450; break;
			case 60:   freq=27.7550; break;

      case 61:   freq=26.7650; break; //Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland
			case 62:   freq=26.7750; break;
			case 63:   freq=26.7850; break;
			case 64:   freq=26.7950; break;
			case 65:   freq=26.8050; break;
			case 66:   freq=26.8150; break;
			case 67:   freq=26.8250; break;
			case 68:   freq=26.8350; break;
			case 69:   freq=26.8450; break;
			case 70:   freq=26.8550; break;
			case 71:   freq=26.8650; break; //Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland
			case 72:   freq=26.8750; break;
			case 73:   freq=26.8850; break;
			case 74:   freq=26.8950; break;
			case 75:   freq=26.9050; break;
			case 76:   freq=26.9150; break; //Datenkanal (D)(FM)
			case 77:   freq=26.9250; break; //Datenkanal (D)(FM)
			case 78:   freq=26.9350; break;
			case 79:   freq=26.9450; break;
			case 80:   freq=26.9550; break; //Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland */
			case 81:   exit (0);

			default:		freq=26.9650;
									printf ("\nDefault CB chan = 1 %f \n", freq);
									break;

	}
  printf ("\nUsing channel = %d on freq =  %f \n", channelnumbercb, freq);
	return  freq;
}

void modselect (int argc, char **argv, char *mod)
{
	printf ("\nOpening Modulator... \n");
	if (mod == fm)
	{
		void modulationfm (int argc, char **argv);
	}
	else if (mod == am)
	{
		void modulationam (int argc, char **argv);
	}
	else
	{
		printf ("\nError selecting modulation! \n");
	}
 	return;
}

char modulationselect ()
{
	printf ("\nChoose your Modulation [1] FM // [2] AM // [3] Exit : \n");
	scanf ("%d", &freqmode);
	switch (freqmode)
	{
		case 1: printf ("\nYou selected 1 for FM! \n");
						mod = "fm";
						void modselect (int argc, char **argv, char *mod);
		        break;

		case 2: printf ("\nYou selected 2 for AM! \n");
						mod = "am";
						void modselect (int argc, char **argv, char *mod);
		        break;

		case 3: printf ("\nExiting... \n");
						exit (-1);

		default:	mod = "fm";
						  printf ("\n Default = 1 \n");
						  break;
	}
	return *mod;
}

void channelselect () // make a void
{
	printf ("\nYou selected 1 for Channel-Mode \n");
	printf ("\nChoose your Band: [1] PMR // [2] CB \n");
  scanf  ("%d", &channelmode);
  switch (channelmode) // from here collecting infos and run it step by step, same for freq-mode
  {
					case 1: printf ("\nPMR CHAN-MODE \n");
									step ();
									channelmodepmr (); // gets freq from pmr list
									break;

		   		case 2: printf ("\nCB CHAN-MODE \n");
									channelmodecb ();
									break;

					default:  printf ("\nDefault: PMR\n");
										step ();
										channelmodepmr (); // gets freq from pmr list
									  break;
	}
	return;
}

//--------------LED stuff
//controlling via py possible but c stuff can be useful too by bcm funcs!
//turn on LED (with 100 kOhm pullup resistor while transmitting
int ledinactive (char *filename, float freq, int samplerate)
{
		//check if transmitting
	/*	while (!play_wav (char *filename, float freq, int samplerate))
		{
				//cm2835_gpio_write (PIN17, LOW);
				printf ("\nLED OFF - No Transmission! \n");
		}
    */
    return 0;
}

int ledactive ()
{
//simulation of gpio for debug
/*
  bcm2835_set_debug (1);
  if (!bcm2835_init ())
	{
		printf ("\nBCM 2835 init failed! \n");
  	return 1;
	}
	else if (1)
	{
    // Set the pin to be an outputannels
    //bcm2835_gpio_fsel (PIN17, BCM2835_GPIO_FSEL_OUTP);
  	printf ("\nBCM 2835 init done and PIN 4 activated \n");
    // LED is active during transmission
		while (play_wav (char *filename, float freq, int samplerate))
		{
			// Turn it on
			bcm2835_gpio_write (PIN17, HIGH);
			printf ("\nLED ON - Transmission...! \n");
			// wait a bit
			bcm2835_delay (500);
		}
	}
	else // if no transmission than turn it off // (ledactive != 0)
  {
		cm2835_gpio_write (PIN17, LOW);
		printf ("\nLED OFF - No Transmission \n");
	}
   //bcm2835_close ();
	 */
	 printf ("\nBCM 2835 closing \n");
   return 0;
}

/*
float audiovol ()
{
	float datavalue = data [i] * 4 * volume; // modulation index (AKA volume) logarithmic hearing of human
	for (int i = 0; i < SAMPLES_PER_BUFFER; ++i)
	{
     volbuffer [i] *= volumeMultiplier;
     printf ("\nValues: i: %d, volbuffer: %f, volumeMultiplier: %f \n", i, volbuffer [i], volumeMultiplier);
		 printf ("\nAdresses: i: %p, volbuffer: %p, volumeMultiplier: %p \n", &i, &volbuffer [i], &volumeMultiplier);
     return volbuffer [i];
	}
	return volumeMultiplier;
}
*/

//--------------- Voids
void handSig () // exit func
{
		printf ("\nExiting... \n");
		exit (0);
}

void clearscreen ()
{
  printf ("\n\033[H\033[J\n");
  //fflush (stdin); // alterntives
  //clsscr ();
  //system ("clear");
}

void modulate (int l)
{
	printf ("\nModulate carrier... \n");
	//	ACCESS (CM_GP0DIV) == (CARRIER << 24) + MODULATE + l;  //
}

void getRealMemPage (void **vAddr, void **pAddr) // should work through bcm header!
{
		void *a = valloc (4096);

		((int*) a) [0] = 1; // use page to force allocation

		mlock (a, 4096); // lock into ram

		*vAddr = a; // we know the virtual address now

		int fp = open ("/proc/self/pagemap", O_RDONLY); //"w"
		lseek (fp, ((int) a)/4096*8, SEEK_SET);
		read (fp, &frameinfo, sizeof (frameinfo));

		*pAddr = (void*) ((int) (frameinfo*4096));
}

void freeRealMemPage (void **vAddr)
{
		printf ("\nFreeing vAddr ... \n");
		munlock (vAddr, 4096); // unlock ram
		free    (vAddr); // free the ram#

}

void carrierhigh () // enables it
{
	printf ("\nSetting carrier high ... \n");
/* Added functions to enable and disable carrier */
// Set CM_GP0CTL.ENABLE to 1 HIGH (2nd number) // 0x5A-> CARRIER dec: 90
//struct GPCTL setupword = {6, 1, 0, 0, 0, 1, 0x5A};// set it to ! = LOW
//ACCESS (CM_GP0CTL) == *((int*) &setupword); //setting cm
}

void carrierlow () // disables it
{
	printf ("\nSetting carrier low ... \n");
//struct GPCTL setupword = {6, 0, 0, 0, 0, 1, 0x5A};// set it to 0 = LOW
//ACCESS (CM_GP0CTL) == *((int*) &setupword);
}

void setupfm ()
{
  printf ("\nSetting up FM... \n");
    // open /dev/mem
  if ((mem_fd = open ("/dev/mem", O_RDWR|O_SYNC) ) < 0)
	{
        printf ("\nCan't open /dev/mem ! \n"); // via bcm possible
        return;
  }

  allof7e = (unsigned*) mmap (
								NULL,
								0x01000000, // LENGTH
								PROT_READ|PROT_WRITE, //
								MAP_SHARED, //
								mem_fd, //
								0x20000000); //  PERIPH_VIRT_BASE

  if ((int) allof7e == -1)
	{
		exit (-1);
	}

   //SETBIT (GPFSEL0, 14);
   //CLRBIT (GPFSEL0, 13);
   //CLRBIT (GPFSEL0, 12);

	 carrierhigh ();
}

///------------------------------------
//relevant for transmitting stuff
void play_list () // exit func
{
		printf ("\nOpening playlist-folder (dummy) \n"); // in sounds/playlist

}

void play_wav (char *filename, float freq, int samplerate)
{

	/*wiki https://en.wikipedia.org/wiki/WAV
	  https://en.wikipedia.org/wiki/44,100_Hz
    NTSC: 44056 Hz
    245 × 60 × 3 = 44100
    245 active lines/field × 60 fields/second × 3 samples/line = 44100 samples/second
    (490 active lines per frame, out of 525 lines total)

    PAL:294 × 50 × 3 = 44100
    294 active lines/field × 50 fields/second × 3 samples/line = 44100 samples/second
    (588 active lines per frame, out of 625 lines total)
	*/

	play_list ();
  // after getting filename insert then open
	printf ("\nAllocating file to mem for wave ... \n");

	int sz = lseek (fp, 0L, SEEK_END);
	/* lseek: repositions the file offset of the open file description
    associated with the file descriptor fd to the argument offset
    according to the directive http://man7.org/linux/man-pages/man2/lseek.2.html
	SEEK_END: The file offset is set to the size of the file plus offset bytes. */
  short *data = (short*) malloc (sz);

  for (int i = 0; i < 22; i++) // why i less then 22?
  {
        read (fp, &data, 2); // read past header (or sz instead on 2 ?)
        printf ("\nReading fp \n");
  }

  while (readBytes == read (fp, &data, 1024))
  {
        float fmconstant = (samplerate*50.0E-6); //1.1025 for pre-emphisis filter, 50us time constant
				printf ("\nfmconstant: %f \n", fmconstant);
        int clocksPerSample = (22050/samplerate*1400); // for timing if 22050 then 1400
				printf ("\nclocksPerSample: %d \n", clocksPerSample);
        // if samplerate > 15.75 then clocks per sample is negetive !! not good
        datanew = ((float) (*data)/excursion); //some constant for unsigned int excursion
				printf ("\ndatanew: %f \n", datanew);
        float sample = datanew + (dataold-datanew)/(1-fmconstant); // fir of 1 + s tau
				printf ("\nsample: %f \n", sample);
        float dval = sample*15.0; // actual transmitted sample, 15 hz is standard bandwidth (about 75 kHz) better 14.5
				printf ("\ndval: %f \n", dval);
        int intval = (int) (round (dval)); // integer component
				printf ("\nintval: %d \n", intval);
        float frac = ((dval - intval)/2 + 0.5);
				printf ("\nfrac: %f \n", frac);
        int fracval = (frac*clocksPerSample);
				printf ("\nfracval: %d \n", fracval);

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

        //ster->consume (data, readBytes); // ster -> for stereo
        printf ("\nReading bytes from fp ... \n");
  }

   close (fp);
   printf ("\nClosing file \n");
}

void setupDMA ()
{
	printf ("\nSetup of DMA starting... \n");
	//atexit (unsetupDMA);
	signal (SIGINT,  handSig);
	signal (SIGTERM, handSig);
	signal (SIGHUP,  handSig);
	signal (SIGQUIT, handSig);

	//allocate a few pages of ram
  //getRealMemPage (&constPage.v, &constPage.p);
	int centerFreqDivider = (int) ((500.0/freq) * (float) (1<<12) + 0.5);
	printf ("\ncenterFreqDivider %d \n", centerFreqDivider);
	// make data page contents - it s essientially 1024 different commands for the
	// DMA controller to send to the clock module at the correct time
	for (int i = 0; i<1024; i++)
	{
	   // ((int*) (constPage.v))[i] = (CARRIER << 24) + centerFreqDivider - 512 + i;
	}

	while (instrCnt < 1024) //BUFFERINSTRUCTIONS
	{
     //getRealMemPage (&instrPage.v, &instrPage.p);

     // make copy instructions
  	 //struct CB* instr0 = (struct CB*)instrPage.v;

     for (int i = 0; i<4096/sizeof (struct CB); i++)
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
   //ACCESS (CLKBASE + 40*4) == (PWMCLK_CNTL); // (dec: 1509949478)
   //usleep (1000);

   //ACCESS (CLKBASE + 41*4) == (PWMCLK_DIV); //
   //usleep (1000);

   // set up pwm
   //ACCESS (PWMBASE + 0x0) == 0;
   //usleep (1000);

   //ACCESS (PWMBASE + 0x4) == -1; // status: clear errors (0x4 in dec: 4)
   //usleep (1000);

   // Use fifo, repeat, serializer, enable ch
   //ACCESS (PWMBASE + 0x0) == -1 | (1<<13) | (1<<10) | (1<<9) | (1<<8);
   //usleep (1000);

   // DMAC then DMA enable in 0x8 dec:8 / pwmbase+8 = 7E20C008 (dec:2116075528) /
   //ACCESS (PWMBASE + 0x8) == (1<<31) | (DMAC);

   //activate dma
   //struct DMAREGS* DMA0 = (struct DMAREGS*)(ACCESS (DMABASE));
   //DMA0->CS = 1<<31; // reset
   //DMA0->CONBLK_AD = 0;
   //DMA0->TI = 0;

   //DMA0->CONBLK_AD = (int) (instrPage.p);
   //DMA0->CS = (1<<0) | (255 <<16);  // enable bit = 0, clear end flag = 1, prio=19-16
   printf ("\nSetup DMA done! \n");
}

void unsetupDMA ()
{
	//struct DMAREGS* DMA0 = (struct DMAREGS*) ACCESS(DMABASE);
	//DMA0->CS = 1<<31; // reset dma controller
	printf ("\nUnsetting DMA done \n");

	exit (-1);
}

//------- main progs

int samplecheck (char *filename, int samplerate) // better name function: sample/bitchecker
{
	printf ("\nSamplerate/bit-checker starting \n");

	/*
  if (!(fp = open (filename, SFM_READ, &sfinfo))) //check wat SFM sfinfo does!?
  {   // Open failed so print an error message.
        printf ("\nNot able to open input file for samplecheck %s \n", filename);
				printf ("\nNot able to open filepointer for samplecheck %d \n", fp);
        // Print the error message from libsndfile.
        return 1;
  }

	//sfinfo.samplerate = samplerate;
	if (sfinfo.samplerate == 22050) //44 or 48 kHz needs testing
	{
		printf ("\nSamplerate is 22050! (%d)\n", sfinfo.samplerate);
		return sfinfo.samplerate;
	}
	else if (sfinfo.samplerate == 14500)
	{
			printf ("\nSamplerate is 14500! (%d)\n", sfinfo.samplerate);
			return sfinfo.samplerate;
	}
	else
  {
	printf ("\nInput samplerate must be at least 22.050 [kHz] for FM or 14.500 [kHz] for AM! \n");
	return 1;
}
*/

	// check filebits here somehow (via SFM)
	if (filebit != 16)
	{

		printf ("\nError: Input must be 16 bit! \n");
		return 1;
	}
	else
	{
		printf ("\nInput is 16 bit! \n");
		return 0;
	}

	//While there are frames in the input file, read them,
	//process them and write them to the output file
	//----------------------
  while (readcount == read (fp, data, BUFFER_LEN))
  {
	 // where to input the freq like in fm?
	  for (k = 0; k < nb_samples; k++)
	  {
		  char b = data [k*channels];
			printf ("\nChannel buffer b = %c \n", b);
			if (channels == 0)
			{
				printf ("\nSample Error! NO (0) channels \n"); // >1 in stereo or dual mono with half samplerate
			}
			else if (channels == 1)
			{
				printf ("\n File has %d channel (MONO)! \nReading ... \n", channels);
				// stereo file, avg left + right --> should be mono at 22.05kHz
				b += data [k*channels+1];
				b /= 2; // maybe *2 to make a dual mono and not doing stereo in half!
				printf ("\nb = %c \n", b);
			}
			else if (channels == 2)
			{
				printf ("\nFile has 2 Channels (STEREO)! \n");// >1 in stereo or dual mono with half samplerate
			}
			else
			{
					printf ("\nError: File has %d Channels! (> 2 channels) \n", channels);
			}
      nb_samples = (readcount/channels);
 			// was defined as global var above
			printf ("\nnb_samples: %d \n", nb_samples);
			printf ("\nCompression prameter A: %f \n", A);
			//maybe here am option for amplitude factor input!?
			printf ("\nFactamplitude: %f \n", FactAmplitude);

			ampf = (x/32767.0f);
			printf ("\nampf1: %f \n", ampf);

  		ampf2 = (fabs (ampf) < 1.0f/A) ? A * fabs (ampf)/(1.0f+ln (A)) : (1.0f+ln (A * fabs (ampf)))/(1.0f + ln (A)); //compand
			printf ("\ncompand ampf2: %f \n", ampf2);

			x = (int) (round (ampf2 * 32767.0f));
			printf ("\nnew x: %f \n", x);

		  factorizer = (x * 32767.0f * FactAmplitude);
			printf ("\nfactorizer: %f \n", factorizer);

			sampler = (1E9/samplerate); //44.000
			printf ("\nsampler: %f \n", sampler);
			printf ("\nNow writing tone in am ... \n");
			void WriteTone (float freq); // somehow input freq here ?!?
			//return channels, ampf, ampf2, x, factorizer, sampler;
	  } // for loop
		printf ("\nwhile readcount ... \n");
  } // while loop
    // Close input and output files
    //fclose (FileFreqTiming);
  close (fp);
  printf ("\nFile saved! \n");
	return samplerate;
}
// return freqmode, channels, ampf, ampf2, x, factorizer, sampler;

// all subch. -> base/default case 0 -> channel 0
// if subchannels is 0 = all ch. then check special stuff -> maybe scan func ?
// squelch/treshhold to build in maybe -> scan function till signal?

//AM
void WriteTone (float freq)
{
	float Frequencies = freq;
	typedef struct
	{

		float Frequency;
		uint32_t WaitForThisSample;
	} samplerf_t;
	samplerf_t RfSample;
	RfSample.Frequency = Frequencies;
	RfSample.WaitForThisSample = Timing; //in 100 of nanoseconds
	printf ("\nFreq = %f, Timing = %d \n", RfSample.Frequency, RfSample.WaitForThisSample);
	if (write (fp, &RfSample, sizeof (samplerf_t)) != sizeof (samplerf_t))
	{
		fprintf (stderr, "\nUnable to write sample! \n");
	}
	printf ("\nWriting tone \n");
}

char callname ()
{
    //if (*callsign == NULL){
		printf ("\nYou don't have specified a callsign yet!\nPress (1) for custom or (2) default 'callsign': \n");
		scanf ("%d", &callnameselect);
		switch (callnameselect)
	  {
	   case 1: printf ("\nType in your callsign: \n");
						 scanf  ("%s", &callsign);
						 printf ("\nYour callsign is: %s \n", callsign);
						 break;

		 case 2: callsign = "callsign"; //default callsign
						 printf ("\nUsing default callsign: %s \n", callsign);
						 break;

		 default: callsign = "callsign"; //default callsign
		 					printf ("\nError! Using default callsign: %s \n", callsign);
							break;
    }
  	return callsign; //, &callsign, *callsign;
}

void modetype (float freq)
{
	printf ("\nChoose Mode: [1] Channelmode // [2] Frequencymode \n");
	scanf ("%d", &modeselect);

	switch (modeselect)
	{
		case 1:	 	printf ("\n[1] Channelmode: \n");
							channelselect (freq); //undefined reference
							break;

		case 2:		printf ("\n[2] Frequencymode: \n");
							freqselect (freq);
							break;

		default: printf ("\nError! \n");
						 break;
	}
	return;
}

int powerselect ()
{
	printf ("\nType in powerlevel (0-7 from 2-14 mA): \n");
	scanf ("%d", &power);
	printf ("\nPowerlevel was set to: %d \n", power);
	return power;
}

// read / import csv for pmr
char csvreader ()
{
    printf ("\nChecking CSV-file for CTSS-Tones (Coded Tone Control Squelch System)... \n");
		printf ("\nOrder of the list: \nLocation, Name, Frequency, Duplex, Offset, Tone,\nrToneFreq, cToneFreq, DtcsCode, DtcsPolarity, Mode,\nTStep, Skip, Comment, URCALL, RPT1CALL, RPT2CALL\n");

    rfp = fopen ("ctsspmr.csv", "r"); //read-only!
    wfp = fopen ("ctsswriter.csv", "w+"); //with + it updates, if exists overwrites
    while (!feof (rfp))
    {
    	//here check for semicolon or comma delimiter (default)
    	j = fgetc (rfp);
    	fputc (j, wfp);
    }
		printf ("\n%d\n", j);
    fclose (rfp);
    fclose (wfp);
    printf ("\nCSV-import of CTSS-list finished! \n");
    return j;
}

void modulationam (int argc, char **argv)
{
	/*{IQ (FileInput is a mono wav contains I on left channel, Q on right channel)}
		{IQFLOAT (FileInput is a Raw float interlaced I, Q)}
		{RF (FileInput is a (float) Frequency, Time in nanoseconds}
		{RFA (FileInput is a (float) Frequency, (int) Time in nanoseconds, (float) Amplitude}
		{VFO (constant frequency)} */
		printf ("\nam modulator starting \n");
		void WriteTone (float freq);// actual modulation stuff here for am -> wrrite tone?
		ledactive ();
	  return;
}

void modulationfm (int argc, char **argv)//FM
{
  	printf ("\nPreparing for FM... \n");
    setupfm (); // gets filename & path or done by filecheck () func
	  printf ("\nSetting up DMA... \n");
		setupDMA (); //setupDMA (argc>2 ? atof (argv [2]):100.00000); // : default freq
    //play_wav (char *filename, float freq, int samplerate); // atof (argv [3]):22050)
		return;
}

int tx (int argc, char **argv)
{
  //pads need to be defined
  //Drive Strength (power 7 standard): 0 = 2mA, 7 = 16mA. Ref: https://www.scribd.com/doc/101830961/GPIO-Pads-Control2
  //pad_reg [GPIO_PAD_0_27]  = PADGPIO + power;
  //pad_reg [GPIO_PAD_28_45] = PADGPIO + power;
	//GPIO needs to be ALT FUNC 0 to output the clock
	//gpio_reg [reg] = (gpio_reg [reg] & ~(7 << shift));

	//play_wav (char *filename, float freq, int samplerate);
	void modselect (int argc, char **argv, char *mod);
	ledactive ();
	printf ("\nBroadcasting now ...! \n");

	return 0;
}

void cgimodule () // just a small test, not meant for pifunk
{
 printf ("\ncontext-type:text/html\n");
 printf ("<html>\n");
 printf ("<head>\n");
 printf	("PiFunk Project\n");
 printf	("</head>\n");
 printf ("<body>\n");
 printf	("PiFunk - CGI\n");
 printf	("</body>\n");
 printf ("</html>\n");
}

void assistent () //assistent
{
		printf ("\nStarting assistent for setting parameters! \n");
		filecheck (filename, wavefile);
		samplecheck (filename, samplerate);
		modetype (freq);
		callname ();
		powerselect ();
		int gpioselect (int gpiopin);
		int dmaselect (int dmachannel);
		float bandwidthselect (float bandwidth);

		printf ("\nPress all information gathered, going back to main \n");
		//while (getchar () != '');

		return;
}

void menu ()
{
	printf ("\nChoose menu: [1] CMD // [2] CSV-Reader // [3] CGI-Page // [4] Exit: \n");
 	scanf ("%d", &menuoption);
	switch (menuoption)
	{
		case 0: printf ("\nShell - Commandline (main): \n");
						int main (int argc, char **argv);//, const char *short_opt); // go back to cmd if you want
						break;

		case 1: printf ("\nReading CSV for PMR... \n");
						csvreader ();
						break;

		case 3: printf ("\nReading cgi via text/html for homepage... \n");
						cgimodule ();
						break;

		case 4: printf ("\nExiting... \n");
						exit (0);

		default: printf ("\nMenu: Error! \n");
		 				 break;
	}
	return;
}

//--------- MAIN
int main (int argc, char **argv) // arguments for global use must! be in main! const char *short_opt
{
	const char *short_opt = "n:f:s:m:c:p:g:d:b:ahu"; // program flags
	int options = 0;
	argv [0] = "pifunk";
	char *filename = "sound.wav"; // = argv [1];
	float freq = fabs (446.006250); // =strtof (argv [2], NULL); //float only accurate to .4 digits idk why, from 5 it will round ?!
	int samplerate = abs (22050);// =atof (argv [3]); //maybe check here on != 22050 on 16 bits as fixed value (eventually allow 48k)
	char *mod = "fm";// =argv [4];
	char *callsign = "callsign";// =argv [5];
	int power = 7;// =argv [6];
	int dmachannel = 0; // =argv [7];
	float bandwidth = 15.00; // =argv [8];
	int gpiopin = abs (4); // =argv [9];
	/* atoll () is meant for integers & it stops parsing when it finds the first non-digit
	/ atof () or strtof () is for floats. Note that strtof () requires C99 or C++11
	abs () for int
	fabs () for double must be constant
	fabsf () for float
	*/
	// for custom programname, default is the filename itself
	void title ();
	printf ("\nArguments: %d / internal name: %s \n", argc, argv [0]);
	printf ("\nProgram name is %s \n", __FILE__);
	printf ("\nProgram was processed on %s at %s \n", __DATE__, __TIME__);
	printf ("\nshort_opt: %s \n", short_opt);
	infos (); //information, disclaimer
	int timer (time_t t); // date and time print

	while ((options = getopt (argc, argv, short_opt)) != -1) // short_opt must be constants
	{
		if (argc == 0)
		{
				fprintf (stderr, "\nArgument-Error! Use Parameters 1-6 to run: [-n <filename>] [-f <freq>] [-s <samplerate>] [-m <mod (fm/am)>] [-c <callsign (optional)>] [-p <power (0-7>]\nThere is also an assistent [-a] or for help [-h]! The *.wav-file must be 16-bit @ 22050 [Hz] Mono \n");
		}
		/*else
		{ */
		switch (options)
		{
			case 'n':
							filename = optarg;
							printf ("\nFilename is %s \n", filename);
							//break;

			case 'f':
							freq = atof (optarg);
							printf ("\nFrequency is %f \n", freq);
							//break;

			case 's':
							samplerate = atoi (optarg);
							printf ("\nSamplerate is %d \n", samplerate);
							samplecheck (filename, samplerate);
							break;

			case 'm':
							mod = optarg;
							if (!strcmp (mod, "fm"))
							{
								mod = optarg;
								printf ("\nPushing args to fm Modulator... \n");
							  //void modulationfm (int argc, char **argv); // idk if here to jump to the modulator or just parse it?!
								//break;
							}
							else if (!strcmp (mod, "am"))
							{
								printf ("\nPushing args to am Modulator... \n");
								//void modulationam (int argc, char **argv);
								//break;
							}
							else
							{
								printf ("\nError in -m \n");
								break;
								//return 1;
							}

			case 'c':
							callsign = optarg;
							printf ("\nCallsign is %s \n", callsign);
							//break;

			 //power managment
			case 'p':
							power = atoi (optarg);
							printf ("\nPower is %d \n", power);
							//break;

			case 'g':
								gpiopin = atof (optarg);
								printf ("\nGPIO-Pin is %d \n", gpiopin);
								//break;

			case 'd':
								dmachannel = atof (optarg);
								printf ("\nDMA-channel is %d \n", dmachannel);
								//break;

			case 'b':
								bandwidth = atoi (optarg);
								printf ("\nBandwidth is %f \n", bandwidth);
								//break;

			case 'a':
							if (argc == 1)
							{
								printf ("\nAssistent activated! \n");
								assistent ();
								break;
							}
							else
							{
								printf ("\nError in -a \n");
								break;
								//return 1;
							}

			case 'h':
							if (argc == 1)
							{
								printf ("\nHELP: Use Parameters to run: \n[-n <filename (*.wav)>] [-f <freq>] [-s <samplerate>] [-m <mod (fm/am)>] \n[-c <callsign (optional)>] [-p <power (0-7>]\nThere is also an assistent [-a] \n");
								break;
							}
							else
							{
								printf ("\nError in -h \n");
								break;
							}

			case 'u':
							if (argc == 1)
							{
									printf ("\nOpening menu \n");
									menu (); // extra menu for csv
									break;
							}
							else
							{
									printf ("\nError in -u (menu) \n");
									break;
							}

			default:
								printf ("\nArgument-Error! Use Parameters to run: \n[-n <filename>] [-f <freq>] [-s <samplerate>] [-m <mod (fm/am)>] \n[-c <callsign (optional)>] [-p <power (0-7>]\n There is also an assistent [-a] or for help [-h]! The *.wav-file must be 16-bit @ 22050 [Hz] Mono \n");
								return 1;
		} // end of switch

		break;
	} // end of while
 	//}//end of else
		//-- for debugging or information :)
	printf ("\n-----------------\n");
	printf ("\nChecking short_opt: %s \n", short_opt);
	printf ("\nChecking File: %s \n", filename);
	printf ("\nChecking Freq: %f [MHz] \n", freq);
	printf ("\nChecking Samplerate: %d [Hz] \n", samplerate);
	printf ("\nChecking Modulation: %s \n", mod);
	printf ("\nChecking Callsign: %s \n", callsign);
	printf ("\nChecking Output-Power: %d \n", power);
	printf ("\nChecking GPIO-Pin: %d \n", gpiopin);
	printf ("\nChecking DMA-channel: %d \n", dmachannel);
	printf ("\nChecking Bandwidth: is %f \n", bandwidth);
	printf ("\nChecking Hostname: %s, WAN+LAN-IP: %s, Port: %d \n", host, localip, port);
	printf ("\nChecking &Adresses: argc: %p / Name: %p / File: %p / Freq: %p \nSamplerate: %p / Modulation: %p / Callsign: %p / Power: %p / GPIO: %d \n", &argc, &argv [0], &filename, &freq, &samplerate, &mod, &callsign, &power, &gpiopin);
	printf ("\nChecking *Pointers-> argc: %p / Name: %p / File: %p / Freq: %p \nSamplerate: %p / Modulation: %p / Callsign: %p / Power: %p / GPIO: %p \n", argc, *argv [0], *filename, freq, samplerate, *mod, *callsign, power, gpiopin);
	printf ("\nChecking GPS-coordinates long: %f / lat: %f / alt: %f  \n", longitude, latitude, altitude);
/*
	printf ("\n*Pointers-> argc: %p / Name: %p / File: %p / Freq: %p \nSamplerate: %p / Modulation: %p / Callsign: %p / Power: %p / GPIO: %p \n", argc, *argv [0], *filename, freq, samplerate, *mod, *callsign, power, gpiopin);
	printf ("\n*Pointers-> argc: %p / Name: %p / File: %p / Freq: %p / Samplerate: %p / Modulation: %p / Callsign: %p / Power: %p / GPIO: %d \n", argc, *argv [0], *argv [1], *argv [2], *argv [3], *argv [4], *argv [5], *argv [6]);
	printf ("\nArguments: argc: %d / argv(0): %s / argv(1): %s \n argv(2): %f / argv(3): %d / argv(4): %s / argv(5): %s / argv(6): %d / GPIO: %d \n", argc, argv [0], argv [1], argv [2], argv [3], argv [4], argv [5], argv [6]);
	printf ("\n&Adresses-> argc: %p / Name: %p \nFile: %p / Freq: %p \nSamplerate: %p / Modulation: %p / Callsign: %p / Power: %p / GPIO: %d \n", &argc, &argv [0], &argv [1], &argv [2], &argv [3], &argv [4], &argv [5], &argv [6]);

	printf ("\n GPS-Module (Neo-7M) %gps  \n", gps);
	printf ("\nclient ip+port: %s:%d \n", inet_ntoa (client_addr.sin_addr), (int) ntohs (client_addr.sin_port));
	printf ("local ip+port: %s:%d \n", inet_ntoa (local.sin_addr), ntohs (local.sin_port));
*/
	// gathering and parsing all given arguments to parse it to player
	int tx (int argc, char **argv); //transmission

	printf ("\nEnd of Program! Closing... \n"); // EOF
	return 0;
}
