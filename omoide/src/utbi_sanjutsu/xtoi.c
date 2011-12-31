/* xtoi.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

unt xtoi(unt atai, char x)
{
	atai = (atai << 4);

	switch(x){
		case '0':
			atai = atai | 0x0;
			break;
		case '1':
			atai = atai | 0x1;
			break;
		case '2':
			atai = atai | 0x2;
			break;
		case '3':
			atai = atai | 0x3;
			break;
		case '4':
			atai = atai | 0x4;
			break;
		case '5':
			atai = atai | 0x5;
			break;
		case '6':
			atai = atai | 0x6;
			break;
		case '7':
			atai = atai | 0x7;
			break;
		case '8':
			atai = atai | 0x8;
			break;
		case '9':
			atai = atai | 0x9;
			break;
		case 'A':
		case 'a':
			atai = atai | 0xA;
			break;
		case 'B':
		case 'b':
			atai = atai | 0xB;
			break;
		case 'C':
		case 'c':
			atai = atai | 0xC;
			break;
		case 'D':
		case 'd':
			atai = atai | 0xD;
			break;
		case 'E':
		case 'e':
			atai = atai | 0xE;
			break;
		case 'F':
		case 'f':
			atai = atai | 0xF;
			break;
	}
	return atai;
}
