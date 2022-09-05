//global variables
static unsigned int state;
String bitSequence="";
int payloadLen=0; 
int HEADER_LEN=16;

enum state {waitingPreamble, receiveData};


void setup() {
  //setup timer interrputs for smapling of received bits
  cli();//clear interrupts

  //Timer Register Inits
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1  = 0; //initialize counter value to 0;
  //set timer1 interrupt at 1kHz 
  // set timer count for 1khz increments
  OCR1A = 2001;// = (16*10^6) / (1000*8) - 1 = 2001 for 1kHz;
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS11 bit for 8 prescaler
  TCCR1B |= (1 << CS11);   
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  sei();//enable interrupts
  
  Serial.begin(9600);

  //Input Pin for the Solar Cell
  pinMode(A0,INPUT);

  //Set state to unsyncronized (looking for preamble)
  state = 0;
}

//Timer interrupt handler
ISR(TIMER1_COMPA_vect) 
{
  String data="0";
  int solarCellAnalogValue = analogRead(A0);
  float voltageQuantize = solarCellAnalogValue * (5.0 / 1023.0);

  if (voltageQuantize>=1) 
  {
    bit="1";
    //Serial.println("1");
  }
  else
  {
    bit="0";
    //Serial.println("0");
  }

  //This is the "real" loop function
  switch (state)
  {
    case waitingPreamble:
      //look for preamble 
      checkforPreamble(bit);
      break;
    case receiveData:
      receiveData(data);
      break;
  }
  
}


void loop() {
  // put your main code here, to run repeatedly:
}


void checkforPreamble(String bit)
{
  String preamble="1010101111111111";
  bitSequence.concat(bit);
  bitSequence.remove(0,1);
  if (bitSequence==preamble)
  {
    Serial.println("synchronization done");
    state=receiveData;  
    bitSequence="";
  }
}

void receiveData(String bit)
{
  bitSequence.concat(bit);

  if (bitSequence.length()==HEADER_LEN)
  {
    //Serial.println("data Bits: "+dataBits);
    char charArray[HEADER_LEN+1];
    dataBits.toCharArray(charArray, HEADER_LEN);
    payloadLen = strtol(charArray, NULL, 2);
    //Serial.println(decimalValue);
  }

  if(dataBits.length()==payloadLen+HEADER_LEN) //recieved the entire message
  {
      //Decode the ascii to char
      String output = "";
      for(int i = HEADER_LEN/8; i < (dataBits.length()/8); i++)
      {
          String pl = "";
          for(int l = i*8; l < 8*(i+1); l++){ 
              pl += dataBits[l];
          }    
          
          int n = 0;
          for(int j = 0; j < 8; j++)
          {
              int x = (int)(pl[j])-(int)'0';
              for(int k = 0; k < 7-j; k++)  x *= 2;
              n += x;
          }
          output += (char)n;
      }
      Serial.println(output);
    
      bitSequence="";
      state = waitingPreamble; 
  }
}
