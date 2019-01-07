/***************************************************
This is an example of how to use the uCommander library
to accept parameters of various types and respond accordingly. 

The example builds on the concepts introduced in the simpler
LED example, which may be a good starting point if you've not
seen it yet.

This example works best with the corresponding Python library.

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

    // Register a command with the uCommander object that
    //   accepts a single positional argument of type
    //   signed char (i.e. a signed one byte integer).
    //
    // The uCommander library provides macros that correspond
    //   to most types supported by the Arduino library. See
    //   the docs for a complete list.
    uADDCMD(uCmd, ledIfHappy, uSCHAR);

    // As above, but this time with a float.
    uADDCMD(uCmd, ledIfPi, uFLOAT);

    // To ease discovery, you can supply a comma separated list of
    //    parameter names. These will appear in a discovery request and
    //    the Python code knows how to map these to positional arguments.
    //
    // You will still need to get the argument values by position. 
    //
    // This also demonstrates how to support multiple arguments - just keep
    //    adding types to the uADDCMD or uADDCMD_NAMEDARGS macros.
    uADDCMD_NAMEDARGS(uCmd, ledIfSmart, "a,b,sum", uUINT, uUINT, uUINT);

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

uCALLBACK(ledIfHappy) {
    /* 
    Turns the LED on when it gets a positive number and off when it's negative.

    The uCALLBACK macro generates a function signature like this:
        void ledIfHappy(uPacket * packet)
    
    You can access the parameters by calling the getArg method on the packet object.

    The first line looks odd if you're not used to templates and macros. If you
    expand the macros, you get this:
        int8_t mood = packet->getArg<int8_t>(0);
    
    When the compiler sees that, it generates a function with the proper return type
    to match the type specified in the <angle> brackets.

    The _T macros are provided for convenience and to ensure you get the right type
    for the argument you specified in uADDCMD above. 
    */
    uSCHAR_T mood = packet->getArg<uSCHAR_T>(0);
    if (mood > 0)
        digitalWrite(LEDPIN, HIGH);
    else
        digitalWrite(LEDPIN, LOW);
}

uCALLBACK(ledIfPi) {
    /* 
    Turns the LED on when it gets a float value of 3.14

    See previous example for an explanation of where "packet" came from and how
    templates work if the syntax here seems unfamiliar to you.
    */
    uFLOAT_T slice = packet->getArg<uFLOAT_T>(0);
    if(slice == 3.14)
        digitalWrite(LEDPIN, HIGH);
    else
        digitalWrite(LEDPIN, LOW);
}

uCALLBACK(ledIfSmart) {
    /* 
    Turns the LED on when you can demonstrate your ability to successfully
    add two numbers together. 

    See earlier example for an explanation of where "packet" came from and how
    templates work if the syntax here seems unfamiliar to you.
    */
    uUINT_T a = packet->getArg<uUINT_T>(0);
    uUINT_T b = packet->getArg<uUINT_T>(1);
    uUINT_T sum = packet->getArg<uUINT_T>(2);

    if(a + b == sum)
        digitalWrite(LEDPIN, HIGH);
    else
        digitalWrite(LEDPIN, LOW);
}

