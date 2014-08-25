//***************************************************************************************
//  MSP430 Blink the LED Demo - Software Toggle P1.0
//
//  Description; Toggle P1.0 by xor'ing P1.0 inside of a software loop.
//  ACLK = n/a, MCLK = SMCLK = default DCO
//
//                MSP430x5xx
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |             P1.0|-->LED
//
//  J. Stevenson
//  Texas Instruments, Inc
//  July 2011
//  Built with Code Composer Studio v5
//***************************************************************************************

#include <msp430.h>				

#define	VIBEN	BIT6
/*
void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  P1DIR |= BIT2+BIT3+BIT4+BIT5;                       // P1.2 and P1.3 output
  P1SEL |= BIT2+BIT3+BIT4+BIT5;                       // P1.2 and P1.3 options select

  // enable power for motor drivers
  P1DIR |= VIBEN;
  P1OUT |= VIBEN;

  TA0CCR0 = 512-1;                          // PWM Period
  TA0CCTL1 = OUTMOD_7;                       // CCR1 reset/set
  TA0CCR1 = 384;                            // CCR1 PWM duty cycle
  TA0CCTL2 = OUTMOD_7;                      // CCR2 reset/set
  TA0CCR2 = 384;                            // CCR2 PWM duty cycle
  TA0CCTL3 = OUTMOD_7;                      // CCR3 reset/set
  TA0CCR3 = 384;                            // CCR3 PWM duty cycle
  TA0CCTL4 = OUTMOD_7;                      // CCR4 reset/set
  TA0CCR4 = 384;                            // CCR4 PWM duty cycle
  TA0CTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, up mode, clear TAR

  __bis_SR_register(LPM0_bits);             // Enter LPM0
  __no_operation();                         // For debugger
}
*/

void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

	// disable power for motor drivers
	P1DIR &= ~VIBEN;
	P1OUT &= ~VIBEN;

	P4SEL |= BIT4+BIT5;                       // P4.4,5 = USCI_A1 TXD/RXD
	UCA1CTL1 |= UCSWRST;                      // **Put state machine in reset**
	UCA1CTL1 |= UCSSEL_2;                     // SMCLK
	UCA1BR0 = 9;                              // 1MHz 115200 (see User's Guide)
	UCA1BR1 = 0;                              // 1MHz 115200
	UCA1MCTL |= UCBRS_1 + UCBRF_0;            // Modulation UCBRSx=1, UCBRFx=0
	UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	UCA1IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

	__bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
	__no_operation();                         // For debugger
}

// Echo back RXed character, confirm TX buffer is ready first
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
switch(__even_in_range(UCA1IV,4))
{
case 0:break;                             // Vector 0 - no interrupt
case 2:                                   // Vector 2 - RXIFG
  while (!(UCA1IFG&UCTXIFG));             // USCI_A0 TX buffer ready?
  UCA1TXBUF = UCA1RXBUF;                  // TX -> RXed character
  break;
case 4:break;                             // Vector 4 - TXIFG
default: break;
}
}

