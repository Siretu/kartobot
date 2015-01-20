/*
  Print.h - Base class that provides print() and println()
  Copyright (c) 2008 David A. Mellis.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef Print_h
#define Print_h

#include <inttypes.h>
#include <stdio.h> // for size_t

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2
#define BYTE 0

class Print
{
  private:
    void printNumber(unsigned long, uint8_t);
    void printFloat(double, uint8_t);
  public:
    virtual void write(uint8_t) = 0;
    virtual void write(const char *str);
    virtual void write(const uint8_t *buffer, size_t size);

    void print(const char[]);
    void print(char, uint8_t = BYTE);
    void print(unsigned char, uint8_t = BYTE);
    void print(int, uint8_t = DEC);
    void print(unsigned int, uint8_t = DEC);
    void print(long, uint8_t = DEC);
    void print(unsigned long, uint8_t = DEC);
    void print(double, uint8_t = 2);

    void println(const char[]);
    void println(char, uint8_t = BYTE);
    void println(unsigned char, uint8_t = BYTE);
    void println(int, uint8_t = DEC);
    void println(unsigned int, uint8_t = DEC);
    void println(long, uint8_t = DEC);
    void println(unsigned long, uint8_t = DEC);
    void println(double, uint8_t = 2);
    void println(void);
};

#endif
