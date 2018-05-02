// to run and compile to binary use:
// install gcc run: 
//gcc -lm -std=c99 pifunk.c -o pifunk // run with admin/root permissions!!

//=> real gpio hardware can't be simulated by c or py code! must be executed and compiled on linux 
// virtual maschine possible with qemu
// or alternative with everpad: nor sure about this, rather not using it 
// wget -o -http://beta.etherpad.org/p/pihackfm/export/txt >/dev/null | gcc -lm -std=c99 -g -xc - && ./a.out sound.wav
/*
gcc 5.4.1 compiler + gdb 7.11.1 debugger (online & local)
g++ 5.4.1 c++11 (or 14)
// compilation tried with mingw-x64 on win 10 but strange, trying on rabian strech incl desktop v4.14

!!!!!!! needs more testing on real pi !!!!! 

-----Disclaimer-----
Rewritten for own purpoes! 
no guarantee, waranty for anything! Usage at own risk!
you should ground your antenna, eventually diode or 10uF-caps 
usage of dummyloads 50 ohm @  4 watts , (max 100) possible and compare signals wit swr/pwr-meter!

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
name & license stuff
*/

// normal includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h> // for c99
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
//#include <conio.h> // dos header
#include <curses.h> //ncurses for  gcc version of conio
#include <termios.h>
#include <pthread.h>
#include <fenv.h> 
#include <inttypes.h>
#include <tgmath.h>
#include <complex.h>
#include <grp.h>
#include <pwd.h>
#include <wchar.h>
#include <wctype.h>
#include <iso646.h> //c95 - backcompatible

// on posix linux
#include <features.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> 

//-- c11 
#include <stdalign.h>
#include <stdnoreturn.h>
#include <stdatomic.h>
#include <uchar.h>
//for c++
/*
#include <iostream>
#include <threads.h>
using namespace std; 
*/

#include "bcm2835.h" // broadcom arm processor for the pis

// windows (10) if needed for maybe rpi3 
/*
#include <windows.h>
#include <win.h>
#include <windef.h>
#include <winnt.h>
#include <winbase.h>
*/
 
// see http://www.mega-nerd.com/libsndfile/api.html for API
// needed for am -> ALSA sound
#include "sndfile.h"

//extra libary https://github.com/libusb/libusb
//#include "/usr/include/libusb/libusb.h"
//#include <libusb.h>

//python stuff, maybe wrapper too??

//---------------------------------------------------------------//
#define VERSION "0.1.6.4 a"
#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_BUILD 6
#define VERSION_STATUS a

//----------------------------------------------------
#define	ln(x) log(x)/log(2.718281828459045235f)
#define PI 3.14159265

//---- PI specific stuff
#define IN 0
#define OUT 1
#define LOW 0
#define HIGH 1
//-------GPIO
#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)
#define	BUFFER_LEN (8*1024)
#define BUFFERINSTRUCTIONS (65536) // [1024];

// --------I-O access
volatile unsigned *gpio;
volatile unsigned *allof7e;
// GPIO setup macros: Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // setsbits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0
#define GPIO_GET *(gpio+13) // sets bits which are 1 ignores bits which are 0

#define length    (0x01000000)
#define base      (0x20000000) // base=GPIO offset 
#define ADR       (0x7E000000)
#define CM_GP0CTL (0x7E101070) //p.107
#define GPFSEL0   (0x7E200000) //p.90
#define CM_GP0DIV (0x7E101074) //p.108
#define CLKBASE   (0x7E101000)
#define DMABASE   (0x7E007000)
#define PWMBASE   (0x7E20C000) // PWM controller 
#define BCM2836_PERI_BASE (0x3F000000) // register physical address
#define GPIO_BASE (BCM2836_PERI_BASE + base) 
#define PWMCLK_DIV (0x5A002800) // PWMCLK_DIV
#define PWMCLK_CNTL (0x5A000016)  //PWMCLK_CNTL

#define ACCESS(base) (volatile int*)(base+(volatile int)allof7e-ADR)
#define SETBIT(base, bit) ACCESS(base) || 1<<bit //   |=
#define CLRBIT(base, bit) ACCESS(base) && ~(1<<bit) // &=
#define sleep (1000)

//possibility to give argv 0-4 an specific adress or pointer
//Adresses-> at least on my system-tests
#define argc_adr (0x7FFFFFFFEB0C) 
#define Name_adr (0x7FFFFFFEC08)                                                                       
#define File_adr (0x7FFFFFFFEC10) 
#define Freq_adr (0x7FFFFFFFEC18)                                                                                       
#define Samplerate_adr (0x7FFFFFFFEC20) 
#define Modulation_adr (0x7FFFFFFFEC28)   
#define callsign_adr (0x6052C0)
#define callsign2_adr (0x7FFFFFFFEAEF) 
#define callsign3_adr (0x7FFFFFFFEAE8)

 
//Pointers->
#define argc_ptr (0x5) 
#define Name_ptr (0x2F)
#define File_ptr (0x73) 
#define Freq_ptr (0x31) //$ means is in RDS data
#define Samplerate_ptr (0x32) //$ means its in RDS data
#define Modulation_ptr (0x66) //$ means ist in RDS data
#define callsign_ptr (0x6D)
#define CurBlock (0x04)
#define DMAref (0x7F) //pwm base reference or sth like that?
//--------------------------------------------------//
//pi variables:  -> need to be activated and pulled up with python-script, or automaticly by system

int   mem_fd;
char *gpio_mem, *gpio_map;
char *spi0_mem, *spi0_map;

//--------------LED stuff with 100 ohm resistor!
//controlling via py possible but c stuff cant be useful

//-----------------------------------------
// programm Arguments
// custom programm-name. system default is the filename itself! 
char *description = "(experimental)";
char *filename;
float freq;
int samplerate;
char *mod;
char *fm = "fm"; 
char *am = "am";
int volume, gain = 0;
char *callsign;
int channels = 1;
int channelmode;
int channelnumbercb;
int channelnumberpmr;

// programm variables
time_t rawtime;
struct tm *info;
char buffer [80];
char data [1024];
int i;
int k;
float x;

FILE infile;
FILE outfile;
int fp = STDIN_FILENO;
int filebit;
float datanew, dataold = 0;
int readBytes;

float ampf;
float factorizer;
float sampler;
int readcount, nb_samples;
int Excursion = 6000;
char  *outfilename;
    
SF_INFO	sfinfo;
SNDFILE	*infile, *outfile;
char    SFM_READ;     
char	sfinfo;

unsigned long frameinfo;
int FileFreqTiming;
int instrs [BUFFERINSTRUCTIONS]; // [1024];
int bufPtr = 0;
int instrCnt = 0;
int constPage;
int instrPage;

//--------------------------------------------------
// Structs

struct PageInfo 
{
		void *p;  // physical address BCM2836_PERI_BASE (0x3F000000)
		void *v;  // virtual address
		int instrPage;
		int constPage; 
		int instrs [BUFFERINSTRUCTIONS]; // [1024];

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

// basic function then specified one after another

int infos ()
{
    printf ("\nWelcome to the Pi-Funk! v%s %s for Raspian ARM \n", VERSION, description);
	printf ("Radio works with *.wav-file with 16-bit @ 22500 [Hz] Mono / 1-500.00000 MHz Frequency\nUse '. dot' as decimal-comma seperator! \n");
    //Here some Warnings to implement and infos
    printf ("Pi oparates with square-waves (²/^2) PWM on GPIO 4 (Pin 7 @ ~500 mA). \nUse power supply with enough specs only! \n=> Use Low-/Highpassfilters and/or ~10 uF-caps and resistors if needed! \nCheck laws of your country! \n"); // i try to smooth it out with 1:1 baloon
    printf ("HELP: Use Parameters to run: [filename] [freq] [samplerate] [mod (fm/am)] or [menu] or [help]! \n");
    printf ("for testing (default setting) run: sound.wav 100.0000 22500 fm \n");
    return 0;
}

int timer ()
{
   
   time (&rawtime);
   info = localtime (&rawtime);
   printf ("Current local time and date: %s \n", asctime (info));

   return info;
}

//--------------------------------------------------


char filenamepath ()
{
    printf ("\nPlease enter the full path including name of the *.wav-file you want to use: \n");
    scanf ("%s", filename);
   
    if (filename == NULL)
	{
	    printf ("%s File not found! \n", filename);
	    return -1;
	}
	else
	{
	   printf ("Trying to play default sound.wav ... \n");
	   *filename = open ("sound.wav", "r");
	   return filename;
	    
	}
	return filename;
}

int freqselect () // gets freq by typing in
{
   
	//printf ("\n*** You selected 1 for Frequency-Mode *** \n"); 
	printf ("Type in Frequency (1-500.00000 MHz): ");
	scanf  ("%f", freq);
	printf ("You chose: %f MHz \n", freq);
    return freq;
}

int channelselect ()
{
	printf ("*** You selected 1 for Channel-Mode *** \n"); 
	printf ("Choose your Type [1] PMR // [2] CB // [3] Exit : ");
    
    scanf  ("%d ", &channelmode);
    
    switch (channelmode) // from here collecting infos and run it step by step, same for freq-mode
            {
            case 1: printf ("*** PMR CHAN-MODE in FM *** "); 
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
	
return channelmode;
}

// Channelmode 
//PMR

int channelmodepmr ()
{
	printf ("\nChoose PMR-Channel 0-17 (18 to exit): "); 
	
	scanf ("%d ", &channelnumberpmr);
	switch (channelnumberpmr)
	{
	 //---- Analog & digital 
	 case 0: freq=446.00625; printf ("\nDUMMY all-chan: Chan 0-> default Chan 1 %f ", freq); break;	// Scan all Chan till active , now chan1
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
	
	scanf ("%d", &channelnumbercb);
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
			 case 1111: freq=27.0950;  break; //Eurobalise-Energieversorgung
			 
			 case 12:  freq=27.1050;  break;
			 case 13:  freq=27.1150;  break;
			 case 14:  freq=27.1250;  break; //oft verwendet fuer Spielzeug-Fernsteuerungen (mittels Selektivton)
			 case 15:  freq=27.1350;  break; //inoffizieller Anrufkanal SSB (USB)	
			 case 1515: freq=27.1450; break;
			 
			 case 16:  freq=27.1550;  break; //Funkverkehr mit und zwischen Wasserfahrzeugen
			 case 17:  freq=27.1650;  break; //Kanal wird von daenischen Schwertransportfahrern in Deutschland und Daenemark benutzt
			 case 18:  freq=27.1750;  break;
			 case 19:  freq=27.1850;  break; //empfohlener Fernfahrerkanal (FM)/oft von Walkie-Talkies genutzt/teilweise auch als Notrufkanal angegeben/auch von Babyfonen genutzt	
			 case 1919: freq=27.1950; break;
										
			 case 20:  freq=27.2050; break; //zum Antennenabgleich genutzte Mitte bei 40-Kanal-Geraeten, 
										    //#wird in oesterreich sehr oft fuer Schwertransportfahrten benutzt

		 	//## 40 chan devices
			 case 21:  freq=27.2150;  break; //tuerkischer Anrufkanal in Deutschland und Europa (FM)	
			 case 22:  freq=27.2250;  break; //oft von Walkie-Talkies genutzt, auch von Babyfonen genutzt, wird auch als Anrufkanal fuer rumaenische Fernlastfahrer verwendet
			 case 23:  freq=27.2550;  break; //Die Kanaele 23, 24, 25 sind sog. Dreher, sie folgen nicht dem aufsteigenden 10-kHz-Raster	
			 case 24:  freq=27.2350;  break; //Datenkanal (D)
			 case 25:  freq=27.2450;  break; //Datenkanal (D), USB ROS Intern
			 case 26:  freq=27.2650;  break;
			 case 27:  freq=27.2750;  break;
			 case 28:  freq=27.2850;  break; //Kanal wird von polnischen Fernfahrern in Deutschland benutzt, Anrufkanal in Polen, wobei allgemein die CB-Kanalfrequenz in Polen um 5 kHz niedriger ist
			 case 29:  freq=27.2950;  break; //Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ber eine Internetverbindung in Deutschland	
			 case 30:  freq=27.3050;  break; //inoffizieller DX-Kanal (FM), Anrufkanal fuer Funker aus dem ehemaligen Jugoslawien
			 case 31:  freq=27.3150;  break; //inoffizieller DX-Kanal (FM)
			 case 32:  freq=27.3250;  break;
			 case 33:  freq=27.3350;  break;
			 case 34:  freq=27.3450;  break; //freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland
			 case 35:  freq=27.3550;  break; //oeffentlicher Kanal
			 case 36:  freq=27.3650;  break; //Datenkanal USB ROS international
			 case 37:  freq=27.3750;  break; //Gateway-Kanal oesterreich, FM	
			 case 38:  freq=27.3850;  break; //inoffizieller internationaler DX-Kanal (LSB)
			 case 39:  freq=27.3950;  break; //Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland	
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
			case 52:  freq=27.6750; break; //Datenkanal (D)(FM)
			case 53:  freq=27.6850; break; //Datenkanal (D)(FM)	
			case 54:  freq=27.6950; break; 
			case 55:  freq=27.7050; break;
			case 56:  freq=27.7150; break;
			case 57:  freq=27.7250; break;
			case 58:  freq=27.7350; break;
			case 59:  freq=27.7450; break;
			case 60:  freq=27.7550; break;
            case 61:  freq=26.7650; break; //Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland	
			case 62:  freq=26.7750; break;
			case 63:  freq=26.7850;	break;
			case 64:  freq=26.7950; break;
			case 65:  freq=26.8050; break;
			case 66:  freq=26.8150; break;
			case 67:  freq=26.8250;	break;
			case 68:  freq=26.8350; break;
			case 69:  freq=26.8450; break;
			case 70:  freq=26.8550; break;
			case 71:  freq=26.8650; break; //Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland	
			case 72:  freq=26.8750; break;
			case 73:  freq=26.8850; break;
			case 74:  freq=26.8950; break;
			case 75:  freq=26.9050; break;
			case 76:  freq=26.9150; break; //Datenkanal (D)(FM)
			case 77:  freq=26.9250; break; //Datenkanal (D)(FM)
			case 78:  freq=26.9350; break;
			case 79:  freq=26.9450; break;
			case 80:  freq=26.9550; break; //Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland */
			case 81:  return 0; break; 
			//default:  freq=26.9650; printf ("\nDefault: CB chan = 1 %f \n", &freq); break;
            return freq;	
	    
	}
	
}



//------------------------------------// 
// Voids
// FM ones
	
void handSig ()
{
		exit (0);
}

void modulate (int m)
{
		ACCESS (CM_GP0DIV) == (0x5A << 24) + 0x4D72 + m;
}

void getRealMemPage (void** vAddr, void** pAddr)
{
		void* a = valloc (4096);
    
		((int*)a)[0] = 1;  // use page to force allocation
    
		mlock (a, 4096);  // lock into ram
    
		*vAddr = a;  //  we know the virtual address now
    
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
                  0x01000000,  //length
                  PROT_READ|PROT_WRITE,
                  MAP_SHARED,
                  mem_fd,
                  0x20000000);  //base

   if ((int)allof7e == -1) exit (-1);

   SETBIT(GPFSEL0, 14);
   CLRBIT(GPFSEL0, 13);
   CLRBIT(GPFSEL0, 12);
   struct GPCTL setupword = {6, 1, 0, 0, 0, 1,0x5A};

   ACCESS (CM_GP0CTL) == *((int*)&setupword);
    
}

///------------------------------------
//relevant for transmitting stuff 

void playWav (char *filename, int samplerate)
{
    // after getting filename insert then open
	lseek (fp, 0L, SEEK_SET);
	int sz = lseek (fp, 0L, SEEK_END); 
    
    short* data = (short*)malloc (sz);
    
    if (filename [0] != NULL) 
    {
        fp = open (filename, 'r');
        printf ("if falename != NULL");
    }   
   /*
    if (channel = 2) // bool stereo = true          from git just trying
        {
         StereoModulator* sm = new StereoModulator (new RDSEncoder(new Outputter(152000)));
         ss = new StereoSplitter
         ( 
        // left
        new PreEmp (samplerate, new Resamp (samplerate, 152000, sm->getChannel(0))), 
        // Right
        new PreEmp (samplerate, new Resamp (samplerate, 152000, sm->getChannel(1))) );
        }
        else { ss = new Mono (new PreEmp (samplerate, new Outputter (samplerate))); }
    */
    for (int i = 0; i<22; i++) 
    { 
        read (fp, &data, 2);  // read past header  (or sz instead on 2 ?)
        printf ("for i=0: read fp ");
        
    }
    
    while (readBytes = read (fp, &data, 1024)) 
    {
        
        float value = data[i]*4*volume;  // modulation index (AKA volume)
        float fmconstant = (samplerate*50.0E-6);  // for pre-emphisis filter, 50us time constant
        int clocksPerSample = (22500/samplerate*1400.0);  // for timing
        
        datanew = ((float)(*data)/32767.0f); //some constant for modulation ??
        
        float sample = datanew + (dataold-datanew)/(1-fmconstant);  // fir of 1 + s tau
        float dval = sample*15.0;  // actual transmitted sample,  15 is bandwidth (about 75 kHz)
        
        int intval = (int)(round (dval));  // integer component
        float frac = ((dval - (float)intval)/2 + 0.5);
        unsigned int fracval = (frac*clocksPerSample);
        
        //problem still with .v & .p endings for struct!! 
        //time++;
        bufPtr++;
              

        while (ACCESS(DMABASE + 0x04 & ~ 0x7F) ==  (int)(instrs[bufPtr].p) );  // CurBlock 0x04 of struct PageInfo
        usleep (1000);
        
        // Create DMA command to set clock controller to output FM signal for PWM "LOW" time
        (struct CB*)(instrs[bufPtr].v))->SOURCE_AD = ((int)constPage.p + 2048 + intval*4-4);
       
        bufPtr++;
        
        //while (ACCESS(DMABASE + 0x04) ==  (int)(instrs[bufPtr].p)); 
        usleep (1000);
        
        // Create DMA command to delay using serializer module for suitable time
        ((struct CB*)(instrs[bufPtr].v))->TXFR_LEN = clocksPerSample-fracval;
        
        bufPtr++;
        
        while (ACCESS(DMABASE + 0x04) ==  (int)(instrs[bufPtr].p)); 
        usleep (1000);
        
        // Create DMA command to set clock controller to output FM signal for PWM "HIGH" time.
        ((struct CB*)(instrs[bufPtr].v))->SOURCE_AD = ((int)constPage.p + 2048 + intval*4+4);
        
        while (ACCESS(DMABASE + 0x04) ==  (int)(instrs[bufPtr].p));
        usleep (1000);
        // Create DMA command for more delay.
       ´((struct CB*)(instrs[bufPtr].v))->TXFR_LEN = fracval;
       
        bufPtr = (bufPtr+1) % (BUFFERINSTRUCTIONS); // [1024] for buffer
        
        dataold = datanew;
        //ss->consume (data, readBytes);// ss-> for stereo
        printf (" while readbytes");  
    }
    
   close (fp);
   close (filename);
   printf (" while closing filenames");  
}
 
void unSetupDMA ()
{
	
	struct DMAregs* DMA0 = (struct DMAregs*)(ACCESS(DMABASE));
	DMA0->CS == 1<<31;  // reset dma controller
	printf("\nExiting...\n");
	exit (-1);
}

void setupDMA (float freq)
{
	atexit (unSetupDMA);
	signal (SIGINT,  handSig);
	signal (SIGTERM, handSig);
	signal (SIGHUP,  handSig);
	signal (SIGQUIT, handSig);
	
	// allocate a few pages of ram
  	getRealMemPage (&constPage.v, &constPage.p);
	int centerFreqDivider = (int)((500.0/freq) * (float)(1<<12) + 0.5);
	// make data page contents - it's essientially 1024 different commands for the
	// DMA controller to send to the clock module at the correct time
	for (int i=0; i<1024; i++)
	{
	    ((int*)(constPage.v))[i] = (0x5a << 24) + centerFreqDivider - 512 + i;
	}
   

	while (instrCnt < 1024) //BUFFERINSTRUCTIONS
	{
     getRealMemPage (&instrPage.v, &instrPage.p);
     
     // make copy instructions
	 struct CB* instr0 = (struct CB*)instrPage.v;
     
     for (int i = 0; i<4096 / sizeof (struct CB); i++) 
     {
         
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
         
        if (i%2) 
	    {
         instr0->DEST_AD = CM_GP0DIV;
         instr0->STRIDE = 4;
         instr0->TI = (1<<26) ;
        }
      
       if (instrCnt!=0) ((struct CB*)(instrs[instrCnt-1].v))->NEXTCONBK = (int)instrs[instrCnt].p;
       instr0++;
       
       
       instrCnt++;
       
     } 
     
   }
   
   ((struct CB*)(instrs[1023].v))->NEXTCONBK = (int)instrs[0].p;
   
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
   printf ("unSetupDMA done");
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
	printf ("Freq: %f , Timing: %d \n", RfSample.Frequency, RfSample.WaitForThisSample);
	
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

    if (argc>1)
 	{
	  printf ("\nChecking Path... \n");
      setupfm (); // gets filename & path

	  printf ("\nSetting up DMA... \n");
     // setupDMA (freq);
      setupDMA (argc>2 ? atof (argv[2]):100.00000); // default freq, maybe do input here? 
	  
	  printf ("\nTesting Samplerate... \n");
	  //playWav (argv[1], samplerate);
      playWav (argv[1], argc>3 ? atof (argv[3]):22500); // <--- in 22.5 kHz, should be same as AM!!
     
	  printf ("\nNow transmitting... \n");
    } 
	else fprintf (stderr, "\nUse Parameters to run: [filename] [freq] [samplerate] [mod (fm/am)] \nWhere wav-file is 16-bit @ 22500 Hz Mono.\nSet wavfile to '-' to use stdin.\nFrequency is between 1-500.0000 [MHz] (default 100.0000)\nYou can play an empty file to transmit silence. \n");
	
return modulationfm;
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
    
     
    nb_samples = (readcount/channels);
    
    float FactAmplitude = 2.0; 
	printf ("Factamplitude: %f \n", FactAmplitude);
			
	// log Modulation 
	float A = 87.7f; // compression parameter, stauchung
	printf ("Compression prameter A: %f \n", A);
//------------------
	if (argc=4) 
	{
       printf ("filefreqtiming test");
		//FileFreqTiming = open (outfilename, O_CREAT | O_WRONLY | O_TRUNC, 0644); //  O_RDWR
	}
	else 
	{
		outfilename = (char *) malloc (128);// not sure about that in else or what it does
		sprintf (outfilename, "%s", "out.ft");
	}
//-------                           
    if (!(filename = open (filename, SFM_READ, &sfinfo)))
    {   // Open failed so print an error message. 
        printf ("\nNot able to open input file %s \n", filename);
        // Print the error message from libsndfile. 
        return  1;
    }
//-----------------
	if (samplerate != 22500) //(sfinfo.samplerate != 22500) 
	{ 
		printf ("\nInput samplerate must be 22.5 [kHz] (mono)! \n");
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
	  
	  printf ("\n nb_samples: %f \n", nb_samples);
	  
	  for (k = 0 ; k < nb_samples ; k++)
	  {
	        if (channels != 1) printf ("File is NOT mono 1 Channel! \n");

			x = data[k*channels];
			if (channels == 1)
			{
				// stereo file, avg left + right --> should be mono at 22.5kHz
				x += data[k*channels+1];
				x /= 2; // maybe *2 to make a dual mono and not doing stereo in half!
				return x;
			}
			

			//maybe here am option for amplitude factor input!?
			
			
			ampf = (x/32767.0f);
			printf ("ampf1: %f \n", ampf);
			
      		ampf = (fabs(ampf) < 1.0f/A) ? A*fabs(ampf)/(1.0f+ln(A)) : (1.0f+ln(A*fabs(ampf)))/(1.0f+ln(A)); //compand
			printf ("compand ampf2: %f \n", ampf);	
			
			x = (int)(round(ampf*32767.0f));
			printf ("new x: %f \n", x);
			
		    factorizer = (x*32767.0f*FactAmplitude);
			printf ("factorizer: %f \n", factorizer);
			
			sampler = (1E9/samplerate); //44.444
			printf ("sampler: %f \n", sampler);
			
			WriteTone (factorizer, sampler); // somehow input freq here ?!?
            printf ("\nNow writing tone in AM... \n");
            
	    }    
	    
    }
    printf ("Reading file: %s \n", filename);
    printf ("Freq: %f \n", freq);
	printf ("Sample Rate: %d \n", samplerate);
	printf ("Channels: %d \n", channels);
    printf ("Writing file: %s \n", outfilename);
 
    // Close input and output files 
    fclose (FileFreqTiming);
    fclose (filename); 
    printf ("\nFile saved! \n");
    
    return channels, ampf, x, factorizer, sampler;
	//return 0;
}

int modulationselect ()
{
    
	printf ("Choose your Modulation [1] FM // [2] AM // [3] Exit : ");
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

    *sfp = fopen ("csvpmr.csv","r");// readonly!
    *dfp = fopen ("csvwriter.csv","w+"); // with + it updates , if exists overwrites
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
    
    return 0;

}

int GetUserInput () //my menu-assistent
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
			case 4: printf ("\nShell - Commandline: \n");
			        int main (int argc, char **argv); // go back to cmd if you want
					break;
			
			case 5: printf ("\nExiting... \n");
					exit (-1);
					break;
			
			//default: printf ("\nDefault = 1 \n"); break;
			 
    }

    return modeselect;
}


int main (int argc, char **argv) // arguments for global use must! be in main
{

   argv[0] = "pifunk"; // for custom  programename, default is the fiename
   printf ("%s: ", argv[0]);
   
   //scanf  ("%s %f %d %s", argv[1], argv[2], argv[3], argv[4]); //direct input if needed
   
   char *filename = argv[1];
    
   // atoll() is meant for integers & it stops parsing when it finds the first non-digit
   // atof () or strtof () is for floats. Note that strtof () requires C99 or C++11
             
   float freq = strtof (argv[2], NULL); //float only accurate to .4 digits idk why, from 5 it will round ?!
   int samplerate = atof (argv[3]); //maybe check here on != 22500 on 16 bits as fixed value (eventually allow 48k)-> otherwise in dma or playwav func
   
   char *mod = argv[4];
   int volume = argv[5]; // argc>4 ? atoi(argv[5]):4  => (atoi gives the value of a string) in playwav possible
   int gain = atoi (argv[5]);
   char *callsign = argv[6];
   //-- for debugging or information :)
   printf ("\nArguments(argc): %d /Programm(0): %s / File(1): %s \nFreq(2): %s / Samplerate(3): %s / Modulation(4): %s / Volume(5): %d / Gain: %d \n", argc, argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], gain);
   printf ("&Adresses-> Arguments: %p / Name: %p \nFile: %p / Freq: %p \nSamplerate: %p / Modulation: %p / Volume: %p / Gain: %p \n", &argc, &argv[0], &argv[1], &argv[2], &argv[3], &argv[4], &argv[5], &gain);
   printf ("*Pointers-> argc: %p / Name: %p / File: %p / Freq: %p / Samplerate: %p / Modulation: %p / Volume: %p \n", argc, *argv[0], *argv[1], *argv[2], *argv[3], *argv[4], *argv[5]);
   
  //---
  
   infos (); //information, disclaimer
   timer (); //local time
  
   char callname ()
   {

        if (argv[6] == NULL)
        {
        printf ("\nType in your callsign: ");
	    scanf  ("%s", callsign);
	    printf ("\nYour callsign is: %s \n", *callsign);
        }
        else
        {
        *callsign = "callsign"; //default callsign
        printf ("\nUsing Default callsign: %s \n", *callsign);
        }
        printf ("Adress %p , Pointer %p \n", &callsign, *callsign);
	    return callsign, &callsign, *callsign;
	
    }
 //---  
   //if (argc=0||NULL) printf ("No Arguments ..\n "); return -1;

   if (argc=1 & !strcmp (argv[1], "menu"))  
   {
      printf ("\nMenu/Assistent activated! \n"); 
      GetUserInput (); //  to menu
   }
   else if (argc=1 & !strcmp (argv[1], "help"))  
   {
     int infos ();
     printf ("\nUse Parameters to run: [filename] [freq] [samplerate] [mod (fm/am)] volume or [menu] or [help]! *.wav-file must be 16-bit @ 22500 [Hz] Mono \n");
   }
   else if (argc=5) 
   { 
            printf ("Checking File: %s \n", argv[1]); 
            printf ("String-Conversion to Freq: %f [MHz] @ Samplerate: %d [Hz] \n", freq, samplerate);
            printf ("Checking Gain/Volume: %d \n", gain); 
            printf ("Checking Modulation: %s \n", mod); 
            if (mod != NULL) // may be put it outside as a single func?
            {
                if (!strcmp (mod, "fm"))
                {
               
                printf ("Pushing args to FM Modulator... \n"); 
                int modulationfm (int argc, char **argv); // idk if here to jump to the modulator or just parse it?!
                return 0; // here bool with true/false instead of compare?
                }
                else if (!strcmp (mod, "am"))
                {
                
                printf ("Pushing args to AM Modulator... \n"); 
                // !!! jumps direcly to func?? maybe sth else here ?
                int modulationam (int argc, char **argv); 
                return 0;
                }
            }
            else 
            {
             printf ("No Modulation specified! fm or am? \n");
             
             return -1;
            }
    
    }
    else if (argc>5)
    {
       callname ();
    }
    else 
    {
        fprintf (stderr, "Argument-Error! \nUse Parameters to run: [filename] [freq] [samplerate] [mod (fm/am)] or [menu] or [help]!  *.wav-file must be 16-bit @ 22500 [Hz] Mono.\nSet wavfile to '-' to use stdin.\nFrequency is between 1-500.0000 [MHz] (default 100.0000)\nYou can play an empty file to transmit silence. \n");
        //GetUserInput ();
    }
    
    //into the fm or an function to run your file
    printf ("End on main \n"); 
    printf ("Returning args 0 to %d ... \n", argc); 
    return argc, argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], filename, freq, samplerate, mod, gain;
    //return 0;
}


//EOF











