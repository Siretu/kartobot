/*
 * myclasses.cpp
 *
 *  Created on: 07.12.2009
 *      Author: mthomas
 */

#include "term_io.h"
#include "myclasses.h"

void Root::say_hello()
{
	inc_cnt();
	xprintf("Hello from Root(%d)\n", cnt);
}

void Child1::say_hello()
{
	inc_cnt();
	xprintf("Hello from Child1 (%d)\n", cnt);
}


void Child2::say_hello()
{
	inc_cnt();
	xprintf("Hello from Child2 (%d)\n", cnt);
}
