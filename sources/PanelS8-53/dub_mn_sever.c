//????? ????????? ?????? ? ?????.
//????????? ?????? ? ????? ?????????? ?? SPI(??????? ?????) ? ??????????? ????????? ??????????? ?????? 10?? 

#include <18F2515.h>
#include <P18F2515.INC>
#device ADC=10
//#include "stdio.h"
//#include "stdlib.h"
//#include "math.h"

// Fuses: LP,XT,HS,EC_IO,INTRC_IO,INTRC,RC_IO,RC,NOWDT,WDT,PUT,NOPUT
// Fuses: NOMCLR,MCLR,NOBROWNOUT,BROWNOUT,PROTECT,NOPROTECT,CPD,NOCPD
//#fuses INTRC_IO, NOWDT, NOPROTECT, MCLR, BROWNOUT, PUT
#use delay(clock=8000000)
//#use fast_io(C)
//#use fixed_io(c_outputs=PIN_C0,PIN_C1,PIN_C2)

#zero_ram

#define SL0 PIN_A0
#define SL1 PIN_A1
#define SL2 PIN_A2
#define SL3 PIN_A3
#define SL4 PIN_A4
#define SL5 PIN_A5

#define sw_K1 PIN_C0
#define sw_K2 PIN_C1
#define sw_Sx PIN_C2

//#define SL0_H	OUTPUT_HIGH(SL0)

#define SL0_L	OUTPUT_LOW(SL0)
#define SL1_L	OUTPUT_LOW(SL1)
#define SL2_L	OUTPUT_LOW(SL2)
#define SL3_L	OUTPUT_LOW(SL3)
#define SL4_L	OUTPUT_LOW(SL4)
#define SL5_L	OUTPUT_LOW(SL5)

const char mask[6] = {0x3e,0x3d,0x3b,0x37,0x2f,0x1f};
const char mask_kn[6] = {0x21,0x21,0x21,0x39,0x21,0x3f};

#bit RC6=PORTC.6
#bit RC0=PORTC.0
#bit RA0=PORTA.0
void pit_bkl(void);    
void pit_bykl(void);


char tek_state_rb[6]; //, tek_state_ruch[5];
char tek1_state_rb,tek2_state_rb,tek3_state_rb;
char pr_state_rb[6]; //, pr_state_ruch[5];
char ii,ib,pr_ruk;
char kod,du_pA;
char buf_per,buf_prm,pr_wkl_pit; 
char buff[100],tt,i;
/*
#INT_TIMER0  //???????? ?????? 500 mks
void sch_wr500(void)
{ 
  set_timer0(64536);     //184  256-(0,0005/(4/1000000))=65036 -- (500 mks)       
 // RC6=~RC6;  	
  if (RC6==0)   RC6=1;  //RC6=~RC6;
	else  RC6=0; 
  //  set_timer0(131);     //184  65536-(0,0005/(4/1000000))=65036 -- (500 mks)      
}//@
*/

#INT_TIMER1 //???????? ?????? 10ms
void Prd_kn(void)  //?? SPI ?????. ????. ?? ? ???? ????????? ?? STM
{
// set_timer1(63036);  //65536-(0,01/(4/1000000))=65536-10000=63036 -- (10ms)  
//if (spi_data_is_in())  //true-???? ? ????? SPI ??????? ??????
//  buf_prm=spi_read(); 
 while (!(sspstat&0x01))  {}  
  buf_prm=SSPBUF;            //?????? ?????? SPI
  kod=buf_prm&0x7f;

  if (( kod > 0)&&( kod <4))  //Kan1,Kan2,Sinchr,
     {
      //???????? ??? ????? ?????????:
      if  (buf_prm &(1 << 7))
           //???????? - (0)
           switch(kod)
			{
             case 1: 
               {
                 output_low(sw_K1); 
                 break; 
               }
              case 2: 
               {
                 output_low(sw_K2); 
                 break; 
               }
              case 3: 
               {
                 output_low(sw_Sx); 
                 break; 
               }   
            default:
              break;  
            }
      else    //????? - (1)
          switch(kod)
			{
             case 1: 
               {
                 output_high(sw_K1); 
                 break; 
               }
              case 2: 
               {
                 output_high(sw_K2); 
                 break; 
               }
              case 3: 
               {
                 output_high(sw_Sx); 
                 break; 
               }   
            default:
              break;  
            }
     }

  if ( kod ==4)
     if  (buf_prm & (1 << 7))
          pit_bkl();
     else
          pit_bykl();

  sspcon1=sspcon1&0x7f;   //????? ???? ?????????
    //????? ????? ? ????????:   
     
    output_high(pin_C6); 
   // spi_write(buf_per); //?????? ????? ? SPI
    SSPBUF=buf_per;
    buf_per=0;
    output_low(pin_C6);
 
 set_timer1(45536);  //65536-(0,01/(4/8000000))=20000 -- (10ms)  

}//@


void main ()
{
disable_interrupts(GLOBAL);   //????? ?????????? ??????????
setup_oscillator(OSC_8MHZ);  //|OSC_INTRC
 
set_tris_a(0x00);   //??????
set_tris_b(0xff);   //?????
set_tris_c(0x10);   //??????, RC4-????(?????? ?? STM)

//output_c(0x4f);
portc=0x4f;
porta=0x7f;
portb=0xff;

//set_tris_c(0x10);   //??????, RC4-????(?????? ?? STM)
//output_a(0x7f);
//output_b(0xff);

pit_bykl();
pr_wkl_pit=0;   //??. ???. ??????? (???. ??? 1-?? ??????? ??. "???????")

delay_ms(1000);  //??? ????????? STM

//????????????? ?????? MSSP(????? SPI)
SSPCON1=0x31;
SSPSTAT=0;    //0x80;
SSPBUF=0x55;

//????. ????????:
//t0con=0xc8; //8-?? ????
t0con=0x88; //16-?? ???? ???????
T1CON=0x85;
setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);  

set_timer1(62286);
set_timer0(64536); 

//enable_interrupts(INT_TIMER2);
enable_interrupts(INT_TIMER1);
//enable_interrupts(INT_TIMER0);
enable_interrupts(GLOBAL);

//????????? ?????? ? ????? ?? ?????????:
//----------------------------------------
for (ii=0; ii<6;ii++) 
 {
   du_pA=porta&0xc0|mask[ii];
   output_a(du_pA);   
   pr_state_rb[ii] = input_b();
 }

while(1)
{
   for (ii=0; ii<6;ii++) 
 {
   pr_ruk=0;
  // output_a(mask[ii]);              //SL... -> PORTA
   du_pA=porta&0xc0|mask[ii];
   output_a(du_pA); 
   tek_state_rb[ii] = input_b();
   if (pr_state_rb[ii] ^ tek_state_rb[ii])
    //????????  ??????? ??? ???????? ?
   {
    if ((tek_state_rb[ii]&mask_kn[ii])!=mask_kn[ii])  //???? ??? ??????, ?? ???????. ???????
       { //??????
	    delay_ms(1);
	    tek1_state_rb = input_b();
	    delay_ms(1);
	    tek2_state_rb = input_b();
	    delay_ms(1);
	    tek3_state_rb = input_b();
	
	//    if (tek_state_rb[ii] == tek3_state_rb)  //??????? ???.-???? ???????/??????? ??? ????????!
       }
     else  //?????
       {
         tek3_state_rb=tek_state_rb[ii];

       }

       if (tek_state_rb[ii] == tek3_state_rb)  //??????? ???.-???? ???????/??????? ??? ????????!

       switch(ii)   //????? ??????? SL0,SL1,SL2,SL3,SL4,SL5
			{
              case 0:  //SL0
                //?????? K1, Ser:
				if (!(tek3_state_rb & (1 << 0)))  { buf_per=0x81;  //?1-???.
                                                    do
                                                     tek3_state_rb = input_b(); 
                                                    while (!(tek3_state_rb & (1 << 0)));
                                                    //??????? ?????????
                                                    while (buf_per!=0) {}
                                                     buf_per=0x01;  //?1-???.
                                                    break; 
                                                  }
                  if (!(tek3_state_rb & (1 << 5))) { buf_per=0x82;  //?2-???.
                                                    do
                                                     tek3_state_rb = input_b(); 
                                                    while (!(tek3_state_rb & (1 << 5)));
                                                    //??????? ?????????
                                                    while (buf_per!=0) {}
                                                     buf_per=0x02;  //?2-???.
                                                    break; 
                                                  }

                //????? -B_K1, +B_K1 -Sm_K1, +Sm_K1:
                //***********************************
                
              if  ((tek3_state_rb & 0x02)&&(tek3_state_rb & 0x04))  //RB1 & RB2==1
                    { 
                     for (ib=0; ib<2;ib++) 
                     {
                       if ((tek3_state_rb & (0x02 << ib)) && (!(pr_state_rb[ii] & (0x02 << ib))))
                            
                            switch(ib)
			                {
                               case 0: 
                                pr_ruk=1;
                                buf_per=0x14;  //?20-????? 
                               break;

                               case 1:
                                pr_ruk=1;  
                                buf_per=0x94;  //?20-??????
                               break;
                            }//switch 
                     } //for  ib    
                   } //if

          //    else
          //     {                 
               if  ((tek3_state_rb & 0x08)&&(tek3_state_rb & 0x10)&& (!pr_ruk))   //RB3 & RB4==1 & pr_ruk=0
                 {
                     for (ib=0; ib<2;ib++) 
                   {

                     if ((tek3_state_rb & (0x08 << ib)) && (!(pr_state_rb[ii] & (0x08 << ib))))
                             
                            switch(ib)
			                {
                               case 0: 
                               {
                                buf_per=0x15;  //?21-????? 
                                break;
                               }

                               case 1: 
                               {
                                buf_per=0x95;  //?21-??????
                               break;
                               }
                            }//switch ib
                     } //for
                   
                   } //if
         //      } //else
              pr_state_rb[ii]=tek3_state_rb;
              pr_ruk=0;
			  break;
                               
              case 1:    //SL1
               //?????? K2, DISP:
				if (!(tek3_state_rb & (1 << 0))) { buf_per=0x83;  //?3-???.
                                                    do
                                                     tek3_state_rb = input_b(); 
                                                    while (!(tek3_state_rb & (1 << 0)));
                                                    //??????? ?????????
                                                    while (buf_per!=0) {}
                                                     buf_per=0x03;  //?2-???.
                                                    break; 
                                                  }
                  if (!(tek3_state_rb & (1 << 5))) { buf_per=0x84;  //?4-???.
                                                    do
                                                     tek3_state_rb = input_b(); 
                                                    while (!(tek3_state_rb & (1 << 5)));
                                                    //??????? ?????????
                                                    while (buf_per!=0) {}
                                                     buf_per=0x04;  //?4-???.
                                                    break; 
                                                  }

                //????? -B_K2, +B_K2 -Sm_K2, +Sm_K2:
                //***********************************
                
              if  ((tek3_state_rb & 0x02)&&(tek3_state_rb & 0x04))  //RB1 & RB2==1
                    { 
                     for (ib=0; ib<2;ib++) 
                     {
                       if ((tek3_state_rb & (0x02 << ib)) && (!(pr_state_rb[ii] & (0x02 << ib))))
                             
                            switch(ib)
			                {
                               case 0: 
                                pr_ruk=1;
                                buf_per=0x16;  //?22-?????
                               break;

                               case 1: 
                                pr_ruk=1;
                                buf_per=0x96;  //?22-??????
                               break;
                            }//switch 
                     } //for      
                   } //if

           //   else
           //    {                 
               if  ((tek3_state_rb & 0x08)&&(tek3_state_rb & 0x10) && (!pr_ruk))   //RB3 & RB4==1 & (!pr_ruk)
                 {
                     for (ib=0; ib<2;ib++) 
                   {

                     if ((tek3_state_rb & (0x08 << ib)) && (!(pr_state_rb[ii] & (0x08 << ib))))
                             
                            switch(ib)
			                {
                               case 0: 
                               {
                                buf_per=0x17;  //?23-?????
                                break;
                               }

                               case 1: 
                               {
                               buf_per=0x97;  //?23-??????
                               break;
                               }
                            }//switch 
                     } //for
                   
                   } //if
           //    } //else
//              pr_state_rb[ii]=tek3_state_rb;
              pr_ruk=0;
						 		                        
			  break;
      
              case 2: //SL2
               //?????? Raz, Pam:
				if (!(tek3_state_rb & (1 << 0))) { buf_per=0x85;  //?5-???.
                                                    do
                                                     tek3_state_rb = input_b(); 
                                                    while (!(tek3_state_rb & (1 << 0)));
                                                    //??????? ?????????
                                                    while (buf_per!=0) {}
                                                     buf_per=0x05;  //?5-???.
                                                    break; 
                                                  }
                  if (!(tek3_state_rb & (1 << 5))) { buf_per=0x86;  //?6-???.
                                                    do
                                                     tek3_state_rb = input_b(); 
                                                    while (!(tek3_state_rb & (1 << 5)));
                                                    //??????? ?????????
                                                    while (buf_per!=0) {}
                                                     buf_per=0x06;  //?6-???.
                                                    break; 
                                                  }
                //????? -T, +T -Del, +Del:
                //***********************************
                
              if  ((tek3_state_rb & 0x02)&&(tek3_state_rb & 0x04))  //RB1 & RB2==1
                    { 
                     for (ib=0; ib<2;ib++) 
                     {
                       if ((tek3_state_rb & (0x02 << ib)) && (!(pr_state_rb[ii] & (0x02 << ib))))
                             
                            switch(ib)
			                {
                               case 0: 
                                pr_ruk=1;
                                buf_per=0x18;  //?24-?????
                               break;

                               case 1: 
                                pr_ruk=1;
                                buf_per=0x98;  //?24-??????
                               break;
                            }//switch 
                     } //for      
                   } //if

           //   else
               {                 
               if  ((tek3_state_rb & 0x08)&&(tek3_state_rb & 0x10) && (!pr_ruk))   //RB3 & RB4==1 & (!pr_ruk)
                 {
                     for (ib=0; ib<2;ib++) 
                   {

                     if ((tek3_state_rb & (0x08 << ib)) && (!(pr_state_rb[ii] & (0x08 << ib))))
                             
                            switch(ib)
			                {
                               case 0: 
                               {
                                buf_per=0x19;  //?25-?????
                                break;
                               }

                               case 1: 
                               {
                               buf_per=0x99;  //?25-??????
                               break;
                               }
                            }//switch 
                     } //for
                   
                   } //if
               } //else
//              pr_state_rb[ii]=tek3_state_rb; 
			  pr_ruk=0;			 		                        
			  break; 

              case 3:  //SL3

               //?????? Sinch, Pusk, Kurs, Izm:
				if (!(tek3_state_rb & (1 << 0))) { buf_per=0x87;  //?7-???.
                                                    do
                                                     tek3_state_rb = input_b(); 
                                                    while (!(tek3_state_rb & (1 << 0)));
                                                    //??????? ?????????
                                                    while (buf_per!=0) {}
                                                     buf_per=0x07;  //?7-???.
                                                    break; 
                                                  }
                if (!(tek3_state_rb & (1 << 3))) { buf_per=0x88;  //?8-???.
                                                    do
                                                     tek3_state_rb = input_b(); 
                                                    while (!(tek3_state_rb & (1 << 3)));
                                                    //??????? ?????????
                                                    while (buf_per!=0) {}
                                                     buf_per=0x08;  //?8-???.
                                                    break; 
                                                  }
                if (!(tek3_state_rb & (1 << 4))) { buf_per=0x89;  //?9-???.
                                                    do
                                                     tek3_state_rb = input_b(); 
                                                    while (!(tek3_state_rb & (1 << 4)));
                                                    //??????? ?????????
                                                    while (buf_per!=0) {}
                                                     buf_per=0x09;  //?9-???.
                                                    break; 
                                                  }
                if (!(tek3_state_rb & (1 << 5))) { buf_per=0x8A;  //?10-???.
                                                    do
                                                     tek3_state_rb = input_b(); 
                                                    while (!(tek3_state_rb & (1 << 5)));
                                                    //??????? ?????????
                                                    while (buf_per!=0) {}
                                                     buf_per=0x0A;  //?10-???.
                                                    break; 
                                                  }

                //????? -Ur, +Ur :
                //****************
                
              if  ((tek3_state_rb & 0x02)&&(tek3_state_rb & 0x04))  //RB1 & RB2==1
                    { 
                     for (ib=0; ib<2;ib++) 
                     {
                       if ((tek3_state_rb & (0x02 << ib)) && (!(pr_state_rb[ii] & (0x02 << ib))))
                             
                            switch(ib)
			                {
                               case 0: 
                                buf_per=0x1A;  //?26-?????
                               break;

                               case 1:
                               buf_per=0x9A;  //?26-??????
                               break;
                            }//switch 
                     } //for      
                   } //if
//			    pr_state_rb[ii]=tek3_state_rb; 			 		                        
			  break;

              case 4: // SL4
              //?????? Pit, Sbor:
				if (!(tek3_state_rb & (1 << 0)))  { buf_per=0x8B;  //?11-???.
                                                    if (pr_wkl_pit==0)
                                                      {
                                                       pit_bkl();
                                                       
                                                      } 
                                                    break; 
                                                  }
                  if (!(tek3_state_rb & (1 << 5))) { buf_per=0x8C;  //?12-???.
                                                    do
                                                     tek3_state_rb = input_b(); 
                                                    while (!(tek3_state_rb & (1 << 5)));
                                                    //??????? ?????????
                                                    while (buf_per!=0) {}
                                                     buf_per=0x0C;  //?12-???.
                                                    break; 
                                                  }


                //????? -Ur, +Ur :
                //****************
                
              if  ((tek3_state_rb & 0x02)&&(tek3_state_rb & 0x04))  //RB1 & RB2==1
                    { 
                     for (ib=0; ib<2;ib++) 
                     {
                       if ((tek3_state_rb & (0x02 << ib)) && (!(pr_state_rb[ii] & (0x02 << ib))))
                             
                            switch(ib)
			                {
                               case 0: 
                                buf_per=0x1B;  //?27-?????
                               break;

                               case 1: 
                                buf_per=0x9B;  //?27-??????
                               break;
                            }//switch 
                     } //for      
                   } //if
//			     pr_state_rb[ii]=tek3_state_rb; 			 		                        
			  break; 

              case 5: //SL5 
               //?????? Men,"1", "2", "3", "4", "5":
				if (!(tek3_state_rb & (1 << 0))) { buf_per=0x8D;  //?13-???.
                                                    do
                                                     tek3_state_rb = input_b(); 
                                                    while (!(tek3_state_rb & (1 << 0)));
                                                    //??????? ?????????
                                                    while (buf_per!=0) {}
                                                     buf_per=0x0D;  //?13-???.
                                                    break; 
                                                  }
                if (!(tek3_state_rb & (1 << 1))) { buf_per=0x8e;  //?14-???.
                                                    do
                                                     tek3_state_rb = input_b(); 
                                                    while (!(tek3_state_rb & (1 << 1)));
                                                    //??????? ?????????
                                                    while (buf_per!=0) {}
                                                     buf_per=0x0e;  //?14-???.
                                                    break; 
                                                  }
                if (!(tek3_state_rb & (1 << 2))) { buf_per=0x8f;  //?15-???.
                                                    do
                                                     tek3_state_rb = input_b(); 
                                                    while (!(tek3_state_rb & (1 << 2)));
                                                    //??????? ?????????
                                                    while (buf_per!=0) {}
                                                     buf_per=0x0f;  //?15-???.
                                                    break; 
                                                  }
                if (!(tek3_state_rb & (1 << 3))) { buf_per=0x90;  //?16-???.
                                                    do
                                                     tek3_state_rb = input_b(); 
                                                    while (!(tek3_state_rb & (1 << 3)));
                                                    //??????? ?????????
                                                    while (buf_per!=0) {}
                                                     buf_per=0x10;  //?16-???.
                                                    break; 
                                                  }
                if (!(tek3_state_rb & (1 << 4))) { buf_per=0x91;  //?17-???.
                                                    do
                                                     tek3_state_rb = input_b(); 
                                                    while (!(tek3_state_rb & (1 << 4)));
                                                    //??????? ?????????
                                                    while (buf_per!=0) {}
                                                     buf_per=0x11;  //?17-???.
                                                    break; 
                                                  }
                if (!(tek3_state_rb & (1 << 5))) { buf_per=0x92;  //?18-???.
                                                    do
                                                     tek3_state_rb = input_b(); 
                                                    while (!(tek3_state_rb & (1 << 5)));
                                                    //??????? ?????????
                                                    while (buf_per!=0) {}
                                                     buf_per=0x12;  //?18-???.
                                                    break; 
                                                  }

  
                pr_state_rb[ii]=tek3_state_rb; 						 		                        
			  break;

              default:
              break;  

            }//switch(ii) 
    pr_state_rb[ii]=tek3_state_rb;         
   }//if
     
 }//for (ii)

}//while(1)
//output_bit(PIN_C3,1);
//BUF_CAP[i-1]=input(PIN_C4);
//output_bit( PIN_B2, 0);  //Ldac  ? 0

//level = input_state(pin_A3);

}  //main


void pit_bkl(void)
{
  output_high(pin_A6);
  output_low(pin_A7);
  output_high(pin_C7);
  pr_wkl_pit=1;
}
    
void pit_bykl(void)
{
  output_low(pin_A6);
  output_high(pin_A7);
  output_low(pin_C7);
  pr_wkl_pit=0; 
}

/*  !!!!!!!!!!!!!!!!!!!!
void pit_bkl(void)
{
  porta=porta & 0xbf;
  porta=porta | 0x80;
  portc=portc | 0x90;
  pr_wkl_pit=1;
}
    
void pit_bykl(void)
{
  porta=porta | 0x40;
  porta=porta & 0x7f;
  portc=portc & 0x6f;
  pr_wkl_pit=0; 
}


//????? ?? STM

  buf_prm=SSPBUF; 
  kod=buf_prm&0x7f;

  if (( kod !=0)&&( kod <4))  //Kan1,Kan2,Sinchr,
     {
      if  (buf_prm & (1 << 7))
          portc=portc | (1 << (kod -1));  //bit=1
      else 
          portc=portc ^ (1 << (kod -1));  //bit=0
     }
  if ( kod ==4)
     if  (buf_prm & (1 << 7))
          pit_bkl();
     else
          pit_bykl();



    if ((sspstat&0x01)==1)
   {
    SSPBUF=0x55;
   }
   set_timer1(63036);
*/ 






/*



void obr_ruk_Bp_K1(void)
{ 
 buf[ik]=tek3_state_rb;
 ik=ik+1;

  buf_per[kk]=2;
 kk=kk+1; 
  buf_per[kk]=0x0a;
  kk=kk+1;
 //putc('2');
} 
*/
