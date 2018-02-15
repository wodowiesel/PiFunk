
//Pi-AM version - amplitude modulation up/down ↑ ↓

//#include <Python.h>
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

//----------------------------------------------------------------------//
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
//----------------------------------------------------------------------//
int  mem_fd;
char *gpio_mem, *gpio_map;
char *spi0_mem, *spi0_map;


// I/O access
volatile unsigned *gpio;
volatile unsigned *allof7e;

int FileFreqTiming;
// Test program using SNDFILE
// see http://www.mega-nerd.com/libsndfile/api.html for API

void WriteTone(double Frequency,uint32_t Timing)
{
	typedef struct {
		double Frequency;
		uint32_t WaitForThisSample;
	} samplerf_t;
	samplerf_t RfSample;

	RfSample.Frequency=Frequency;
	RfSample.WaitForThisSample=Timing; //en 100 de nanosecond
	//printf("Freq =%f Timing=%d\n",RfSample.Frequency,RfSample.WaitForThisSample);
	if (write(FileFreqTiming,&RfSample,sizeof(samplerf_t)) != sizeof(samplerf_t)) {
		fprintf(stderr, "Unable to write sample\n");
	}

}

int main(int argc, char **argv) {
	float data [2*BUFFER_LEN] ;
	float data_filtered[2*BUFFER_LEN] ; // we generate complex I/Q samples
	SNDFILE      *infile, *outfile ;
	SF_INFO		sfinfo ;

    int			readcount, nb_samples ;
    char	*infilename  ;
    char	*outfilename  ;
	int k   ;
	float x ;

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

	if( sfinfo.samplerate != 48000 ) {
		printf("Input rate must be 48K.\n");
		return 1;
	}

	FileFreqTiming = open(outfilename, O_WRONLY|O_CREAT, 0644);

	/** **/
	printf ("Reading file : %s\n", infilename );
	printf ("Sample Rate : %d\n", sfinfo.samplerate);
	printf ("Channels    : %d\n", sfinfo.channels);
	printf ("----------------------------------------\n");
	printf ("Writing file : %s\n", outfilename );

	/* While there are.frames in the input file, read them, process
    ** them and write them to the output file.
    */

    while ((readcount = sf_readf_float(infile, data, BUFFER_LEN)))
    {
		nb_samples = readcount / sfinfo.channels;
		for( k=0 ; k < nb_samples ; k++ ) {
			x = data[k*sfinfo.channels];
			if( sfinfo.channels == 2 ) {
				// stereo file, avg left + right
				x += data[k*sfinfo.channels+1];
				x /= 2;
			}
			printf("%f \n",x);
			float FactAmplitude=2.0; // To be analyzed more deeply !
			/*
			 double A = 87.7f; // compression parameter
						double ampf=x/32767.0;
      						 ampf = (fabs(ampf) < 1.0f/A) ? A*fabs(ampf)/(1.0f+ln(A)) : (1.0f+ln(A*fabs(ampf)))/(1.0f+ln(A)); //compand
						x= (int)(round(ampf * 32767.0f));
			*/
			WriteTone(x*32767*FactAmplitude,1e9/48000.0);

		}

    }

    /* Close input and output files. */
    sf_close (infile) ;
	close(FileFreqTiming);

	return 0;
}
