/***************************************************
This is an example of how to use the uCommander library
to control an LED. 

It works best with the corresponding Python library
but it also works by simply sending single-byte serial 
commands.

Visit https://github.com/aeetos/ucommander for source code
and documentation.

Copyright (C) 2019 Mark A Kendrick
code at ikend dot com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
***************************************************/

#include "uCommander.h"

#define LEDPIN 13

uCommander uCmd = uCommander();

void setup() {
    // Initialize the serial port at 115200 baud rate
    Serial.begin(115200);

    /* 
    Register two commands with the uCommander object
        Each command is a function you write here in your
        main program - scroll down to see their implementations.
      
    You may add up to 50 commands by default; see the docs for how
        to customize this to your requirements.
    */
    uADDCMD(uCmd, ledOn);     
    uADDCMD(uCmd, ledOff);

    // Set the LED pin to output mode
    pinMode(LEDPIN, OUTPUT);

    // Blink the LED at startup
    digitalWrite(LEDPIN, HIGH);
    delay(1000);
    digitalWrite(LEDPIN, LOW);
}

void loop() {
    // Tell the uCommander which serial port to read from
    uCmd.readSerial(Serial);
}


void ledOn(uPacket * packet) {
    /* 
    Turns the LED on.
      
    The function signature here is important. You must
    define your callback function to return void and accept
    a single parameter of type pointer to uPacket. See the next
    function for a simpler way to handle this.
    
    This example doesn't use any data sent into the command, but 
    if it did, you'd access it by calling methods on the packet
    object.

    */
    digitalWrite(LEDPIN, HIGH);
}

uCALLBACK(ledOff) {
    /* 
    Turns the LED off.

    This is the simpler and recommended way to define your callback
    methods. It uses a macro defined in the uCommander library to 
    generate a function with the correct signature.

    */
    digitalWrite(LEDPIN, LOW);
}