#include <avr/io.h>
#include<stdlib.h>
#include<avr/interrupt.h>
#include<math.h>

int cnt=0,chkm=0,j;
char a,c,ch;
int i,k=0; 
int param[2]={0,0}; 
int p;
int flag=0; 
float l1=10,l2=10; 
int val_a,val_b;

void angle(float a,float b)
{
  val_a=round(110+((420.0/180.0)*a));
  val_b=round(110+((420.0/180.0)*b));
 
  OCR1A=val_a;
  OCR1B=val_b;
}

void ik(int a,int b)
{    
  float A1=0,A2=0; 
  float u=0,l=0,m=0,n=0; 
  if(a==0||b==0)
    flag=1;
  else
  {    
    u=(l2*l2)+(l1*l1)-(a*a)-(b*b);
    l=(2*l1*l2);
    A2= acos(u/l);
       
    if(a==0)
      m=3.14/2;
    else
      m=atan(b/a);
      
    n=atan((l2*sin(A2))/(l1+(l2*cos(A2))));   
    A1= m-n;
              
    A1*=180/3.14;                  
    A2*=180/3.14;                     

    A1=round(A1);
    A2=round(A2);
        
    if(A1>180||A1<0||A2>180||A2<0)
      flag=1;
    
  }
  if(flag==1)  //// if coor values are inappropriate
    {
      char str[]="\n out of reach \n";
      for(j=0;str[j]!='\0';j++)
      {
        ch=str[j];
        UART_send(ch);
      }
      flag=0; 
    }
    
  else    //// if angle values are available
    {    
        angle(A1,A2);
        
        A1=A1/10;
        UART_send((A1+'0'));
        UART_send(' ');
        A2=A2/10;
        UART_send((A2+'0'));
        UART_send(' ');                     
    }
}

void add(char a)
{
    chkm++;
    if(a==',')
    {        
        k++;
    }
    else if(a!=':')
    {
       p=a;
       param[k]=(param[k]*10)+(p-48); //ASCII TO DECIMAL       
    }
    else if(a==':')      
    {
                
       ik(param[0],param[1]);
       for(i=0;i<3;i++)
         param[i]=0;
       k=0; 
       chkm=0;      
    }
}

void UART_init()
{
  UBRR0=103;  // BAUD RATE
  UCSR0B|=(1<<RXEN0)|(1<<TXEN0);  //ENABLING RECIEVE INTERRUPT ENEBLE,TRANSMITTER AND RECIEVER
  UCSR0C|=(1<<UCSZ01)|(1<<UCSZ00);//8BIT TRANSFER
}

char UART_receivechar()
{
  char ch;
  while(UCSR0A!= (UCSR0A|(1<<RXC0)));
  ch=UDR0;
  return(ch);  
}

void UART_send(char X)
{
  while(!(UCSR0A&(1<<UDRE0)));
  UDR0=X;
  UCSR0A|=1<<UDRE0;
}

void timer_init()
{
  DDRB |=(1<<PB1)|(1<<PB2);
  TCCR1A |= (1<<WGM11) |(1<<COM1A1)|( 1<<COM1B1);
  TCCR1B |= (1<<WGM13) | (1<<WGM12) | (1<<CS11)|(1<<CS10);//Prescalar 64
  ICR1=4999;
}

int main(void)
{
  sei();
  UART_init(); 
  timer_init();
   
  angle(20,20);        
  
  while (1)
  {
    if(chkm==0) /// it is 0 when the conversion of coor to angle is done and given to servo
    {
      char str[]="\nEnter the coordinates: (x,y:)  \n";
      for(j=0;str[j]!='\0';j++)
      {
        ch=str[j];
        UART_send(ch);
      }
    }
    c=UART_receivechar();
    add(c);
  }
  
  return 0;
  
} /// end of main
