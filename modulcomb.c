// rewritten for own purpose Sources:
// Access from ARM Running Linux, mostly on Raspberry Pi (me B+ rev2)

// 1) Pi-FM version - freqency modulation left/right ← , → 
// wget -O - http://beta.etherpad.org/p/pihackfm/export/txt 2>/dev/null | gcc -lm -std=c99 -g -xc - && ./a.out sound.wav

// 2) Pi-AM version - amplitude modulation up/down ↑ , ↓
// Test program using SNDFILE, see http://www.mega-nerd.com/libsndfile/api.html for API

// python stuff, maybe wrapper too??
//#include <python.h>


// normal includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdint.h>

#include <math.h>
#include <time.h>
#include <errno.h>

#include <assert.h>
#include <malloc.h>
#include <dirent.h>
#include <sndfile.h>

#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

//---------------------------------------------------------------//
#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)
#define	BUFFER_LEN	(8*1024)

#define	ln(x) (log(x)/log(2.718281828459045235f))

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0
#define GPIO_GET *(gpio+13)  // sets   bits which are 1 ignores bits which are 0

#define ACCESS(base) *(volatile int*)((int)allof7e+base-0x7e000000)
#define SETBIT(base, bit) ACCESS(base) |= 1<<bit
#define CLRBIT(base, bit) ACCESS(base) &= ~(1<<bit)

#define CM_GP0CTL (0x7e101070)
#define GPFSEL0 (0x7E200000)
#define CM_GP0DIV (0x7e101074)
#define CLKBASE (0x7E101000)
#define DMABASE (0x7E007000)
#define PWMBASE  (0x7e20C000) /* PWM controller */


//--------------------------------------------------//
int  mem_fd;
char *gpio_mem, *gpio_map;
char *spi0_mem, *spi0_map;
int  FileFreqTiming;
// FileFreqTiming = open(outfilename, O_WRONLY|O_CREAT, 0644);

// I/O access
volatile unsigned *gpio;
volatile unsigned *allof7e;


// Structs

struct PageInfo constPage;   
struct PageInfo instrPage;
struct PageInfo instrs[1024];

struct PageInfo {
		void* p;  // physical address
		void* v;  // virtual address
};



struct GPCTL {
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

struct CB {
		volatile unsigned int TI;
		volatile unsigned int SOURCE_AD;
		volatile unsigned int DEST_AD;
		volatile unsigned int TXFR_LEN;
		volatile unsigned int STRIDE;
		volatile unsigned int NEXTCONBK;
		volatile unsigned int RES1;
		volatile unsigned int RES2;
};

struct DMAregs {
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
	
void handSig() {
		exit(0);
}

void modulate(int m)
{
		ACCESS(CM_GP0DIV) = (0x5a << 24) + 0x4d72 + m;
}

void freeRealMemPage(void* vAddr) {
    
		munlock(vAddr, 4096);  // unlock ram
    
		free(vAddr);
}

void unSetupDMA(){
		printf("Exiting...\n");
		struct DMAregs* DMA0 = (struct DMAregs*)&(ACCESS(DMABASE));
		DMA0->CS =1<<31;  // reset dma controller
}


void getRealMemPage(void** vAddr, void** pAddr) {
		void* a = valloc(4096);
    
		((int*)a)[0] = 1;  // use page to force allocation
    
		mlock(a, 4096);  // lock into ram
    
		*vAddr = a;  //  we know the virtual address now
    
		unsigned long long frameinfo;
    
		int fp = open("/proc/self/pagemap", 'r');
		lseek(fp, ((int)a)/4096*8, SEEK_SET);
		read(fp, &frameinfo, sizeof(frameinfo));
    
		*pAddr = (void*)((int)(frameinfo*4096));
}


void setup_fm()
{

    /* open /dev/mem */
    if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
        printf("can't open /dev/mem \n");
        exit (-1);
    }
    
    allof7e = (unsigned *)mmap(
                  NULL,
                  0x01000000,  //len
                  PROT_READ|PROT_WRITE,
                  MAP_SHARED,
                  mem_fd,
                  0x20000000  //base
              );

    if ((int)allof7e==-1) exit(-1);

    SETBIT(GPFSEL0 , 14);
    CLRBIT(GPFSEL0 , 13);
    CLRBIT(GPFSEL0 , 12);

 
    struct GPCTL setupword = {6/*SRC*/, 1, 0, 0, 0, 1,0x5a};

    ACCESS(CM_GP0CTL) = *((int*)&setupword);
}



void playWav(char* filename, float samplerate)
{
    int fp= STDIN_FILENO;
    if(filename[0]!='-') fp = open(filename, 'r');
	//->what are the funcs doing? probably save/mem space?
    // was commented before but opened to see behavior of infos
	int sz = lseek(fp, 0L, SEEK_END); 
    lseek(fp, 0L, SEEK_SET);
    short* data = (short*)malloc(sz);
    read(fp, data, sz);
	//
    
    int bufPtr=0;
    float datanew, dataold = 0;
    short data;
    
    for (int i=0; i<22; i++)
       read(fp, &data, 2);  // read past header
    
    while (read(fp, &data, 2)) {
        float fmconstant = samplerate * 50.0e-6;  // for pre-emphisis filter,  50us time constant
        int clocksPerSample = 22500.0/samplerate*1400.0;  // for timing
        
        datanew = (float)(data)/32767;
        
        float sample = datanew + (dataold-datanew) / (1-fmconstant);  // fir of 1 + s tau
        float dval = sample*15.0;  // actual transmitted sample,  15 is bandwidth (about 75 kHz)
        
        int intval = (int)(round(dval));  // integer component
        float frac = (dval - (float)intval)/2 + 0.5;
        unsigned int fracval = frac*clocksPerSample;
         
        bufPtr++;
        while( ACCESS(DMABASE + 0x04 /* CurBlock*/) ==  (int)(instrs[bufPtr].p)) usleep(1000);
        ((struct CB*)(instrs[bufPtr].v))->SOURCE_AD = (int)constPage.p + 2048 + intval*4 - 4 ;
        
        bufPtr++;
        while( ACCESS(DMABASE + 0x04 /* CurBlock*/) ==  (int)(instrs[bufPtr].p)) usleep(1000);
        ((struct CB*)(instrs[bufPtr].v))->TXFR_LEN = clocksPerSample-fracval;
        
        bufPtr++;
        while( ACCESS(DMABASE + 0x04 /* CurBlock*/) ==  (int)(instrs[bufPtr].p)) usleep(1000);
        ((struct CB*)(instrs[bufPtr].v))->SOURCE_AD = (int)constPage.p + 2048 + intval*4+4;
        
        bufPtr=(bufPtr+1) % (1024);
        while( ACCESS(DMABASE + 0x04 /* CurBlock*/) ==  (int)(instrs[bufPtr].p)) usleep(1000);
        ((struct CB*)(instrs[bufPtr].v))->TXFR_LEN = fracval;
        
        dataold = datanew;
    }
    close(fp);
}


void setupDMA( float centerFreq ){

	atexit(unSetupDMA);
	signal (SIGINT, handSig);
	signal (SIGTERM, handSig);
	signal (SIGHUP, handSig);
	signal (SIGQUIT, handSig);

	// allocate a few pages of ram
	getRealMemPage(&constPage.v, &constPage.p);
   
	int centerFreqDivider = (int)((500.0 / centerFreq) * (float)(1<<12) + 0.5);
   
	// make data page contents - it's essientially 1024 different commands for the
	// DMA controller to send to the clock module at the correct time
	for (int i=0; i<1024; i++)
		((int*)(constPage.v))[i] = (0x5a << 24) + centerFreqDivider - 512 + i;
   
	 int instrCnt = 0;
   
	while (instrCnt<1024) {
     getRealMemPage(&instrPage.v, &instrPage.p);
     
     // make copy instructions
     struct CB* instr0= (struct CB*)instrPage.v;
     
     for (int i=0; i<4096/sizeof(struct CB); i++) {
       instrs[instrCnt].v = (void*)((int)instrPage.v + sizeof(struct CB)*i);
       instrs[instrCnt].p = (void*)((int)instrPage.p + sizeof(struct CB)*i);
       instr0->SOURCE_AD = (unsigned int)constPage.p+2048;
       instr0->DEST_AD = PWMBASE+0x18 /* FIF1 */;
       instr0->TXFR_LEN = 4;
       instr0->STRIDE = 0;
	   
	    // what does it do?
       instr0->NEXTCONBK = (int)instrPage.p + sizeof(struct CB)*(i+1);
	   
       instr0->TI = (1/* DREQ  */<<6) | (5 /* PWM */<<16) |  (1<<26/* no wide*/) ;
       instr0->RES1 = 0;
       instr0->RES2 = 0;
       
       if (i%2) {
         instr0->DEST_AD = CM_GP0DIV;
         instr0->STRIDE = 4;
         instr0->TI = (1<<26/* no wide*/) ;
        }
       
       if (instrCnt!=0) ((struct CB*)(instrs[instrCnt-1].v))->NEXTCONBK = (int)instrs[instrCnt].p;
       instr0++;
       instrCnt++;
    }
   }
    ((struct CB*)(instrs[1023].v))->NEXTCONBK = (int)instrs[0].p;
   
    // set up a clock for the PWM
    ACCESS(CLKBASE + 40*4 /*PWMCLK_CNTL*/) = 0x5A000026;
    usleep(1000);
    ACCESS(CLKBASE + 41*4 /*PWMCLK_DIV*/)  = 0x5A002800;
    ACCESS(CLKBASE + 40*4 /*PWMCLK_CNTL*/) = 0x5A000016;
    usleep(1000); 

    // set up pwm
    ACCESS(PWMBASE + 0x0 /* CTRL*/) = 0;
    usleep(1000);
    ACCESS(PWMBASE + 0x4 /* status*/) = -1;  // clear errors
    usleep(1000);
    ACCESS(PWMBASE + 0x0 /* CTRL*/) = -1; //(1<<13 /* Use fifo */) | (1<<10 /* repeat */) | (1<<9 /* serializer */) | (1<<8 /* enable ch */) ; 
    //what does this parameters mean ???
   
    usleep(1000);
   ACCESS(PWMBASE + 0x8 /* DMAC*/) = (1<<31 /* DMA enable */) | 0x0707;
   
    //activate dma
    struct DMAregs* DMA0 = (struct DMAregs*)&(ACCESS(DMABASE));
    DMA0->CS =1<<31;  // reset
    DMA0->CONBLK_AD=0; 
    DMA0->TI=0; 
    DMA0->CONBLK_AD = (unsigned int)(instrPage.p);
    DMA0->CS =(1<<0)|(255 <<16);  // enable bit = 0, clear end flag = 1, prio=19-16
}

	// AM ones


void WriteTone(double Frequency,uint32_t Timing)
{
	typedef struct {
		double Frequency;
		uint32_t WaitForThisSample;
	} 
	samplerf_t;
	samplerf_t RfSample;

	RfSample.Frequency=Frequency;
	RfSample.WaitForThisSample=Timing; //in 100 of nanoseconds
	printf("Freq =%f Timing=%d\n",RfSample.Frequency,RfSample.WaitForThisSample);
	
	if (write(FileFreqTiming,&RfSample,sizeof(samplerf_t)) != sizeof(samplerf_t)) {
		fprintf(stderr, "Unable to write sample\n");
	}
}

//---------------------//
// main progs



// shell- MENU (probably with input & cases


int GetUserInput( void )
{
    int menu;

    for (;;)
    {
        menu = 0;
     
		printf ("Choose your Radio-Modulation: \n");
		printf ("[1] FM // [2] AM // [3] CH-Mode // [4] Exit \n");
		
        if ( scanf( "%d", &menu ) != 1 )
            exit( 1 );

        if ( menu >= 1 && menu <= 4 )
            return menu;

        printf( "Enter choice from 1 to 3 only\n" );
    }
}

int main( void )
{
    int menu = 0;

    while ( menu != 4 )
    {
        menu = GetUserInput();
        switch ( menu )
        {
            case 1:  printf( "\n*** You selected 1 for FM ***\n\n" ); 
					 //start fm function
					 maifmn(int argc, char **argv)
					 break;
			
			
            case 2:  printf( "\n*** You selected 2 for AM ***\n\n" ); 
					 //start fm function
					 mainam(int argc, char **argv)
					 break;
					
		    case 3:  printf( "CH-Mode PMR a+d \n" );
			
					channelpmr(freq);
					 // return 0;
					 break;
					
            case 4:  printf( "Exiting... \n" );
					 // return 0;
					 break;
        }
    }
	return 0;
}

//FM
int maifm(int argc, char **argv)
{
	printf( "\n*** Type in  FM Frequency:  ***\n\n" ); 
	
    if (argc>1) {
      setup_fm();
	  printf( "\n*** Type in  FM Frequency (default=100)  ***\n\n" ); 
	  
      setupDMA(argc>2?atof(argv[2]):100.0000); // default freq, maybe do input here 
	  
      playWav(argv[1], argc>3?atof(argv[3]):22500); // <--- in 22,5 kHz, should be same as AM!!
	  
	  printf("Now receiving...\n");
    } 
	else    {
			fprintf(stderr, "Usage:   program wav-file.wav [freq] [sample rate] \n\nWhere wav-file is  16 bit 22.5kHz Mono.  Set wavfile to '-' to use stdin.\nFrequency is between 1-500.0000 [MHz] (default 100.0000)\nSample-rate of wav-file in Hz\n\nPlay an empty file to transmit silence\n");
			}
    printf("Listening...!\n");
    return 0;
}

//AM

int mainam(int argc, char **argv) {
	
	float data [2*BUFFER_LEN] ;
	float data_filtered[2*BUFFER_LEN] ; // we generate complex I/Q samples
	SNDFILE		*infile, *outfile ;
	SF_INFO		sfinfo ;

    int		readcount, nb_samples ;
    char	*infilename  ;
    char	*outfilename  ;
	int k   ;
	float x ;
	
	printf( "\n*** Type in  AM Frequency:  ***\n\n" ); 

	if( argc < 2 ) {
		printf("Usage : %s in.wav [out.wav]\n", argv[0]);
		return(1);
	}
	infilename = argv[1];
	if( argc == 3 ) {
		outfilename = argv[2];
	} else {
		outfilename = (char *)malloc( 128 );
		sprintf( outfilename, "%s", "out.ft");
	}
    if (! (infile = sf_open (infilename, SFM_READ, &sfinfo)))
    {   /* Open failed so print an error message. */
        printf ("Not able to open input file %s.\n", infilename);
        /* Print the error message from libsndfile. */
        puts (sf_strerror (NULL));
        return  1;
    }

	if( sfinfo.samplerate != 22500 ) {   // not really 48k! 22,5k but maybe dual-mono
		printf("Input rate must be 22.5kHz (mono)!\n");
		return 1;
	}

	FileFreqTiming = open(outfilename, O_WRONLY|O_CREAT, 0644);

	/** **/
	printf ("Reading file : %s\n", infilename );
	printf ("Sample Rate :  %d\n", sfinfo.samplerate);
	printf ("Channels :	    %d\n", sfinfo.channels);
	printf ("Writing file : %s\n", outfilename );

	/* While there are frames in the input file, read them, process
    ** them and write them to the output file */

    while ((readcount = sf_readf_float(infile, data, BUFFER_LEN)))
    {
		nb_samples = readcount / sfinfo.channels;
		for( k=0 ; k < nb_samples ; k++ ) {
			x = data[k*sfinfo.channels];
			if( sfinfo.channels == 1 ) { // 1 chan
				// stereo file, avg left + right --> should be mono at 22.5kHz
				x += data[k*sfinfo.channels+1];
				x /= 2; // maybe *2 to make a dual mono and not doing stereo in half!
				
			}
			printf("%f \n",x);
			
			//maybe hier am option for amplitude factor input!?
			printf ("factamplude input print %s\n", outfilename );
			float FactAmplitude=2.0; // To be analyzed more deeply !
			
			/* log Modulation, 
			 double A = 87.7f; // compression parameter,  stauchung
			 
			 
						double ampf=x/32767.0;
      						 ampf = (fabs(ampf) < 1.0f/A) ? A*fabs(ampf)/(1.0f+ln(A)) : (1.0f+ln(A*fabs(ampf)))/(1.0f+ln(A)); //compand
							 
						x= (int)(round(ampf * 32767.0f));
			*/
			
			WriteTone(x*32767*FactAmplitude,1e9/22500); //

		}

    }

    /* Close input and output files */
    sf_close (infile) ;
	close(FileFreqTiming);
	
	printf("File saved!\n");
	return 0;
}
//------------------//

// pmr stuff to do!


/


// Channelmode 
//PMR

int channelmodepmr(int freq)
{
	
	
	switch 
	{
		
	 case [1]: freq=446.00625 //Standard
	 case [2]: freq=446.01875 //Geocaching
	 case [3]: freq=446.03125// random
	 case [4]: freq=446.04375 //at 3-chan-PMR-devices its ch. 2
	 case [5]: freq=446.05625 //Contest
	 case [6]: freq=446.06875 //Events
	 case [7]: freq=446.08125 //at 3-chanl-PMR-devices it's ch. 3
	 case [8]: freq=446.09375 //random talk stuff

	// dpmr digital new since 28.09.2016
	// erweitert um extra 8 chan
	// 12.5 kHz steps too
	
	 case [a]:  freq=446.10312 // 6.25 kHz steps
	 case [9]:  freq=446.10625
	 case [10]: freq=446.11875
	 case [11]: freq=446.13125
	 case [12]: freq=446.14375
	 case [13]: freq=446.15625
	 case [14]: freq=446.16875
	 case [15]: freq=446.18125
	 case [16]: freq=446.19375

	}
}


int channelmodecb(int freq)
{
	
	
	switch 
	{
		/* */
			case [1]:   freq=26.965 //empfohlener Anrufkanal (FM)	
			case [2]:   freq=26.975 //inoffizieller Berg-DX-Kanal (FM)
			case [3]:   freq=26.985 
			case [4]:   freq=27.005 //empfohlener Anrufkanal (AM)/Anrufkanal Feststationen (AM)
			case [5]:   freq=27.015 //Kanal wird von italienischen Fernfahrern in Deutschland und Italien benutzt.
			case [6]:   freq=27.025 //Datenkanal (D)
			case [7]:   freq=27.035 //Datenkanal (D)
			case [7A]:  freq=27.045 
			
			case [8]:   freq=27.055
			case [9]:   freq=27.065 //Fernfahrerkanal (AM)/weltweiter Notrufkanal	EMG
			case [10]:  freq=27.075 //Antennen-abgleich - halbe chan-anzahl!! ansonsten nr 20 oder 40

			/* # Bei genauerer Betrachtung obiger Tabelle fallen einige Stellen auf, 
			#an denen sich Nachbarkanaele nicht um 10 kHz, sondern um 20 kHz unterscheiden. 
			# Die dazwischen versteckten Kanaele werden ueblicherweise folgenderweise bezeichnet:
			# Diese Kanaele sind in den meisten Laendern nicht fuer CB-Funk zugelassen. 
			# Allerdings werden sie in einigen Laendern, darunter auch Deutschland[3], fuer andere Zwecke
			# wie z. B. Funkfernsteuerungen, Babyphones, kabellose Tastaturen und Maeuse u. a. verwendet.
			*/

			 case [11]:  freq=27.085 //freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland
			 case [11A]: freq=27.095 //Eurobalise-Energieversorgung
			 
			 case [12]:  freq=27.105 
			 case [13]:  freq=27.115 
			 case [14]:  freq=27.125 //oft verwendet fuer Spielzeug-Fernsteuerungen (mittels Selektivton)
			 case [15]:  freq=27.135 //inoffizieller Anrufkanal SSB (USB)	
			 case [15A]: freq=27.145 
			 
			 case [16]:  freq=27.155 //Funkverkehr mit und zwischen Wasserfahrzeugen
			 case [17]:  freq=27.165 //Kanal wird von daenischen Schwertransportfahrern in Deutschland und Daenemark benutzt.
			 case [18]:  freq=27.175
			 case [19]:  freq=27.185 //empfohlener Fernfahrerkanal (FM)/oft von Walkie-Talkies genutzt/teilweise
										// auch als Notrufkanal angegeben/auch von Babyfonen genutzt	
			 case [19A]: freq=27.195 
										
			 case [20]:  freq=27.205 //zum Antennenabgleich genutzte Mitte bei 40-Kanal-Geraeten, 
										//#wird in oesterreich sehr oft fuer Schwertransportfahrten benutzt

			//##40ch devices
			 case [21]:  freq=27.215 //tuerkischer Anrufkanal in Deutschland und Europa (FM)	
			 case [22]:  freq=27.225 //oft von Walkie-Talkies genutzt, auch von Babyfonen genutzt,
									// wird auch als Anrufkanal fuer rumaenische Fernlastfahrer verwendet
			 case [23]:  freq=27.255 //Die Kanaele 23, 24, 25 sind sog. Dreher, sie folgen nicht dem aufsteigenden 10-kHz-Raster	
			 case [24]:  freq=27.235 //Datenkanal (D)
			 case [25]:  freq=27.245 //Datenkanal (D), USB ROS Intern.	
			 case [26]:  freq=27.265 
			 case [27]:  freq=27.275 
			 case [28]:  freq=27.285 //Kanal wird von polnischen Fernfahrern in Deutschland benutzt, Anrufkanal in Polen, 
									// wobei allgemein die CB-Kanalfrequenz in Polen um 5 kHz niedriger ist.
			 case [29]:  freq=27.295 //Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ber eine Internetverbindung in Deutschland	
			 case [30]:  freq=27.305 //inoffizieller DX-Kanal (FM), Anrufkanal fuer Funker aus dem ehemaligen Jugoslawien
			 case [31]:  freq=27.315 //inoffizieller DX-Kanal (FM)
			 case [32]:  freq=27.325 
			 case [33]:  freq=27.335 
			 case [34]:  freq=27.345 //freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland
			 case [35]:  freq=27.355 //oeffentlicher Kanal
			 case [36]:  freq=27.365 //Datenkanal USB ROS international
			 case [37]:  freq=27.375 //Gateway-Kanal oesterreich, FM	
			 case [38]:  freq=27.385 //inoffizieller internationaler DX-Kanal (LSB)
			 case [39]:  freq=27.395 //Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland	
			 case [40]:  freq=27.405 //ab Maerz 2016 freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete 
									  //ueber eine Internetverbindung in Deutschland (FM/AM/SSB in D)


			/* ## 80 chan devices
			# Auf den nationalen Zusatzkanaelen 41 bis 80 ist nur die Modulationsart FM erlaubt. 
			# Nachfolgend sind die Frequenzen der nationalen Zusatzkanaele, die im CB-Funk benutzt werden duerfen, aufgelistet: */

			case [41]:  freq=27.565 //Ab Maerz 2016 Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete 
									  //ueber eine Internetverbindung in Deutschland (FM),inoffizieller DX-Kanal (FM)
			case [42]:  freq=27.575 //inoffizieller DX-Kanal (FM)
			case [43]:  freq=27.585 
			case [44]:  freq=27.595 
			case [45]:  freq=27.605 
			case [46]:  freq=27.615
			case [47]:  freq=27.625 
			case [48]:  freq=27.635
			case [49]:  freq=27.645
			case [50]:  freq=27.655 
			case [51]:  freq=27.665 
			case [52]:  freq=27.675 //Datenkanal (D)(FM)
			case [53]:  freq=27.685 //Datenkanal (D)(FM)	
			case [54]:  freq=27.695 
			case [55]:  freq=27.705 
			case [56]:  freq=27.715 
			case [57]:  freq=27.725 
			case [58]:  freq=27.735 
			case [59]:  freq=27.745 
			case [60]:  freq=27.755 

			case [61]:  freq=26.765 //Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland	
			case [62]:  freq=26.775
			case [63]:  freq=26.785	
			case [64]:  freq=26.795
			case [65]:  freq=26.805
			case [66]:  freq=26.815
			case [67]:  freq=26.825	
			case [68]:  freq=26.835
			case [69]:  freq=26.845
			case [70]:  freq=26.855
			case [71]:  freq=26.865 //Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland	
			case [72]:  freq=26.875
			case [73]:  freq=26.885
			case [74]:  freq=26.895
			case [75]:  freq=26.905
			case [76]:  freq=26.915 //Datenkanal (D)(FM)
			case [77]:  freq=26.925 //Datenkanal (D)(FM)
			case [78]:  freq=26.935
			case [79]:  freq=26.945
			case [80]:  freq=26.955 //Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland
			break;
	 
	}
}
/*
##-------------------------------------------------------------------------
## subchannels: (0/1-38) CTSS/ DCS tones !!!

## 0 = all subch. -> base/default
## if subchannels is 0 = all ch. ??? check special stuff
##->read / import csv */


int ctss()
{
	switch 
	{
		/*
	
	*/
	}
}






// EOF

