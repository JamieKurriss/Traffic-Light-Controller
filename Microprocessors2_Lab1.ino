// Lab 1 Traffic Light Controller
// Micromaniacs
// Jeremy Flotildes
// James Kurriss
// Shawn Bucklew

// Toggle variable for interrupts
boolean toggle1 = 0;

// Stores the state of the traffic light -- in wait state or operation
// Initial waiting state = 1; operation state = 0
boolean initState = 1;

boolean stateToggle = 0; // Toggle to keep traffic light in operation;
boolean initBlink; // Variable to turn red LED on and off during wait state

// Global button press variable
boolean buttonPress = 0;

int timer = 0; // Counts seconds using the interrupt stimulus

#define button 2
#define green 3
#define yellow 4
#define red 5
#define buzzer 6

void setup() {
  Serial.begin(9600); // For printing to terminal

  // Sets pins as outputs
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(button, INPUT);

  cli(); // Stop interrupts

  // Set timer1 interrupt at 1Hz
  TCCR1A = 0; // Set entire TCCR1A register to 0
  TCCR1B = 0; // Same for TCCR1B
  TCNT1 = 0; // Initialize counter value to 0
  // Set compare match register for 1hz increments
  OCR1A = 15624; // = (16*10^6) / (1*1024) - 1 (must be <65536)
  // Turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // Enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei(); // Allow interrupts

} // End setup

// Timer1's interrupt service routing (ISR)
// The code in ISR will be executed every time timer1 interrupt occurs
// That is, the code will be called once every second
ISR(TIMER1_COMPA_vect) { // Generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)              
  if (toggle1) {
    initBlink = true;
    timer += 1;
    toggle1 = 0;
    Serial.print(timer, DEC);
  } else {
    initBlink = false;
    timer += 1;
    toggle1 = 1;
    Serial.print(timer, DEC);
  }
}

void loop() {
  buttonPress = digitalRead(button);
  if (buttonPress) { // If a button is pressed, change state
    initState = 0;
    timer = 0;
  } else { // While the button hasn't been pressed, just blink every second
    if (initBlink) {
      digitalWrite(red, HIGH);
    } else {
      digitalWrite(red, LOW);
    }
  }

  if (initState == 0) { // In the operation state
    do {
      while (timer < 20) { // While the timer is under 20 seconds, keep the red light on
        digitalWrite(red, HIGH);
        if (timer > 16) { // With 3 seconds left, turn on buzzer
          digitalWrite(buzzer, HIGH);
        }
      }

      digitalWrite(red, LOW);
      digitalWrite(buzzer, LOW);

      timer = 0; // Reset timer

      while (timer < 20) { // Turn on green light for 20 seconds
        Serial.print(timer, DEC);
        digitalWrite(green, HIGH);
        if (timer > 16) {
          digitalWrite(buzzer, HIGH);
        }
      }

      digitalWrite(green, LOW);
      digitalWrite(buzzer, LOW);

      timer = 0;

      while (timer < 3) { // Turn on green light
        Serial.print(timer, DEC);
        digitalWrite(yellow, HIGH);
        digitalWrite(buzzer, HIGH);
      }

      digitalWrite(yellow, LOW);
      digitalWrite(buzzer, LOW);
      
      timer = 0;

      stateToggle = 1; // Set toggle to 1 to allow for operation indefinetly
    } while (stateToggle == 1);
  }
}

// References

// Reference 1: Used to get an idea of how to get LEDs working
// https://www.circuitbasics.com/arduino-basics-controlling-led/#:~:text=To%20turn%20on%20an%20LED,the%20HIGH%20and%20LOW%20states.

// Reference 2: Used to get an idea of how to control LEDs with a push button
// https://www.programmingelectronics.com/tutorial-17-using-a-button-old-version/

// Reference 3: Used the code provided by Professor Yan Lou to assist with the timer interrupt
// https://github.com/ACANETS/eece-4520-5520-labs/blob/main/lab1/TimerSample/TimerSample.ino