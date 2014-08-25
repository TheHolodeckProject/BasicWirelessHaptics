
#include <msp430.h>
#include "queuelist.h"

#define M               2
#define M1				BIT4
#define M2				BIT2
#define M3				BIT3
#define M4				BIT5
// state machines
#define CHANNEL			0
#define INTENSITY		1
#define DURATION		2
#define DISCONNECTED	3
#define CONNECT			128

#define IDLE           0
#define VIBRATING      1

#define	VIBEN	BIT6

/*
typedef struct
{
  int intensity;
  int duration;
} Vibration;

short chList[M] = {M1,M2};

// create global fifo queue for vibration events
QueueList <Vibration> vibQ[M];

// temporary Vibration variable
Vibration vibtmp;

// global state variable for processing UART cmd
int vibCh;

// global current vibration state
Vibration curVib[M];

*/
// global queue status (idle or vibrating)
int Qstatus[M];

short rdata = 0, sel = 0, state = 0;

void configureMotors(void);
void configureBluetooth(void);
void configure1MSTimer(void);
int chMatch(short);
void pwmSet(int, int);

int main(void)
{
	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

	configureMotors();
	configureBluetooth();
	//				configure1MSTimer();

	for(int i=0; i<M; i++) {
		Qstatus[i] = IDLE;
	}
	state = CHANNEL;

	__bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled

	__no_operation();                         // For debugger

	return 0;
}

void configureMotors(void)
{
	// configure pins
	P1DIR |= M1+M2+M3+M4;         	  // P1.2, P1.3, P1.4, P1.5 output
	P1SEL |= M1+M2+M3+M4;             // P1.2, P1.3, P1.4, P1.5 options select

	// enable power for motor drivers
	P1DIR |= VIBEN;
	P1OUT |= VIBEN;

	// configure timer. initialize PWMs to 0% duty cycle
	TA0CCR0 = 256-1;                          // PWM Period
	TA0CCTL1 = OUTMOD_7;                      // CCR1 reset/set
	TA0CCR1 = 0;                              // CCR1 PWM duty cycle
	TA0CCTL2 = OUTMOD_7;                      // CCR2 reset/set
	TA0CCR2 = 0;                              // CCR2 PWM duty cycle
	TA0CCTL3 = OUTMOD_7;                      // CCR3 reset/set
	TA0CCR3 = 0;                              // CCR3 PWM duty cycle
	TA0CCTL4 = OUTMOD_7;                      // CCR4 reset/set
	TA0CCR4 = 0;                              // CCR4 PWM duty cycle
	TA0CTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, up mode, clear TAR
}

void configure1MSTimer()
{
	TA1CCTL0 = CCIE;                          // CCR0 interrupt enabled
	TA1CCR0 = 1045;							  // 1045/1.045Mhz = 1ms
	TA1CTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, upmode, clear TAR
}

void configureBluetooth()
{
	P4SEL |= BIT4+BIT5;                       // P4.4,5 = USCI_A1 TXD/RXD
	UCA1CTL1 |= UCSWRST;                      // **Put state machine in reset**
	UCA1CTL1 |= UCSSEL_2;                     // SMCLK
	UCA1BR0 = 9;                              // 1MHz 115200 (see User's Guide)
	UCA1BR1 = 0;                              // 1MHz 115200
	UCA1MCTL |= UCBRS_1 + UCBRF_0;            // Modulation UCBRSx=1, UCBRFx=0
	UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	UCA1IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
}

// Echo back RXed character, confirm TX buffer is ready first
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
	switch(__even_in_range(UCA1IV,4))
	{
	case 0:break;                             // Vector 0 - no interrupt
	case 2:                                   // Vector 2 - RXIFG
		while (!(UCA1IFG&UCTXIFG));           // USCI_A0 TX buffer ready?
		rdata = UCA1RXBUF;
		UCA1TXBUF = rdata;                    // TX -> RXed character

		switch(state) {
		case CHANNEL:
/*			vibCh = chMatch(rdata);
			if(vibCh != -1) {
				state = INTENSITY;
			}
			break;
		case INTENSITY:
			vibtmp.intensity = rdata;
			if(vibtmp.intensity < 0)
				vibtmp.intensity = 0;
			state = DURATION;
			break;
		case DURATION:
			vibtmp.duration = rdata;
			if(vibtmp.duration > 0)
				vibQ[vibCh].push(vibtmp);
			state = CHANNEL;
			break;
			*/
		}
		break;
	case 4:break;                             // Vector 4 - TXIFG
	default: break;
	}
}

/*
// Timer1 A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
	for(int i=0; i<M; i++) {
		switch(Qstatus[i]) {
		case IDLE:
			if(!vibQ[i].isEmpty()) {
				curVib[i] = vibQ[i].pop();
				pwmSet(chList[i], curVib[i].intensity);
				curVib[i].duration--;
				Qstatus[i] = VIBRATING;
			}
			break;
		case VIBRATING:
			if(curVib[i].duration > 0) {
				curVib[i].duration--;
			}else {
				pwmSet(chList[i], 0);
				Qstatus[i] = IDLE;
			}
			break;
		}
	}
}

int chMatch(short ch)
{
  int idx = -1;
  for(int i=0; i<M; i++) {
    if(ch == chList[i]) {
      idx = i;
      break;
    }
  }
  return idx;
}
*/

void pwmSet(int ch, int val)
{
	switch(ch) {
	case M1:
		TA0CCR3 = val;
		break;
	case M2:
		TA0CCR1 = val;
		break;
	case M3:
		TA0CCR2 = val;
		break;
	case M4:
		TA0CCR4 = val;
		break;
	}
}
