#include <LiquidCrystal_I2C.h>

const float BPS = 50;
const float DELAY_COMM = 1000 / BPS;
const float DELAY_GEN = 10;

const int COMMU_PIN = 13;
const int SPEAKER_PIN = 12;
const int LED_PIN = 10;
const int RECIEVE_PIN = 9;

String binaryString = "";
String sentence = "";

bool start_message = false;
bool end_message = false;
bool saw_backslash_start = false;
bool saw_backslash_end = false;

char CHARACTER_MAP[32] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  '-', '.', '\\', '(', ')', ' '
};

LiquidCrystal_I2C lcd(0x26, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

char getChar(String binaryString){
  int value = 0;

  for (int i = 0; i < binaryString.length(); i++) {
    // Left-shift the current value by 1 (same as multiplying by 2).
    value <<= 1;

    // If the character is '1', add 1 to the result.
    if (binaryString.charAt(i) == '1') {
      value += 1;
    }
  }
  return CHARACTER_MAP[value];
}

bool check_seq(char ch, char marker, bool &flag, bool &saw_backslash) {
  if (!flag) {
    if (!saw_backslash) {
      if (ch == '\\') {
        saw_backslash = true;  // saw backslash
      }
    } else {
      if (ch == marker) {
        flag = true;           // found sequence
      }
      saw_backslash = false;   // reset state
    }
  }
  return flag;
}

void setup() {
  lcd.init(); // initialize the lcd
  lcd.backlight(); // Turn backlight On
  pinMode(COMMU_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(RECIEVE_PIN, INPUT);
}

void loop() {
  if (digitalRead(RECIEVE_PIN) == HIGH){
    digitalWrite(LED_PIN, HIGH);
  }

  while (digitalRead(COMMU_PIN) == LOW){
    if (digitalRead(RECIEVE_PIN) == LOW){
      digitalWrite(LED_PIN, LOW);
    }
  }

  char nextBin = ' ';
  delay(DELAY_COMM + 1);
  if (digitalRead(COMMU_PIN) == HIGH){
    nextBin = '1';
  }
  else {
    nextBin = '0';
  }

  binaryString += nextBin;
  lcd.setCursor(0, 1);
  lcd.print(binaryString);

  if (binaryString.length() == 5){
    char ch = getChar(binaryString);
    binaryString = "";
	
    if (!start_message && !end_message){
      check_seq(ch, 'B', start_message, saw_backslash_start);
      check_seq(ch, 'S', end_message, saw_backslash_end);
    }
    else if (start_message && !end_message){
      check_seq(ch, 'S', end_message, saw_backslash_end); // keep checking for end
      if (!end_message) {
        tone(SPEAKER_PIN, 440, 10);
        sentence += ch;
      }
      else{
      	sentence.remove(sentence.length() - 1);
        tone(SPEAKER_PIN, 2000, 100);
      }
    }
    //sentence += ch;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(sentence);

  }
  
  delay(DELAY_COMM);
}
