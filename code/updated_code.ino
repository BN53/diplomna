#define E_DIG1 8
#define D_DIG1 18
#define C_DIG1 17
#define DP1_DIG1 16
#define E_DIG2 15     // 7-segment display pins
#define D_DIG2 7
#define G_DIG2 6
#define C_DIG2 5
#define DP2_DIG2 4
#define F_DIG1 48
#define G_DIG1 45
#define A_DIG1 0
#define B_DIG1 35
#define F_DIG2 36
#define A_DIG2 37
#define B_DIG2 38

#define BTN 47   //button pin

#define TRIG 2   // trigger pin
#define ECHO 1   // echo pin
#define BUZZ 3   // buzzer signal pin



const int dig1Pins[] = { A_DIG1, B_DIG1, C_DIG1, D_DIG1, E_DIG1, F_DIG1, G_DIG1 };

const int dig2Pins[] = { A_DIG2, B_DIG2, C_DIG2, D_DIG2, E_DIG2, F_DIG2, G_DIG2 };

const byte numbers[10] = {
  0b0111111, // 0
  0b0000110, // 1
  0b1011011, // 2
  0b1001111, // 3
  0b1100110, // 4
  0b1101101, // 5
  0b1111101, // 6
  0b0000111, // 7
  0b1111111, // 8
  0b1101111, // 9
};

const byte calibrateWord[10] = {
  0b0111001, // C
  0b1110111, // A
  0b0111000, // L
  0b0110000, // I
  0b1111100, // b
  0b1010000, // r
  0b1110111, // A
  0b0000111, // T1
  0b0000001, // T2
  0b1111001, // E
};

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 7; i++) {
    pinMode(dig1Pins[i], OUTPUT);
    pinMode(dig2Pins[i], OUTPUT);
  }
  pinMode(BTN, INPUT_PULLUP);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZ, OUTPUT);
}

void displayDigit(int num, int digit) {
  if(digit == 1){
    for (int i = 0; i < 7; i++){
      digitalWrite(dig1Pins[i], !(numbers[num] & (1 << i)));
    }
  }
  else{
    for (int i = 0; i < 7; i++){
      digitalWrite(dig2Pins[i], !(numbers[num] & (1 << i)));
    }
  }
}


void displayCalibrate(){
  for(int letter = 0; letter < 10; letter = letter + 2){
    for(int i = 0; i < 7; i++){
      digitalWrite(dig1Pins[i], !(calibrateWord[letter] & (1 << i)));
      digitalWrite(dig2Pins[i], !(calibrateWord[letter+1] & (1 << i)));
    }
    delay(1000);
  }
}

void showPace(short int pace){
  short int dig1 = pace / 10;
  short int dig2 = pace % 10;
  displayDigit(dig1, 1);
  displayDigit(dig2, 2);
}

volatile unsigned long lastTrigger = 0;

void calculatePace(){
  unsigned long currentTrigger = millis();
  float timeDifference = (currentTrigger - lastTrigger) / 1000.0;
  short int pace = 60 / timeDifference;
  if(pace > 99){
    pace = 99;
  }
  lastTrigger = currentTrigger;
  showPace(pace);
}

short int getDistance(){
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  int duration = pulseIn(ECHO, HIGH);
  return (duration * 0.0343) / 2;
}

short int calibrate(){

  for(int i = 0; i < 5; i++){
    digitalWrite(BUZZ, HIGH);
    delay(200);
    digitalWrite(BUZZ, LOW);
    delay(1000);
  }
  return getDistance();
}


short int minDistance = 20; // 20cm default height without calibration

void loop() {

	if (digitalRead(BTN) == HIGH) {
      displayCalibrate();
      minDistance = calibrate();
	  delay(100);
	}

  if(getDistance() < minDistance){
    digitalWrite(BUZZ, HIGH);
    delay(300);
    digitalWrite(BUZZ, LOW);
  }
  calculatePace();
}