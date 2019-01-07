uCommander - The Micro Commander
================================

This library simplifies interfacing with code written on
a microprocessor like an Arduino, Teensy, or similar devices
via serial communication.

It works best with the 
[corresponding Python library](https://github.com/aeetos/ucommander-py)
that lets you trigger functions in your microprocessor code from
the command line or your own Python code. 

uCommander handles all the serial parsing for you, including
packing larger variables like 16-bit integers, floats, and more.
It also offers a lightweight "discovery" protocol that lets you
start interfacing with your Arduino code without writing any
Python code. 

Quickstart
--------------
Here's how easy it is to control an LED with the uCommander. You'll 
find an annotated version of this example in the
`examples/ucommander_led` folder.

First we write a simple Arduino sketch:

```c++
#include "uCommander.h"
#define LEDPIN 13
uCommander uCmd = uCommander();

void setup() {
    Serial.begin(115200);
    pinMode(LEDPIN, OUTPUT);
    uADDCMD(uCmd, ledOn);     
    uADDCMD(uCmd, ledOff);
}
void loop() {
    uCmd.readSerial(Serial);
}
uCALLBACK(ledOn) {
    digitalWrite(LEDPIN, HIGH);
}
uCALLBACK(ledOff) {
    digitalWrite(LEDPIN, LOW);
}
```

After we upload this to our Arduino or similar device, and
connect it via serial to our host machine, we can control the
led from a command prompt using the ucommander Python library
interactively. 

First we discover what commands are available:
```
$ python3 -m ucommander -p /dev/cu.usbmodem discover
Available commands:
 ledOn
 ledOff
```
Now that we know the command names, we can start controlling our LED:
```
$ python3 -m ucommander -p /dev/cu.usbmodem ledOn

$ python3 -m ucommander -p /dev/cu.usbmodem ledOff
```
Notice that we did not have to write any Python code to make this
work. The uCommander library enabled our code to described its
capabilities to the Python script so no configuration or Python
code was required.

Of course, the Python library includes a complete uCommander class
so you can wire it into your own project.

Passing Arguments
-----------------
Triggering actions in your Arduino is a great start, but things
get more interesting when you can start passing values as
arguments to your callback functions. 

This is nontrivial for a whole heap of reasons, which is why the
uCommander aims to make it as simple as possible. 

Here's a short example that checks whether a value is equal to
a short approximation of pi:
```c
#include "uCommander.h"
#define LEDPIN 13
uCommander uCmd = uCommander();

void setup() {
    Serial.begin(115200);
    pinMode(LEDPIN, OUTPUT);
    uADDCMD(uCmd, ledIfPi, uFLOAT); 
}
void loop() {
    uCmd.readSerial(Serial);
}
uCALLBACK(ledIfPi) {
    if(packet->getArg<uFLOAT_T>(0) == 3.14)
        digitalWrite(LEDPIN, HIGH);
    else
        digitalWrite(LEDPIN, LOW);
}
```
We added a command just like before, except here, we
added the type of the parameter our callback function will accept
(the `uFLOAT` macro). Then in our callback function, we accessed
the argument in position `0`, being sure to tell the compiler
what type we'd expect to get back from the template function
(the `uFLOAT_T` macro).

> Check out the example in the `examples/ucommander_params` 
folder for a more complete explanation of what's happening here,
especially if you're wondering where `packet` came from.

After that code is running on our microprocessor, we can
interact with it like so:
```
$ python3 -m ucommander -p /dev/cu.usbmodem discover
Available commands:
 ledIfPi        FLOAT(4)

$ python3 -m ucommander -p /dev/cu.usbmodem ledIfPi 3.14

$ python3 -m ucommander -p /dev/cu.usbmodem ledIfPi 3.24
```

Multiple Named Arguments
------------------------
You can specify multiple arguments of various types by simply
appending them to the uADDCMD macro. Here's how you'd accept
three unsigned 32-bit integers:
```c
uADDCMD(uCmd, ledIfSmart, uUINT, uUINT, uUINT);
```
If you want to be especially helpful, you can give the arguments
useful names which the Python library will use if they are
available.
```c
uADDCMD_NAMEDARGS(uCmd, ledIfSmart, "a,b,sum", uUINT, uUINT, uUINT);
```
The Python library will show us these names when we discover
capabilities, and it will make them available as keyword 
arguments, at the command line and in code.
```
$ python3 -m ucommander -p /dev/cu.usbmodem discover
Available commands:
 ledIfSmart     a:UNSIGNED_INT(4), b:UNSIGNED_INT(4), sum:UNSIGNED_INT(4)

$ python3 -m ucommander /dev/cu.usbmodem ledIfSmart sum=2048 a=1024 b=1024
```

Caveats
-------
1. Docs are still a work in progress - see the `uCommander.h` source for the available argument types.
1. The library allocates enough space for 50 commands. If you
need more, modify the `UCOMMANDER_CMDCOUNT` constant in `uCommander.h`
1. The uCommander was built and tested on a Teensy which has a
pretty healthy amount of ram. If you're using something else,
or just have a lot of stuff going on, you might want to keep
a close eye on your available program space. 
1. This C library was written by a coder who wished he was
writing Python. Like the author himself, it undoubtedly has 
issues. Feedback welcome.

Planned Enhancements
--------------------
1. Support for byte arrays as argument types.
1. Returning data from callbacks, with proper type packing.
1. Macros to help with accessing arguments so you don't need
to learn how templates work.
1. Possibly add support for accessing arguments by name.

Why?
----
The author, Mark Kendrick, spent years learning about byte 
arrays, serial communications, [endianness](https://en.wikipedia.org/wiki/Endianness), 
the [Python `struct` library](https://docs.python.org/3/library/struct.html), and
many other esoteric concepts while trying to talk to various
Arduino and Teensy projects he was building for "fun". 

His various failures conspired to rob him of most of the joy of
these projects, and he began to dream of a generic library that
would let him easily call C functions from Python over serial.

Even as his skills improved, Mark still found himself shying
away from interesting projects because eventually, he knew he'd
have to create yet another bit-shifting, byte-parsing horror
that would crash for no reason and quite literally leave him 
in the dark (Mark is big fan of top quality, 
[high CRI LED](https://www.yujiintl.com) tape light).

Finally, at the end of 2018, Mark installed more Adafruit
[Dotstars](https://www.adafruit.com/category/885) 
on the front of his mother's house than would fit in
a single byte integer, and he decided right then that it was
time to stop everything and fix this madness once and for all.

Or at least consolidate that madness in one place. 

--------------------------------------------------------------
Copyright (C) 2019 Mark A Kendrick  
This content is licensed under a  
[Creative Commons Attribution-NoDerivs 3.0 United States License](http://creativecommons.org/licenses/by-nd/3.0/us/)


