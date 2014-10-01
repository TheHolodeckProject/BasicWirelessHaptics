/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//   MSP430F552x Demo - Software Toggle P1.0
//
//   Description: Toggle P1.0 by xor'ing P1.0 inside of a software loop.
//   ACLK = 32.768kHz, MCLK = SMCLK = default DCO~1MHz
//
//                MSP430F552x
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |                 |
//            |             P1.0|-->LED
//
//   Bhargavi Nisarga
//   Texas Instruments Inc.
//   April 2009
//   Built with CCSv4 and IAR Embedded Workbench Version: 4.21
//******************************************************************************

#include <msp430.h>
#include "queuelist.h"

#define M               	5
#define M1					BIT1
#define M2					BIT2
#define M3					BIT3
#define M4					BIT4
#define M5					BIT5

// state machines
#define	HAND_INFORMATION	0
#define FINGER_INTENSITY	1

#define LEFT_HAND 			1
#define RIGHT_HAND			0

#define LEFT_THUMB			BIT0
#define LEFT_INDEX			BIT1
#define LEFT_MIDDLE			BIT2
#define LEFT_RING			BIT3
#define LEFT_PINKY			BIT4

#define RIGHT_THUMB			BIT4
#define RIGHT_INDEX			BIT3
#define RIGHT_MIDDLE		BIT2
#define RIGHT_RING			BIT1
#define RIGHT_PINKY			BIT0

#define	VIBEN	BIT6

typedef struct
{
	int thumbIntensity;
	int indexIntensity;
	int middleIntensity;
	int ringIntensity;
	int pinkyIntensity;
} Hand;

Hand rightHand;
Hand leftHand;

int HandForPCB;

// global state variable for processing UART cmd
int isItLeft;
int fingerSelection;
int state;
int rdata;

void configureMotors(void);
void configureBluetooth(void);
void setMotors(int,int);

int main(void)
{
	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
	HandForPCB = RIGHT_HAND;

	configureMotors();
	configureBluetooth();
	//configure1MSTimer();

	//for(int i=0; i<M; i++) {
		//Qstatus[i] = IDLE;
	//}
	state = HAND_INFORMATION;

	//setMotors(0,3);

	__bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled

	__no_operation();                         // For debugger
}

void configureMotors(void)
{
	P1DIR |= 0x3F;                            // P1.0 - P1.5 output
	P1SEL |= 0x3C;                            // P1.0 - P1.5 TA1/2 options
	P1OUT |= 0x3F;
	TA0CTL = TASSEL_2 + MC_1+TACLR;                 // SMCLK, up mode
	TA0CCTL1 = OUTMOD_7;                      // CCR1 reset/set
	TA0CCTL2 = OUTMOD_7;
	TA0CCTL3 = OUTMOD_7;
	TA0CCTL4 = OUTMOD_7;
	TA0CCR0 = 255;

	//P1DIR |= VIBEN;
	//P1OUT |= VIBEN;

	// configure pins
	//P1SEL |= M1+M2+M3+M4+M5;             // P1.2, P1.3, P1.4, P1.5 options select
	//P1DIR |= M1+M2+M3+M4+M5;         	  // P1.2, P1.3, P1.4, P1.5 output
	//P1SEL ^= 11111111;

	// enable power for motor drivers
	//P1DIR |= VIBEN;
	//P1OUT |= VIBEN;

	//P1OUT |= VIBEN;//+M1+M2+M3+M4+M5;

	// configure timer. initialize PWMs to 0% duty cycle
	//TA0CCTL0 = OUTMOD_7;
	//TA0CCR0 = 10;                          // PWM Period
	//TA0CCTL1 = OUTMOD_7;                      // CCR1 reset/set
	//TA0CCR1 = 10;                              // CCR1 PWM duty cycle
	//TA0CCTL2 = OUTMOD_7;                      // CCR2 reset/set
	//TA0CCR2 = 10;                              // CCR2 PWM duty cycle
	//TA0CCTL3 = OUTMOD_7;                      // CCR3 reset/set
	//TA0CCR3 = 10;                              // CCR3 PWM duty cycle
	//TA0CCTL4 = OUTMOD_7;                      // CCR4 reset/set
	//TA0CCR4 = 10;                              // CCR4 PWM duty cycle
	//TA0CTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, up mode, clear TAR
}

void setMotors(int direction, int speed)
{
	switch (direction)
	{
		case 0:
		{
			P1DIR ^= VIBEN;
			P1OUT ^= VIBEN;

			TA0CCR1 = 0;
			TA0CCR2 = 0;
			TA0CCR3 = 0;
			TA0CCR4 = 0;

			break;
		}

		case 1:
		{
			P1DIR ^= VIBEN;
			P1OUT ^= VIBEN;

			TA0CCR0 = 255;
			TA0CCR1 = 0;
			TA0CCR2 = 0;
			TA0CCR3 = 0;
			TA0CCR4 = 0;

			break;
		}

		case 2:
		{
			P1DIR |= VIBEN;
			P1OUT |= VIBEN;

			TA0CCR1 = 255;	//rightRing
			TA0CCR2 = 0;	//rightMiddle
			TA0CCR3 = 0;	//rightIndex
			TA0CCR4 = 0;	//rightThumb
			//TA0CCR1 = speed;
			//TA0CCR4 = speed;
			break;
		}

		case 3:
		{
			P1DIR |= VIBEN;
			P1OUT |= VIBEN;

			TA0CCR1 = 255;	//rightRing
			TA0CCR2 = 0;	//rightMiddle
			TA0CCR3 = 0;	//rightIndex
			TA0CCR4 = 0;	//rightThumb

			break;
		}

		case 4:
		{
			P1DIR |= VIBEN;
			P1OUT |= VIBEN;

			TA0CCR1 = 0;	//rightRing
			TA0CCR2 = 255;	//rightMiddle
			TA0CCR3 = 0;	//rightIndex
			TA0CCR4 = 0;	//rightThumb

			break;
		}

		case 6:
		{
			P1DIR |= VIBEN;
			P1OUT |= VIBEN;

			TA0CCR1 = 255;	//rightRing
			TA0CCR2 = 255;	//rightMiddle
			TA0CCR3 = 0;	//rightIndex
			TA0CCR4 = 0;	//rightThumb

			break;
		}

		case 16:
		{
			int test = TA0CCR0;
			int test2 = TA0CCR0;
			P1DIR |= VIBEN;
			P1OUT |= VIBEN;
			int test3 = TA0CCR0;

			TA0CCR1 = 0;	//rightRing
			TA0CCR2 = 0;	//rightMiddle
			TA0CCR3 = 0;	//rightIndex
			TA0CCR4 = 255;	//rightThumb

			break;
		}

		case 18:
		{
			P1DIR |= VIBEN;
			P1OUT |= VIBEN;

			TA0CCR1 = 255;	//rightRing
			TA0CCR2 = 0;	//rightMiddle
			TA0CCR3 = 0;	//rightIndex
			TA0CCR4 = 255;	//rightThumb

			break;
		}

		case 20:
		{
			P1DIR |= VIBEN;
			P1OUT |= VIBEN;

			TA0CCR1 = 0;	//rightRing
			TA0CCR2 = 255;	//rightMiddle
			TA0CCR3 = 0;	//rightIndex
			TA0CCR4 = 255;	//rightThumb

			break;
		}

		case 22:
		{
			P1DIR |= VIBEN;
			P1OUT |= VIBEN;

			TA0CCR1 = 255;	//rightRing
			TA0CCR2 = 255;	//rightMiddle
			TA0CCR3 = 0;	//rightIndex
			TA0CCR4 = 255;	//rightThumb

			break;
		}
	}
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
		case HAND_INFORMATION:
			isItLeft = rdata & 0x80;
			fingerSelection = rdata & 0x7F;
			state = FINGER_INTENSITY;
			break;

		case FINGER_INTENSITY:
			P1DIR |= VIBEN;
			P1OUT |= VIBEN;

			if(isItLeft)
			{
				switch(fingerSelection)
				{
					case LEFT_THUMB:
						TA0CCR4 = leftHand.thumbIntensity = rdata;
						break;
					case LEFT_INDEX:
						TA0CCR3 = leftHand.indexIntensity = rdata;
						break;
					case LEFT_MIDDLE:
						TA0CCR2 = leftHand.middleIntensity = rdata;
						break;
					case LEFT_RING:
						TA0CCR1 = leftHand.ringIntensity = rdata;
						break;
					case LEFT_PINKY:
						leftHand.pinkyIntensity = rdata;
						break;
				}
			}
			else
			{
				switch(fingerSelection)
				{
					case RIGHT_THUMB:
						TA0CCR4 = rightHand.thumbIntensity = rdata;
						break;
					case RIGHT_INDEX:
						TA0CCR3 = rightHand.indexIntensity = rdata;
						break;
					case RIGHT_MIDDLE:
						TA0CCR2 = rightHand.middleIntensity = rdata;
						break;
					case RIGHT_RING:
						TA0CCR1 = rightHand.ringIntensity = rdata;
						break;
					case RIGHT_PINKY:
						rightHand.pinkyIntensity = rdata;
						break;
				}
			}
			state = HAND_INFORMATION;

			break;

		}
		break;
	case 4:break;                             // Vector 4 - TXIFG
	default: break;
	}
}
