#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t button_event_flag = 0;

void PIN_Init(void);

int main(void)
{
    uint8_t button_state = 0; 

    PIN_Init();
    sei();

    while (1)
    {
        if (button_event_flag)
        {
            _delay_ms(25); 

            if (button_state == 0 && !(PINB & (1 << PB0)))
            {
                PORTB ^= (1 << PB1); 
                button_state = 1;     
            }
            
            button_event_flag = 0;
        }

        if (button_state == 1 && (PINB & (1 << PB0)))
        {
            button_state = 0; 
        }
    }
}

void PIN_Init(void)
{
    DDRB &= ~(1 << PB0);
    PORTB |= (1 << PB0);
    PCICR |= (1 << PCIE0);
    PCMSK0 |= (1 << PCINT0);
    DDRB |= (1 << PB1);
}

ISR(PCINT0_vect)
{
    button_event_flag = 1;
}