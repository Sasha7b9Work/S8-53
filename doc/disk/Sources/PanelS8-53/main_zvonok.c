#include <12f629.h>

// Fuses: LP,XT,HS,EC_IO,INTRC_IO,INTRC,RC_IO,RC,NOWDT,WDT,PUT,NOPUT
// Fuses: NOMCLR,MCLR,NOBROWNOUT,BROWNOUT,PROTECT,NOPROTECT,CPD,NOCPD
#fuses INTRC_IO, NOWDT, NOPROTECT, MCLR, BROWNOUT, PUT, NOCPD
#use delay(clock=4000000)

#zero_ram



/*
------------------------
	Подключение к МК
-----------------------
	A1 ->  GP1
	B1 ->  GP2
	C1 ->  GP4
	zvonok -> GP5
*/


#define  GP0	PIN_A0
#define  GP1	PIN_A1
#define  GP2	PIN_A2
#define  GP3	PIN_A3
#define  GP4	PIN_A4
#define  GP5	PIN_A5


//const int16	timeout_array[8] = {179,192,208,227,250,278,313,357};

const int16	timeout_array[10] = {344,345,333,313,294,277,266,258,250,243};

//const int16	timeout_array[2] = {500,250};
int8 current_input_state = 0xff;
int8 new_input_state;

int8 array_index;
/*
void main(void)  //TEST
{
	set_tris_a(0b011111);//GP5 output
	port_a_pullups(0b010110);//enable pull-up resistors
	
	while(1)
	{	

      for (array_index=0; array_index<8; array_index++)

        {
		output_high(GP5);
		delay_us(timeout_array[array_index]);
		output_low(GP5);
		delay_us(timeout_array[array_index]);
        delay_ms(1000);

       	}
      delay_ms(1500);		
	}

}

*/

int main(void)
{
	set_tris_a(0b011111);//GP5 output
	port_a_pullups(0b010110);//enable pull-up resistors
	
	while(1)
	{	
		new_input_state = input_a();
		if(current_input_state ^ new_input_state)
		{
			current_input_state = new_input_state;
			array_index = 0;
		
			if(current_input_state & (1 << 1))
				array_index |= (1 << 0);
			if(current_input_state & (1 << 2))
				array_index |= (1 << 1);
 			if(current_input_state & (1 << 4))
				array_index |= (1 << 2);
		
		//	array_index++;
		//	array_index %= 8;
		}
		output_high(GP5);
		delay_us(timeout_array[array_index]);
		output_low(GP5);
		delay_us(timeout_array[array_index]);			
	}

	return 0;
}

