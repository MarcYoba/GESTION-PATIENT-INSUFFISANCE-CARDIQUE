#define samp_siz 4
#define rise_threshold 5
#include <SoftwareSerial.h>
SoftwareSerial SIM900(2, 3); 

void sendSMS(char * message) {
  // AT command to set SIM900 to SMS mode
  SIM900.print("AT+CMGF=1\r"); 
  delay(100);

  
  SIM900.println("AT+CMGS=\"+237672558082\""); 
  delay(100);
  
  
  SIM900.println(message); 
  delay(100);

  
  SIM900.println((char)26); 
  delay(100);
  SIM900.println();
  // Give module time to send SMS
  delay(5000); 
}

void callSomeone() {
  SIM900.println("ATD + +237672558082;");
  delay(100);
  SIM900.println();
  
 
  delay(30000);
  
  SIM900.println("ATH"); // hang up
}


void setup() {
    Serial.begin(9600);
    pinMode(A1, INPUT);
    pinMode(10, OUTPUT);
    SIM900.begin(19200);
    delay(20000);   
  
  // Send the SMS
    sendSMS("initialisation ......");
    delay(5000);
    callSomeone();
}

void loop() {

  float reads[samp_siz], sum;
  long int now, ptr;
  float last, reader, start;
  float first, second, third, before, print_value;
  bool rising;
  int rise_count;
  int n;
  long int last_beat;

  for (int i=0; i<samp_siz; i++)
    reads[i] = 0;
  sum = 0;
  ptr = 0;

  while (1) {
    n = 0;
    start = millis();
    reader = 0.;
    do {
      reader += analogRead(A1);
      n++;
      now = millis();
    }

    while (now < start + 20);
    reader /= n;  // we got an average
    sum -= reads[ptr];
    sum += reader;
    reads[ptr] = reader;
    last = sum / samp_siz;

    if (last > before) {
      rise_count++;

      if (!rising && rise_count > rise_threshold) {
        digitalWrite(10, HIGH);
        rising = true;
        first = millis() - last_beat;
        last_beat = millis();

        print_value = 60000. / (0.4 * first + 0.3 * second + 0.3 * third);
        Serial.println(print_value);
        char  BMP = (print_value);
        sendSMS(BMP);
        third = second;
        second = first;
      }
    }
    else {
      rising = false;
      rise_count = 0;
      digitalWrite(10, LOW);
    }
    before = last;
    ptr++;
    ptr %= samp_siz;
  }
}
