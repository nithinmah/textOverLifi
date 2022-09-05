#include <stdio.h>
#include <malloc.h>
#include <sys/time.h>
#include <string.h>
#include <wiringPi.h>


//char result[3000]={'1','0','1','0','1','0','1','0','1','0','1','1','1','1','1','1','1','1','1','1'};
//int counter=20;

typedef struct {
  char preamble[20];
  char payload_size[16];
  char payload[2500];
} wiremsg;

void chartobin(char c)
{
    int i;
    for(i=7; i>=0;i--){
        result[counter]= (c&(1<<i))?'1':'0';  
        counter++;
    }
}

void int2bin(unsigned integer, int n)
{  
    for (int i=0;i<n;i++)   
    {
    //binary[i] = (integer & (int)1<<(n-i-1)) ? '1' : '0';
    //binary[n]='\0';
    result[counter]= (integer & (int)1<<(n-i-1)) ? '1' : '0';
    result[36]='\0';
    counter++;
    }
    
}

void setHeaderPayloadSizeField(unsigned msgLen, wiremsg* lifiWireMsg_p)
{
  int payload_size_filed_len = sizeof(lifiWireMsg_p->payload_size);
  for (int i=0;i<payload_size_filed_len;i++)   
  {
  //binary[i] = (integer & (int)1<<(n-i-1)) ? '1' : '0';
  //binary[n]='\0';
    lifiWireMsg_p->payload_size[i] = (msgLen & (int)1<<(payload_size_filed_len-i-1)) ? '1' : '0';
  }
  lifiWireMsg_p->payload_size[i] = result[36]='\0';
}

void setPayload(unsigned msgLen, wiremsg* lifiWireMsg_p, char * input_buffer)
{
  for (int i=0;i<msgLen;i++)   
  {
    for(int j=7; j>=0;j--)
    {
        lifiWireMsg_p->payload[i]= (input_buffer[i]&(1<<j))?'1':'0';  
    }
  }
}


int pos=0;

int main()
{
  struct timeval tval_before, tval_after, tval_result;
  char input_buffer[2500];
  wiremsg lifiWireMsg = 
  {
    .preamble = {'1','0','1','0','1','0','1','0','1','0','1','1','1','1','1','1','1','1','1','1'};
  }
    
  wiringPiSetup () ;
  pinMode (0, OUTPUT) ;

  while(1) 
  {
    printf(" Please input the message to send");
    scanf("%[^'\n']",input_buffer);
    int msg_len = strlen(input_buffer);
    setHeaderPayloadSizeField(msg_len, &lifiWireMsg); 
    setPayload(msg_len, &lifiWireMsg, input_buffer);
    int bitPos = 0;
    
  /*Read message
  //int len, k, length;
       
        
 //       int2bin(len*8, 16); //Mal 8, weil ein Byte 8 Bit sind   
 //       printf ("Frame Header (Synchro and Textlength = %s\n", result);
        
        for(k=0;k<len;k++)
        {
                chartobin(input_buffer[k]);            
        }
        */
    
    
    
    wireMsgLen = sizeof(lifiWireMsg.preamble) + sizeof(lifiWireMsg.payload_len) + msg_len;
    //length=strlen(result);
    gettimeofday(&tval_before, NULL);
    //while(pos!=length)
    while(bitPos <= wireMsgLen)
    {
      gettimeofday(&tval_after, NULL);
      timersub(&tval_after, &tval_before, &tval_result);
      double time_elapsed = (double)tval_result.tv_sec + ((double)tval_result.tv_usec/1000000.0f);
        
      while(time_elapsed < 0.001)
      {
        gettimeofday(&tval_after, NULL);
        timersub(&tval_after, &tval_before, &tval_result);
        time_elapsed = (double)tval_result.tv_sec + ((double)tval_result.tv_usec/1000000.0f);
      }
      gettimeofday(&tval_before, NULL);
      char bitToSend;
      if (bitPos < sizeof(lifiWireMsg.preamble))
      {
        bitToSend = lifiWireMsg.preamble[bitPos]
      }
      else if (bitPos < sizeof(lifiWireMsg.preamble) + sizeof(lifiWireMsg.payload_len))
      {
        bitToSend = lifiWireMsg.payload_size[bitPos - sizeof(lifiWireMsg.preamble)];
      }
      else
      {
        bitToSend = lifiWireMsg.payload_size[bitPos - sizeof(lifiWireMsg.preamble) -sizeof(lifiWireMsg.payload_len)];
      }
        
      //if (result[pos]=='1')
      if (bitToSend=='1')
      {
        digitalWrite (0, HIGH);
      }
//      else if(result[pos]=='0'){
      else 
      {
        digitalWrite (0,  LOW) ;
      }
      bitToSend++; 
    }
  }
  //return 0;
}
