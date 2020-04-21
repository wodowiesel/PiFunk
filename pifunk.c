/* Program: PiFunk (C)
Copyright: 2018 - 2020
Author: D. W. a.k.a. WodoWiesel
version = 0.1.7.8e

----Licence----------------------------------------------------------------------------------------------
                    GPL v3 !!

-----Disclaimer------------------------------------------------------------------------------------------
- Rewritten for own purposes!

- Private Project! It's Early Access & Work in Progress (WIP)!

- I'm not a professional, so **NO guarantees or warranty** for any damage etc.!!

- Usage at **your own risk** !!

- Check laws of your country first! Some Frequencies (MHz) & Powerlevels (W / Watt) are prohibited or need at least a HAM-License!

  transmissions in EU only 0.4 mW ERP "effective radiated power" allowed

- Pi operates with square-waves (²/^2)!!

-> "dirty" transmissions (TX) simultaneously on permitted frequencies possible!

  Use Low-/High-Band-Pass-Filters !!

  with dry (not electrolytic) capacitors (C=10-100 pF (Farad)) with solenoid (ring),

  toroid (ferrite material) chokes with inductance (B=10-50 uH (Henry) like the FT37-43, depends on the Freq.)

  or resistors (R=~10 kOhm), diodes to prevent backflow

- You should always ground your antenna to prevent further damage!!

- at least use dummyloads with 50 Ohm @ max. 4 Watt (S=0 level) and compare signals with swr/pwr-meter when no antenna is attached!

- Do not shortout or do overstress it with more than 3.3V, it may cause damages!

  more infos about GPIO electronics https://de.scribd.com/doc/101830961/GPIO-Pads-Control2

- Access on ARM-System and running Linux, normaly on Raspberry Pi (my Pi Model B+ Rev. 1.2 2014)
  used python 3.7.4+ on original Raspbian

!!!!!!! program needs more testing on real pi !!!!!!!
--------------------------------------------------------------------------------------------------------
Setups & dependencies:
OS: Raspbian Buster - Kernel 4.19.97+ (01.04.2020) full incl. desktop & recommended software based on debian
don't forget to sudo apt-get update && upgrade

SHA-256: ac557f27eb8697912263a1de812dfc99fa8d69bd6acc73a0b7756a1083ba0176
-> get 3 different versions here: https://www.raspberrypi.org/downloads/raspbian/
-> or direct link: https://downloads.raspberrypi.org/raspbian_full_latest/

gcc >=9.2.0 compiler or g++>=5.4.1 for 11/14/17
gdb >=7.11.1 debugger

->get project:
git clone https://github.com/wodowiesel/PiFunk/

->instructions:
You will need "alsa" library for this:
sudo apt-get install libsndfile1-dev

sudo apt-get install libraspberrypi-dev raspberrypi-kernel-headers
-lbcm_host // firmware v1.20190718 located in /opt/vc/include/

cd PiFunk // goto path
// https://gcc.gnu.org/onlinedocs/gcc-4.9.2/gcc/Preprocessor-Options.html
// https://renenyffenegger.ch/notes/development/languages/C-C-plus-plus/GCC/options/index
-> -lm flag for math lib (obligatory), -g3 for debugger level, -c for not linkin to library
 => compile with admin/root permissions!!
 Libs: sudo gcc -g3 -std=gnu99 -Iinclude -Llib -L/opt/vc/lib -D_USE_MATH_DEFINES -D_GNU_C_SOURCE -lgnu -lm -lbcm_host -lbcm2835 -lsndfile -fPIC pifunk.c -O3 -o include/pifunk.i lib/pifunk.s lib/pifunk.o lib/pifunk.a lib/pifunk.so lib/pifunk.lib
 program: sudo gcc -g3 -std=gnu99 -Iinclude -D_USE_MATH_DEFINES -D_GNU_SOURCE -Llib -L/opt/vc/lib -lm -Llib -lbcm2835 -lsndfile -lgnu -lbcm_host -fPIC pifunk.c -O3 -o bin/pifunk bin/pifunk.out
 or do make (compile flags in make included)
 -lpifunk own lib from this project
 -D_POSIX_C_SOURCE=200809L // already in gnu_source included
 -std=c99 is the same as -std=iso9899:1999
 or -std=gnu99 supports c99 + additional gnu extensions
 or -std=c17 (11/14 also possible)
 -E tells to stop after preprocessing stage
 -v verbose
 `-a` keyword performs basic-block counting annotations for gdb & gprof
This option is supported for HP/UX compatibility.
The keyword argument must be one of the strings ‘archive’, ‘shared’, or ‘default’.
 ‘-aarchive’ is functionally equivalent to ‘-Bstatic’, and the other two keywords are functionally equivalent to ‘-Bdynamic’.
  This option may be used any number of times.
 `-Q` Print function names. Show statistics about compilation passes.

-> real gpio hardware can't be simulated by VM! must be executed and compiled on pi wtith linux!
As virtual machine best possible with Qemu
or rather not using it http://beta.etherpad.org/p/pihackfm/export/txt (old)

Usage:
default: sudo ./pifunk -n sound.wav -f 446.006250 -s 22050 -m fm -p 7 -c callsign
possible freq-shift of ~0.005 MHz -> 5kHz!!

1) Pi-FM version - frequency modulation direction left/right ← , →
2) Pi-AM version - amplitude modulation direction up/down ↑ , ↓
--> 700 MHz system clock of the pi1 -> please use heatsink + fan

todo:
memory-stuff
pointer & address corrections
make compatible arguments/funcs for py/shell scripts
*/

// std C includes
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
#include <iso646.h> // c95 back-compatible -std=iso9899:199409
#include <argp.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <utime.h>
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
#include <gnumake.h>
//#include <metrics.h>
//#include <missing.h>

// for c++99/11/14/17/20
/*
#include <iostream> //
#include <iomanip> //
#include <iosfwd> //
#include <ios> //
#include <istream> //

#include <stdalign> //
#include <stdnoreturn> //
#include <stdatomic> //
#include <cstdlib>	// General purpose utilities: program control, dynamic memory allocation, random numbers, sort and search
#include <cstdio> //
#include <cstdarg>	// Handling of variable length argument lists
#include <cstdint> // (since C++11)	fixed-size types and limits of other types
#include <csignal>	// Functions and macro constants for signal management
#include <cstring> //
#include <cmath>	// Common mathematics functions
#include <ctime>	// C-style time/date utilites
#include <climits>	// limits of integral types
#include <cfloat>	// limits of float types

#include <uchar> //
#include <threads> //
#include <algorithm> //
#include <vector> //
#include <fstream> //
#include <sstream> //
#include <ostream> //
#include <streambuf> //
#include <strstream> // depr c98
#include <syncstream> // c++20
#include <locale>	// Localization utilities
#include <clocale>	// C localization utilities
#include <codecvt> //
#include <regex> //
#include <atomic> //
#include <complex>	// Complex number type
#include <valarray>	// Class for representing and manipulating arrays of values
#include <random> // (since C++11)	Random number generators and distributions
#include <numeric>	// Numeric operations on values in containers
#include <ratio> // (since C++11)	Compile-time rational arithmetic
#include <cfenv> // (since C++11)	Floating-point environment access functions
#include <bit> // (since C++20)	Bit manipulation functions
#include <concepts> // (since C++20)	Fundamental library concepts
#include <coroutine> // (since C++20)	Coroutine support library
#include <csetjmp>	// Macro (and function) that saves (and jumps) to an execution context
#include <typeinfo>	// Runtime type information utilities
#include <typeindex> // (since C++11)	std::type_index
#include <type_traits> // (since C++11)	Compile-time type information
#include <bitset>	// std::bitset class template
#include <functional>	// Function objects, Function invocations, Bind operations and Reference wrappers
#include <utility>	// Various utility components
#include <chrono> // (since C++11)	C++ time utilites
#include <initializer_list> // (since C++11)	std::initializer_list class template
#include <tuple> // (since C++11)	std::tuple class template
#include <any> // (since C++17)	std::any class
#include <optional> // (since C++17)	std::optional class template
#include <variant> // (since C++17)	std::variant class template
#include <compare> // (since C++20)	Three-way comparison operator support
#include <version> // (since C++20)	supplies implementation-dependent library information
#include <source_location> // (since C++20)	supplies means to obtain source code location
#include <new>	// Low-level memory management utilities
#include <memory>	//Higher level memory management utilities
#include <scoped_allocator> // (since C++11)	Nested allocator class
#include <memory_resource> // (since C++17)	Polymorphic allocators and memory resources
#include <cinttypes> // (since C++11)	formatting macros , intmax_t and uintmax_t math and conversions
#include <limits>	// standardized way to query properties of arithmetic types
#include <exception>	// Exception handling utilities
#include <stdexcept>	// Standard exception objects
#include <cassert>	// Conditionally compiled macro that compares its argument to zero
#include <system_error> // (since C++11)	defines std::error_code, a platform-dependent error code
#include <cerrno>	// Macro containing the last error number
#include <cctype>	// Functions to determine the type contained in character data
#include <cwctype>	// Functions to determine the type contained in wide character data
#include <cstring>	// various narrow character string handling functions
#include <cwchar>	// various wide and multibyte string handling functions
#include <cuchar> // (since C++11)	C-style Unicode character conversion functions
#include <string>	// std::basic_string class template
#include <string_view> // (since C++17)	std::basic_string_view class template
#include <charconv> //(since C++17)	std::to_chars and std::from_chars
#include <format> // (since C++20)	Formatting library including std::format
#include <array> // (since C++11)	std::array container
#include <vector>	// std::vector container
#include <deque>	// std::deque container
#include <list>	// std::list container
#include <forward_list> // (since C++11)	std::forward_list container
#include <set>	// std::set and std::multiset associative containers
#include <map>	// std::map and std::multimap associative containers
#include <unordered_set> // (since C++11)	std::unordered_set and std::unordered_multiset unordered associative containers
#include <unordered_map> // (since C++11)	std::unordered_map and std::unordered_multimap unordered associative containers
#include <stack>	// std::stack container adaptor
#include <queue>	// std::queue and std::priority_queue container adaptors
#include <span> // (since C++20)	std::span view
#include <iterator> //
#include <ranges> // (since C++20)	Range access, primitives, requirements, utilities and adaptors
#include <execution> // (since C++17)	Predefined execution policies for parallel versions of the algorithms
#include <filesystem> // (since C++17)	std::path class and supporting functions
#include <forward_list> // (since C++11)	std::forward_list container
#include <concepts> // (since C++20)	Fundamental library concepts
#include <coroutine> // (since C++20)	Coroutine support library

using namespace std; //
*/

// on posix linux
#include <sys/cdefs.h>
#include <sys/time.h>
#include <sys/timex.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/file.h>
#include <sys/sysmacros.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <sys/poll.h>

// linux kernel driver headers: path /usr/src/linux-headers-4.19.97+/include/
#include <linux/types.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/reboot.h>
#include <linux/kd.h>
#include <linux/compiler.h>
/* used for bcm from header to std include /usr/include/linux/
#include "/usr/src/linux-headers-4.19.97+/include/linux/list.h"
#include "/usr/src/linux-headers-4.19.97+/include/linux/poison.h"
#include "/usr/src/linux-headers-4.19.97+/include/linux/kernel.h"
#include "/usr/src/linux-headers-4.19.97+/include/linux/const.h"
#include "/usr/src/linux-headers-4.19.97+/include/linux/preempt.h"
#include "/usr/src/linux-headers-4.19.97+/include/linux/linkage.h"
#include "/usr/src/linux-headers-4.19.97+/include/linux/compiler_types.h"
#include "/usr/src/linux-headers-4.19.97+/include/linux/stringify.h"
#include "/usr/src/linux-headers-4.19.97+/include/linux/export.h"
#include "/usr/src/linux-headers-4.19.97+/include/linux/asm/linkage.h"
#include "/usr/src/linux-headers-4.19.97+/include/linux/thread_info.h"
#include "/usr/src/linux-headers-4.19.97+/include/linux/bug.h"
#include "/usr/src/linux-headers-4.19.97+/include/linux/build_bug.h"
#include "/usr/src/linux-headers-4.19.97+/include/linux/restart_block.h"
#include "/usr/src/linux-headers-4.19.97+/include/linux/time64.h"
#include "/usr/src/linux-headers-4.19.97+/include/linux/math64.h"
#include "/usr/src/linux-headers-4.19.97+/include/uapi/linux/time.h"
#include "/usr/src/linux-headers-4.19.97+/include/linux/bitops.h"
#include "/usr/src/linux-headers-4.19.97+/include/linux/bits.h"
#include "/usr/src/linux-headers-4.19.97+/include/linux/irqflags.h"
#include "/usr/src/linux-headers-4.19.97+/include/asm-generic/bitops/_ffs.h"
#include "/usr/src/linux-headers-4.19.97+/include/asm-generic/cmpxchg-local.h"
#include "/usr/src/linux-headers-4.19.97+/include/linux/atomic.h"
*/

/*
#include <linux/init.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/cpu.h>
#include <linux/cpufreq.h>
#include <linux/cpumask.h>
#include <linux/cpu_cooling.h>
#include <linux/math64.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/notifier.h>
#include <linux/bcd.h>
#include <linux/interrupt.h>
#include <linux/completion.h>
#include <linux/platform_device.h>
#include <linux/of_platform.h>
#include <linux/pm_opp.h>
#include <linux/export.h>
#include <linux/sched/signal.h>
#include <linux/device.h>
#include <linux/ctype.h>
#include <linux/pm_runtime.h>
#include <linux/reset.h>
#include <linux/dma-mapping.h>
#include <linux/mailbox_client.h>
#include <linux/pm_domain.h>
#include <linux/textsearch.h>
#include <linux/config.h>
#include <linux/regulator/consumer.h>
*/

// I2C & SPI support need
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>

// FW
#include <soc/bcm2835/raspberrypi-firmware.h>
#include <drm/drm_fb_cma_helper.h>
#include <drm/drm_fb_helper.h>

// RTC support
#include <linux/rtc.h>
//#include <linux/rtc/ds1307.h>
#include <linux/rtc/ds3231.h>
#include "rtc/ds3231.h" // my rtc

// ip host socket
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <netdb.h>
#include <ifaddrs.h>

// windows (10) if needed for maybe rpi3/4 aarch64
/*
#include <windows.h>
#include <win.h>
#include <windef.h>
#include <winnt.h>
#include <winbase.h>
#include <conio.h> // dos-header
*/

// broadcom arm processor for mapping phys. addresses
#include <bcm_host.h>
#include <bcm2835.h> // -lbcm2835

#include "opt/vc/include/bcm_host.h" // firmware stuff
#include "opt/vc/include/interface/vcos/vcos.h" // Video Core OS Abstraction Layer
#include "bcm2709/src/bcm2709.h" // pi 1 & 2 A/A+ & B/B+ processor family
//#include "bcm2711/src/bcm2711.h" // pi 3 & 4 A/B coprocessor

// activate for your specific system
#include "bcm2835/src/bcm2835.h" // pi 0/1
//#include "bcm2836/src/bcm2836.h" // pi 2
//#include "bcm2837/src/bcm2837.h" // pi 3
//#include "bcm2838/src/bcm2838.h" // pi 4
//#include "bcm2838B0/src/bcm2838B0.h" // pi 4 B

// RPi.GPIO lib, 0.7.0 used with pi4 support
//#include "RPI.GPIO/source/i2c.h"
#include "RPI.GPIO/source/c_gpio.h"
#include "RPI.GPIO/source/event_gpio.h"
//#include "RPI.GPIO/source/py_pwm.h"
#include "RPI.GPIO/source/soft_pwm.h"
//#include "RPI.GPIO/source/constants.h"
#include "RPI.GPIO/source/common.h"
#include "RPI.GPIO/source/cpuinfo.h"

// see http://www.mega-nerd.com/libsndfile/api.html for API needed for am -> ALSA sound
// download from mainpage http://www.alsa-project.org/main/index.php/Main_Page
//#include <sndfile.h>
//#include "include/sndfile.h" // has problems!!! with @typedef sf_count somehow -> set as int

// extra library https://github.com/libusb/libusb
// for usb soundcards for mic and alsa usage
#include "libusb/libusb/libusb.h"
//#include "libusb/libusb.h"
//#include "libusb/libusb/libusbi.h"
//#include "libusb/libusb/hotplug.h"
#include "libusb/libusb/version.h"
#include "libusb/libusb/version_nano.h"
//#include "libusb/libusb/os/linux_usbfs.h"
#include "libusb/libusb/os/poll_posix.h"
#include "libusb/libusb/os/threads_posix.h"

// custom header for pifunk (definitions)
#include "include/pifunk.h"

//------------------------------------------------------------------------------
// preproccessor definitions
#ifndef _PIFUNK_C_
  #define _PIFUNK_C_
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
  //#pragma ARM
  // same as -CODE32
  //#error NOT ARM
#endif

#ifdef __ARM64__
  #warning Program runs under ARM64-Architecture!
  //#pragma ARM64
  //#error NOT ARM64
#endif

#ifdef __GNUC__
  #warning Using GNU C with ANSI ISO C99 as GNU99!
   //#pragma GCC system_header
#endif

#ifdef _GNU_SOURCE
  //#define basename __basename_gnu
   #warning Using GNU Source Macro!
#endif

#ifdef __CPLUSPLUS
  #warning Using GNU C++ with ANSI ISO C++ 11/17/20!
  extern "C"
  {
   printf ("\n__CPLUSPLUS \n");
  }
#endif

#ifdef _POSIX
#define _POSIX_C_SOURCE   		(200809L) // or 199309L
//#define _USE_MATH_DEFINES 1 // for math lm lib
#endif

#ifdef __STDC_VERSION__
   //#define _STDC_VERSION (199901L)  // -std=c99
   #warning Using GNU C with C99 standard!

#endif

//------------------------------------------------------------------------------
// definitions & Makros
#define VERSION 						 "0.1.7.8" // my version
#define VERSION_MAJOR        (0) //
#define VERSION_MINOR        (1) //
#define VERSION_BUILD        (7) //
#define VERSION_PATCH        (8) //
#define VERSION_STATUS 			 "experimental" // WIP work in progress

// simple operators
#define IN                    (0) //
#define OUT                   (1) //
#define FALSE                 (0) //
#define TRUE                  (1) //

// predefine if needed when not using bcm header
//#define HIGH 						   (0x1) // 1
//#define LOW 							 (0x0) // 0
//#define sleep 					   [1000] // for waiting between functions & tasks
#define usleep 							 [1000] //

// mathematical stuff
#define EULER                         (2.718281828459045235360287471352f) // log e(euler) = 0.4342944819
//#define log(EULER)                    (0.4342944819)
#define lg(EULER)                     (1.44269504089)
#define ln(x)                         (log(x)/log(EULER))
#define PI                            (3.14159265358979323846) // radial constant
#define PHASE                         (2*PI) // 6.28318530718
#define HALF_PERIOD                   (1/PI) // 0.31830988618
#define PERIOD                        (1/PHASE) // 0.15915494309

// buffers
#define PAGE_SIZE                     (4*1024) // 4096
#define BLOCK_SIZE                    (4*1024) // 4096
#define BUFFER_LEN                    (8*1024) // 8192
#define BUFFERINSTRUCTIONS            (65536) // [1024]

// I-O access via GPIO
volatile unsigned 										(*gpio); //
volatile unsigned 										(*allof7e); //

// GPIO setup macros: Always use INP_GPIO (x) before using OUT_GPIO (x) or SET_GPIO_ALT (x, y)
#define ALLOF7EB											(*allof7e-SUB_BASE)
#define GPIO_SET 											*(gpio+7)  // setsbits which are 1 ignores bits which are 0
#define GPIO_CLR 											*(gpio+10) // clears bits which are 1 ignores bits which are 0
#define GPIO_GET 											*(gpio+13) // sets bits which are 1 ignores bits which are 0

#define INP_GPIO(g)                   *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3)) // % means here Modulo-operation for remainder
#define OUT_GPIO(g)                   *(gpio+((g)/10)) |=  (1<<(((g)%10)*3)) //
#define SET_GPIO_ALT(g, a)            *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))


// specific pi adresses & definitions
// alternative old/different versions
#ifdef  RASPBERRY || RPI // and RPI == 1
#define PERIPH_VIRT_BASE               (0x20000000) // dec:536870912
#define PERIPH_PHYS_BASE               (0x7E000000) // dec:536870912
#define BCM2835_VIRT_BASE              (0x20000000) // dec:536870912
#define DRAM_PHYS_BASE                 (0x40000000) // dec: 1073741824
#define GPIO_BASE_OFFSET               (0x00200000) // dec: 2097152

#define MEM_FLAG                       (0x0C) // alternative
#define CURBLOCK                       (0x04) // dec: 4
#define CLOCK_BASE									   (19.2E6) //

#define DMA_CHANNEL										 (14) //
#define PLLD_FREQ 										 (500000000.) //

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

#define DMA_CHANNEL										 (14) //
#define PLLD_FREQ											 (500000000.) //
#endif

// pi 1 - BCM2835 -> my version
#ifdef  RASPI1 // == 1
#define PERIPH_VIRT_BASE               (0x20000000) // base=GPIO_offset dec: 2 virtual base
#define PERIPH_PHYS_BASE               (0x7E000000) // dec: 2113929216
#define BCM2835_VIRT_BASE              (0x20000000) // dec:536870912
#define DRAM_PHYS_BASE                 (0x40000000) // dec: 1073741824
#define GPIO_BASE_OFFSET               (0x00200000) // dec: 2097152

#define MEM_FLAG                       (0x0C) // alternative
#define CURBLOCK                       (0x0C) // dec: 12
#define CLOCK_BASE										 (19.2E6) //

#define DMA_CHANNEL										 (14) //
#define PLLD_FREQ											 (500000000.) //
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

#define DMA_CHANNEL										 (14) //
#define PLLD_FREQ 										 (500000000.) //
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

#define DMA_CHANNEL										 (14) //
#define PLLD_FREQ 										 (500000000.) //
#endif

// pi 4 - BCM2838
#ifdef  RASPI4 //== 4
#define PERIPH_VIRT_BASE               (0xFE000000) // dec: 4261412864
#define PERIPH_PHYS_BASE               (0x7E000000) // dec: 2113929216
#define GPIO_BASE_OFFSET               (0x7E215000) // GPIO register base address
#define BCM2838_PERI_BASE              (0x3F000000) // dec: 1056964608
#define BCM2711_PERI_BASE              (0x3F000000) // coprocessor !!!
#define DRAM_PHYS_BASE                 (0xC0000000) // dec: 3221225472

#define MEM_FLAG                       (0x04) // dec: 4
#define PAGE_SIZE 										 (4096) //
#define CLOCK_BASE									   (19.2E6) //
#define XTAL_CLOCK                     (54.0E6) // = 54000000

#define DMA_CHANNEL                    (14) // 4A
#define DMA_CHANNELB                   (7) // BCM2711 (Pi 4 B only)  chan=7
#define PLLD_FREQ 										 (750000000.) // has higher freq than pi 0-3

#define BUFFER_TIME 									 (1000000) //
#define PWM_WRITES_PER_SAMPLE 				 (10) //
#define PWM_CHANNEL_RANGE 						 (32) //
#endif

// standard & general definitions
#define PIN_7                           (4) // pin 4
#define GPIO_4                          (PIN_7)

#define PIN_40                          (40) // pin 40
#define GPIO_21                         (PIN_40)

#define GPIO_BASE                       (BCM2835_PERI_BASE + PERIPH_VIRT_BASE) // hex: 0x5F000000 dec: 1593835520
#define GPIO_BASE1                      (BCM2836_PERI_BASE + PERIPH_VIRT_BASE) // hex: 0x5F000000 dec: 1593835520
#define GPIO_BASE2                      (BCM2837_PERI_BASE + PERIPH_VIRT_BASE) // hex: 0x5F000000 dec: 1593835520
#define GPIO_BASE3                      (BCM2837B0_PERI_BASE + PERIPH_VIRT_BASE) // hex: 0x5F000000 dec: 1593835520
#define GPIO_BASE4                      (BCM2838_PERI_BASE + PERIPH_VIRT_BASE) // hex: 0x5F000000 dec: 1593835520

#define LENGTH                          (0x01000000) // dec: 1
#define SUB_BASE                        (0x7E000000) // dec: 2113929216 phys base

#define CLKBASE                         (0x7E101000) // dec: 2114981888

#define PWMBASE                         (0x7E20C000) // controller dec: 2116075520
#define MODULATE                        (0x4D72) // dec: 19826
#define FIFO                            (0x18)   // dec: 24
#define CARRIER                         (0x5A)   // dec: 90

#define DMABASE                         (0x7E007000) // dec: 2113957888
#define DMAREF                          (0x7F)   // dec: 127 dma base reference
#define DMAC                            (0x0707) // dec: 1799

#define PWMCLK_CNTL0                    (0x5A000016) // dec: 1509949462
#define PWMCLK_DIV0                     (0x5A002800) // dec: 1509959680

// possibility to give argv 0-4 a specific addresses or pointers
#define ARGC_ADR                        (0x7FFFFFFFEB0C) // dec: 140737488349964
#define NAME_ADR                        (0x7FFFFFFEC08)  // dec: 8796093017096
#define FILE_ADR                        (0x7FFFFFFFEC10) // dec: 140737488350224
#define FREQ_ADR                        (0x7FFFFFFFEC18) // dec: 140737488350232
#define SAMPLERATE_ADR                  (0x7FFFFFFFEC20) // dec: 140737488350240
#define MODULATION_ADR                  (0x7FFFFFFFEC28) // dec: 1407374883502484
#define CALLSIGN1_ADR                   (0x6052C0)       // dec: 6312640
#define CALLSIGN2_ADR                   (0x7FFFFFFFEAEF) // dec: 140737488349935
#define CALLSIGN3_ADR                   (0x7FFFFFFFEAE8) // dec: 140737488349928

// Pointers
#define FLOOR                           (0x0) // dec: 0

#define PWMADD1                         (0x4) // dec: 4
#define PWMADD2                         (0x8) // dec: 8

#define ARGC_PTR                        (0x5) // dec: 5
#define NAME_PTR                        (0x2F) // dec: 47
#define FILE_PTR                        (0x73) // dec: 115
#define FREQ_PTR                        (0x31) // $ means is in RDS data dec: 49
#define SAMPLERATE_PTR                  (0x32) // $ in RDS data  dec: 50
#define MODULATION_PTR                  (0x66) //  $ means isin RDS data // dec: 102
#define CALLSIGN_PTR                    (0x6D) // dec: 109

// the normal fm-script didn't specified that
#define DMA0_BASE_OFFSET                (0x00007000) // dec: 28672
#define DMA15_BASE_OFFSET 						  (0x00E05000) // dec: 14700544

#define TIMER_BASE_OFFSET 						  (0x00003000) // dec: 12288

#define PWM_BASE_OFFSET                 (0x0020C000) // dec: 2146304
#define PWM_LEN                         (0x28) // dec: 40

#define CLK_BASE_OFFSET                 (0x00101000) // dec: 1052672
#define CLK0_BASE_OFFSET 							  (0x00101070) // dec: 1052784
#define CLK1_BASE_OFFSET                (0x00101078) // dec: 1052792
#define CLK_LEN                         (0x1300) // dec: 4864

#define GPIO_LEN                        (0x100) // dec: 256

#define PCM_BASE_OFFSET                 (0x00203000) // dec: 2109440
#define PCM_LEN                         (0x24) // dec: 36

#define DMA_VIRT_BASE                   (PERIPH_VIRT_BASE + DMA0_BASE_OFFSET) //

#define PWMCLK_BASE_OFFSET              (0x001010A0) // dec: 1052832
#define PWM_VIRT_BASE                   (PERIPH_VIRT_BASE + PWM_BASE_OFFSET) //
#define PWM_PHYS_BASE                   (PERIPH_PHYS_BASE + PWM_BASE_OFFSET) //

#define CLK_VIRT_BASE                   (PERIPH_VIRT_BASE + CLK_BASE_OFFSET) //

#define GPIO_VIRT_BASE                  (PERIPH_VIRT_BASE + GPIO_BASE_OFFSET) //
#define GPIO_PHYS_BASE                  (PERIPH_PHYS_BASE + GPIO_BASE_OFFSET) //

#define PAD_VIRT_BASE                   (PERIPH_VIRT_BASE + PAD_BASE_OFFSET) //

#define PCM_VIRT_BASE                   (PERIPH_VIRT_BASE + PCM_BASE_OFFSET) //
#define PCM_PHYS_BASE                   (PERIPH_PHYS_BASE + PCM_BASE_OFFSET) //

// GPIO
// https://elinux.org/RPi_BCM2835_GPIOs
// https://books.google.de/books?id=gks1CgAAQBAJ&pg=PA407&lpg=PA407&dq=GPCLK2+address&source=bl&ots=OQkStH20YL&sig=ACfU3U2tp104Z3TTsrmU67Ai4L54JhF1uA&hl=de&sa=X&ved=2ahUKEwjw-uKyhZDnAhUCU1AKHcuRDMgQ6AEwAXoECBQQAQ#v=onepage&q=GPCLK2%20address
#define GPFSEL0                         (0x00/4) // p.90 dec: 0
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
// Ref: https://www.scribd.com/doc/101830961/GPIO-Pads-Control2
// Drive Strength (power 7 standard): 0 = 2 mA, 7 = 16 mA.

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
#define CM_PLLC                         (0x108/4) // 66
#define CM_PLLD                         (0x10C/4) // 67
#define CM_PLLH                         (0x110/4) // 68
#define CM_PLLB                         (0x170/4) // 92

#define CM_LOCK                         (0x114/4) // 69
#define CM_LOCK_FLOCKA                  (1<<8) //
#define CM_LOCK_FLOCKB                  (1<<9) //
#define CM_LOCK_FLOCKC                  (1<<10) //
#define CM_LOCK_FLOCKD                  (1<<11) //
#define CM_LOCK_FLOCKH                  (1<<12) //

/*
https://pinout.xyz/pinout/gpclk
SOURCES:
0     0 Hz     Ground
1     19.2 MHz oscillator
2     0 Hz     testdebug0
3     0 Hz     testdebug1
4     0 Hz     PLLA
5     1000 MHz PLLC (changes with overclock settings)
6     500 MHz  PLLD
7     216 MHz  HDMI auxiliary // not needed here in the program
8-15  0 Hz     Ground

clock-divider in the form of (SOURCE/(DIV_I + DIV_F/4096))
https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2835/BCM2835-ARM-Peripherals.pdf
Note, that the BCM2835 ARM Peripherals document contains an error and states that the denominator of the divider is 1024 instead of 4096.
Uses 3 GPIO pins */

#define A2W_PLLA_ANA0                   (0x1010/4) // 1028
#define A2W_PLLC_ANA0                   (0x1030/4) // 1036
#define A2W_PLLD_ANA0                   (0x1050/4) // 1044
#define A2W_PLLH_ANA0                   (0x1070/4) // 1052
#define A2W_PLLB_ANA0                   (0x10F0/4) // 1084

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
#define PWM_RNG1                        (0x10/4) //4
#define PWM_RNG2                        (0x20/4) //8

#define PWMDMAC_ENAB                    (1<<31)  // shift bit to left
#define PWMDMAC_THRSHLD                 ((15<<8)|(15<<0)) // this means it requests as soon as there is one free slot in the FIFO
// we want this as burst DMA would mess up our timing
// The deviation specifies the bandwidth of the signal: ~20.0 for WBFM (broadcasts) and ~3.5 for NBFM (walkie-talkie)
#define DEVIATION                       (12.50) // in kHz, a-pmr width normal analog
#define DEVIATION2                      (6.25) // d-pmr width digital
#define DEVIATION3                      (20.00) // dmr width
#define DEVIATION4                      (25.00) // dmr mixed
#define DEVIATION5                      (10.00) // CB width

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
// Technically 2708 is the family-chipname, and 2835/5/7 is a specific implementation for arm
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

// more DMA stuff
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

#define BUS_TO_PHYS(x)                  ((x)&~0xC0000000) // dec: 3221225472
#define ACCESS(PERIPH_VIRT_BASE)        (PERIPH_VIRT_BASE+ALLOF7EB) // volatile + int* volatile unsigned*
#define SETBIT(PERIPH_VIRT_BASE, bit)   ACCESS(PERIPH_VIRT_BASE) || 1<<bit // |=
#define CLRBIT(PERIPH_VIRT_BASE, bit)   ACCESS(PERIPH_VIRT_BASE) == ~(1<<bit) // &=

//
#define timerisset(tvp)        ((tvp)->tv_sec || (tvp)->tv_usec)
#define timerclear(tvp)        ((tvp)->tv_sec = (tvp)->tv_usec = 0)
#define timercmp(a, b, CMP)                                                  \
  (((a)->tv_sec == (b)->tv_sec) ?                                             \
   ((a)->tv_usec CMP (b)->tv_usec) :                                          \
   ((a)->tv_sec CMP (b)->tv_sec))

#define timeradd(a, b, result)                                                \
  do {                                                                        \
    (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;                             \
    (result)->tv_usec = (a)->tv_usec + (b)->tv_usec;                          \
    if ((result)->tv_usec >= 1000000)                                         \
      {                                                                       \
        ++(result)->tv_sec;                                                   \
        (result)->tv_usec -= 1000000;                                         \
      }                                                                       \
  } while (0)

#define timersub(a, b, result)                                               \
  do {                                                                        \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;                             \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;                          \
    if ((result)->tv_usec < 0)
    {                                                                         \
      --(result)->tv_sec;                                                     \
      (result)->tv_usec += 1000000;                                           \
    }                                                                         \
  } while (0)

// RTC (DS3231/DS1307 driver as bcm)
#define RTC_PWR                         (PIN_1) // +3.3 V
#define RTC_PWR2                        (PIN_4) // dec: 104 +5 V
#define RTC_GND                         (PIN_9) // RTC ground

#define RTC_DS3231_I2C_ADDRESS          (0x68) // dec: 104
#define DS3231_TEMPERATURE_MSB          (0x11) // dec: 17
#define DS3231_TEMPERATURE_LSB          (0x12) // dec: 18

#define DS1307_I2C_INPUT_ADDR           (0xD0) // read dec: 208
#define DS1307_I2C_OUTPUT_ADDR          (0xD1) // write dec: 209
// the same addresses
#define SLAVE_ADDR_WRITE                b(11010000) // binary -> dec:208, hex: 0xD0
#define SLAVE_ADDR_READ                 b(11010001) // dec:209, hex: 0xD1

// GPS ublox neo-7M pps
#define GPS_MODULE_NAME                 "GPS UBLOX NEO 8 M PPS" // module name
#define GPS_MODULE_VERSION              (8) // revision of the ublox model from 6-8+

// GND (PIN 6)
#define GPS_GND                         (PIN_6) // ground pin

// PPS to PCM_CLK (GPIO 18, PIN 12) or alternative PIN_24 and ESP32 on PIN_19
#define GPS_PPS                         (PIN_12) // pulse per second for locking GPS-timing

// RX to UART0_TXD (GPIO 14 PIN 8) crosswired
#define GPS_I2C_RX                      (UART0_TXD) // data in gps, receive

// TX to UART0_RXD (GPIO 15, PIN 10) crosswired
#define GPS_I2C_TX                      (UART0_RXD) // data out in gps, send

// LED
#define PIN_LED_GND                     (14) // which is the GND pin 27 for led
#define PIN_17                          (RPI_GPIO_P17) // which is the GPIO pin 17 for led1
#define PIN_27                          (RPI_GPIO_P27) // which is the GPIO pin 27 for led2

// try a modprobe of i2C-BUS
// if (system ("/sbin/modprobe i2c_dev" || "/sbin/modprobe i2c_bcm2835") == -1) {printf ("\nmodprobe test\n"); /*ignore errors*/}

//----------------------------------
// declaring normal variables

// program version status and default device
const char *description = "experimental - WIP"; // version-stage
const char *device = "default"; // playback device

// iterators for loops
int w = (0);
int m;
int i;
int k;
int l;
char j;
float x;

//pi memory-map peripherials:
char *gpio_mem;
char *gpio_map;

char *spi0_mem;
char *spi0_map;

unsigned bcm_host_get_peripheral_address (); // // This returns the ARM-side physical address where peripherals are mapped.
// This is (0x20000000) on the Pi Zero, Pi Zero W, and the first generation of the Raspberry Pi and Compute Module and (0x3F000000) on the Pi 2, Pi 3, 4 and Compute Module 3.
unsigned bcm_host_get_peripheral_size (); // This returns the size of the peripheral's space, which is (0x01000000) for all models.
unsigned bcm_host_get_sdram_address (); // This returns the bus address of the SDRAM.
// This is (0x40000000) on the Pi Zero, Pi Zero W, and the first generation of the Raspberry Pi and Compute Module (GPU L2 cached), and (0xC0000000) on the Pi 2, Pi 3, 4 and Compute Module 3 (uncached).

//-----------------------------------------
// arguments
int opt;
char *filename = "sound.wav";
float freq = fabs (446.006250);
double shift_ppm = (0.0);
float xtal_freq = (1.0/19.2E6); // LOCK_BASE
float subfreq = (67.0);
float ctss_freq = (67.0);
int samplerate = abs (22050);
int channels = 1;
uint32_t Timing;
char *mod; // = "fm"
char *fm = "fm";
char *am = "am";
int power = (7);
int powerlevel = abs (power);
char *callsign;
int type; // analog or digital
char *mod_type; // = "a"
char *analog = "a"; // type = 1
char *digital = "d"; // type = 2
float bandwidth;
int dmachannel;
int gpiopin;
char *gps;

float divider = (PLLD_FREQ / (2000 * 228 * (1.+shift_ppm/1.E6))); // 2000*228=456000 -> previously as int
uint32_t idivider = (float) divider;
uint32_t fdivider = (uint32_t) ((divider-idivider) * pow (2, 12));

// menu variables
int menuoption;
int channelnumbercb;
int channelnumberpmr;
int subchannelnumberpmr;
int channelmode;
int freqmode;
int modeselect;
int callnameselect;
int num;
time_t t;

// files
FILE *rfp, *wfp;
FILE FileFreqTiming;
FILE wavefile;
int MEM_FD = open ("/dev/mem", O_RDWR | O_SYNC | O_NONBLOCK);
//SNDFILE *infile;
//SNDFILE *outfile;
//snd_output_t *output = NULL;
int fp;// = STDIN_FILENO;
int filebit = abs (16); // for now 16 until i can read the value from an audio file
int readcount;
int readBytes;
float datanew = (0);
float dataold = (0);
float data [2*BUFFER_LEN];
float data_filtered [2*BUFFER_LEN];
char data_name [1024];
char buffer [80];

// audio & sample control
// logarithmic modulation
// volume in dB -> 0 db = unity gain, no attenuation, full amplitude signal
// -20 db = 10x attenuation, significantly more quiet
const float volume_reference =	(1.1f);
float volume = (1.1f);
float volbuffer [512];
float volumeLevelDb = (-6.f); // cut amplitude in half
float volumeMultiplier = (10E-1); // 10*10^-1 = 1

// samples max. 15.75 kHz resolution for AM / 14.50 kHz FM radio can be recorded
//SF_INFO sfinfo;
float samp1 = (15.75);
float samp2 = (14.50);
int nb_samples;
float timeconst = (50.0E-6); // 0.00005 = => 50 us (microseconds) time constant
int excursion = (6000);
int excursion2 = (32767); // found another value but dont know on what this is based on
float A = (87.6f); // compression parameter
uint32_t carrier_freq = (87600000); // this might be the carrier too, why this value?
float FactAmplitude = (1.0); // maybe here amp-modulator input?
float ampf;
float ampf2;
float factorizer;
float sampler;

// IQ & carrier http://whiteboard.ping.se/SDR/IQ
uint16_t pis = (0x1234); // dec: 4660
float angle = ((PHASE*freq)+shift_ppm); // A*cos(2pi*freq+phaseshift)
float I = FactAmplitude*cosf (angle); // real! In-Phase signal component
float Q = FactAmplitude*sinf (angle); // Quadrature signal component
float RF_SUM = (I+Q);
float ampl = sqrtf(((I*I)+(Q*Q)));

// instructor for access
unsigned long frameinfo;
int instrs [BUFFERINSTRUCTIONS]; // [1024]
int bufPtr = 0;
int instrCnt 	= 0;
int instrPage;
int constPage;

static volatile uint32_t *pad_reg;
static volatile uint32_t *pad_reg1;
static volatile uint32_t *pad_reg2;
static volatile uint32_t *pad_val;

pad_reg1 = pad_reg [GPIO_PAD_0_27]; // pi-gpio bank-row1
pad_reg2 = pad_reg [GPIO_PAD_28_45]; // pi-gpio bank-row2
/*
pad_val = (PADGPIO + power);
if ((pad_reg1 || pad_reg2) == pad_val) // check equality
{
  printf ("\npad_reg1 & pad_reg2 = pad_val -> %u / %u / %u \n", pad_reg1, pad_reg2, pad_val);
}
else
{
  printf ("\npad_reg are NOT the same -> %u / %u / %u \n", pad_reg1, pad_reg2, pad_val);
}
*/
static volatile uint32_t *pwm_reg;
static volatile uint32_t *clk_reg;
static volatile uint32_t *gpio_reg;
static volatile uint32_t *dma_reg;
int reg 	= 0; // = (gpio / 10)
int shift = 0; // = (gpio % 10) * 3
//gpio_reg [reg] = (gpio_reg [reg] & ~(7 << shift)); // alternative regshifter

// GPS-coordinates
// default Germany-Frankfurt (Main) in decimal °grad (centigrade) -> easier value to handle
char *position; // for live gps-module input later
char *long_pos = "E";
char *lat_pos = "N";
float longitude = (8.682127); // E
float latitude = (50.110924); // N
double elevation = (100.00); // meter
int altitude	= fabs (elevation); // elevation in meter above see level (u.N.N.)

// network sockets
// custom ip/port via tcp or udp
socklen_t addressLength;
char *localip = "127.0.0.1";
char *host 	= "localhost";
int port 	= (8080);
char *udp;
char *tcp;

const char *short_cw = ".";  // morse-code short beep
const char *long_cw = "-"; // morse-code long beep
char *message;

static const char *alpha [] =
{
  ".-",   // A
  "-...", // B
  "-.-.", // C
  "-..",  // D
  ".",    // E
  "..-.", // F
  "--.",  // G
  "....", // H
  "..",   // I
  ".---", // J
  "-.-",  // K
  ".-..", // L
  "--",   // M
  "-.",   // N
  "---",  // O
  ".--.", // P
  "--.-", // Q
  ".-.",  // R
  "...",  // S
  "-",    // T
  "..-",  // U
  "...-", // V
  ".--",  // W
  "-..-", // X
  "-.--", // Y
  "--..", // Z
  "----",  // CH

  // special chars
  "..--",  // :Ü = ue
  "---.",  // :Ö = oe
  "..-..", // Ê
  ".-.-",  // :A = ae
  ".--.-", // Â
  "..--.-", // ~N
  "*--**", // ]p (thorn)
  "**--*" // -D (eth)
};

static const char *numb [] =
{
  "-----", // 0
  ".----", // 1
  "..---", // 2
  "...--", // 3
  "....-", // 4
  ".....", // 5
  "-....", // 6
  "--...", // 7
  "---..", // 8
  "----.",  // 9

  // special chars
  ".-.-.-", // .
  "..--.-",  // -
  ".-.-.",  // +
  "-....-",  // ~
  "--..--", // ,
  "----.",  // ;
  "..--..", // ?
  "..--.", // !
  "---...", // :
  ".-..-.", // "
  ".----.", // '
  "-...-", // =
  "-..-.", // /
  "...-..-", // $
  ".-...",  // &
  ".----.", // |
  "-.--.", // (
  "-.--.-", // )
  "**--*-", // _
  ".--.-."  // @
};

static const char **table [] = {alpha, numb};

//--------------------------------------------------
// Structs
struct tm *info;
struct sockaddr_in localAddress;
struct client_addr sin_addr;
struct local sin_addr; // local.sin_addr;

struct PAGEINFO // should use here bcm intern funcs -> repair p/v
{
		void *p; // physical address BCMXXXX_PERI_BASE
		void *v; // virtual address
		int instrPage;
		int constPage;
		int instrs [BUFFERINSTRUCTIONS]; // [1024]
};

struct GPCTL // 9 parameters
{
		char SRC         : 4; //
		char ENAB        : 1; //
		char KILL        : 1; //
		char IDK1        : 1; // what is the blank char? gave it a dummyname (IDK) for now
		char BUSY        : 1; //
		char FLIP        : 1; //
		char MASH        : 2; //
		unsigned int IDK2 : 13; // what is the blank int?
		char PASSWD      : 8; //
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

// program flag options
static struct option long_opt [] =
{
		{"filename",		required_argument, 0, 'n'}, // 1
		{"freqency",   	required_argument, 0, 'f'}, // 2
    {"samplerate", 	required_argument, 0, 's'}, // 3
    {"mod",       	required_argument, 0, 'm'}, // 4
    {"power", 			required_argument, 0, 'p'}, // 5
    {"callsign",	  required_argument, 0, 'c'}, // 6
    {"gpiopin",	  	required_argument, 0, 'g'}, // 7
		{"dmachannel",	required_argument, 0, 'd'}, // 8
		{"bandwidth",	  required_argument, 0, 'b'}, // 9
    {"type",	  		required_argument, 0, 't'}, // 10
    {"gps",	  		  required_argument, 0, 'x'}, // 11
    {"assistant",		no_argument,       0, 'a'}, // 12
    {"menu",	  		no_argument,       0, 'u'}, // 14
    {"help",	  		no_argument,       0, 'h'},  // 13
    {0,             0,                 0,  0 }
};

//----------------------------------------------------
// basic functions specified one after another
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
    printf ("\nWelcome to the Pi-Funk! v %s %s for Raspbian ARM! \n", VERSION, description);
   	printf ("\nRadio works with *.wav-file with 16-bit @ 22050 [Hz] Mono / 1-700.00000 MHz frequency \nUse '. dot' as decimal-comma seperator! \n");
    printf ("\nPi operates with square-waves (²/^2) PWM on GPIO 4 (PIN 7 @ ~500 mA & max. +3.3 V). \nUse power supply with enough specs only! \n=> Use Low-/Highpassfilters and/or ~10 uF-cap, isolators or resistors if needed! \nYou can smooth it out with 1:1 balun. Do NOT shortcut, use a dummyload instead! \nCheck laws of your country! \n");
    printf ("\nFor testing (default settings) run: sudo ./pifunk -n sound.wav -f 100.0000 -s 22050 -m fm -c callsign -p 7 \n");
		printf ("\nDevicename: %s \n", device);
    printf ("\nxtal_freq: %f \n", xtal_freq);
 		return;
}

int timer (time_t t)
{
	 time (&t);
   //info = localtime (&rawtime);
	 //strftime (buffer, 80, "%x - %I:%M%p", info);
   printf ("\nCurrent formated date & time: %s \n", ctime (&t));
   return (0);
}

//-----------------------------------------
// program functions
int gpioselect ()
{
  // how to change pin-config on boot
  // https://www.raspberrypi.org/documentation/configuration/pin-configuration.md
	printf ("\nPlease choose GPIO-Pin (GPIO 4 = PIN 7 -> default) or GPIO 21 = PIN 40, alternatives: 20, 29, 32, 34, 38 (not recommended) \n");
  scanf ("%d", &gpiopin);
	printf ("\nYour GPIO for transmission is %d ... \n", gpiopin);
  if (gpiopin == 4)
  {
    printf ("\nUsing default GPIO 4 \n");
    // set PIN value
  }
  else if (gpiopin == 21)
  {
    printf ("\nUsing GPIO 21, mostly used for Pi 4 \n");
    //
  }
  else if (gpiopin == 20 || 29 || 32 || 34 || 38)
  {
    printf ("\nUsing alternative GPIO setup ... \n");
    //
  }
  else
  {
    printf ("\nError: not recognized! Using default GPIO 4 \n");
    gpiopin = 4;
  }
	return gpiopin;
}

int dmaselect ()
{
	printf ("\nPlease choose the DMA-Channel: 7 (Pi 4) / 14 (default) / 255 (off): \n");
  scanf ("%d", &dmachannel);
	printf ("\nThe DMA-Channel is %d ... \n", dmachannel);
  if (dmachannel == 255)
  {
    printf ("\nThe DMA-Channel is deactivated! \n");
    dmachannel = 255;
    //DMA_CHANNEL = 255;
  }
  else if (dmachannel == (7)
  {
    printf ("\nThe DMA-Channel is activated! \n");
    dmachannel = 7;
    //DMA_CHANNEL = 7;
  }
  else if (dmachannel == (14)
  {
    printf ("\nThe DMA-Channel is activated! \n");
    dmachannel = 14;
    //DMA_CHANNEL = 14;
  }
  else
  {
    printf ("\nThe DMA-Channel not recognized, using default 14! \n");
    dmachannel = 14;
  }
	return dmachannel;
}

int filecheck (char *filename)  // expected int
{
  printf ("\nPlease enter the full path including name of the *.wav-file you want to use: \n");
  //scanf ("%s", &filename);
	printf ("\nTrying to play %s ... \n", filename);
	printf ("\nOpening file ... \n");
	printf ("\nAllocating filename memory ... \n");
	char *filename = (char *) malloc (128); // allocating memory for filename
	sprintf (filename, "%s", "file.ft");
	char *stdfile = "sound.wav";
  if (filename != stdfile)
	{
    int fp = open (filename, O_RDONLY | O_CREAT | O_WRONLY | O_TRUNC | O_NONBLOCK, 0644); // O_RDWR
	}
	else
	{
	  int fp = open ("sound.wav", O_RDONLY | O_CREAT | O_WRONLY | O_TRUNC | O_NONBLOCK, 0644); // sounds/sound.wav directory should be tested
	}
	return fp;
}

float freqselect () // gets freq by typing in
{
	printf ("\nYou selected 1 for Frequency-Mode \n");
	printf ("\nType in Frequency (0.1-1200.00000 MHz): \n"); // 1B+ for 700 MHz chip, pi3 1.2 GHz pi4
	scanf  ("%f", &freq);
	printf ("\nYou chose: %f MHz \n", freq);
  return freq;
}

float audiovolume () // audio & freq stuff
{
	float datavalue = (data [i] * 4 * volume); // modulation index (AKA volume) logarithmic hearing of human
  printf ("\ndatavalue: %f, SAMPLES_PER_BUFFER: %d \n", datavalue, SAMPLES_PER_BUFFER);
	for (i = 0; i < SAMPLES_PER_BUFFER; ++i)
	{
     volbuffer [i] *= volumeMultiplier;
     printf ("\nValues-> i: %d, volbuffer: %f, volumeMultiplier: %f \n", i, volbuffer [i], volumeMultiplier);
		 printf ("\nAdresses.> i: %p, volbuffer: %p, volumeMultiplier: %p \n", &i, &volbuffer [i], &volumeMultiplier);
     return volbuffer [i];
	}
	return volumeMultiplier;
}

float bandwidthselect ()
{
	printf ("\nChoose Bandwidth-Steps: 6.25 / 10.00 / 12.50 (default) / 20.00 / 25.00 kHz: \n");
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
  return bandwidth;
}

float channelmodepmranalog ()
{
  printf ("\nChoose analog PMR-Channel 1-17 (18 to exit): \n");
  scanf ("%d", &channelnumberpmr);

  switch (channelnumberpmr)
   {
   // Analog & DMR
   case 0: freq=446.00000; printf ("\naPMR-Chan 1 on %f \n", freq); break;	// base
   case 1: freq=446.00625; printf ("\naPMR-Chan 1 on %f \n", freq); break;	// Standard
   case 2: freq=446.01875; printf ("\naPMR-Chan 2 on %f \n", freq); break; // Geocaching
   case 3: freq=446.03125; printf ("\naPMR-Chan 3 on %f \n", freq); break; // Standard
   case 4: freq=446.04375; printf ("\naPMR-Chan 4 on %f \n", freq); break; // at 3-chan-PMR-devices its ch. 2
   case 5: freq=446.05625; printf ("\naPMR-Chan 5 on %f \n", freq); break; // Contest
   case 6: freq=446.06875; printf ("\naPMR-Chan 6 on %f \n", freq); break; // Events
   case 7: freq=446.08125; printf ("\naPMR-Chan 7 on %f \n", freq); break; // at 3-channel-PMR-devices it's ch. 3
   case 8: freq=446.09375; printf ("\naPMR-Chan 8 on %f \n", freq); break; // Standard

  // Digital only
  // dmr (tier 1) digital new since 28.09.2016
  // extra 8 chan
  // 12.5 kHz steps
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
 return freq;
}

float channelmodepmrdigital ()
{
  printf ("\nChoose ditigal PMR-Channel 1-32 (33 to exit): \n");
	scanf ("%d", &channelnumberpmr);

	switch (channelnumberpmr)
	 {
   // FD-PMR 6.25 kHz steps  & for DCDM devices: CC1 TG99 TS1 = Contact, CC1 TG9112 TS1 = EmCOM
   case 1:	freq=446.000000; printf ("\ndPMR-Chan 1 on %f \n", freq); break;
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
	 // normally up to 32 chan in dpmr
	 case 33: 		printf ("\nExit ... \n"); exit (0);

	 default:			freq=446.003125;
	 							printf ("\nDefault channelnumber = 1 on freq = %f \n", freq);
								break;
	 }

   printf ("\ndigital-freq is %f \n", freq);
   return freq;
}

// Channel-mode
int channelmodepmr () // PMR
{
	printf ("\nChoose PMR-Type (1) analog / (2) digital : \n");
	scanf ("%d", &type);

  if (type==1)
  {
    printf ("\nYou chose type (1) analog  \n");
    channelmodepmranalog ();
  }
  else if (type==2)
	{
    printf ("\nYou chose type (2) digital \n");
    channelmodepmrdigital ();
  }
  else
	{
    type=1;
		printf ("\nNO type could be determined, wrong input! Using %s as standard \n", analog);
	}
  printf ("\nOn type = %d with Channelnumber = %d on freq = %f \n", type, channelnumberpmr, freq);
	return freq;
}

float subchannelmodepmr () // Pilot-tone
{
	printf ("\nChoose Sub-Channel 0-38 (39 to exit): \n");
	scanf ("%d", &subchannelnumberpmr);

	switch (subchannelnumberpmr)
	{
	 // FYI 19 (38)-kHz-Pilottone on UKW
	 // Analog & digital
	 case 0:	subfreq=67.000; printf ("\nCTSS-Chan 0, default base on %f \n", subfreq); break;	// Scan all Chan till active , now chan0 base
	 case 1:  subfreq=67.900; printf ("\nCTSS-Chan 1 on %f \n", subfreq); break;	// 4.90 Hz steps
	 case 2: 	subfreq=71.900; printf ("\nCTSS-Chan 2 on %f \n", subfreq); break;
	 case 3: 	subfreq=74.400; printf ("\nCTSS-Chan 3 on %f \n", subfreq); break;
	 case 4: 	subfreq=77.000; printf ("\nCTSS-Chan 4 on %f \n", subfreq); break; // at 3-chan-PMR-devices it's ch. 2
	 case 5: 	subfreq=79.700; printf ("\nCTSS-Chan 5 on %f \n", subfreq); break; // Contests
	 case 6: 	subfreq=82.500; printf ("\nCTSS-Chan 6 on %f \n", subfreq); break; // Events
	 case 7: 	subfreq=85.400; printf ("\nCTSS-Chan 7 on %f \n", subfreq); break; // at 3-channel-PMR-devices it's ch. 3
	 case 8: 	subfreq=88.500; printf ("\nCTSS-Chan 8 on %f \n", subfreq); break; // Standard opening chan
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

	 case 39: printf ("\nExit ... \n"); exit (0);
	 default: subfreq=67.900;
						printf ("\nDefault subchannel = 1 on subfreq = %f \n", subfreq);
						break;
	}
  printf ("\nSubchannelnumber = %d on subfreq = %f \n", subchannelnumberpmr, subfreq);
	return subfreq;
}

float channelmodecb () // CB
{
	printf ("\nChoose CB-Channel 0-80 (81 to exit): \n");
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

       /*		 difference for neighbourchannels not 10 kHz, but 20 kHz steps
			 this channels for CB are in morst countries NOT permitted!!
			 e.g. for purposes like remote controls, toys, Babyphones, wireless mouse, keyboards etc */
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

       // 80 chan devices
			 // On national extra-chan 41 - 80 is only modulation-type FM permitted!
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

			default:		freq=26.9650;
									printf ("\nDefault CB chan = 1 on %f MHz \n", freq);
									break;
	}
  printf ("\nUsing channel = %d on freq = %f \n", channelnumbercb, freq);
	return freq;
}

char modulationselect ()
{
	printf ("\nChoose your Modulation [1] FM // [2] AM // [3] Exit : \n");
	scanf ("%d", &freqmode);

	switch (freqmode)
	{
		case 1: printf ("\nYou selected 1 for FM! \n");
						mod = "fm";
		        break;

		case 2: printf ("\nYou selected 2 for AM! \n");
						mod = "am";
		        break;

		case 3: printf ("\nExiting... \n");
						exit ();

		default:	mod = "fm";
						  printf ("\n Default = 1 \n");
						  break;
	}
	return mod;
}

char callsignselect ()
{
    //if (*callsign == NULL) {
		printf ("\nYou don't have specified a callsign yet! \nPress (1) for custom or (2) default 'callsign': \n");
		scanf ("%d", &callnameselect);
		switch (callnameselect)
	  {
	   case 1: printf ("\nType in your callsign: \n");
						 scanf  ("%s", &callsign);
						 printf ("\nYour callsign is: %s \n", callsign);
						 break;

		 case 2: callsign = "callsign"; // default callsign
						 printf ("\nUsing default callsign: %s \n", callsign);
						 break;

		 default: callsign = "callsign"; // default callsign
		 					printf ("\nError! Using default callsign: %s \n", callsign);
							break;
    }
  	return callsign;
}

int powerselect ()
{
	printf ("\nType in powerlevel (0-7 from 2-16 mA): \n");
	scanf ("%d", &powerlevel);
	printf ("\nPowerlevel was set to: %d \n", powerlevel);
  power = abs (powerlevel);
	return power;
}

int channelselect ()
{
	printf ("\nYou selected 1 for Channel-Mode \n");
	printf ("\nChoose your Band: [1] PMR // [2] CB \n");
  scanf  ("%d", &channelmode);

  switch (channelmode) // from here collecting infos and run it step by step, same for freq-mode
  {
					case 1: printf ("\nPMR CHAN-MODE \n");
									channelmodepmr (); // gets freq from pmr list
									break;

		   		case 2: printf ("\nCB CHAN-MODE \n");
									channelmodecb ();
									break;

					default:  printf ("\nDefault: PMR CHAN-MODE \n");
										channelmodepmr (); // gets freq from pmr list
									  break;
	}
	return channelmode;
}

int typeselect ()
{
  if (!strcmp (type, "1" || "analog"))
  {
    printf ("\nUsing analog mode \n");
    channelmodepmranalog ();
  }
  else if (!strcmp (type, "2" || "digital"))
  {
    printf ("\nUsing digital mode \n");
    channelmodepmrdigital ();
  }
  else
  {
    printf ("\nError in -t \n");
  }
  return type;
}

int modetypeselect ()
{
	printf ("\nChoose Mode: [1] Channelmode // [2] Frequencymode \n");
	scanf ("%d", &modeselect);
	switch (modeselect)
	{
		case 1:	 	printf ("\n[1] Channelmode: \n");
							channelselect ();
							break;

		case 2:		printf ("\n[2] Frequencymode: \n");
							freqselect ();
							break;

		default: printf ("\nError! Using [1] (default) Channelmode! \n");
             channelselect ();
						 break;
	}
	return modeselect;
}

char gpsselect () // char *gps
{

  if (gps == "on")
  {
  printf ("\nGPS-position is %s \n", *position); // live input here from gps-module
  print ("\nPreset location is: long_pos %s longitude %f / lat_pos %s latitude %f / elevation %lf / altitude %d \n", long_pos, longitude, lat_pos, latitude, elevation, altitude);
  }
  else if (gps == "off")
  {
  printf ("\nGPS-Status is off! you can activate it with flag: -x on \n");
  }
  else
  {
    printf ("\nError: Input not recognized! NOT using GPS \n");
    gps == "off";
  }
  printf ("\nGPS-Status is %s \n", gps);
  return gps;
}

//---------------
// Voids for modulation and memory handling

void modulate (int l)
{
	printf ("\nModulate carrier ... \n");
	ACCESS(CM_GP0DIV) == ((CARRIER << 24) + (MODULATE + l));  //
  return;
}

void getRealMemPage (void *vAddr, void *pAddr) // should work through bcm header!
{
		void *m = valloc (4096);

		((int*) m) [0] = (1); // use page to force allocation

		mlock (m, 4096); // lock into ram

		*vAddr = m; // we know the virtual address now

		int fp = open ("/proc/self/pagemap", O_RDONLY | O_NONBLOCK); // "w"
		lseek (fp, ((int) m)/4096*8, SEEK_SET);
		read (fp, &frameinfo, sizeof (frameinfo));

		*pAddr = (void*) ((int) (frameinfo*4096));
    fprintf ("\nCould not map memory! \n");
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
// Added functions to enable and disable carrier GPCTL has 9 parameters but here 7 used ?
// Set CM_GP0CTL.ENABLE to 1 HIGH (2nd number) as 0x5A -> CARRIER dec: 90
  struct GPCTL setupword = {6, 1, 0, 0, 0, 1, 0x5A}; // set clock to 1 = HIGH
  ACCESS (CM_GP0CTL) = *((int*) &setupword); // setting cm
  while (!(ACCESS(CM_GP0CTL)&0x80)); // Wait for busy flag to turn on.

  printf ("\nCarrier is high ... \n");
  return;
}

void carrierlow () // disables it
{
	printf ("\nSetting carrier low ... \n");
  struct GPCTL setupword = {6, 0, 0, 0, 0, 1, 0x5A}; // 6 = "SRC", set it to 0 = LOW
  ACCESS (CM_GP0CTL) = *((int*) &setupword);
  while (ACCESS(CM_GP0CTL)&0x80); //

  printf ("\ncarrier is low ... \n");
  return;
}

void handSig () // exit func
{
		printf ("\nExiting ... \n");
		exit ();
}

static void terminate (int num)
{
    // Stop outputting and generating the clock
    if (clk_reg && gpio_reg && vAddr)
    {
        // Set GPIO4 to be an output (instead of ALT FUNC 0, which is the clock)
        gpio_reg [GPFSEL0] = (gpio_reg [GPFSEL0] & ~(7 << 12)) | (1 << 12);
        printf ("\ngpio_reg is %u \n", gpio_reg);
        // Disable the clock generator
        clk_reg [GPCLK_CNTL] = (0x5A);
        printf ("\nclk_reg is %u \n", clk_reg);
    }

    if (dma_reg && vAddr)
    {
        dma_reg [DMA_CS] = BCM2708_DMA_RESET;
        printf ("\ndma_reg is %u \n", dma_reg);
        //udelay (10);
    }

     //fm_mpx_close ();
     //close_control_pipe ();

    if (vAddr != NULL)
    {
        unmapmem (vAaddr, (NUM_PAGES * 4096));
      //  mem_unlock (mbox.handle, mbox.mem_ref);
      //  mem_free (mbox.handle, mbox.mem_ref);
    }

    printf ("\nTerminating: cleanly deactivated the DMA engine and killed the carrier. Exiting \n");

    exit (num);
}

void usleep2 (long us)
{
  nanosleep ((struct timespec []) { {0, us*1000} }, NULL); //
  return;
}

void delayMicrosecondsHard (unsigned int howLong)
{
  struct timeval tNow, tLong, tEnd;

  gettimeofday (&tNow, NULL) ;
  tLong.tv_sec  = howLong / 1000000;
  tLong.tv_usec = howLong % 1000000;
  timeradd (&tNow, &tLong, &tEnd);

  while (timercmp (&tNow, &tEnd, <))
  {
    gettimeofday (&tNow, NULL);
  }
return;
}

void setupio ()
{
  printf ("\nSetting up FM ... \n");

  struct sched_param sp;
  memset (&sp, 0, sizeof (sp));
  sp.sched_priority = sched_get_priority_max (SCHED_FIFO);
  sched_setscheduler (0, SCHED_FIFO, &sp);
  mlockall (MCL_CURRENT | MCL_FUTURE);

  // open /dev/mem
  if (MEM_FD < 0)
	{
        printf ("\nCan't open /dev/mem ! \n"); // via bcm possible
        return;
  }

  // Allocate MAP block
	if ((gpio_mem = malloc (BLOCK_SIZE + (PAGE_SIZE-1))) == NULL)
  {
		printf ("\nAllocation error \n");
		exit ();
	}

	// Make sure pointer is on 4K boundary
	if ((unsigned long) gpio_mem % PAGE_SIZE)
	    gpio_mem += PAGE_SIZE - ((unsigned long) gpio_mem % PAGE_SIZE);

	// Now map it
	  gpio_map = (unsigned char *) mmap (
		gpio_mem,
		BLOCK_SIZE,
		PROT_READ | PROT_WRITE,
		MAP_SHARED | MAP_FIXED,
		MEM_FD,
		GPIO_BASE);

	if ((long) gpio_map < 0)
  {
	    printf ("\nmmap error %d \n", (int) gpio_map);
	    exit ();
  }

   gpio = (volatile unsigned *) gpio_map;

	 carrierhigh ();
   return;
}

void setupfm ()
{
  allof7e = (unsigned*) mmap (
                NULL,
                BCM_HOST_GET_PERIPHERAL_SIZE, // Peripherial LENGTH
                PROT_READ|PROT_WRITE, //
                MAP_SHARED, //
                MEM_FD, //
                PERIPH_VIRT_BASE); // PERIPH_VIRT_BASE, std = 0x20000000

  if ((int) allof7e == -1)
  {
    exit ();
  }

   SETBIT (GPFSEL0, 14);
   CLRBIT (GPFSEL0, 13);
   CLRBIT (GPFSEL0, 12);

  struct GPCTL setupword = {6, 1, 0, 0, 0, 1,0x5A};
	ACCESS (CM_GP0DIV) = (0x5A << 24) + divider;
	ACCESS (CM_GP0CTL) = *((int*) &setupword);

  return;
}

void sendByteAsk (unsigned char byte, int sleep)
{
	for (j = 0; j < 8; j++)
  {
		if ((byte&(1 << j)) > 0)
    {
			carrierlow ();
			delayMicrosecondsHard (sleep);
			usleep2 (sleep);
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

// relevant features for transmitting stuff
void play_list () // exit func
{
		printf ("\nOpening playlist-folder (dummy) \n"); // in sounds/playlist
    // searching for all soundfiles in folder and counting and making a list and order
    return;
}

void play_fm () // char *filename, float freq, int samplerate
{
/*  wiki https://en.wikipedia.org/wiki/WAV
	  https://en.wikipedia.org/wiki/44,100_Hz
    NTSC: 44056 Hz
    245 × 60 × 3 = 44100
    245 active lines/field × 60 fields/second × 3 samples/line = 44100 samples/second
    (490 active lines per frame, out of 525 lines total)

    PAL: 294 × 50 × 3 = 44100
    294 active lines/field × 50 fields/second × 3 samples/line = 44100 samples/second
    (588 active lines per frame, out of 625 lines total)
*/

  // after getting filename insert then open
	printf ("\nAllocating file to memory for wave-data ... \n");

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

        float frac = ((dval-intval)/2+0.5);
				printf ("\nfrac: %f \n", frac);

        int fracval = (frac*clocksPerSample);
				printf ("\nfracval: %d \n", fracval);

        bufPtr++;
        // problem still with .v & .p endings for struct!!
        //while (ACCESS (DMABASE + CURBLOCK & ~ DMAREF) == (int) (instrs [bufPtr].p) ); // CURBLOCK of struct PageInfo
        usleep2 (1000); // leaving out sleep for faster process

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
  printf ("\ndma_reg is %u \n", dma_reg);
	//atexit (unsetupDMA);
	signal (SIGINT,  handSig);
	signal (SIGTERM, handSig);
	signal (SIGHUP,  handSig);
	signal (SIGQUIT, handSig);

	// allocate a few pages of ram
  //getRealMemPage (&constPage.v, &constPage.p);
	int centerFreqDivider = (int) ((500.0/freq) * (float) (1<<12) + 0.5);
	printf ("\ncenterFreqDivider %d \n", centerFreqDivider);
	// make data page contents - its essientially 1024
	// different commands for the DMA controller to send to the clock module at the correct time
	for (int i=0; i<1024; i++)
	{
	   // ((int*) (constPage.v))[i] = (CARRIER << 24) + centerFreqDivider - 512 + i;
	}

	while (instrCnt < 1024) // BUFFERINSTRUCTIONS
	{
     //getRealMemPage (&instrPage.v, &instrPage.p);

     // make copy instructions
  	 //struct CB* instr0 = (struct CB*)instrPage.v;

     for (int i = 0; i < (4096/sizeof (struct CB)); i++)
     {
         /*
         instrs[instrCnt].v = (void*) ((int) instrPage.v + sizeof(struct CB)*i);
         instrs[instrCnt].p = (void*) ((int) instrPage.p + sizeof(struct CB)*i);
         instr0->SOURCE_AD = (unsigned int) constPage.p + 2048;

         instr0->DEST_AD = PWMBASE + (FIFO); //fifo
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
	struct DMAREGS* DMA0 = (struct DMAREGS* ACCESS(DMABASE) );
	//DMA0->CS = 1<<31; // reset dma controller
	printf ("\nUnsetting DMA done \n");

	exit ();
}

// sample funcs

// all subch. -> base/default case 0 -> channel 0
// if subchannels is 0 = all ch. then check special stuff -> maybe scan func ?
// squelch/treshhold to build in maybe -> scan function till signal?

int sampleselect () // char *filename, int samplerate
{
	printf ("\nSamplerate/bit select starting \n");

	/*
  if (!(fp = open (filename, SFM_READ, &sfinfo))) // check what SFM and  sfinfo do!?
  {   // Open failed so print an error message.
        printf ("\nNot able to open input file for sampleselect %s \n", filename);
				printf ("\nNot able to open filepointer for sampleselct %d \n", fp);
        // Print the error message from libsndfile.
        return (1);
  }

	//sfinfo.samplerate = samplerate;
	if (sfinfo.samplerate == 22050) // 44 or 48 kHz needs testing
	{
		printf ("\nSamplerate is 22050! (%d) \n", sfinfo.samplerate);
		return sfinfo.samplerate;
	}
	else if (sfinfo.samplerate == 14500)
	{
		printf ("\nSamplerate is 14500! (%d) \n", sfinfo.samplerate);
		return sfinfo.samplerate;
	}
	else
  {
	printf ("\nInput samplerate must be at least 22.050 kHz for FM or 14.500 kHz for AM! \n");
	return (1);
}
*/

	// check filebits here somehow (via SFM)
	if (filebit != 16)
	{

		printf ("\nError: Input must be 16 bit! \n");
		return (1);
	}
	else
	{

		printf ("\nInput is 16 bit! \n");
		return (0);
	}

	// While there are frames in the input file, read them,
	// process them and write them to the output file
	//----------------------
  nb_samples = (readcount/channels);
  printf ("\nnb_samples: %d \n", nb_samples);

  while (readcount == read (fp, data, BUFFER_LEN))
  {
	 // where to input the freq like in fm
	  for (k = 0; k < nb_samples; k++)
	  {
		  char *b = data [k*channels];
			printf ("\nChannel buffer b = %c \n", b);
			if (channels == 0)
			{
				printf ("\nSample Error! NO (0) channels \n"); // > 1 in stereo or dual mono with half samplerate
        return (-1);
			}
			else if (channels == 1)
			{
				printf ("\n File has %d channel (MONO)! \nReading ... \n", channels);
				// stereo file, avg left + right --> should be mono at 22.050 kHz
				b += data [k*channels+1];
				b /= 2; // maybe *2 to make a dual mono and not doing stereo in half!
				printf ("\nb = %c \n", b);
			}
			else if (channels == 2)
			{
        printf ("\nFile has 2 Channels (STEREO)! maybe supported later! \n"); // >1 in stereo or dual mono with half samplerate
        int halfsamplerate = (samplerate/2);
        printf ("\nUsing half of the samplerate: %d \n", halfsamplerate);
			}
			else
			{
					printf ("\nError: File has %d Channels! channels > 2 not supported yet! \n", channels);
          return (-1);
			}

 			// was defined as global var above
			printf ("\nCompression parameter A: %f \n", A);

			// maybe here am option for amplitude factor input!?
			printf ("\nFactamplitude: %f \n", FactAmplitude);

			ampf = (x/excursion2);
			printf ("\nampf: %f \n", ampf);

  		ampf2 = (fabs (ampf) < 1.0f/A) ? A*fabs (ampf)/(1.0f+ln (A)):(1.0f+ln (A*fabs (ampf)))/(1.0f+ln (A)); //compand
			printf ("\ncompand ampf2: %f \n", ampf2);

			x = (int) (round (ampf2*excursion2));
			printf ("\nnew x: %f \n", x);

		  factorizer = (x*excursion2*FactAmplitude);
			printf ("\nfactorizer: %f \n", factorizer);

			sampler = (1E9/samplerate); // 44.100
			printf ("\nsampler: %f \n", sampler);

			//return channels, ampf, ampf2, x, factorizer, sampler;
	  } // for loop
		printf ("\nEnding readcount ... \n");
  } // while loop
    // Close input and output files
    //fclose (FileFreqTiming);
  close (fp);
  printf ("\nFile closed! \n");
	return samplerate;
}
// return freqmode, channels, ampf, ampf2, x, factorizer, sampler;

// AM
void play_am ()
{
	float Frequencies = freq;
	typedef struct
	{
		float Frequency;
		uint32_t WaitForThisSample;
	} samplerf_t;

	samplerf_t RfSample;
	RfSample.Frequency = Frequencies;
	RfSample.WaitForThisSample = Timing; // in 100 of nanoseconds
	printf ("\nFreq = %f, Timing = %d \n", RfSample.Frequency, RfSample.WaitForThisSample);
	if (write (fp, &RfSample, sizeof (samplerf_t)) != sizeof (samplerf_t))
	{
		fprintf (stderr, "\nUnable to write sample! \n");
	}
	printf ("\nWriting tone in am \n");
  return;
}

void modulationfm () // int argc, char **argv [], char *mod
{
  	printf ("\nPreparing for fm ... \n");
    setupfm (); // gets filename & path or done by filecheck () func
	  printf ("\nSetting up DMA ... \n");
		setupDMA (); // (argc>2 ? atof (argv [2]):100.00000); // default freq
    printf ("\nfm modulator starting ... \n");
    play_fm (); // atof (argv [3]):22050)
		return;
}

void modulationam () // int argc, char **argv [], char, *mod
{
	/* {IQ (FileInput is a mono wav contains I on left channel, Q on right channel)}
		{IQFLOAT (FileInput is a Raw float interlaced I, Q)}
		{RF (FileInput is a (float) Frequency, Time in nanoseconds}
		{RFA (FileInput is a (float) Frequency, (int) Time in nanoseconds, (float) Amplitude}
		{VFO (constant frequency)} */
		printf ("\nam modulator starting \n");
		play_am (); // actual modulation stuff here for am -> write tone? maybe better name later
	  return;
}

void modselect () // int argc, char **argv [], char *mod
{
	printf ("\nOpening Modulator-Selection ... \n");
	if (strcmp (mod, "fm"))
	{
    printf ("\nYou selected 1 for fm! \n");
    printf ("\nPushing args to fm Modulator ... \n");
		modulationfm (); //int argc, char **argv []
	}
	else if (strcmp (mod, "am"))
	{
    printf ("\nYou selected 2 for am! \n");
    printf ("\nPushing args to am Modulator ... \n");
		modulationam (); // int argc, char **argv []
	}
	else
	{
    printf ("\nError in -m selecting modulation! \n");
	}
 	return;
}

/*
// pi4 pin fix for under 93 MHz
class ClockOutput : public ClockDevice
{
        public:
        #ifndef GPIO_21
        ClockOutput (unsigned divisor) : ClockDevice (CLK0_BASE_OFFSET, divisor)
        {
            output = reinterpret_cast<uint32_t *>(peripherals->GetVirtualAddress (GPIO_BASE_OFFSET));
            *output = (*output & 0xFFFF8FFF) | (0x04 << 12);
        #else
        ClockOutput(unsigned divisor) : ClockDevice (CLK1_BASE_OFFSET, divisor)
        {
            output = reinterpret_cast<uint32_t *>(peripherals->GetVirtualAddress (GPIO_BASE_OFFSET + 0x08));
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

char cw ()
{
  sprintf ("\n%s\n", table);
  printf ("\nStd-cw: short_cw: %s, long_cw: %s \n Type in your CW-message: \n", long_cw, short_cw); // morse beeps
  scanf ("%s", &message);
  long int length_m = strlen (message); // size_t,  strlen -> long int

  printf ("\nMessage: %s, length: %ld \n", message, length_m); // length unsigned int %zu

  // tone translation
/*
  while (message [w] != NULL) // Stop looping when we reach the NULL-character "\0"
  {
    if (message [w] == short_cw)
    {
      printf ("\a"); // system beep terminal, maybe beep-function later
    }
    else if (message [w] == long_cw)
    {
      printf ("\a\a");
    }
    else
    {
      printf ("\nMessage error \n");
    }

    printf ("%c", message [w]);  // Print each character of the string
    w++;
  }
*/
  return message;
}

// LED stuff
// controlling via py possible but c stuff can be useful too by bcm funcs!
// turn on LED (with 100 kOhm pullup resistor while transmitting
void ledinactive ()
{
	  // check if transmitting
    printf ("\nChecking transmission status ... \n");
		while (!(void play_fm () )) // || play_am ()))
		{
				//cm2835_gpio_write (PIN_17, LOW);
				printf ("\nLED off - No transmission! \n");
		}
    return;
}

void ledactive ()
{
// initialize bcm
/*
  bcm2835_set_debug (1);
  if (!bcm2835_init ())
	{
		printf ("\nBCM 2835 init failed! \n");
  	return (1);
	}
	else if (1)
	{
    // Set the pin to be an outputannels
    // bcm2835_gpio_fsel (PIN_17, BCM2835_GPIO_FSEL_OUTP);
  	printf ("\nBCM 2835 init done and PIN 4 activated \n");
    // LED is active during transmission
		while (play_fm ()) // char *filename, float freq, int samplerate
		{
			// Turn it on
			bcm2835_gpio_write (PIN_17, HIGH);
			printf ("\nLED on - transmission ... \n");
			// wait a bit
			bcm2835_delay (100);
		}
	}
	else // if no transmission than turn it off // (ledactive != 0)
  {
		cm2835_gpio_write (PIN_17, LOW);
		printf ("\nLED off - No transmission \n");
	}
   //bcm2835_close ();
	 */
	printf ("\nLED active \n");
  return;
}

//--------------------
// read / import csv for pmr
char csvreader ()
{
    printf ("\nChecking CSV-file for CTSS-Tones (Coded Tone Control Squelch System) ... \n");
		printf ("\nOrder of the list: \nLocation, Name, Frequency, Duplex, Offset, Tone, rToneFreq, cToneFreq, DtcsCode, DtcsPolarity, Mode, TStep, Skip, Comment, URCALL, RPT1CALL, RPT2CALL \n");

    rfp = fopen ("ctsspmr.csv", "r"); // read-only!
    wfp = fopen ("ctsswriter.csv", "w+"); // with + it updates, if exists overwrites
    while (!feof (rfp))
    {
    	// here check for semicolon or comma delimiter (default)
    	j = fgetc (rfp);
    	fputc (j, wfp);
    }
		printf ("\n%d\n", j);
    fclose (rfp);
    fclose (wfp);
    printf ("\nCSV-import of CTSS-list finished! \n");
    return j;
}

void cgimodule () //
{
 printf ("\ncontext-type:text/html\n");
 printf ("\n<html>\n");
 printf ("\n<head>\n");
 printf	("\nPiFunk-Project\n");
 printf	("\n</head>\n");
 printf ("\n<body>\n");
 printf	("\nPiFunk-CGI\n");
 printf	("\n</body>\n");
 printf ("\n</html>\n");
 return;
}

void tx () // int argc, char **argv []
{
  printf ("\nPreparing for transmission ... \n");
	while (play_fm ()) // | play_am ())
  {
  ledactive ();
  printf ("\nBroadcasting now! ... \n");
  }
	return;
}

void menu ()
{
	printf ("\nChoose menu: [1] CMD // [2] CSV-Reader // [3] CGI-Page // [4] Exit: \n");
 	scanf ("%d", &menuoption);
	switch (menuoption)
	{
		case 0: printf ("\nShell - Commandline (main): \n");
						int main (int argc, char **argv []); //, const char *short_opt); // go back to cmd if you want
						break;

		case 1: printf ("\nReading CSV for PMR ... \n");
						csvreader ();
						break;

    case 2: printf ("\nStarting CW-Morsecode mode ... \n");
        		cw ();
        		break;

		case 3: printf ("\nReading CGI via text/html for homepage ... \n");
						cgimodule ();
						break;

		case 4: printf ("\nExiting ... \n");
						exit (0);

		default: printf ("\nMenu: Default \n");
             int main (int argc, char **argv []);
		 				 break;
	}
	return;
}

void assistant () // assistant
{
		printf ("\nStarting assistant for setting parameters! \n");
		filecheck (filename);
		sampleselect (); // filename, samplerate
		modetypeselect ();
    modselect ();
    typeselect ();
		powerselect ();
    callsignselect ();
		gpioselect ();
		dmaselect ();
		bandwidthselect ();
    gpsselect ();
		printf ("\nAll information gathered, parsing & going back to main! \n");
    //printf ("Press Enter/return to continue ... \n");
		//while (getchar () != ""); // unsigned char, waiting for return/enter hit, \n = \012, \r return cursor to left

		return;
}

//---------------------------------------------
// MAIN
int main (int argc, char **argv []) // , const char *short_opt
// arguments for global use must be in main!
{
  printf ("\nStarting Main-PiFunk \n");
  // option parameters
  const char *short_opt = "n:f:s:m:p:c:g:d:b:t:x:auh"; // program flags
	//char **argv [0] = "pifunk"; // actual program-name
  char *programname = argv [0]; //
	char *filename = "sound.wav"; // = argv [1]; n=name
	float freq = fabs (446.006250); // = strtof (argv [2], NULL); // float only accurate to .4 digits idk why, from 5 it will round ?!
	int samplerate = abs (22050); // = atof (argv [3]); // maybe check here on != 22050 on 16 bits as fixed value (eventually allow 48k)
	char *mod = "fm"; // = argv [4];
  int power = (7); // = argv [5];
	char *callsign = "callsign"; // = argv [6];
  int gpiopin = abs (4); // = argv [7];
	int dmachannel = (14); // = argv [8];
	float bandwidth = (12.50); // = argv [9];
  int type = (1); // = argv [10]; analog -> default
  char *gps = "on"; // = argv [11]; -> default: off
  // menues
  char *a; // = argv [12];
  char *h; // = argv [13];
  char *u; // = argv [14];
	/* atoll () is meant for integers & it stops parsing when it finds the first non-digit
	/ atof () or strtof () is for floats. Note that strtof () requires C99 or C++11
	abs () for int
	fabs () for double must be constant
	fabsf () for float
	*/
	// for custom program-name, default is the filename itself
	printf ("\nArguments: %d / internal name: %s \n", argc, argv [0]);
	printf ("\nProgram name is %s, FILE: %s \n", programname, __FILE__);
	printf ("\nProgram was processed on %s at %s \n", __DATE__, __TIME__);

	void infos (); // information, disclaimer
	int timer (time_t t); // date and time print

  bcm_host_get_peripheral_address ();
  bcm_host_get_peripheral_size ();
  bcm_host_get_sdram_address ();

  int option_index = (0);
  int options = getopt (argc, argv, short_opt); // short_opt must be constant
  int flags = getopt_long (argc, argv, short_opt, long_opt, &option_index);

  printf ("\n-----------------\n");
	printf ("\nChecking short_opt: %c \n", short_opt);
  printf ("\nChecking options: %d \n", options);
  printf ("\nChecking options: %d \n", option_index);
  printf ("\nChecking flags long : %d \n", flags);
  printf ("\nChecking long_opt: %s \n", long_opt[option_index].name);

	while (options != -1 || 0) // if -1 then all flags were read, if ? then unknown
	{
		if (argc == 0)
		{
				fprintf (stderr, "\nArgument-Error! Use Parameters 1-11 to run: [-n <filename>] [-f <freq>] [-s <samplerate>] [-m <mod (fm/am)>] [-p <power (0-7>] g d b t x \n[-c <callsign (optional)>] \nThere is also an assistant [-a] or for help [-h] or menu [-u]!\n The *.wav-file must be 16-bit @ 22050 [Hz] Mono \n");
        return (-1);
    }
		else
		{
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
							 //break;

			case 'm':
							 mod = optarg;
               printf ("\nMod is %s \n", mod);
               //break;

      case 'p':
         				power = atoi (optarg);
         				printf ("\nPower is %d \n", power);
         				//break;

			case 'c':
							  callsign = optarg;
							  printf ("\nCallsign is %s \n", callsign);
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

      case 't':
                type = atof (optarg);
                printf ("\nType is %d \n", type);
                //break;

      case 'x':
               gps = optarg;
               printf ("\nGPS-Tracking-Status is %s \n", gps);
               //break;

     // additional help functions
			case 'a':
							 if (argc == 1)
							 {
								printf ("\nAssistant activated! \n");
								void assistant ();
								break;
							 }
							 else
							 {
								printf ("\nError in -a \n");
								break;
							 }

      case 'u':
         		   if (argc == 1)
         			 {
         				 printf ("\nOpening menu \n");
         				 void menu (); // extra menu for csv
         				 break;
         				}
         				else
         				{
         					printf ("\nError in -u menu \n");
         					break;
         				}

			case 'h':
							 if (argc == 1)
							 {
								printf ("\nHELP: Use Parameters to run: \n[-n <filename (*.wav)>] [-f <freq>] [-s <samplerate 22050>] [-m <mod (fm/am)>] [-p <power (0-7)>] \n[-c <callsign>] [-g <GPIO-pi (7)>] [-d <DMA-channels (14)>] [-b <bandwidth (15)>] [-t <type a/d>] [-x <GPS on/off>]\nThere is also an assistant [-a], or menu [-u] or for help [-h]  \n");
								break;
							 }
							 else
							 {
								printf ("\nError in -h help\n");
								break;
							 }

      case '?':
                  printf ("Unknown option: %c \n", optopt);
                  break;

			default:
								printf ("\nArgument-Error! Use Parameters to run: \n[-n <filename>] [-f <freq>] [-s <samplerate>] [-m <mod (fm/am)>] [-p <power (0-7>] \n[-c <callsign (optional)>] [-g GPIO-pin] [-d DMA-channels] [-b bandwidth] [-t <type a/d>] [-x <GPS on/off>]\n There is also an assistant [-a], menu [-u] or for help [-h]! The *.wav-file must be 16-bit @ 22050 [Hz] Mono \n");
								break;
		} // end of switch

		break;
	} // end of while

  } // end of else

	// for debugging or information
	printf ("\n-----------------\n");
	printf ("\nChecking File: %s \n", filename);
	printf ("\nChecking Freq: %f [MHz] \n", freq);
	printf ("\nChecking Samplerate: %d [Hz] \n", samplerate);
	printf ("\nChecking Modulation: %s \n", mod);
	printf ("\nChecking Callsign: %s \n", callsign);
	printf ("\nChecking Output-Powerlevel: %d \n", power);
	printf ("\nChecking GPIO-PIN: %d \n", gpiopin);
	printf ("\nChecking DMA-channel: %d \n", dmachannel);
	printf ("\nChecking Bandwidth: %f [Hz] \n", bandwidth);
	printf ("\nChecking Type: is %d \n", type);  // 1/analog, 2/digital:
  printf ("\nangle: %f \n", angle);
  printf ("\nI-value: %f \n", I);
  printf ("\nQ-value: %f \n", Q);
  printf ("\nRF-SUM (I+Q): %f \n", RF_SUM);
  printf ("\nAmplitude-value: %f \n", ampl);
  printf ("\nChecking GPS-Status: %s \n", gps);
  printf ("\nChecking GPS-coordinates: long: %f / lat: %f / alt: %d \n", longitude, latitude, altitude);
	printf ("\n-----------------\n");
	printf ("\nChecking Hostname: %s, WAN/LAN-IP: %s, Port: %d \n", host, localip, port);
  printf ("\nshort_cw: %s \n", short_cw); // morse beeps
  printf ("\nlong_cw: %s \n", long_cw); //
  //printf ("\nChecking &Adresses: argc: %p / Name: %p / File: %p / Freq: %p \nSamplerate: %p / Modulation: %p / Callsign: %p / Power: %p / GPIO: %d \n", &argc, &argv [0], &filename, &freq, &samplerate, &mod, &callsign, &power, &gpiopin);
	//printf ("\nChecking *Pointers-> argc: %p / Name: %p / File: %p / Freq: %p \nSamplerate: %p / Modulation: %p / Callsign: %p / Power: %p / GPIO: %p \n", argc, *argv [0], *filename, freq, samplerate, *mod, *callsign, power, gpiopin);

	//printf ("\nclient ip+port: %s:%d \n", inet_ntoa (client_addr.sin_addr), (int) ntohs (client_addr.sin_port));
	//printf ("local ip+port: %s:%d \n", inet_ntoa (local.sin_addr), ntohs (local.sin_port));

	// gathering and parsing all given arguments it to player
  printf ("\nTransmission starting ... \n"); // EOF
	void tx (); // int argc, char **argv [] transmission
  printf ("\nTransmission ended! \n");
  static void terminate (int num);

	printf ("\nEnd of Program! Closing ... \n"); // EOF
	return (0);
}
