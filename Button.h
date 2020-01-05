class Button {
  public: 
    int pin = 0;          // pin number
    int state = 0;        // current button state counter
    int btn = 0;          // button state
    int btn_last = 0;     // last button state
    unsigned long btn_last_time = 0;   // last press of button
    bool isPressed;       // when button is pressed
    bool isReleased;      // when button is released (!isPressed)
    bool isHeld;          // when button is held (key repeat)
    bool lastPressed;     // last state of isPressed
    bool isJustPressed;   // when button is just pressed
    bool isJustReleased;  // when button is just released

    Button(int _pin);

    bool Update();
};

Button::Button(int _pin) {
  pin = _pin;
}


bool Button::Update() {
  // Read and set button states
  btn = digitalRead(pin);
  isPressed  = (btn == LOW);
  isReleased = !isPressed;
  
  // Set button event states
  isJustPressed  = ( isPressed && !lastPressed);
  isJustReleased = (!isPressed &&  lastPressed);
  
  // Set repeat if pressed, last cycle is not pressed, and time is up
  isHeld = (isPressed && !isHeld && (millis() > btn_last_time + 500));
  // Set last triggered time if rising trigger or held detected
  if (isJustPressed) btn_last_time = millis();
  if (isHeld) btn_last_time = millis();
  
  // Increment internal button state 
  if (isJustPressed || isHeld) state++;
  
  // Save last button state
  lastPressed = isPressed;
  
  return isPressed;
}
