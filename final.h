// ECE 532 - Header for Final Project
// Connor Prikkel
// 7/30/24

// locations in memory
#define UART_BASE 0x50000
#define SWITCHES_BASE 0x60000
#define GREEN_LEDS 0x70000
#define BUTTON_BASE 0x80000
#define HEX0_BASE 0x90000
#define HEX1_BASE 0xA0000
#define HEX2_BASE 0xB0000
#define HEX3_BASE 0xC0000
#define HEX4_BASE 0xD0000
#define HEX5_BASE 0xE0000
#define HEX6_BASE 0xF0000
#define HEX7_BASE 0x1F0000
#define TIMER_BASE 0x2F0000
#define I2C_BASE 0x3F0000
#define RED_LEDS 0x4F0000
#define FINAL_GREEN 0x6F0000
// this is also the location of block 1 for sdram
#define SDRAM_BASE 0x10000000
#define BLOCK_TWO 0x10100000
#define BLOCK_THREE 0x10200000
#define DMA_BASE 0x7F0000

// interrupt defs
#define BUTTON_IRQ 1
#define TIMER_IRQ 2

// defs
#define TEN_MS_TICKS 500000
#define HUNDRED_MS_TICKS 5000000
#define TWO_HUNDRED_FIFTY_MS_TICKS 12500000
#define ONE_SEC_TICKS 50000000
// the sdram is size 2^(25)
#define SDRAM_SIZE_WORDS 33554432

// fn. prototypes
void task0(void* pdata);
void task1(void* pdata);
void task2(void* pdata);
void task3(void* pdata);
void task4(void* pdata);
void task5(void* pdata);
void button_isr(void);
void timer_isr(void);
void dma_copy(unsigned short read_addr, unsigned short write_addr, unsigned int num_bytes);
void copy_data(unsigned int *src, unsigned int *dst, unsigned int nwords);
void hex_display(unsigned int val, unsigned int HEX_BASE);
void i2c_init(void);
void eep_write(unsigned short addr, unsigned char data);
unsigned char eep_read(unsigned short addr);

// structs
typedef struct gr{
	unsigned int data;
	unsigned int dir;
	unsigned int intmask;
	unsigned int edge;
	unsigned int outset;
	unsigned int outclear;
} gpio_regs;

typedef struct ti{
	unsigned int status;
	unsigned int control;
	unsigned int periodl;
	unsigned int periodh;
	unsigned int snapl;
	unsigned int snaph;
} timer_regs;

// for uart display
typedef struct ua{
	unsigned int rxdata;
	unsigned int txdata;
	unsigned int status;
	unsigned int control;
	unsigned int divisor;
	unsigned int endofpacket;
} uart_regs;

// for eeprom
typedef struct ep{
	unsigned int tfrcmd;
	unsigned int rxdata;
	unsigned int ctrl;
	unsigned int iser;
	unsigned int isr;
	unsigned int status;
	unsigned int tfrcmdfifolvl;
	unsigned int rxdatafifolvl;
	unsigned int scllow;
	unsigned int sclhigh;
	unsigned int sdahold;
} eep_regs;

// for dma controller
typedef struct dma{
	unsigned int status;
	unsigned int readaddress;
	unsigned int writeaddress;
	unsigned int length;
	unsigned int control;
} dma_regs;
