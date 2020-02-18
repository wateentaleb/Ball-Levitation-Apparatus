//
//  ball.c
//  Ball Levitation
//
//  Created by Wateen Taleb on 2020-02-17.
//  Copyright © 2020 Wateen Taleb. All rights reserved.
//


/*  Definitions  */
#define TASK_STK_SIZE		256         // Size of each task's stacks (# of bytes)
#define PotMax				1056		// Maximum POT poistion reading ~0v
#define	PotMin				2060		// Minimum POT position reading ~7.5v
#define	IrVmax				1500	    // Maximum IR Sesnor reading
#define	IrVmin				1990		// Minimum IR Sensor reading
#define MAX_COUNT			100
#define	ON					1	    	// Both experimental, and development board work on an active low configuration
#define	OFF					0			// Both experimental, and development board work on an active low configuration
#define	N_TASKS				4			// Number of tasks



// Redefine uC/OS-II configuration constants as necessary
#define  OS_MAX_EVENTS		2           // Maximum number of events (semaphores, queues, mailboxes)
#define  OS_MAX_TASKS		11          // Maximum number of tasks system can create (less stat and idle tasks)
#define  OS_TASK_STAT_EN	1           // Enable statistics task creation
#define  OS_TICKS_PER_SEC	128         // Number of Ticks per second

/* Other Definitions */
#define POT_CHAN				0              // channel 0 of ADC (ADC0)
#define IRS_CHAN				1			   // channel 1 of ADC (ADC1)
#define MOT_CHAN				1              // channel 1 of digital output (OUT1)
#define MAX_PWIDTH			    1024           // the maximum pulse width in TMRB clock cycles

//#define STDIO_DISABLE_FLOATS

/* Variable declarations */
char TMRB_MSB;									// this 8-bit value will be written to TBM2R (bits 8 and 9 of TMRB2 match register)
char TMRB_LSB;									// this 8-bit value will be written to TBL2R (bits 0 to 7 of TMRB2 match register)
int	PulseWidth;                                 // Duty Cycle of the PWM signal
float PotNorm;									// Scaled value of the POT reading


char TMRB_10_count;                       // This variable is incrimented at the begining of every PWM cycle. When it is equal to ten, it is
                                          // reset to zero, and duty cycle values are calculated from updated ADC input.
                                          // The value is updated in the Timer B ISR to either 1 during the duty cycle or 0 for remainder.
#use "ucos2.lib"

UBYTE TaskData[N_TASKS];      		    // Parameters to pass to each task
OS_EVENT *ADCSem;						// Semaphore to access ADC

//char decToHex(int dec);                  // Convert decimal number to hex
void InitializeTimers();				 // Setup Timer A and B interrupts
void CalculateDutyCycle();               // Update the duty cycle
void Tmr_B_ISR();                        // Timer B interrupt service routine
void ShowStat();                         // Display update
void TaskInput(void *data);				 // Function prototypes of the task
void TaskControl (void *data);			 // Function prototypes of the task
void TaskStart(void *data);              // Function prototype of startup task
void DispStr(int x, int y, char *s);


void main (void)
{
   brdInit();                             // Initialize MCU baord
   OSInit();                              // Initialize uC/OS-II
   ADCSem = OSSemCreate(1);               // Semaphores for ADC inputs

   OSTaskCreate(TaskStart, (void *)0, TASK_STK_SIZE, 10);
   OSStart();                             // Start multitasking
}


void TaskStart (void *data)
{
   OSStatInit();
   OSTaskCreate(TaskInput, (void *)&TaskData[1], TASK_STK_SIZE, 11);
   OSTaskCreate(TaskControl, (void *)&TaskData[2], TASK_STK_SIZE, 5);
   InitializeTimers();							// Setup timer A and B interrupts


   DispStr(0, 10, "Analog Input Reading: xxxx  ");
   DispStr(0, 11, "Normalized Input Reading: xxxx  ");
   DispStr(0, 13, "#Tasks          : xxxxx  CPU Usage: xxxxx   %");
   DispStr(0, 14, "#Task switch/sec: xxxxx");
   DispStr(0, 15, "<-PRESS 'Q' TO QUIT->");

   for (;;) {
         ShowStat();
         OSTimeDly(OS_TICKS_PER_SEC);     // Wait one second
    }
}


  int s;
nodebug void TaskInput (void *date)
{
    auto UBYTE err;
    char display[64];
    int PotRead;
    int temp;
    auto WORD   key;
    PotRead = 0;

    s=0;
    for(;;) {

        PotRead = anaIn(POT_CHAN);					// Read POT output voltage

        sprintf(display, "%d", PotRead);			// Write POT voltage on STDIO
        DispStr(22, 10, display);

        /*******************************************************/
        /* During the lab session, you will add your code here */
        /*                                                     */
        /* 5.1.1 Analog to Digital Conversion:  			   */
        /*    Write codes to scale PotRead to a value PotNorm  */
        /*    that is between 0 and 1 (regardless)             */
        /*******************************************************/
          if (kbhit()) {
            key = getchar();
				switch (key){
            	case 0x6D:	// middle
            	PotNorm = 0.55;
                  break;

            	case 0x30:	// zero the ball
            		PotNorm= 0 ;
                  break;
              	case 0x73:	// switch

                    PotNorm = ((float)(PotRead-PotMin) / (float)(PotMax-PotMin));
                    if(PotNorm > 1) PotNorm=1;
          				if(PotNorm <0) PotNorm=0;

                  break;

        		}
        }
        //PotNorm = ((float)(PotRead-PotMin) / (float)(PotMax-PotMin));      // Normalize the POT voltage
       // if(PotNorm > 1) PotNorm=1;
       // if(PotNorm <0) PotNorm=0;
        sprintf(display, "%f", PotNorm*100);                     // Write normalized POT voltage on STDIO
        DispStr(30, 11, display);

        OSTimeDly(OS_TICKS_PER_SEC);
      }
}

 nodebug void TaskControl (void *data)
{
   auto UBYTE err;
   int IrSen;
   float IrNorm, ErrSig;
   float minPW;
    float Kp;
    float Kd;
    float Ki;
    float prev_err;
    float integral;
    float derivative;
    float iterationTime;
    iterationTime = 1;

    integral = 0;
    // tuned kp to 0.7, it gave the clearest step signal on the oscilloscope
   //Kp= 0.4;
   Kp = 0.4;
   Ki = 0.00115;
   Kd = 0.075;

// Ki = need to tune it
// Kd = need to tune it
   minPW=0.6;


   for (;;) {

   /*************************************************************/
   /*	5.1.2 Pulse Width Modulation:						    */
   /*		Write code to convert the result of 5.1.1 into      */
   /*       a 10-bit Duty Cycle value which is used to set the  */
   /*		values of two 8-bit registers used by Timber B ISR. */
   /*		Timber B ISR sets the value of Match 2 Registers.   */
   /*														    */
   /* PulseWidth is Duty Cycle based on the value of PotNorm    */
   /*	TMRB_LSB is the 8-bit register written to TBL2R		    */
   /*	TMRB_MSB is the 8-bit register written to TBM2R		    */
   /*************************************************************/
       /* set up timer B B1 interrupt */

       IrSen = anaIn(IRS_CHAN);
       // casting it to float to get accuracy and not lose decimal points
       IrNorm = ((float)(IrSen-IrVmin) / (float)(IrVmax-IrVmin));
       ErrSig = PotNorm - IrNorm;
         integral = integral + (ErrSig * iterationTime) ;
         derivative = (float)(ErrSig-prev_err)/(float) iterationTime;
      PulseWidth = (int)((minPW + Kp*ErrSig + Ki * integral + Kd * derivative)*MAX_PWIDTH);
      // PulseWidth = (int)((minPW +  kP*ErrSig)*MAX_PWIDTH);
       TMRB_LSB = (char)PulseWidth;                        // calculate MSB as a number from 0->1024
       TMRB_MSB = (char)(PulseWidth>>2);                 // calculate LSB as a number from 0->1024

       OSTimeDly(1);
      }
}
/**************************************************************************/
/*   This is the body of Task that needs to be added during lab session   */
/**************************************************************************/
/*
nodebug void TaskControl (void *data)
{
   auto UBYTE err;
   int IrSen;
   float IrNorm, ErrSig;
   float minPW;
	float kp;
   kp = 0.4;
   minPW=0.6;

   for (;;) {

   /*************************************************************/
   /*	5.1.2 Pulse Width Modulation:						    */
   /*		Write code to convert the result of 5.1.1 into      */
   /*       a 10-bit Duty Cycle value which is used to set the  */
   /*		values of two 8-bit registers used by Timber B ISR. */
   /*		Timber B ISR sets the value of Match 2 Registers.   */
   /*														    */
   /* PulseWidth is Duty Cycle based on the value of PotNorm    */
   /*	TMRB_LSB is the 8-bit register written to TBL2R		    */
   /*	TMRB_MSB is the 8-bit register written to TBM2R		    */
   /*************************************************************/
       /* set up timer B B1 interrupt */
       /*
       IrSen = anaIn(IRS_CHAN);
       IrNorm = ((float)(IrSen-IrVmin) / (float)(IrVmax-IrVmin));
       ErrSig = PotNorm - IrNorm;
       //Note*** TMRB_M2R = (TMRB_MSB * 4) - TMRB_LSB
       //50.0% duty cycle = 512 = 128 * 4 - 0 --> 0x80 , 0x00
       //38.9% duty cycle = 398 = 128 * 4 - 114 --> 0x80, 0x72
       //PulseWidth = PotNorm * 1024;                       // Pulse Width in units of clock periods

       PulseWidth = (int)((minPW +  kp*ErrSig)*MAX_PWIDTH);
       TMRB_LSB = (char)PulseWidth;                        // calculate MSB as a number from 0->1024
       TMRB_MSB = (char)(PulseWidth>>2);                 // calculate LSB as a number from 0->1024

       OSTimeDly(1);			    // Delay the task 1 tick
      }
}
  */
nodebug root interrupt void Tmr_B_ISR()
{
   char TMRB_status;

   TMRB_status = RdPortI(TBCSR);				// Read-out and clear interrupt status flags
   if(TMRB_status & 0x02){				    	// A new PWM cycle, if Match 1 reg is triggered
      digOut(MOT_CHAN, ON);						// Set PWM output high
      WrPortI(TBM1R, NULL, 0x00);				// set up Match 1 reg to interrupt at the begining of the next cycle
      WrPortI(TBL1R, NULL, 0x00);				// set up Match 1 reg to interrupt at the begining of the next cycle
   }
   else if(TMRB_status & 0x04){					// If Match 2 reg is triggered, output will be low for the rest of the cycle
      digOut(MOT_CHAN, OFF);					// Set PWM output low */                                       // Drop output flag to 0
      WrPortI(TBM2R, NULL, TMRB_MSB);			// set up Match 2 reg to corespond with duty cycle
      WrPortI(TBL2R, NULL, TMRB_LSB);			// set up Match 2 reg to corespond with duty cycle
   }
   OSIntExit();
}


/*
*********************************************************************************************************
*                                      HELPER FUNCTIONS
*********************************************************************************************************
*/

void InitializeTimers()
{
   TMRB_MSB = 0x40;									// Initialize TMRB2 match register to coincide with 50% duty cycle
   TMRB_LSB = 0xFF;									// Initialize TMRB2 match register to coincide with 50% duty cycle
   TMRB_10_count = 0;								// Initialize the Timer B interrupt counter (PWM cycle counter) to zero

   /* Setup Timer A */
   WrPortI(TAT1R, &TAT1RShadow, 0xFF);			// set TMRA1 to count down from 255
   WrPortI(TACR, &TACRShadow, 0x00);			// Disable TMRA interrupts (TMRA used only to clock TMRB)
   WrPortI(TACSR, &TACSRShadow, 0x01);			// Enable main clock (PCLK/2) for TMRA1

   /* set up Timer B for generating PWM */
   SetVectIntern(0x0b, Tmr_B_ISR);				// set up timer B interrupt vector
   WrPortI(TBCR, &TBCRShadow, 0x05);			// clock timer B with TMRA1 to priority 1 interrupt
   WrPortI(TBM1R, NULL, 0x00);					// set up match register 1 to 0x00
   WrPortI(TBL1R, NULL, 0x00);					// set up match register 1 to 0x00
   WrPortI(TBM2R, NULL, TMRB_MSB);				// set up match register 2 to to give an initial 50% duty cycle
   WrPortI(TBL2R, NULL, TMRB_LSB);				// set up match register 2 to to give an initial 50% duty cycle
   WrPortI(TBCSR, &TBCSRShadow, 0x07);			// enable Timer B to interrupt on B1 and B2 match
}

void ShowStat()
{
   static char Key, s[50];

   sprintf(s, "%5d", OSTaskCtr);				// Display #tasks running
   DispStr(18, 13, s);
   sprintf(s, "%5d", OSCPUUsage);				// Display CPU usage in %
   DispStr(36, 13, s);
   sprintf(s, "%5d", OSCtxSwCtr);				// Display avg #context switches per 5 seconds
   DispStr(18, 14, s);
   OSCtxSwCtr = 0;

   if (kbhit()) {							// See if key has been pressed
      Key = getchar();
      if (Key == 0x71 || Key == 0x51)			// Yes, see if it's the q or Q key
         exit(0);
   }
}

void DispStr (int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

