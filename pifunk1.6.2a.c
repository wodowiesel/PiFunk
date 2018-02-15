// to run and compile to binary use:

//gcc -lm -std=c99 pifunk.c -o pifunk with admin/root permissions!!

//=> real gpio hardware can't be simulated by c or py code! must be executed and compiled on linux 
// virtual mashine possible with qemu
// or alternative with everpad: nor sure about this, rather not using it 
// wget -o -http://beta.etherpad.org/p/pihackfm/export/txt >/dev/null | gcc -lm -std=c99 -g -xc - && ./a.out sound.wav

/*
gcc 5.4.1 compiler + gdb 7.11.1 debugger (online & local)
g++ 5.4.1 c++11(or 14)
//=> compile with mingw-x64 win 10



-----Disclaimer-----
Rewritten for own purpoes! 
no guarantee, waranty for anything! Usage at own risk!
you should ground your antenna, eventually diode or 10uF-caps 
for dummyloads 50 ohm @ min 4 watts , max 100 

Access on ARM-System !!! Running Linux, mostly on Raspberry Pi (me B+ rev.2)
used python 2.7.x & 3.6.x on orig. Raspian
don't forget to apt-get upgrade and update

1) Pi-FM version - freqency modulation direction left/right ← , → 
2) Pi-AM version - amplitude modulation direction up/down ↑ , ↓

-->> 500 MHz (max) system clock of the pi -> pls use heatsink (if you want with fan)

todo: 
pointer & adress corrections 
make compatible py/shell script with args & mic & mp3
github link+release package
maybe 1 try ncurses for menus
*/

// normal includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>

#include <math.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <setjmp.h>
#include <limits.h>
#include <float.h>
#include <locale.h>
#include <malloc.h>
#include <dirent.h>
#include <conio.h>
#include <pthread.h>
#include <fenv.h> 
#include <inttypes.h>
#include <tgmath.h>
#include <complex.h>

#include <wchar.h>
#include <wctype.h>
#include <iso646.h>//c95


// on posix linux
#include <features.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> 

//-- c11 only
#include <stdalign.h>
#include <uchar.h>
#include <stdnoreturn.h>
#include <stdatomic.h>

//for c++
//#include <iostream.h>
//#include <threads.h>
//using namespace std; 

// see http://www.mega-nerd.com/libsndfile/api.html for API
// needed for am -> ALSA sound
//#include <sndfile.h> 
// or try with " " directly

//extra libary https://github.com/libusb/libusb
//#include </usr/include/libusb/libusb.h>
//#include <libusb.h>

//#include <bcm2835.h> // broadcom

// windows if needed
/*
#include <windows.h>
#include <win.h>
#include <windef.h>
#include <winnt.h>
#include <winbase.h>
*/

//python stuff, maybe wrapper too??
//---------------------------------------------------------------//

// I-O access
volatile unsigned *gpio;
volatile unsigned *allof7e;
//----
#define	ln(x) log(x)/log(2.718281828459045235f)
#define PI 3.14159265

#define IN 0
#define OUT 1
#define LOW 0
#define HIGH 1

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)
#define	BUFFER_LEN (8*1024)
#define BUFFERINSTRUCTIONS 65536

// GPIO setup macros: Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // setsbits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0
#define GPIO_GET *(gpio+13) // sets bits which are 1 ignores bits which are 0

#define length    (0x01000000)
#define base      (0x20000000) // base=GPIO offset (0x200000).
#define ADR       (0x7E000000)
#define CM_GP0CTL (0x7E101070) //p.107
#define GPFSEL0   (0x7E200000) //p.90
#define CM_GP0DIV (0x7E101074) //p.108
#define CLKBASE   (0x7E101000)
#define DMABASE   (0x7E007000)
#define PWMBASE   (0x7E20C000) // PWM controller 
#define BCM2836_PERI_BASE (0x3F000000) // register physical address
#define GPIO_BASE (BCM2836_PERI_BASE + base) 

#define ACCESS(base) (volatile int*)(base+(int)allof7e-ADR)
#define SETBIT(base, bit) ACCESS(base) || 1<<bit //   |=
#define CLRBIT(base, bit) ACCESS(base) && ~(1<<bit) // &=


//--------------------------------------------------//
//pi variables:  -> need to be activated and pulled up with python-script, or automaticly by system

int   mem_fd;
char *gpio_mem, *gpio_map;
char *spi0_mem, *spi0_map;

// programm variables
float freq;
int samplerate;
int i;
int k;
float x;
int channels;
int FileFreqTiming;
int volume, gain;
int filebit;

char *filename;
FILE infile;

int timer ()
{
   time_t rawtime;
   struct tm *info;
   char buffer [80];

   time (&rawtime);

   info = localtime (&rawtime);
   printf ("Current local time and date: %s \n", asctime (info));

   return 0;
}

int infos ()
{
    printf ("Welcome to the Pi-Funk Tool! v0.1.x a (experimental) for Raspian ARM \n");
	printf ("Radio works for 22,5 kHz Mono / 1-500 MHz \n");
    //Here some Warnings to implement and infos
    printf ("Pi oparates with square-waves PWM on GIO4 (Pin 7 @ ~500mA) \n=> Use Low-/Highpassfilters or ~10uF-caps! Check laws of your Country! \n"); // i try to smooth it out with 1:1 baloon
    return 0;
}

char filenamepath ()
{
    printf ("\nPlease Enter the Full Path including Name of the .wav file you want to use: \n");
    scanf ("%s", filename);
   
    if (filename == NULL)
	{
	    printf ("%s File not found! \n", filename);
	    return 1;
	}
	else
	{
	   printf ("Trying to play default sound.wav ... \n");
	   *filename = open ("sound.wav", "r");
	   return filename;
	    
	}
	return 0;
}

int freqselect () // gets freq by typing
{
   
	//printf ("\n*** You selected 1 for Frequency-Mode *** \n"); 
	printf ("Type in Frequency (1-500.00000 MHz): ");
	scanf ("%f", freq);
	printf ("You chose %f MHz \n", freq);
    return freq;
}

int channelselect ()
{
	printf ("*** You selected 1 for Channel-Mode *** \n"); 
	printf ("Choose your Type [1] PMR // [2] CB // [3] Exit : ");
    int channelmode;
    scanf ("%d ", &channelmode);
    
    switch (channelmode) // from here collecting infos and run it step by step, same for freq-mode
            {
            case 1: printf ("*** PMR CHAN-MODE im FM *** "); 
					int channelmodepmr (); // gets freq from pmr list
					filenamepath ();
					int modulationfm (int argc, char **argv);
					break;
					
		    case 2: printf ("*** CB CHAN-MODE SELECT *** "); 
					int channelmodecb (); // gets freq for chan
					filenamepath (); //gets file
					int modulationselect (); //selects modulation
					break;
					 
            case 3:  printf ("\nExiting... \n");
					 exit (-1);
					 break;
			//default: printf ("\nDefault: Returning to Menu... \n"); GetUserInput (); break;
		    }
	
return 0;
}

// Channelmode 
//PMR

int channelmodepmr ()
{
	printf ("\nChoose PMR-Channel 0-17 (18 to exit): "); 
	int channelnumberpmr;
	scanf ("%d ", &channelnumberpmr);
	switch (channelnumberpmr)
	{
	 //---- Analog & digital 
	 case 0: freq=446.00625; printf ("\nDUMMY scan: Chan 0-> default Chan 1 %f ", freq); break;	// Scan all Chan till active , now chan1
	 case 1: freq=446.00625; break;	 //Standard
	 case 2: freq=446.01875; break; //Geocaching
	 case 3: freq=446.03125; break; // random
	 case 4: freq=446.04375; break; //at 3-chan-PMR-devices its ch. 2
	 case 5: freq=446.05625; break; //Contest
	 case 6: freq=446.06875; break; //Events
	 case 7: freq=446.08125; break; //at 3-chanl-PMR-devices it's ch. 3
	 case 8: freq=446.09375; break; //random talk stuff
//---------------------------Digital only
	// dpmr digital new since 28.09.2016
	// extra 8 chan
	// 12.5 kHz steps too
	 case 9:  freq=446.10312; break; // 6.25 kHz steps
	 case 10: freq=446.10625; break;
	 case 11: freq=446.11875; break;
	 case 12: freq=446.13125; break;
	 case 13: freq=446.14375; break;
	 case 14: freq=446.15625; break;
	 case 15: freq=446.16875; break;
	 case 16: freq=446.18125; break;
	 case 17: freq=446.19375; break;
	 case 18: channelselect (); break;
	 //default: freq=446.00625; printf ("\nDefault chan = 1 %f \n", freq);  break;
	}
	return freq;
}

// CB
int channelmodecb ()
{
	printf ("\nChoose CB-Channel 0-80 (81 to exit): "); 
	int channelnumbercb;
	scanf ("%d ", &channelnumbercb);
	switch (channelnumbercb)
	{
		
            case 0:   freq=27.0450; printf ("\nSpecial freq for digital %f \n", freq);  break;
			case 1:   freq=26.9650; break; //empfohlener Anrufkanal (FM)	
			case 2:   freq=26.9750; break; //inoffizieller Berg-DX-Kanal (FM)
			case 3:   freq=26.9850; break;
			case 4:   freq=27.0050; break; //empfohlener Anrufkanal (AM)/Anrufkanal Feststationen (AM)
			case 5:   freq=27.0150; break; //Kanal wird von italienischen Fernfahrern in Deutschland und Italien benutzt.
			case 6:   freq=27.0250; break; //Datenkanal (D)
			case 7:   freq=27.0350; break; //Datenkanal (D)
			case 8:   freq=27.0550; break;
			case 9:   freq=27.0650; break; //Fernfahrerkanal (AM)/weltweiter Notrufkanal EMG
			case 10:  freq=27.0750; break; //Antennen-abgleich - halbe Channel-Anzahl!! ansonsten Chan 20 oder 40
/*
			# Bei genauerer Betrachtung obiger Tabelle fallen einige Stellen auf, 
			# an denen sich Nachbarkanaele nicht um 10 kHz, sondern um 20 kHz unterscheiden. 
			# Die dazwischen versteckten Kanaele werden ueblicherweise folgenderweise bezeichnet:
			# Diese Kanaele sind in den meisten Laendern nicht fuer CB-Funk zugelassen. 
			# Allerdings werden sie in einigen Laendern, darunter auch Deutschland[3], fuer andere Zwecke
			# wie z. B. Funkfernsteuerungen, Babyphones, kabellose Tastaturen und Maeuse verwendet
*/
			 case 11:  freq=27.0850; break; //freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland
			 case 1111: freq=27.0950;  break;//Eurobalise-Energieversorgung
			 
			 case 12:  freq=27.1050;  break;
			 case 13:  freq=27.1150;  break;
			 case 14:  freq=27.1250;  break;//oft verwendet fuer Spielzeug-Fernsteuerungen (mittels Selektivton)
			 case 15:  freq=27.1350;  break;//inoffizieller Anrufkanal SSB (USB)	
			 case 1515: freq=27.1450; break;
			 
			 case 16:  freq=27.1550;  break;//Funkverkehr mit und zwischen Wasserfahrzeugen
			 case 17:  freq=27.1650;  break;//Kanal wird von daenischen Schwertransportfahrern in Deutschland und Daenemark benutzt
			 case 18:  freq=27.1750;  break;
			 case 19:  freq=27.1850;  break;//empfohlener Fernfahrerkanal (FM)/oft von Walkie-Talkies genutzt/teilweise auch als Notrufkanal angegeben/auch von Babyfonen genutzt	
			 case 1919: freq=27.1950; break;
										
			 case 20:  freq=27.2050; break; //zum Antennenabgleich genutzte Mitte bei 40-Kanal-Geraeten, 
										//#wird in oesterreich sehr oft fuer Schwertransportfahrten benutzt

		 	//## 40 chan devices
			 case 21:  freq=27.2150;  break;//tuerkischer Anrufkanal in Deutschland und Europa (FM)	
			 case 22:  freq=27.2250;  break;//oft von Walkie-Talkies genutzt, auch von Babyfonen genutzt, wird auch als Anrufkanal fuer rumaenische Fernlastfahrer verwendet
			 case 23:  freq=27.2550;  break;//Die Kanaele 23, 24, 25 sind sog. Dreher, sie folgen nicht dem aufsteigenden 10-kHz-Raster	
			 case 24:  freq=27.2350;  break;//Datenkanal (D)
			 case 25:  freq=27.2450;  break;//Datenkanal (D), USB ROS Intern
			 case 26:  freq=27.2650;  break;
			 case 27:  freq=27.2750;  break;
			 case 28:  freq=27.2850;  break;//Kanal wird von polnischen Fernfahrern in Deutschland benutzt, Anrufkanal in Polen, wobei allgemein die CB-Kanalfrequenz in Polen um 5 kHz niedriger ist
			 case 29:  freq=27.2950;  break;//Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ber eine Internetverbindung in Deutschland	
			 case 30:  freq=27.3050;  break;//inoffizieller DX-Kanal (FM), Anrufkanal fuer Funker aus dem ehemaligen Jugoslawien
			 case 31:  freq=27.3150;  break;//inoffizieller DX-Kanal (FM)
			 case 32:  freq=27.3250;  break;
			 case 33:  freq=27.3350;  break;
			 case 34:  freq=27.3450;  break;//freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland
			 case 35:  freq=27.3550;  break;//oeffentlicher Kanal
			 case 36:  freq=27.3650;  break;//Datenkanal USB ROS international
			 case 37:  freq=27.3750;  break;//Gateway-Kanal oesterreich, FM	
			 case 38:  freq=27.3850;  break;//inoffizieller internationaler DX-Kanal (LSB)
			 case 39:  freq=27.3950;  break;//Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland	
			 case 40:  freq=27.4050;  break; //ab Maerz 2016 freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete 
									  //ueber eine Internetverbindung in Deutschland (FM/AM/SSB in D)
			/* 80 chan devices
			 Auf den nationalen Zusatzkanaelen 41 bis 80 ist nur die Modulationsart FM erlaubt 
			 Nachfolgend sind die Frequenzen der nationalen Zusatzkanaele, die im CB-Funk benutzt werden duerfen, aufgelistet: 
            */
			case 41:  freq=27.5650; break; //Ab Maerz 2016 Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland (FM),inoffizieller DX-Kanal (FM)
			case 42:  freq=27.5750; break; //inoffizieller DX-Kanal (FM)
			case 43:  freq=27.5850; break;
			case 44:  freq=27.5950; break;
			case 45:  freq=27.6050; break;
			case 46:  freq=27.6150; break;
			case 47:  freq=27.6250; break;
			case 48:  freq=27.6350; break;
			case 49:  freq=27.6450; break;
			case 50:  freq=27.6550; break;
			case 51:  freq=27.6650; break;
			case 52:  freq=27.6750; break;//Datenkanal (D)(FM)
			case 53:  freq=27.6850; break;//Datenkanal (D)(FM)	
			case 54:  freq=27.6950; break; 
			case 55:  freq=27.7050; break;
			case 56:  freq=27.7150; break;
			case 57:  freq=27.7250; break;
			case 58:  freq=27.7350; break;
			case 59:  freq=27.7450; break;
			case 60:  freq=27.7550; break;
            case 61:  freq=26.7650; break;//Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland	
			case 62:  freq=26.7750; break;
			case 63:  freq=26.7850;	break;
			case 64:  freq=26.7950; break;
			case 65:  freq=26.8050; break;
			case 66:  freq=26.8150; break;
			case 67:  freq=26.8250;	break;
			case 68:  freq=26.8350; break;
			case 69:  freq=26.8450; break;
			case 70:  freq=26.8550; break;
			case 71:  freq=26.8650; break;//Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland	
			case 72:  freq=26.8750; break;
			case 73:  freq=26.8850; break;
			case 74:  freq=26.8950; break;
			case 75:  freq=26.9050; break;
			case 76:  freq=26.9150; break;//Datenkanal (D)(FM)
			case 77:  freq=26.9250; break;//Datenkanal (D)(FM)
			case 78:  freq=26.9350; break;
			case 79:  freq=26.9450; break;
			case 80:  freq=26.9550; break;//Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland */
			case 81:  return 0; break; 
			//default:  freq=26.9650; printf ("\nDefault: CB chan = 1 %f \n", &freq); break;
            return freq;	
	    
	}
	
}

// RDS data.  Send MSB first.  Google search gr_rds_data_encoder.cc to make your own data.
/*
const unsigned char RDSDATA[] = 
{

    0x50, 0xFF, 0xA9, 0x01, 0x02, 0x1E, 0xB0, 0x00, 0x05, 0xA1, 0x41, 0xA4, 0x12, 
    0x50, 0xFF, 0xA9, 0x01, 0x02, 0x45, 0x20, 0x00, 0x05, 0xA1, 0x19, 0xB6, 0x8C, 
    0x50, 0xFF, 0xA9, 0x01, 0x02, 0xA9, 0x90, 0x00, 0x05, 0xA0, 0x80, 0x80, 0xDC, 
    0x50, 0xFF, 0xA9, 0x01, 0x03, 0xC7, 0xD0, 0x00, 0x05, 0xA0, 0x80, 0x80, 0xDC, 
    0x50, 0xFF, 0xA9, 0x09, 0x00, 0x14, 0x75, 0x47, 0x51, 0x7D, 0xB9, 0x95, 0x79, 
    0x50, 0xFF, 0xA9, 0x09, 0x00, 0x4F, 0xE7, 0x32, 0x02, 0x21, 0x99, 0xC8, 0x09, 
    0x50, 0xFF, 0xA9, 0x09, 0x00, 0xA3, 0x56, 0xF6, 0xD9, 0xE8, 0x81, 0xE5, 0xEE, 
    0x50, 0xFF, 0xA9, 0x09, 0x00, 0xF8, 0xC6, 0xF7, 0x5B, 0x19, 0xC8, 0x80, 0x88, 
    0x50, 0xFF, 0xA9, 0x09, 0x01, 0x21, 0xA5, 0x26, 0x19, 0xD5, 0xCD, 0xC3, 0xDC, 
    0x50, 0xFF, 0xA9, 0x09, 0x01, 0x7A, 0x36, 0x26, 0x56, 0x31, 0xC9, 0xC8, 0x72, 
    0x50, 0xFF, 0xA9, 0x09, 0x01, 0x96, 0x87, 0x92, 0x09, 0xA5, 0x41, 0xA4, 0x12, 
    0x50, 0xFF, 0xA9, 0x09, 0x01, 0xCD, 0x12, 0x02, 0x8C, 0x0D, 0xBD, 0xB6, 0xA6, 
    0x50, 0xFF, 0xA9, 0x09, 0x02, 0x24, 0x46, 0x17, 0x4B, 0xB9, 0xD1, 0xBC, 0xE2, 
    0x50, 0xFF, 0xA9, 0x09, 0x02, 0x7F, 0xD7, 0x34, 0x09, 0xE1, 0x9D, 0xB5, 0xFF, 
    0x50, 0xFF, 0xA9, 0x09, 0x02, 0x93, 0x66, 0x16, 0x92, 0xD9, 0xB0, 0xB9, 0x3E, 
    0x50, 0xFF, 0xA9, 0x09, 0x02, 0xC8, 0xF6, 0x36, 0xF4, 0x85, 0xB4, 0xA4, 0x74, 
    0x50, 0xFF, 0xA9, 0x09, 0x03, 0x11, 0x92, 0x02, 0x00, 0x00, 0x80, 0x80, 0xDC, 
    0x50, 0xFF, 0xA9, 0x09, 0x03, 0x4A, 0x02, 0x02, 0x00, 0x00, 0x80, 0x80, 0xDC, 
    0x50, 0xFF, 0xA9, 0x09, 0x03, 0xA6, 0xB2, 0x02, 0x00, 0x00, 0x80, 0x80, 0xDC, 
    0x50, 0xFF, 0xA9, 0x09, 0x03, 0xFD, 0x22, 0x02, 0x00, 0x00, 0x80, 0x80, 0xDC
};
*/

// Structs
void* p;  
void* v;  
struct PageInfo 
{
		void* p;  // physical address
		void* v;  // virtual address
		int constPage;
		int instrPage;
		int instrs [1024];
};

struct GPCTL
{
		char SRC         : 4;
		char ENAB        : 1;
		char KILL        : 1;
		char             : 1; // what is the blank char?
		char BUSY        : 1;
		char FLIP        : 1;
		char MASH        : 2;
		unsigned int     : 13;
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

struct DMAregs 
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

//------------------------------------// 
// Voids
// FM ones
	
void handSig ()
{
		exit (0);
}

void modulate (int m)
{
		ACCESS (CM_GP0DIV) == (0x5a << 24) + 0x4d72 + m;
}

void getRealMemPage (void** vAddr, void** pAddr)
{
		void* a = valloc (4096);
    
		((int*)a)[0] = 1;  // use page to force allocation
    
		mlock (a, 4096);  // lock into ram
    
		*vAddr = a;  //  we know the virtual address now
    
		unsigned long frameinfo;
    
		int fp = open ("/proc/self/pagemap", 'r');
		lseek (fp, ((int)a)/4096*8, SEEK_SET);
		read (fp, &frameinfo, sizeof (frameinfo));
    
		*pAddr = (void*)((int)(frameinfo*4096));
}

void freeRealMemPage (void* vAddr)
{
		munlock (vAddr, 4096);  // unlock ram
		free (vAddr); // free the ram
}

void setupfm ()
{
    printf ("\nSetting up FM... \n");
    // open /dev/mem 
    if ((mem_fd = open ("/dev/mem", O_RDWR|O_SYNC) ) < 0) 
	{
        printf ("\nCan't open /dev/mem! \n");
        exit (-1);
    }
    
    allof7e = (unsigned *) mmap (
                  NULL,
                  0x01000000,  //len
                  PROT_READ|PROT_WRITE,
                  MAP_SHARED,
                  mem_fd,
                  0x20000000);  //base

   if ((int)allof7e==-1) exit(-1);

   SETBIT(GPFSEL0, 14);
   CLRBIT(GPFSEL0, 13);
   CLRBIT(GPFSEL0, 12);
   struct GPCTL setupword = {6, 1, 0, 0, 0, 1,0x5a};

   ACCESS (CM_GP0CTL) == *((int*)&setupword);
    
}

///------------------------------------
//relevant for transmitting stuff 

void playWav (char *filename, int samplerate)
{
    // after getting filename insert then open
    int fp = STDIN_FILENO;
    if (filename [0] != NULL) fp = open (filename, 'r');
	
	int sz = lseek (fp, 0L, SEEK_END); 
    lseek (fp, 0L, SEEK_SET);
    short* data = (short*) malloc (sz);
    read (fp, data, sz);

    int bufPtr=0;
    float datanew, dataold = 0;

    for (int i=0; i<22; i++)
        read (fp, &data, 2);  // read past header
    
    while (read (fp, &data, 2))
	{
     
        float fmconstant = (samplerate*50.0E-6);  // for pre-emphisis filter, 50us time constant
        int clocksPerSample = (22500/samplerate*1400.0);  // for timing
        
        datanew = ((float)(*data)/32767.0f); //some constant for modulation ??
        
        float sample = datanew + (dataold-datanew)/(1-fmconstant);  // fir of 1 + s tau
        float dval = sample*15.0;  // actual transmitted sample,  15 is bandwidth (about 75 kHz)
        
        int intval = (int)(round (dval));  // integer component
        float frac = ((dval - (float)intval)/2 + 0.5);
        unsigned int fracval = (frac*clocksPerSample);
        int instrPage;
        int constPage;
        int instrs [1024];
        
        //problem still with .v & .p endings! 
          
        bufPtr++;
        //(ACCESS(DMABASE + 0x04) ==  (int)(instrs[bufPtr].p));  // CurBlock 0x04 PageInfo.
        usleep (1000);
        //((struct CB*)(instrs[bufPtr].v))->SOURCE_AD = (int)constPage.p + 2048 + intval*4-4;
       
        bufPtr++;
       // while (ACCESS(DMABASE + 0x04) ==  (int)(instrs[bufPtr].p)); 
        usleep (1000);
       // ((struct CB*)(instrs[bufPtr].v))->TXFR_LEN = clocksPerSample-fracval;
        
        bufPtr++;
       // while (ACCESS(DMABASE + 0x04) ==  (int)(instrs[bufPtr].p)); 
        usleep (1000);
       // ((struct CB*)(instrs[bufPtr].v))->SOURCE_AD = (int)constPage.p + 2048 + intval*4+4;
        
        bufPtr = (bufPtr+1) % (1024);
      //while (ACCESS(DMABASE + 0x04) ==  (int)(instrs[bufPtr].p));
        usleep (1000);
       //((struct CB*)(instrs[bufPtr].v))->TXFR_LEN = fracval;
        
        dataold = datanew;
        
    }
    
   fclose (fp);
   close (filename);
}
 
void unSetupDMA ()
{
	printf("\nExiting...\n");
	struct DMAregs* DMA0 = (struct DMAregs*)(ACCESS(DMABASE));
	DMA0->CS == 1<<31;  // reset dma controller
	exit (-1);
}

void setupDMA (float freq)
{
	atexit (unSetupDMA);
	signal (SIGINT,  handSig);
	signal (SIGTERM, handSig);
	signal (SIGHUP,  handSig);
	signal (SIGQUIT, handSig);

    int constPage;
    int instrCnt = 0;
    int instrPage;
    int instrs [1024];
	// allocate a few pages of ram
//  	getRealMemPage (&constPage.v, &constPage.p);
	int centerFreqDivider = (int)((500.0 / freq) * (float)(1<<12) + 0.5);
	// make data page contents - it's essientially 1024 different commands for the
	// DMA controller to send to the clock module at the correct time
	for (int i=0; i<1024; i++)
	{
	  //  ((int*)(constPage.v))[i] = (0x5a << 24) + centerFreqDivider - 512 + i;
	}
   

	while (instrCnt < 1024) 
	{
    //getRealMemPage (&instrPage.v, &instrPage.p);
     
     // make copy instructions
	///struct CB* instr0 = (struct CB*)instrPage.v;
     
    for (int i=0; i<4096/sizeof (struct CB); i++) 
     {
         /*
         instrs[instrCnt].v = (void*)((int)instrPage.v + sizeof(struct CB)*i);
         instrs[instrCnt].p = (void*)((int)instrPage.p + sizeof(struct CB)*i);
         instr0->SOURCE_AD = (unsigned int)constPage.p + 2048;
	   
         instr0->DEST_AD = PWMBASE + 0x18; //fifo
         instr0->TXFR_LEN = 4;
         instr0->STRIDE = 0;
         instr0->NEXTCONBK = (int)instrPage.p + sizeof (struct CB)*(i+1);
	   
	     // DREQ then PWM then no-wide
         instr0->TI = (1<<6) | (5<<16) |  (1<<26);
         instr0->RES1 = 0;
         instr0->RES2 = 0;
         */
        if (i%2) 
	    {
         //instr0->DEST_AD = CM_GP0DIV;
         //instr0->STRIDE = 4;
         //instr0->TI = (1<<26) ;
        }
      
       // if (instrCnt!=0) ((struct CB*)(instrs[instrCnt-1].v))->NEXTCONBK = (int)instrs[instrCnt].p;
       // instr0++;
       // instrCnt++;
       
     } 
     
   }
   
   //((struct CB*)(instrs[1023].v))->NEXTCONBK = (int)instrs[0].p;
   
    // set up a clock for the base
   ACCESS (CLKBASE + 40*4) == (0x5A000026); //PWMCLK_CNTL
   usleep (1000);
   
   ACCESS (CLKBASE + 41*4) == (0x5A002800); //PWMCLK_DIV
   ACCESS (CLKBASE + 40*4) == (0x5A000016); //PWMCLK_CNTL
   usleep (1000); 

    // set up pwm
   ACCESS (PWMBASE + 0x0) == 0;
   usleep (1000);
   
   ACCESS (PWMBASE + 0x4) == -1;  // status: clear errors
   usleep (1000);
   
   // Use fifo , repeat, serializer, enable ch
   ACCESS (PWMBASE + 0x0) == -1 | (1<<13) | (1<<10) | (1<<9) | (1<<8); 
   usleep (1000);
   
   ACCESS (PWMBASE + 0x8) == (1<<31) | 0x0707; // DMAC then DMA enable 
   
  //activate dma
   struct DMAregs* DMA0 = (struct DMAregs*)(ACCESS(DMABASE));
   DMA0->CS = 1<<31;  // reset
   DMA0->CONBLK_AD = 0; 
   DMA0->TI = 0; 
  
  //DMA0->CONBLK_AD = (unsigned int)(instrPage.p);
   DMA0->CS = (1<<0)|(255 <<16);  // enable bit = 0, clear end flag = 1, prio=19-16
  
}

// AM ones
void WriteTone (float Frequency, uint32_t Timing)
{
	typedef struct 
	{
	    float Frequency;
		uint32_t WaitForThisSample;
	} 
	
	samplerf_t;
	samplerf_t RfSample;
	
    freq = Frequency;
	RfSample.Frequency = Frequency;
	
	RfSample.WaitForThisSample = Timing; //in 100 of nanoseconds
	printf ("Freq=%f Timing=%d \n", RfSample.Frequency, RfSample.WaitForThisSample);
	
	if (write (FileFreqTiming, &RfSample, sizeof (samplerf_t)) != sizeof (samplerf_t)) 
	{
		fprintf (stderr, "\nUnable to write sample \n");
	}
}

//---------------------//
// main progs

//FM
int modulationfm (int argc, char **argv)
{

    printf ("\nPreparing for FM... \n");
    /// compare to original function in pifm
    if (argc>1)
 	{
	  printf ("\nChecking Path... \n");
      setupfm (); // gets filename & path

	  printf ("\nSetting up DMA... \n");
     // setupDMA (freq);
      setupDMA (argc>2 ? atof (argv[2]):100.00000); // default freq, maybe do input here 
	  
	  printf ("\nTesting Samplerate... \n");
	  //playWav (argv[1], samplerate);
      playWav (argv[1], argc>3 ? atof (argv[3]):22500); // <--- in 22,5 kHz, should be same as AM!!
     
	  printf ("\nNow transmitting... \n");
    } 
	else fprintf (stderr, "\nUse Parameters to run: [filename] [freq] [samplerate] [mod (fm/am)] \nWhere wav-file is 16-bit @ 22500Hz Mono.\nSet wavfile to '-' to use stdin.\nFrequency is between 1-500.0000 [MHz] (default 100.0000)\nYou can play an empty file to transmit silence. \n");
	
return 0;
}

//AM --- not yet adapted, needs revision for freq
int modulationam (int argc, char **argv) 
{
    /*
              {IQ (FileInput is a Mono Wav contains I on left Channel, Q on right channel)}
              {IQFLOAT (FileInput is a Raw float interlaced I,Q)}
              {RF (FileInput is a (float) Frequency,Time in nanoseconds}
              {RFA (FileInput is a (float) Frequency, (int) Time in nanoseconds, (float) Amplitude}
              {VFO (constant frequency)}
    */

    float data [2*BUFFER_LEN];
    float data_filtered [2*BUFFER_LEN]; // we generate complex I-Q samples
    float ampf;
    float factorizer;
    float sampler;
    int	  readcount, nb_samples;
    char  *outfilename;
    int Excursion = 6000;
    
    //SF_INFO	sfinfo;
    //SNDFILE	*infile, *outfile;
    char    SFM_READ;     
    char	sfinfo;
    char	*outfile;
    
	if (argc=4) 
	{
		*filename = argv[1]; // if needed to directly clarify argv 1
		FileFreqTiming = open (outfilename, O_WRONLY|O_CREAT, 0644);
	}
	else 
	{
		outfilename = (char *)malloc (128);
		sprintf (outfilename, "%s", "out.ft");
	}
                              
    if (!(filename = open (filename, SFM_READ, &sfinfo)))
    {   // Open failed so print an error message. 
        printf ("\Not able to open input file %s \n", filename);
        // Print the error message from libsndfile. 
        return  1;
    }

	if (samplerate != 22500) //(sfinfo.samplerate != 22500) 
	{ 
		printf ("\nInput samplerate must be 22.5 [kHz] (mono)! \n");
		return 1;
	}
	
	if (filebit != 16) 
	{ 
		printf ("\nInput must be 16 bit (mono)! \n");
		return 1;
	}
    
	// While there are frames in the input file, read them, 
	//process them and write them to the output file 

    while (readcount = read (filename, data, BUFFER_LEN))
    {
	 // where to input the freq like in fm??
	  nb_samples = (readcount/channels);
	  printf ("\nnb_samples: %f \n", nb_samples);
	  
	  for (k=0 ; k < nb_samples ; k++)
	   {
			x = data[k*channels];
			if (channels == 1)
			{
				// stereo file, avg left + right --> should be mono at 22.5kHz
				x += data [k*channels+1];
				x /= 2; // maybe *2 to make a dual mono and not doing stereo in half!
			}
			else printf ("File is NOT mono! \n");
		
			//maybe here am option for amplitude factor input!?
			float FactAmplitude = 2.0; 
			printf ("Factamplitude: %f \n", FactAmplitude);
			
			// log Modulation, 
			float A = 87.7f; // compression parameter, stauchung
			printf ("Compression prameter A: %f \n", A);
			
			ampf = (x/32767.0f);
			printf ("ampf1: %f \n", ampf);
			
      		ampf = (fabs (ampf) < 1.0f/A) ? A*fabs(ampf)/(1.0f + ln(A)) : (1.0f + ln(A*fabs(ampf)))/(1.0f + ln(A)); //compand
			printf ("compand ampf2: %f \n", ampf);	
			
			x = (int)(round (ampf*32767.0f));
			printf ("new x: %f \n", x);
			
		    factorizer = (x*32767.0f*FactAmplitude);
			printf ("factorizer: %f \n", factorizer);
			
			sampler = (1E9/samplerate);
			printf ("sampler: %f \n", sampler);
			
			WriteTone (factorizer, sampler); 
            printf ("\nNow writing tone in AM... \n");
            
            printf ("Reading file: %s \n", filename);
            printf ("Freq: %f \n", freq);
	        printf ("Sample Rate: %d \n", samplerate);
	        printf ("Channels: %d \n", channels);
	        printf ("Writing file: %s \n", outfilename);
    }

    // Close input and output files 
    close (FileFreqTiming);
    close (filename); 
    printf ("\nFile saved! \n");
    
    //return channels, ampf, x, factorizer, sampler;
	return 0;
}

int modulationselect ()
{
    
	printf ("Choose your Modulation [1] FM // [2] AM [3] Exit : ");
	int freqmode;
	scanf ("%d", &freqmode);
	switch (freqmode)
	{
		case 1: printf ("\n*** You selected 1 for FM! \n");
		        int modulationfm (int argc, char **argv); 
		        break;

		case 2: printf ("\n*** You selected 2 for AM! \n");
		        int modulationam (int argc, char **argv); 
		        break;
	
		case 3: printf ("\nExiting... \n"); exit (-1); break;
		//default: printf ("\n Default = 1 \n"); break;
	}

return freqmode;
}
//------------------//
// all subch. -> base/default case 0 -> channel 0
// if subchannels is 0 = all ch. then check special stuff -> maybe scan func ?
// squelch/treshhold to build in maybe -> scan function till signal?

// read / import csv for pmr

int csvreader()
{
    printf ("\nChecking for CSV-file... \n");
    char *sfp, *dfp;
    char c;

    *sfp = fopen ("csvpmr.csv","r");
    *dfp = fopen ("csvwriter.csv","w+");
    while (!feof (*sfp))
    {
    //here check for semicolon or comma delimiterb (default)
    c = fgetc (*sfp);
    fputc (c,*dfp);
    }
    
    fclose (*sfp);
    fclose (*dfp);
    printf ("%c \n", c);
    printf ("CSV-Import of ctss-List finished! \n");
    
    return 0;

}

int GetUserInput () //my menu
{
    int modeselect;
    infos ();
    printf ("Press Enter to Continue ");
    while (getchar () != '\n');
    
    
	printf ("Choose a Mode [1] Channel-Mode // [2] Frequency-Mode // [3] CSV-Reader // [4] CMD // [5] Exit : ");
	scanf ("%d ", &modeselect);
    
	switch (modeselect)
        {
            case 1: channelselect ();
					break;
					 
		    case 2:	printf ("\n*** You selected 2 for Frequecy-Mode ***\n");
		            char filenamepath ();
		            int freqselect ();
		            int modulationselect ();
					break;
					 
			case 3: printf ("\nReading CSV for PMR: \n");
					int csvreader ();
					break; 
			case 4: printf ("\nShell Command-line: \n");
			        int main (int argc, char **argv); // go back to cmd if you want
					break;
			
			case 5: printf ("\nExiting... \n");
					exit (-1);
					break;
			
			//default: printf ("\nDefault = 1 \n"); break;
			 
        }

    return modeselect;
}

int main (int argc, char **argv)
{
   // arguments for global use must be in main
   infos ();
   timer ();
   printf ("%s/pifunk:", argv[0]);
   //scanf  ("%s %s %s %s", argv[1], argv[2], argv[3], argv[4]);
  
   char *filename = argv[1]; 

   char *mod = argv[4];
   char fm, am;

   if (argc=1 & !strcmp (argv[1],"menu"))  
   {
      printf ("Menu activated! \n"); 
      GetUserInput (); //  to menu
   }
   else if (argc=1 & !strcmp (argv[1],"help"))  
   {
     int infos ();
     printf ("\nHELP: Use Parameters to run: [filename] [freq] [samplerate] [mod (fm/am)] or [menu] or [help] \nWhere wav-file is 16-bit @ 22500 [Hz] Mono \n");
   }
   else if (argc=4) 
   { 
           printf ("\nTrying to run with %d Arguments->\nFile: %s Freq: %s Samplerate: %s Modulation: %s \n", argc, argv[1], argv[2], argv[3], argv[4]);
           
           // atoll() is meant for integers & it stops parsing when it finds the first non-digit
           // atof ()/strtof () is for floats. Note that strtof () requires C99 or C++11
           freq = strtof (argv[2], NULL); //float only accurate to .4 digits idk why, from 5 it will round ?!
           samplerate = atof (argv[3]); //maybe check here on != 22500 on 16 bits as fixed value (eventually allow 48k)-> otherwise in dma or playwav func
    
           printf ("String-Conversion to Freq: %f [MHz] @ Samplerate %d [Hz] \n", freq, samplerate);
           printf ("Checking Modulation: %s \n", mod); 
           if (mod != NULL)
           {
             if (!strcmp (mod,"fm"))
             {
                printf ("Pushing args to FM Modulator... \n"); 
                int modulationfm (int argc, char **argv);
             }
             else if (!strcmp (mod,"am"))
             {
                printf ("Pushing args to AM Modulator... \n"); 
                int modulationam (int argc, char **argv);
             }
           }
           else 
           {
             printf ("No Modulation specified! fm or am? \n"); 
           }
  
    }
    else fprintf (stderr, "Argument-Error! \nUse Parameters to run: [filename] [freq] [samplerate] [mod (fm/am)] or [menu] or [help] \nWhere wav-file is 16-bit @ 22500 [Hz] Mono.\nSet wavfile to '-' to use stdin.\nFrequency is between 1-500.0000 [MHz] (default 100.0000)\nYou can play an empty file to transmit silence. \n");
  
    //into the fm or an function to run your file
    printf ("Returning args 0 to 4... \n"); 
    return argc, argv[0], argv[1], argv[2], argv[3], argv[4], filename, freq; samplerate, mod;
    //return 0;
    
}
//EOF



