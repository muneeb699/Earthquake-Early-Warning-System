//==========================================================================
//==================inckude=================================
#include<pic.h>

//===============configuration==============================
__CONFIG (0x3F32);

//===============define IO port=============================
#define		sensor1		    RD2    //21

#define		CHANNEL0	    0b10000001	// AN0 pin2		

//==============FUNCTION PTOTOTYPE=========================
void read_adc(void);
//void bcd_ascii_3digits (unsigned int value);
void bcd_ascii_4digits (unsigned int value);
void send_data();
void msdelay (unsigned int itime);


unsigned int result;
unsigned char msb, hundreth, tenth, lsb;
unsigned char vibration_msb, vibration_hundreth, vibration_tenth, vibration_lsb;
unsigned char sensor1_status;
//float float_variable;

void main ()
{

	ADRESH=0;					//clear A/D result
	ADRESL=0;					//clear A/D result

    ///////////////////////////////////////////////////////
	TRISA0=1;			        //configure PORTA I/O direction

	TRISD2=1;			        //configure PORTD I/O direction
	/////////////////setting ADCON1 Register///////////////
	ADCON1=0b11001110;			// A/D result right justified, conversion clk=fosc/64, just A0 as analog input

	//////////////////Configure UART////////////////////////
	TRISC6=0;			//set RC6 as output TX
	TRISC7=1;			//set RC7 as input  RX 
	SPBRG=31;			//set baud rate as 9600 baud @XTAL=20MHZ
//	BRGH=1;				//baud rate high speed option
	TXEN=1;				//enable transmission
	TX9 =0;				//8-bit transmission
	RX9 =0;				//8-bit reception	
	CREN=1;				//enable reception       
	SPEN=1;				//enable serial port


    msdelay(532);          // 4sec switching tyme



    ///////////////////////////////////////////////////////
    while(1)
    {
	   ADCON0=CHANNEL0;				//ADCON0=0b10000001 =>conversion clk=fosc/64, CHANNEL0, ADC ON 
	   read_adc();
       bcd_ascii_4digits (result);
       vibration_msb = msb;
       vibration_hundreth = hundreth;
       vibration_tenth = tenth;
       vibration_lsb = lsb;

       if ( sensor1==0 )
       {
			sensor1_status = '1';
	   }
       else 
       {
			sensor1_status = '0';
	   }
	   send_data();	
       msdelay(140);          // 

    }
} 




///////////////////////////////////////////////////////

void read_adc(void)
{
	unsigned char i;
	unsigned long result_temperary=0;  // 4 byte long
	for(i=200;i>0;i-=1)			    //looping 200 times for getting average value 
	{
//        msdelay(4);                 //30msec
		ADGO = 1;					//ADGO is the bit 2 of the ADCON0 register,,start conversion
		while(ADGO==1);				//ADC start, ADGO=0 after finish ADC progress
		result=ADRESH;
		result=result<<8;			//shift to left for 8 bit
		result=result|ADRESL;		//10 bit result from ADC ,, or function

		result_temperary+=result;		// result_temperary = result_temperary + result
	}
	result = result_temperary/200;		//getting average value

}
///////////////////////////////////////////////////////
 
/*void bcd_ascii_3digits (unsigned int value)
{
	unsigned char i;
    i=value/10; 
    lsb=value%10;
    msb=i/10;
    tenth=i%10;
	msb=msb+0x30;
	tenth=tenth+0x30;
	lsb=lsb+0x30;
} */

///////////////////////////////////////////////////////
void bcd_ascii_4digits(unsigned int value)
{
	unsigned char i,j;
    i = value/10; 
    lsb = value%10;

    j = i/10; 
    tenth = i%10;

    msb = j/10;
    hundreth = j%10;

    lsb = lsb + 0x30;   
    tenth = tenth + 0x30;   
    hundreth = hundreth + 0x30;   
    msb = msb + 0x30;   
} 

///////////////////////////////////////////////////////
void send_data ()
{
    TXREG='A';
	while(TXIF==0);							//only send the new data after the previous data finish sent

    TXREG=vibration_msb;
	while(TXIF==0);							//only send the new data after the previous data finish sent

    TXREG=vibration_hundreth;
	while(TXIF==0);							//only send the new data after the previous data finish sent
 
    TXREG=vibration_tenth;
	while(TXIF==0);							//only send the new data after the previous data finish sent

    TXREG=vibration_lsb;
	while(TXIF==0);							//only send the new data after the previous data finish sent


    TXREG=sensor1_status;
	while(TXIF==0);							//only send the new data after the previous data finish sent

    msdelay(50);                              


}  

///////////////////////////////////////////////////////
 
void msdelay (unsigned int itime)
{
	unsigned int i,j;
	for (i=0;i<itime;i++)
		for (j=0;j<1275;j++);
}
