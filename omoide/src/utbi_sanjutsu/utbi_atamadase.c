/* utbi_atamadase.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

int utbi_atamadase(unt x)
{
#ifdef ___ASSEMBLE_WIN32___
	_asm{
		mov ebx, [x]
		bsr eax, ebx
	}
#else
	if(x & 0xffff0000){
		if(x & 0xff000000){
			if(x & 0xf0000000){
				if(x & 0xc0000000){
					if(x & 0x80000000){
						return 31;
					}else{
						return 30;
					}
				}else{
					if(x & 0x20000000){
						return 29;
					}else{
						return 28;
					}
				}
			}else{
				if(x & 0x0c000000){
					if(x & 0x08000000){
						return 27;
					}else{
						return 26;
					}
				}else{
					if(x & 0x02000000){
						return 25;
					}else{
						return 24;
					}
				}
			}
		}else{
			if(x & 0x00f00000){
				if(x & 0x00c00000){
					if(x & 0x00800000){
						return 23;
					}else{
						return 22;
					}
				}else{
					if(x & 0x00200000){
						return 21;
					}else{
						return 20;
					}
				}
			}else{
				if(x & 0x000c0000){
					if(x & 0x00080000){
						return 19;
					}else{
						return 18;
					}
				}else{
					if(x & 0x00020000){
						return 17;
					}else{
						return 16;
					}
				}
			}
		}
	}else{
		if(x & 0x0000ff00){
			if(x & 0x0000f000){
				if(x & 0x0000c000){
					if(x & 0x00008000){
						return 15;
					}else{
						return 14;
					}
				}else{
					if(x & 0x00002000){
						return 13;
					}else{
						return 12;
					}
				}
			}else{
				if(x & 0x00000c00){
					if(x & 0x00000800){
						return 11;
					}else{
						return 10;
					}
				}else{
					if(x & 0x00000200){
						return 9;
					}else{
						return 8;
					}
				}
			}
		}else{
			if(x & 0x000000f0){
				if(x & 0x000000c0){
					if(x & 0x00000080){
						return 7;
					}else{
						return 6;
					}
				}else{
					if(x & 0x00000020){
						return 5;
					}else{
						return 4;
					}
				}
			}else{
				if(x & 0x0000000c){
					if(x & 0x00000008){
						return 3;
					}else{
						return 2;
					}
				}else{
					if(x & 0x00000002){
						return 1;
					}else if(x & 0x00000001){
						return 0;
					}else{
						return -1;
					}
				}
			}
		}
	}
#endif
}
