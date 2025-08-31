#define PLAYER_WAIT_TIME 2000 

byte sequence[100];           
byte  curLen = 0;          
byte inputCount =  0;        
byte lastInput = 0;         
byte  expRd = 0;              
bool  btnDwn = false;         
bool wait =  false;          
bool  resetFlag = false;       

byte soundPin = 5;            

byte noPins =  4;          
byte pins[] = {13, 10};  
                              
long inputTime = 0;           

void setup()  {
  delay(3000);                
  Serial.begin(9600);    
  Reset();
}

void setPinDirection(byte dir){
  for(byte i = 0; i < noPins; i++){
    pinMode(pins[i], dir); 
  }
}

void writeAllPins(byte val){
  for(byte  i = 0; i < noPins; i++){
    digitalWrite(pins[i], val); 
  }
}

void beep(byte freq){
  analogWrite(soundPin,  2);
  delay(freq);
  analogWrite(soundPin, 0);
  delay(freq);
}

void flash(short freq){
  setPinDirection(OUTPUT);  
  for(int i = 0; i < 5; i++){
    writeAllPins(HIGH);
    beep(50);
    delay(freq);
    writeAllPins(LOW);
    delay(freq);
  }
}


void Reset(){
  flash(500);
  curLen = 0;
  inputCount  = 0;
  lastInput = 0;
  expRd = 0;
  btnDwn = false;
  wait = false;
  resetFlag = false;
}


void Lose(){
  flash(50);  
}


void playSequence(){
  for(int i = 0; i < curLen; i++){
      Serial.print("Seq: ");
      Serial.print(i);
      Serial.print("Pin: ");
      Serial.println(sequence[i]);
      digitalWrite(sequence[i],  HIGH);
      delay(500);
      digitalWrite(sequence[i], LOW);
      delay(250);
    } 
}


void DoLoseProcess(){
  Lose();         
  delay(1000);
  playSequence();    
  delay(1000);
  Reset();          
}


void  loop() {  
  if(!wait){      
    setPinDirection(OUTPUT);                      
    
    randomSeed(analogRead(A0));                  
    sequence[curLen] = pins[random(0,noPins)];    
    curLen++;                                     
    
    playSequence();                               
    beep(50);                                     
    
    wait = true;                                
    inputTime  = millis();                       
  }else{ 
            
    setPinDirection(INPUT);                       

    if(millis() - inputTime  > PLAYER_WAIT_TIME){  
      DoLoseProcess();                            
      return;
    }      
        
    if(!btnDwn){                                  
      expRd  = sequence[inputCount];              
      Serial.print("Expected: ");                 
      Serial.println(expRd);                      
      
      for(int i = 0; i < noPins; i++){           
        if(pins[i]==expRd)                        
          continue;                              
        if(digitalRead(pins[i]) == HIGH){         
          lastInput = pins[i];
          resetFlag = true;                       
          btnDwn = true;                          
          Serial.print("Read: ");                 
          Serial.println(lastInput);              
        }
      }      
    }

    if(digitalRead(expRd) ==  1 && !btnDwn)       
    {
      inputTime  = millis();                        
      lastInput = expRd;
      inputCount++;                              
      btnDwn = true;                             
      Serial.print("Read: ");                    
      Serial.println(lastInput);                 
    }else{
      if(btnDwn && digitalRead(lastInput) == LOW){  
        btnDwn = false;
        delay(20);
        if(resetFlag){                              
          DoLoseProcess();                          
        }
        else{
          if(inputCount == curLen){               
            wait = false;                          
            inputCount  = 0;                         
            delay(1500);
          }
        }
      }
    }    
  }
}
