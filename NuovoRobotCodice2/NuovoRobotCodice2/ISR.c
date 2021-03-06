/*
 * ISR.c
 *
 * Created: 09/06/2021 12:34:06
 *  Author: tollardogiacomo
 */ 

#define F_CPU 16000000ul
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "ISR.h"
#include "PWM.h"
#include "serial.h"
#include "Millis.h"
#include "BNO.h"

volatile unsigned long topRaggiunti = 0;

void InitISR(void){
	PORTD|=(1<<PD3);
	EIMSK|=(1<<INT2)|(1<<INT3)|(1<<INT4)|(1<<INT5);
	EICRA|=(1<<ISC21)|(1<<ISC20)|(1<<ISC31)|(1<<ISC30);
	EICRB|=(1<<ISC41)|(1<<ISC40)|(1<<ISC51)|(1<<ISC50);
	sei();
}


	
ISR(TIMER5_COMPA_vect){
	current_millis=millis();
	if (current_millis>=(old_millis+10)) {
		angle=gradiGiroscopio180();
		old_millis=current_millis;
	}
	dutyMSxA = CalculatePID(MSxA);
	dutyMDxA = CalculatePID(MDxA);
	dutyMSxD = CalculatePID(MSxD);
	dutyMDxD = CalculatePID(MDxD);
	
}

ISR(TIMER4_COMPA_vect){
	topRaggiunti++;	
	
}

ISR(INT2_vect){
	//imp1++;	
	tImp1 = TCNT4;
	topRaggiuntiProv1=topRaggiunti;
	if(topRaggiuntiProv1==vTopRaggiunti1){
		difftImp1 = tImp1 - vtImp1;
	}
	else{
		difftImp1 = (OCR4A - vtImp1) + (tImp1) + ((topRaggiuntiProv1-vTopRaggiunti1)*OCR4A);
		
	}
	vtImp1 = tImp1;
	vTopRaggiunti1 = topRaggiuntiProv1;
	speed[1] = 62500.0 / (difftImp1 * 622.0);
}

ISR(INT3_vect){
	tImp2 = TCNT4;
	topRaggiuntiProv2=topRaggiunti;
	if(topRaggiuntiProv2==vTopRaggiunti2){
		difftImp2 = tImp2 - vtImp2;
	}
	else{
		difftImp2 = (OCR4A - vtImp2) + (tImp2) + ((topRaggiuntiProv2-vTopRaggiunti2)*OCR4A);
		
	}
	vtImp2 = tImp2;
	vTopRaggiunti2 = topRaggiuntiProv2;
	speed[2] = 62500 / (difftImp2 * 622);
}

ISR(INT5_vect){
	tImp3 = TCNT4;
	topRaggiuntiProv3=topRaggiunti;
	if(topRaggiuntiProv3==vTopRaggiunti3){
		difftImp3 = tImp3 - vtImp3;
	}
	else{
		difftImp3 = (OCR4A - vtImp3) + (tImp3) + ((topRaggiuntiProv3-vTopRaggiunti3)*OCR4A);
		
	}
	vtImp3 = tImp3;
	vTopRaggiunti3 = topRaggiuntiProv3;
	speed[3] = 62500 / (difftImp3 * 622);
}

ISR(INT4_vect){
	tImp4 = TCNT4;
	topRaggiuntiProv4=topRaggiunti;
	if(topRaggiuntiProv4==vTopRaggiunti4){
		difftImp4 = tImp4 - vtImp4;
	}
	else{
		difftImp4 = (OCR4A - vtImp4) + (tImp4) + ((topRaggiuntiProv4-vTopRaggiunti4)*OCR4A);
		
	}
	vtImp4 = tImp4;
	vTopRaggiunti4 = topRaggiuntiProv4;
	speed[4] = 62500 / (difftImp4 * 622);
}
ISR (USART0_RX_vect){
	char n=UDR0;
	if (n!= 'e'){
		
		num[countForSer]=n;				//riempie la stringa dalla cifra pi? significativa
		countForSer++;
		Serial_Tx(n);
		SerialN();
	}
	
	else{
		operation=num[countForSer-1];
		Serial_Tx(operation);
		Serial_Send("\t");
		num[countForSer-1]=0;
		for(int i=0;i<countForSer-1;i++)Serial_Tx(num[i]);
		Serial_Send("\t");
		float wantedSpeed=atof(num);
		Serial_Send(wantedSpeed);
		SerialN();
		switch (operation)
		{
			case '0':
			if (wantedSpeed<0){
				PORTD=(1<<PIND6)|(1<<PIND4);
				wantedSpeed=-wantedSpeed;
			}
			else PORTD=(1<<PIND7)|(1<<PIND5);
			
			for(int i=0; i<2; i++)setpoint[i]=wantedSpeed;
			if(wantedSpeed==0)for(int i=0; i<2; i++)setpoint[i]=-1000;
			break;
			
			case '1':
			Serial_Send(1);
			SerialN();
			SerialN();
			break;
			
			case '2':
			angle=wantedSpeed;
			Serial_Send(angle);
			SerialN();
			SerialN();
			break;
		
			default:
			Serial_Send("ciao");
			SerialN();
			SerialN();
			break;
		}

		for(countForSer; countForSer>=0; countForSer--) num[countForSer]=0;
		countForSer++;
		
	}
}
