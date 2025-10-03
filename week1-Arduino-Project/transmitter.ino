  #include <LiquidCrystal_I2C.h>

  char CHARACTER_MAP[32] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '-', '.', '\\', '(', ')', ' '
  };

  const float BPS = 50;
  const float DELAY_COMM = 1000 / BPS;
  const float DELAY_GEN = 10;

  const int COMMU_PIN = 13;
  const int SPEAKER_PIN = 12;
  const int LED_PIN = 10;

  LiquidCrystal_I2C lcd(0x26, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

  String getBinaryString(int number) {
    int value = number % 32;

    String binaryString = "";
    // Create a 5-digit binary string
    for (int i = 4; i >= 0; i--) {
      binaryString += bitRead(value, i) ? '1' : '0';
    }
    
    return binaryString;
  }

  void showSentence(String sentence){
    lcd.setCursor(0, 0);
    lcd.print(sentence);
  }

  int getCharIndex(char ch){
    char upperCh = toupper(ch);
    for (int i = 0; i <= 31; i++){
      if (CHARACTER_MAP[i] == upperCh){
        return i;
      }
    }
    return -1;
  }

  void sentSignal(char bin){
    int b = 0;
    if(bin == '1'){
      b = 1;
    }
    else{
      b = 0;
    }
    digitalWrite(COMMU_PIN, HIGH);
    delay(DELAY_COMM);
    digitalWrite(COMMU_PIN, b);
    delay(DELAY_COMM);
    digitalWrite(COMMU_PIN, LOW);
    delay(DELAY_COMM);
  }

  void sentBinary(String sentence){
    int i = 0;
    digitalWrite(LED_PIN, HIGH);
    for (char ch: sentence){
      lcd.noCursor();
      // get character index
      int char_idx = getCharIndex(ch);
      
      // get binary string
      String binaryString = getBinaryString(char_idx);
      lcd.setCursor(0, 1);
      lcd.print(binaryString);
      lcd.cursor();
      lcd.setCursor(i, 0);

      // sent signal
      tone(SPEAKER_PIN, 440, 10);

      for (char bin: binaryString){
        sentSignal(bin);
        delay(DELAY_COMM);
      }
      // delay between each character
      delay(DELAY_GEN);
      i++;
    }
    digitalWrite(LED_PIN, LOW);

  }

  void setup() {
    lcd.init(); // initialize the lcd
    lcd.backlight(); // Turn backlight On
    pinMode(COMMU_PIN, OUTPUT);
    pinMode(SPEAKER_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
  }

  void loop() {
    lcd.clear();

    String sentence = "Test \\bHello\\s more";

    // for (char ch: sentence){ 
    //   lcd.print("current: " + ch);
    // }

    showSentence(sentence);

    delay(DELAY_GEN);

    sentBinary(sentence);

    delay(50000);
  }
