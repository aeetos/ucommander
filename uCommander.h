/***************************************************
uCommander - The Micro Commander

This library simplifies interfacing with code written on
a microprocessor like an Arduino, Teensy, or similar devices
via serial communication.

It works best with the corresponding Python library that
lets you trigger functions in your microprocessor code from
the command line or your own Python code. 

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



#ifndef _UCOMMANDER_H
#define _UCOMMANDER_H

#ifndef UCOMMANDER_CMDCOUNT
#define UCOMMANDER_CMDCOUNT 50
#endif
#ifndef UCOMMANDER_BUFSIZE
#define UCOMMANDER_BUFSIZE 50
#endif

#define uCHAR       'c'
#define uCHAR_T     char
#define uSCHAR      'b'
#define uSCHAR_T    int8_t
#define uUCHAR      'B'
#define uUCHAR_T    uint8_t
#define uSHORT      'h'
#define uSHORT_T    int16_t
#define uUSHORT     'H'
#define uUSHORT_T   uint16_t
#define uINT        'i'
#define uINT_T      int32_t
#define uUINT       'I'
#define uUINT_T     uint32_t
#define uLONG       'l'
#define uLONG_T     int32_t
#define uULONG      'L'
#define uULONG_T    uint32_t
#define uFLOAT      'f'
#define uFLOAT_T    float

#define uADDCMD(COMMANDER, FN, ...) COMMANDER.addCmd(FN, #FN, "\0", {__VA_ARGS__})
#define uADDCMD_NAMEDARGS(COMMANDER, FN, NAMES, ...) COMMANDER.addCmd(FN, #FN, NAMES, {__VA_ARGS__})
#define uCALLBACK(FN) void FN(uPacket * packet)
#define uGETARG(POS, TYPE) packet->getArg<TYPE>(POS)

#include <Arduino.h>



class uPacket {

 public:
    uPacket(char* buf, const char* argtypes);
    uPacket(char* buf, const char* argtypes, const char* argnames);
    uint8_t fieldSize(uint8_t pos);
    uint8_t calcOffset(uint8_t pos);
    uint8_t size();
    
    template <class T>
    T getArg (uint8_t pos) {
        T value = 0;
        memcpy(&value, &_buf[calcOffset(pos) + 1], sizeof(value));
        return value;
    }

    /*
        WARNING: This doesn't work yet.
        Getting args by name is still a work in progress.
    */
    /*
    template <class T>
    T getNamedArg(const char * name) {
        char * argname;
        char * argnames = strdup(_argnames);
        uint8_t pos = 0;
        argname = strtok(argnames,",");
        while(argname != NULL) {
            if(name==argname)
                return getArg<T>(pos);
            argname = strtok(NULL,",");
            pos++;
        }
        return 0;
    }
    */
    
    
 private:
    const char * _argtypes;
    const char * _argnames;
    char * _buf;
};

typedef void(*uCmdCallback)(uPacket*);


class uCommander {

 public:
    uCommander();

    boolean begin(void);
    boolean addCmd( uCmdCallback, const char *, const char *, std::initializer_list<char>);

    void readSerial(Stream&);
 
 protected:
    void init();
    void resetBuffer();
    void discover(Stream&);

 private:
    uint8_t _next_cmd_index, _buf_index;
    char _buf[UCOMMANDER_BUFSIZE];
    uPacket * _packet;
    char * _argtypes[UCOMMANDER_CMDCOUNT];
    uCmdCallback _callbacks[UCOMMANDER_CMDCOUNT];
    #ifndef UCOMMANDER_NODISCOVERY
    const char * _argnames[UCOMMANDER_CMDCOUNT];
    const char * _cmdnames[UCOMMANDER_CMDCOUNT];
    #endif
};
















#endif