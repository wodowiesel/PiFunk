 #ifndef _i2c_h_
 #define _i2c_h_

 //#include "rpiGpio.h"
 #include <stdarg.h>
 #include <stdlib.h>
 #include <string.h>
 #include <fcntl.h>
 #include <sys/mman.h>
 #include <unistd.h>
 #include <errno.h>
 #include <stdio.h>
 #include <time.h>

 #define I2C_MAP_SIZE                BSC_DEL_OFFSET

 #define I2C_DEFAULT_FREQ_HZ         100000

 #define NSEC_IN_SEC                 1000000000

 #define CLOCKS_PER_BYTE             9

 #define I2C_C                       *(gI2cMap + BSC_C_OFFSET / sizeof(uint32_t))

 #define I2C_DIV                     *(gI2cMap + BSC_DIV_OFFSET / sizeof(uint32_t))

 #define I2C_A                       *(gI2cMap + BSC_A_OFFSET / sizeof(uint32_t))

 #define I2C_DLEN                    *(gI2cMap + BSC_DLEN_OFFSET / sizeof(uint32_t))

 #define I2C_S                       *(gI2cMap + BSC_S_OFFSET / sizeof(uint32_t))

 #define I2C_FIFO                    *(gI2cMap + BSC_FIFO_OFFSET / sizeof(uint32_t))
 
 #endif /*_I2C_H_*/
