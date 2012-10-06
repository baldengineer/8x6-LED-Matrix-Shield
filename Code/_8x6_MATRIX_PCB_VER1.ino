/*  LED Matrix, starting to play with matrixes for the first time.  */

#define numOfColumns 8
#define numOfRows 6
#define matrixSize 48
#define bigWait 500 // milliseconds
#define povWait 250  // microseconds
#define animationWait 100 // milliseconds
#define frames 10

int columnPins[numOfColumns] = {2,3,4,5,6,7,8,9};
//int sourcePins[numOfColumns] = {2,3,4,5,6,7,8,9};
int rowPins[numOfRows]     = {10,11,12,A0,A1,A2};
//int sinkPins[numOfSinks]     = {10,11,12,A0,A1,A2};


unsigned int rowStates[6] = {0,0,0,0,0,0};
unsigned long characters[4] = {0x699F99,0x99FF99,0xF6666F,0x666606}; // A, H, I, !
unsigned char messageString[25] = {1,2,3}; // "Hi!"

int messageLength = 3;

void setup()   {                
  Serial.begin(9600); 
 // fillRowStates(0);
   setupLEDs();
   blankLEDs();
}

void loop() {  
  for (int messageCharacter=0; messageCharacter < messageLength; messageCharacter++) {
   // load column X for Y letter in message
   for (int characterColumn=4; characterColumn > 0; characterColumn--) {
     shiftDisplayLeft(1,loadCharacterColumn(characterColumn, characters[messageString[messageCharacter]]));
     myDelay(animationWait);
   }
   shiftDisplayLeft(1,0x0);
   myDelay(animationWait);
   shiftDisplayLeft(1,0x0);
   myDelay(animationWait);
  }
  for (int i=0; i<6; i++) {
    shiftDisplayLeft(1,0x0);
    myDelay(animationWait);
  }
} 

// 24  23  22  21
// 20  19  18  17
// 16  15  14  13
// 12  11  10   9
//  8   7   6   5
//  4   3   2   1

int loadCharacterColumn(int column, long character) {
  // example character:  0x699F99
  int returnedValue=0;
  int pointer=0;
  Serial.println(character, HEX);
  for (int i=(20+column); i>0; i=i-4) {
    if (bitRead(character,(i-1))) {    
      bitSet(returnedValue,pointer);
    } else {
      bitClear(returnedValue,pointer);
    }
  pointer++;
  }
  return returnedValue;
}

void loadCharacter(unsigned long thisCharacter) {
  long newCharacter;
  long shiftValue = 0xF00000;
  // thisCharacter example:  0x699F99  
  for(int i=0; i<6; i++) {
    rowStates[(i)] = (shiftValue & thisCharacter) >> (20-(i*4));
    shiftValue = shiftValue >> 4;
  }  
}

void shiftDisplayLeft(int steps, int newRows) {
  int newRowMask = 0x20;
  for (int row=5; row > -1; row--) {
    if (newRows & (newRowMask)) {
      rowStates[row] = (rowStates[row] << steps) | (0x000001);
    } else {
      rowStates[row] = (rowStates[row] << steps) & (0xFFFFFE);
    }
    newRowMask = newRowMask >> 1;
  } 
}


 
void fillRowStates(int setToValue) {
  for (int i=0; i < 8; i++) {
    rowStates[i] = setToValue;
  }
}

void lightMatrix() {
  int LEDCount = -1;
  int thisColumnState = 0;
  
  for (int rowCount = 0; rowCount < numOfRows; rowCount++) {  
    lightColumn(rowStates[rowCount]);
    digitalWrite(rowPins[rowCount], HIGH);
    delayMicroseconds(povWait);
    digitalWrite(rowPins[rowCount], LOW);
  }
} 

void lightColumn(int columnState) {
  for (int columnCount=0; columnCount < numOfColumns; columnCount++) {
    if (bitRead(columnState,columnCount)==1) {
      digitalWrite(columnPins[columnCount], LOW);
    } else {
      digitalWrite(columnPins[columnCount], HIGH);
    }
  }    
}// end of lightColumn

void setupLEDs() {
  // LEDs are forward biased from Row to Column.  Rows have the current limiting resistors.
  for (int i=0; i < numOfColumns; i++) {
    pinMode(columnPins[i], OUTPUT);
    digitalWrite(columnPins[i], HIGH);   // columns need to stay high, to keep LEDs off even for very brief moments.
  }
  
  for (int i=0; i < numOfRows; i++) { 
    pinMode(rowPins[i], OUTPUT);
    digitalWrite(rowPins[i], LOW);  // eliminates the ability for LEDs to turn on
  }
}


void blankLEDs() {
  // the same as the setupLEDs, but this moves a little bit faster!
  for (int i=0; i < numOfColumns; i++) {
    digitalWrite(columnPins[i], HIGH);   // columns need to stay high, to keep LEDs off even for very brief moments.
  }
  
  for (int i=0; i < numOfRows; i++) { 
    digitalWrite(rowPins[i], LOW);  // eliminates the ability for LEDs to turn on
  } 
}

void myDelay(int waitTime) {
  long incomingMills = millis();
  long waitUntilMills = incomingMills + waitTime;
  
  while(millis() < waitUntilMills) {
    lightMatrix();
  }
}


unsigned long pow2(unsigned long j) {
  unsigned long result=1;
  for(int i=0; i < j; i++) {
    result = result * 2;
  }
  return result;
}
