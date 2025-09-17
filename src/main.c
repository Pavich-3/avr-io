// Include necessary libraries for microcontroller operation
#include <avr/io.h>         // Main library for working with input/output ports (DDR, PORT, PIN)
#include <avr/interrupt.h>  // Library for working with interrupts (ISR, sei)
#include <util/delay.h>     // Library for creating delays (_delay_ms)

// --- Global Variables ---

// A global flag for communication between the interrupt (ISR) and the main loop.
// 'volatile' is a keyword that tells the compiler that this variable can be
// changed by an external source (in our case, the interrupt), so it shouldn't be optimized away.
volatile uint8_t button_event_flag = 0;

// --- Function Prototypes ---
void PIN_Init(void); // Declaration of the pin initialization function

// --- Main Program Function ---
int main(void)
{
    // A variable to store the current state of the button. This creates a simple "state machine".
    // 0 - The button is released and ready for a press.
    // 1 - The button has been pressed, and we are waiting for it to be released.
    uint8_t button_state = 0; 

    // Call the function to configure ports and interrupts
    PIN_Init();
    
    // sei() - globally enables all configured interrupts
    sei();

    // The infinite loop where the microcontroller runs continuously
    while (1)
    {
        // Check if the interrupt has set the flag (i.e., if the pin state has changed)
        if (button_event_flag)
        {
            // A short delay for switch debouncing.
            // This helps to ignore brief, false triggers from the button's mechanical contacts.
            _delay_ms(25); 

            // Check two conditions:
            // 1. `button_state == 0`: Was the button previously released? (to prevent repeated actions)
            // 2. `!(PINB & (1 << PB0))`: Is the PB0 pin currently LOW? (i.e., is the button pressed)
            if (button_state == 0 && !(PINB & (1 << PB0)))
            {
                // If both conditions are true, perform the action:
                // `PORTB ^= (1 << PB1)` - inverts the state of pin PB1 (toggles the LED on/off)
                PORTB ^= (1 << PB1); 
                
                // Update the state: the button is now considered "pressed".
                button_state = 1;     
            }
            
            // Reset the flag. We have handled this event and are ready for the next one.
            button_event_flag = 0;
        }

        // This check runs continuously, regardless of the flag.
        // It's needed to reset the button's state after it has been released.
        // 1. `button_state == 1`: Was the button previously pressed?
        // 2. `(PINB & (1 << PB0))`: Is the PB0 pin currently HIGH? (i.e., the button has been released)
        if (button_state == 1 && (PINB & (1 << PB0)))
        {
            // If so, update the state. The system is now ready for a new press.
            button_state = 0; 
        }
    }
}

// --- Pin Initialization Function ---
void PIN_Init(void)
{
    // Configure pin PB0 (button)
    DDRB &= ~(1 << PB0);    // Set pin PB0 as an INPUT
    PORTB |= (1 << PB0);    // Enable the internal pull-up resistor for PB0

    // Configure the interrupt for pin PB0
    PCICR |= (1 << PCIE0);  // Enable Pin Change Interrupt group 0 (PCINT0..7)
    PCMSK0 |= (1 << PCINT0);// Enable the interrupt specifically for pin PCINT0 (which is PB0)

    // Configure pin PB1 (LED)
    DDRB |= (1 << PB1);     // Set pin PB1 as an OUTPUT
}

// --- Interrupt Service Routine (ISR) ---
// This code automatically executes whenever any change occurs on pin PB0 (from HIGH to LOW or vice versa)
ISR(PCINT0_vect)
{
    // The ISR's only job is to quickly set the flag
    // to notify the main loop that an event has occurred.
    button_event_flag = 1;
}