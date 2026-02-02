/*
 * Connor Prikkel
 * ECE 532 Final Project
 * 7/30/24
 */

#include <stdio.h>
#include "includes.h"
#include "final.h"

// define global var to track green LED state
int gled_state;

// def global var to track HEX0 val
static signed int hex0_val = 0;

// set ptr to addr
eep_regs *ep = I2C_BASE;

// def other ptrs
gpio_regs *btn = BUTTON_BASE;
unsigned int *sdram = SDRAM_BASE;

/* definition of task stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];
OS_STK    task3_stk[TASK_STACKSIZE];
OS_STK    task4_stk[TASK_STACKSIZE];
OS_STK    task5_stk[TASK_STACKSIZE];

/* definition of task priorities */
// assigned by freq, i.e. 1/period, frequent tasks should have numerically lower prio
// tasks cannot have the same prio
#define TASK1_PRIORITY      1
#define TASK2_PRIORITY      2
#define TASK3_PRIORITY      3
#define TASK4_PRIORITY      4
#define TASK5_PRIORITY      5

// define message queue event and param.
#define MSG_SIZE 16
void *msg_buf[MSG_SIZE];
OS_EVENT *msg;

// define semaphore event and param
OS_EVENT *sema;
INT16U cnt;

// menu task
void task1(void* pdata)
{

  // create a local buffer for msgs for subtask 3
  char txbuf[10];

  // task loop
  while (1)
  {
	  // dummy var for user input
	  int x;

	  // print out main menus, allow users to select
	  printf("************************************\n");
	  printf("* ECE 532 Final Project \n");
	  printf("* 7/30/24 \n");
	  printf("* Connor Prikkel, 101671463\n");
	  printf("************************************\n");
	  printf("1) HEX Displays\n");
	  printf("2) EEPROM\n");
	  printf("3) SDRAM\n");
	  printf("4) Green LEDs\n");
	  printf("5) DMA Test\n");
	  printf("Enter Selection> ");

	  // read an int in from the user
	  scanf("%d", &x);

	  // write seperate fns. for submenu tasks
	  // if subtask 1 selected
	  if(x == 1){
		  int hex_num;
		  int hex_data;

		  printf("Which display (0-7)?\n");
		  scanf("%d", &hex_num);
		  printf("What hex value (0-F)?\n");
		  scanf("%x", &hex_data);

		  // update correct hex display
		  switch (hex_num) {

		  case 0: hex_display(hex_data, HEX0_BASE);
		  	  	  // keep track of hex0 for task 3
		  	  	  hex0_val = hex_data;
		  	  	  break;
		  case 1: hex_display(hex_data, HEX1_BASE);
		  		  break;
		  case 2: hex_display(hex_data, HEX2_BASE);
		  		  break;
		  case 3: hex_display(hex_data, HEX3_BASE);
		  		  break;
		  case 4: hex_display(hex_data, HEX4_BASE);
		  		  break;
		  case 5: hex_display(hex_data, HEX5_BASE);
		  		  break;
		  case 6: hex_display(hex_data, HEX6_BASE);
		  		  break;
		  case 7: hex_display(hex_data, HEX7_BASE);
		  		  break;

		  }

	  }

	  // if subtask 2 selected
	  if(x == 2) {

		  // dummy var for EEPROM
		  unsigned short addr;
		  unsigned char data;

		  int eep_int;
		  printf("1) Write EEPROM\n");
		  printf("2) Read EEPROM\n");

		  scanf("%d", &eep_int);

		  // setup i2c
		  i2c_init();

		  if(eep_int == 1){

			  // ask user for addr + data in hex
			  printf("Enter EEPROM device address: \n");
			  scanf("%hx", &addr);

			  printf("Enter data to write: \n");
			  scanf("%hhx", &data);

			  // write to eeprom
			  eep_write(addr, data);
		  }
		  if(eep_int == 2){

			  // ask user for addr + data in hex
			  printf("Enter EEPROM device address to read from: \n");
			  scanf("%hx", &addr);

			  // read from eeprom
			  unsigned char read_data;
			  read_data = eep_read(addr);

			  // print to display in hex
			  printf("%02x\n", read_data);
		  }


	  }

	  // if subtask 3 selected
	  if(x == 3) {

		  // dummy var for SDRAM
		  unsigned short s_addr;
		  unsigned char s_data;
		  unsigned int *sd_ptr;

		  // var for SDRAM tests
		  int test_int;
		  unsigned int idx;
		  unsigned int t1, t2;

		  int sdram_int;
		  printf("1) Write SDRAM\n");
		  printf("2) Read SDRAM\n");
		  printf("3) Test SDRAM\n");

		  scanf("%d", &sdram_int);

		  switch(sdram_int) {
		  	  case 1:

		  		// ask user for addr + data in hex
		  		printf("Enter SDRAM address: \n");
		  		scanf("%hx", &s_addr);

		  		// declare a ptr to that addr
		  		sd_ptr = (unsigned int*) s_addr;

		  		printf("Enter data to write: \n");
		  		scanf("%hhx", &s_data);

		  		// write to that ptr (dereference)
		  		*sd_ptr = s_data;

		  		break;

		  	  case 2:
		  		  printf("Enter SDRAM address: \n");
		  		  scanf("%hx", &s_addr);

		  		  // declare a ptr to that addr
		  		  sd_ptr = (unsigned int*) s_addr;

		  		  // get data from that addr
		  		  s_data = *sd_ptr;

		  		  // print to display in hex
		  		  printf("%02x\n", s_data);
		  		  break;


		  	  case 3:
		  		  printf("1) All Zeros Test\n");
		  		  printf("2) All Ones Test\n");
		  		  printf("3) Counting Test\n");
		  		  scanf("%d", &test_int);

		  		  switch (test_int) {

		  		  // zeros test
		  		  case 1:
		  			  t1 = OSTimeGet();

		  			  // set all mem locations to 0
		  			  printf("\r\n Writing zeros...");
		  			  for(idx = 0; idx < SDRAM_SIZE_WORDS; idx++) {
		  				  sdram[idx] = 0;
		  			  }
		  			  t2 = OSTimeGet();
		  			  printf("Process took %u seconds \r\n", (t2-t1)/100);

		  			  // check locations
		  			  printf("\r\n Checking zeros...");
		  			  t1 = OSTimeGet();

		  			  for(idx = 0; idx < SDRAM_SIZE_WORDS; idx++) {
		  				  if(sdram[idx] != 0){
		  					  t2 = OSTimeGet();
		  					  printf("FAIL!\r\n");
		  					  printf("Process took %u seconds\r\n", (t2-t1)/100);
		  					  break;
		  				  }
		  			  }

		  			  t2 = OSTimeGet();
		  			  printf("Success! \r\n");
		  			  printf("Process took %u seconds\r\n", (t2-t1)/100);
		  			  break;

		  		  // ones test
		  		  case 2:
		  			t1 = OSTimeGet();
		  			// set all mem locations to 1
		  			printf("\r\n Writing ones...");
		  			for(idx=0; idx < SDRAM_SIZE_WORDS; idx++) {
		  				sdram[idx] = 1;
		  			}
		  			t2 = OSTimeGet();
		  			printf("Process took %u seconds \r\n", (t2-t1)/100);

		  			// check locations
		  			printf("\r\n Checking ones...");
		  			t1 = OSTimeGet();

		  			for(idx=0; idx < SDRAM_SIZE_WORDS; idx++) {
		  				if(sdram[idx] != 1){
		  					 t2 = OSTimeGet();
		  					 printf("FAIL!\r\n");
		  					 printf("Process took %u seconds\r\n", (t2-t1)/100);
		  					 break;
		  				}
		  			}

		  			t2 = OSTimeGet();
		  			printf("Success! \r\n");
		  			printf("Process took %u seconds\r\n", (t2-t1)/100);
		  			break;

		  		  // counting test
		  		  case 3:
			  		t1 = OSTimeGet();

				  	// each 32-bit word increments by 1
			  		int incrementer = 0;
				  	printf("\r\n Writing values...");
				  	for(idx=0; idx < SDRAM_SIZE_WORDS; idx++) {
				  		sdram[idx] = incrementer;
				  		incrementer++;
				  	}
				  	t2 = OSTimeGet();
				  	printf("Process took %u seconds \r\n", (t2-t1)/100);

				  	// reset incrementer var for test
				  	incrementer = 0;

				  	// check locations
				  	printf("\r\n Checking values...");
				  	t1 = OSTimeGet();

				  	for(idx=0; idx < SDRAM_SIZE_WORDS; idx++) {
				  		if(sdram[idx] != incrementer){
				  			t2 = OSTimeGet();
				  			printf("FAIL!\r\n");
				  			printf("Process took %u seconds\r\n", (t2-t1)/100);
				  			break;
				  		}
				  		incrementer++;
				  	}

				  	t2 = OSTimeGet();
				  	printf("Success! \r\n");
				  	printf("Process took %u seconds\r\n", (t2-t1)/100);
				  	break;

		  		  }

		  }


	  }

	  // if subtask 4 selected
	  if(x == 4) {

		  int gled_pat;

		  // get user input about desired LED pattern
		  printf("Select LED #0-7 Pattern\n");
		  printf("1. GLED 7:0 ALL ON\n");
		  printf("2. GLED 7:0 ALL OFF\n");
		  printf("3. GLED 7:0 Walking 1 L->R\n");
		  printf("4. GLED 7:0 Walking 0 L->R\n");
		  printf("5. GLED 7:0 Walking 1 R->L\n");
		  printf("6. GLED 7:0 Walking 0 R->L\n");

		  scanf("%d", &gled_pat);

		  // print int to str
		  sprintf(txbuf, "%d\n", gled_pat);

		  // send an OSQ message to task 3
		  OSQPost(msg, txbuf);

	  }

	  // if subtask 5 selected
	  if (x == 5) {

		  // initialize block #1 with a constant value 1
		  unsigned int idx;
		  for (idx = SDRAM_BASE; idx < BLOCK_TWO; idx++){
			  sdram[idx] = 1;
		  }

		  // copy block #1 -> #2 using CPU
		  copy_data(SDRAM_BASE, BLOCK_TWO, 0x100000);

		  // copy block #1 -> #3 using DMA
		  dma_copy(SDRAM_BASE, BLOCK_THREE, 0x100000);

	  }
	  OSTimeDlyHMSM(0, 0, 0, 500);
  }
}

// "knight" rider task
void task2(void* pdata)
{

	while (1)
	{
		static unsigned int pattern = 0b100000000000000000;
		static unsigned int shift_right = 1;

		// ptr to red LEDs
	    gpio_regs *re = RED_LEDS;

		// if not shifted all the way to the right
		if(shift_right){
			pattern = pattern >> 1;
		} else {
			pattern = pattern << 1;
		}

		re->data = pattern;

		// switch directions and start shifting the other way
		if (pattern == 0b100000000000000000) {
			shift_right = 1;
		}

		if (pattern == 0b000000000000000001) {
			shift_right = 0;
		}


		OSTimeDlyHMSM(0, 0, 0, 150);
    }
}

// hex0 task
void task3(void* pdata) {

	INT8U err;

	while(1){

		// pend on a semaphore- "waiting" state
		OSSemPend(sema, 0, &err);

		// increment hex display
		hex_display(hex0_val++, HEX0_BASE);

		// only 15 num poss
		if(hex0_val > 15){
			hex0_val = 0;
		}

	}

	// no delay as waiting on semaphore

}

// acts on message from task #1, updates at 4 Hz rate
void task4(void* pdata)
{
	INT8U err;

	// ptr for received msg
	char *rxbuf;

	// ptr to green LEDs
	gpio_regs *gr = GREEN_LEDS;

	while (1)
	{
		int gled_num;

		// wait on msg, don't wait forever (only 250 ms)
		rxbuf = OSQPend(msg, 25, &err);

		// check if msg timeout
		if(rxbuf != NULL) {

			// convert from char -> int based on ASCII codes (start at 48)
			// Ref: https://stackoverflow.com/questions/5029840/convert-char-to-int-in-c-and-c
			gled_num = rxbuf[0] - '0';
		}

		// ensure current pattern keeps running
		else {
			gled_num = gled_state;
		}

		// update global var in case timeout
		gled_state = gled_num;

		// led pattern pos
		static unsigned char off_pattern = 0b00000000;
		static unsigned char walking_one = 0b10000000;
		static unsigned char walking_one_right = 0b00000001;

		// switch statement for different green LED patterns
		switch (gled_num) {

		// all on
		case 1:
			gr->data = ~(off_pattern);
			break;
		// all off
		case 2:
			gr->data = off_pattern;
			break;
		// walking 1 L->R
		case 3:
			gr->data = walking_one;
			walking_one = walking_one >> 1;

			// handle wrap-around
			if(!walking_one) {
				walking_one = 0b10000000;
			}
			break;

		// walking 0 L->R
		case 4:
			gr->data = ~(walking_one);
			walking_one = walking_one >> 1;

			// handle wrap-around
			if(!walking_one) {
				walking_one = 0b10000000;
			}
			break;

		// walking 1 R->L
		case 5:
			gr->data = walking_one_right;
			walking_one_right = walking_one_right << 1;

			// handle wrap-around
			if(!walking_one_right) {
				walking_one_right = 0b000000001;
			}
			break;

		// walking 0 R->L
		case 6:
			gr->data = ~(walking_one_right);
			walking_one_right = walking_one_right << 1;

			// handle wrap-around
			if(!walking_one_right) {
				walking_one_right = 0b000000001;
			}
			break;

		}

		OSTimeDlyHMSM(0, 0, 0, 250);
    }
}

// green LED #8 blinks once per sec
void task5(void *pdata){

	while(1){

		// ptr to green LED #8
		gpio_regs *gr_f = FINAL_GREEN;

		gr_f->data = 1;
		// delay to appear as blinking
		usleep(10000);
		gr_f->data = 0;

		// delay 1 second
		OSTimeDlyHMSM(0, 0, 1, 0);
	}
}

/* create ten tasks using uC/OS-II */
int main(void)
{

  // create semaphore
  sema = OSSemCreate(cnt);

  // setup button interrupt for task 3
  alt_ic_isr_register(0, BUTTON_IRQ, button_isr, NULL, NULL);

  alt_ic_irq_enable(0, BUTTON_IRQ);

  // write to PIO_1 intmask reg for btn interrupt
  btn->intmask = 0x01;

  OSTaskCreateExt(task1,
                  NULL,
                  (void *)&task1_stk[TASK_STACKSIZE-1],
                  TASK1_PRIORITY,
                  TASK1_PRIORITY,
                  task1_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  OSTaskCreateExt(task2,
                  NULL,
                  (void *)&task2_stk[TASK_STACKSIZE-1],
                  TASK2_PRIORITY,
                  TASK2_PRIORITY,
                  task2_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  OSTaskCreateExt(task3,
                    NULL,
                    (void *)&task3_stk[TASK_STACKSIZE-1],
                    TASK3_PRIORITY,
                    TASK3_PRIORITY,
                    task3_stk,
                    TASK_STACKSIZE,
                    NULL,
                    0);

  OSTaskCreateExt(task4,
                      NULL,
                      (void *)&task4_stk[TASK_STACKSIZE-1],
                      TASK4_PRIORITY,
                      TASK4_PRIORITY,
                      task4_stk,
                      TASK_STACKSIZE,
                      NULL,
                      0);

  OSTaskCreateExt(task5,
                      NULL,
                      (void *)&task5_stk[TASK_STACKSIZE-1],
                      TASK5_PRIORITY,
                      TASK5_PRIORITY,
                      task5_stk,
                      TASK_STACKSIZE,
                      NULL,
                      0);

  // create msg queue
  msg = OSQCreate(msg_buf, MSG_SIZE);

  OSStart();
  return 0;
}

// fn. to update hex display
void hex_display(unsigned int val, unsigned int HEX_BASE) {

	  // define a general pointer to limit # of case statements required
	  gpio_regs *hex = (gpio_regs*)HEX_BASE;

	  // decoder should handle conversion from binary -> hex
	  hex->data = val;
}

// handle setup for EEPROM device
void i2c_init(void){

	// disable i2c bus in case left on from previous run
	ep->ctrl = 0x00;

	// set setup/hold times for eeprom
	ep->sclhigh = 1000;
	ep->scllow = 1000;
	ep->sdahold = 500;

	// enable i2c bus
	ep->ctrl = 0x01;
}

void eep_write(unsigned short addr, unsigned char data){

	// set device addr
	ep->tfrcmd = 0xA0 | (1 << 9);

	// set high and low byte addresses
	ep->tfrcmd = (addr >> 8);
	ep->tfrcmd = (addr & 0xFF);

	// get data byte, write until stop bit
	ep->tfrcmd = data | (0x100);

	// poll status register until write complete
	while(ep->status) {

		// delay for at least 6 ms to allow EEPROM time to finish
		OSTimeDlyHMSM(0, 0, 0, 10);
	}

}
unsigned char eep_read(unsigned short addr){

	// dummy var to store read data
	unsigned char data;

	// set device addr to dummy write
	// must write before you can read
	ep->tfrcmd = 0xA0 | (1 << 9);

	// same high and low byte addr as write
	ep->tfrcmd = (addr >> 8);
	ep->tfrcmd = (addr & 0xFF);

	// set device addr to read
	ep->tfrcmd = 0xA1 | (1 << 9);

	// wait until stop bit
	ep->tfrcmd = 0x100;

	// poll status until read complete
	while(ep->status){

		OSTimeDlyHMSM(0, 0, 0, 10);
	}

	// read data
	data = ep->rxdata;

	return data;
}

// copy data using DMA controller
// block #1 -> #3
void dma_copy(unsigned short read_addr, unsigned short write_addr, unsigned int num_bytes){

	// initialize dma controller
	unsigned int t1, t2;
	dma_regs *dma = (dma_regs*)DMA_BASE;

	// write to control register
	dma->control = 0x8C;

	// def param for transfer
	dma->readaddress = read_addr;
	dma->writeaddress = write_addr;
	dma->length = num_bytes;

	printf("\r\n Copying data using the DMA Register...");
	t1 = OSTimeGet();

	// poll the status register, when status = "1" its done
	while(dma->status == 0x0){

		// wait until transfer complete
	}

	t2 = OSTimeGet();
	printf("Process took %u seconds \r\n", (t2-t1)/100);

	// clear the DONE bit
	dma->status = 0x0;

}

// copy data using CPU
// following format from slides
// copy data from block #1 -> #2
void copy_data(unsigned int *src, unsigned int *dst, unsigned int nwords) {
	unsigned int t1, t2;
	unsigned int k;


	printf("\r\n Copying data using the CPU...");
	t1 = OSTimeGet();

	for(k = 0; k < nwords; k++) {
		dst[k] = src[k];
	}

	t2 = OSTimeGet();
	printf("Process took %u seconds \r\n", (t2-t1)/100);
}

void button_isr(void) {

	// post semaphore - "wake up" task waiting
	OSSemPost(sema);

	// clear edge-capture reg
	btn->edge = 0x01;

}
