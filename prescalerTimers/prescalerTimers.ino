#include <avr/io.h>
#include <avr/interrupt.h>
#define LEDPIN 13

void setup()
{
    pinMode(LEDPIN, OUTPUT);

    // инициализация Timer1
    cli(); // отключить глобальные прерывания
    TCCR1A = 0; // установить TCCR1A регистр в 0
    TCCR1B = 0;

    // timer_counts = 0.3s / 6.4e-5s - 1 = 4868
    OCR1A = 4686;
    TCCR1B |= (1 << WGM12); // включение в CTC режим
    // Установить CS10 бит так, чтобы таймер работал при тактовой частоте:
    TCCR1B |= (1 << CS10);
    TCCR1B |= (1 << CS12);

    TIMSK1 = TIMSK1 | (1 << OCIE1A); 

    sei();  // включить глобальные прерывания
}

ISR(TIMER1_COMPA_vect)
{
     digitalWrite(LEDPIN, !digitalRead(LEDPIN));
}

void loop() {
  // put your main code here, to run repeatedly:
}
