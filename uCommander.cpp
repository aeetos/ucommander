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

#include "uCommander.h"



/*
 uCommander
*/


uCommander::uCommander() {
    init();
}

void uCommander::init() {
    _next_cmd_index = 0;
    resetBuffer();
}

boolean uCommander::begin(void) {
    return true;
}

boolean uCommander::addCmd( uCmdCallback fn, const char * cmdname, const char * argnames, std::initializer_list<char> types) {
    if(_next_cmd_index > UCOMMANDER_CMDCOUNT)
        return false;
    _callbacks[_next_cmd_index] = fn;
    std::initializer_list<char>::iterator it;
    _argtypes[_next_cmd_index] = new char[types.size()+1];
    for(it = types.begin(); it != types.end(); ++it )
        _argtypes[_next_cmd_index][it-types.begin()] = *it;
    _argtypes[_next_cmd_index][types.size()] = '\0';
    #ifndef UCOMMANDER_NODISCOVERY
    _argnames[_next_cmd_index] = argnames;
    _cmdnames[_next_cmd_index] = cmdname;
    #endif
    _next_cmd_index++;
    return true;
}


void uCommander::readSerial(Stream &serial) {
    while(serial.available()) {
        _buf[_buf_index++] = serial.read();
        if(_buf_index==1) {
            #ifndef UCOMMANDER_NODISCOVERY
            if(_buf[0]==0xFF)
                discover(serial);
            #endif
            if(_buf[0]==0xFF or (uint8_t)_buf[0] > _next_cmd_index) {
                resetBuffer();
                continue;
            }
            _packet = new uPacket(_buf, _argtypes[(uint8_t)_buf[0]]);
        }
        if(_packet->size()==_buf_index) {
            _callbacks[(uint8_t)_buf[0]](_packet);
            delete _packet;
            resetBuffer();
        }
    }
   

    // @todo: need to clear the buffer after some reasonable timeout

}

void uCommander::resetBuffer() {
    for(uint8_t i=0; i < UCOMMANDER_BUFSIZE; i++) {
        _buf[i] = 0;
        _buf_index = 0;
    }
}

#ifndef UCOMMANDER_NODISCOVERY
void uCommander::discover(Stream &serial) {
    for(uint8_t c=0; c < _next_cmd_index; c++) {
        serial.print(_cmdnames[c]);
        serial.print("(");
        for(uint8_t t=0; t < strlen(_argtypes[c]); t++) {
            serial.print(_argtypes[c][t]);
        }
        if(strlen(_argnames[c])) {
            serial.print('|');
            serial.print(_argnames[c]);
        }
        serial.print(")\n");
    }
}
#endif



/*
 uPacket
*/

uPacket::uPacket(char* buf, const char* argtypes) {
    _buf = buf;
    _argtypes = argtypes;
}

uPacket::uPacket(char* buf, const char* argtypes, const char* argnames) {
    _buf = buf;
    _argtypes = argtypes;
    _argnames = argnames;
}

uint8_t uPacket::calcOffset(uint8_t pos) {
    uint8_t offset = 0;
    for(uint8_t i=0; i < pos; i++) {
        offset += fieldSize(i);
    }
    return offset;
}

uint8_t uPacket::fieldSize(uint8_t pos) {
    switch(_argtypes[pos]) {
        case uCHAR:
        case uSCHAR:
        case uUCHAR:
            return 1;
        case uSHORT:
        case uUSHORT:
            return 2;
        case uINT:
        case uUINT:
        case uLONG:
        case uULONG:
        case uFLOAT:
            return 4;
        default: // this shouldn't happen
            return 1;
    }
}

uint8_t uPacket::size() {
    uint8_t s = 1;
    for(uint8_t pos=0; pos < strlen(_argtypes); pos++)
        s += fieldSize(pos);
    return s;
}
