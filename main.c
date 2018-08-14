#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
//32 bit processor, therefore we have 32 bits to work with
#define LEFT_BUTTON    BIT1 //bit 1 mask used for left button

// THIS MACRO HAS TO BE DEFINED BY YOU
#define RIGHT_BUTTON   BIT4 //input is pin 4 of port 1

#define LED1           BIT0 //least significant bit

// THIS MACRO HAS TO BE DEFINED BY YOU
#define LED2_RED      BIT0 //red pin is pin 0 of port 2

// THIS MACRO HAS TO BE DEFINED BY YOU
#define LED2_GREEN    BIT1 //Green is pin 1 of port 2

// THIS MACRO HAS TO BE DEFINED BY YOU
#define LED2_BLUE     BIT2 //blue is pin 2 of port 2

#define PRESSED 0 //pressed means that bit resets to 0, active high

enum {DEFAULT, RED, BLUE, GREEN, PURPLE} color = DEFAULT;//added a default color to make the starting state LED2 off
//when the program starts, it will go straight to the DEFAULT case. where it will reset all of the colors and then go to PURPLE

void main(void) {

  WDT_A_hold(WDT_A_BASE);

  // Configure the pin connected to LED1 as output
  GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

  //added the following for GPIO
  GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);//red pin for output
  GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);//green pin for output
  GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);//blue pin for output

  GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4); //pushbutton for LED 2 setting as input
  //finished adding for GPIO

  // Configure the pin connected to left button an input with pull-up resistor
  GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN1);

  unsigned char left_button_prev, left_button_cur; //need the same thing for right button
  unsigned char rightButtonPrev, rightButtonCur; //added two variables to keep track of when button is pressed and unpressed
  //just like left button

  P1OUT &= ~LED1;//resetting the LED1 to show off as initialization, then it will toggle
  //P2OUT &= ~LED2_RED & ~(LED2_RED | LED2_BLUE) & ~LED2_GREEN & ~LED2_BLUE;
  //reset all bits corresponding to all colors

  while(1) {
      // P1IN contains all the pins on port1
      // When we mask it with LEFT_BUTTON, all the port bits become 0 except for the one representing left button
      // When this masked value becomes 0, it means this button is pressed (grouneded).
      left_button_cur = (P1IN & LEFT_BUTTON);
      rightButtonCur = (P1IN & RIGHT_BUTTON);//right button will be similar input to left button, ANDed together to give current state
      // We recall that "pushing a button" consists of pressing and then releasing it.
      // If the button "was" pressed, but now it "is not" pressed, it means the user has finished "pushing" the button

      if ((left_button_prev == PRESSED) &&
          (left_button_cur  != PRESSED)) {
          P1OUT ^= LED1; //toggle the LED
      }

      switch(color)
      {
          case RED :
              if (rightButtonPrev == PRESSED && rightButtonCur != PRESSED)//if pressed, change color
              {
                  P2OUT &= ~LED2_RED;//turn red led bit off, (reset)
                  color = BLUE; //change color to next color to be
                  P2OUT ^= LED2_BLUE; //turn on proper LED
                  break;//need this in a switch case
              }

          case BLUE :
              if (rightButtonPrev == PRESSED && rightButtonCur != PRESSED)//if pressed, change color
              {
                  P2OUT &= ~LED2_BLUE;//turn blue led bit off, (reset)
                  color = GREEN;//change color to next color to be
                  P2OUT ^= LED2_GREEN; //turn on green LED by XOR like LED1
                  break;//necessary
              }

          case GREEN :
              if (rightButtonPrev == PRESSED && rightButtonCur != PRESSED)//if gone through press motion
              {
                  P2OUT &= ~LED2_GREEN;//reset green bit
                  color = PURPLE;//color equals next color
                  P2OUT ^= (LED2_RED | LED2_BLUE);//purple is a mix of red and blue in RGB terms
                  break;
              }

          case PURPLE :
              if (rightButtonPrev == PRESSED && rightButtonCur != PRESSED)
              {
                  P2OUT &= ~(LED2_RED | LED2_BLUE);//reset purple
                  color = RED;//revert back to the sequence
                  P2OUT ^= LED2_RED;//red color
                  break;
              }
          case DEFAULT :
              if (rightButtonPrev == PRESSED && rightButtonCur != PRESSED)
              {
                  P2OUT &= ~LED2_RED & ~(LED2_RED | LED2_BLUE) & ~LED2_GREEN & ~LED2_BLUE;//reset all bits corresponding to all colors
                  color = PURPLE;//change color
              }
      }

      // Keep the history of this button
      left_button_prev = left_button_cur;
      rightButtonPrev = rightButtonCur; //keep history of right Button whether it was pressed or not

  }
}
