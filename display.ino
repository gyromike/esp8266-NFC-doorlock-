/*
   setup the OLED
*/
void setupdisplay() {
  display.init();
  display.setI2cAutoInit(true);
}

// utility function for digital clock display: prints leading 0
String twoDigits(int digits) {
  if (digits < 10) {
    String i = '0' + String(digits);
    return i;
  }
  else {
    return String(digits);
  }
}

void displaycardholder() {
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.drawString(displayCenterX, displayCenterY - 24 , "Welcome");
  display.drawString(displayCenterX, displayCenterY + 4, cardHolder[cardIndex]); // display thr card holder on the OLED
//set a timer to blank the screen after 5 seconds
}



