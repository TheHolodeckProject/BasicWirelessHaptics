
#include <msp430.h>

#define M1				BIT2
#define M2				BIT3
#define M3				BIT4
#define M4				BIT5
// state machines
#define INIT			0

#define	VIBEN	BIT6

int state;

void configureMotors();
void configureBluetooth();

void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

	configureMotors();
	configureBluetooth();

	state = INIT;

	__bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
 	__no_operation();                         // For debugger
}

void configureMotors()
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
	TA0CCR1 = 128;                              // CCR1 PWM duty cycle
	TA0CCTL2 = OUTMOD_7;                      // CCR2 reset/set
	TA0CCR2 = 192;                              // CCR2 PWM duty cycle
	TA0CCTL3 = OUTMOD_7;                      // CCR3 reset/set
	TA0CCR3 = 200;                              // CCR3 PWM duty cycle
	TA0CCTL4 = OUTMOD_7;                      // CCR4 reset/set
	TA0CCR4 = 220;                              // CCR4 PWM duty cycle
	TA0CTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, up mode, clear TAR
}

void configureBluetooth()
{
	P5DIR |= BIT1;
	P5OUT &= ~BIT1;
	__delay_cycles (1000);
	P5OUT |= BIT1;

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
	short rdata;
	switch(__even_in_range(UCA1IV,4)) {
	case 0:break;                             // Vector 0 - no interrupt
	case 2:                                   // Vector 2 - RXIFG
		while (!(UCA1IFG&UCTXIFG));           // USCI_A0 TX buffer ready?
		rdata = UCA1RXBUF;
		UCA1TXBUF = rdata;                    // TX -> RXed character

		switch(state) {
		case INIT:
			switch(rdata) {
			case M1:
				state = M1;
				break;
			case M2:
				state = M2;
				break;
			case M3:
				state = M3;
				break;
			case M4:
				state = M4;
				break;
			}
			break;
		case M1:
			TA0CCR4 = rdata;                  // CCR4 PWM duty cycle
			state = INIT;
			break;
		case M2:
			TA0CCR2 = rdata;                  // CCR4 PWM duty cycle
			state = INIT;
			break;
		case M3:
			TA0CCR3 = rdata;                  // CCR4 PWM duty cycle
			state = INIT;
			break;
		case M4:
			TA0CCR4 = rdata;                  // CCR4 PWM duty cycle
			state = INIT;
			break;
		}
		break;
	case 4:break;                             // Vector 4 - TXIFG
	default: break;
	}
}
