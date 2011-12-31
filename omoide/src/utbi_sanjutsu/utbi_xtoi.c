/* utbi_xtoi.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <ctype.h>
#include <string.h>
#include "utbi_sanjutsu.h"

/*2005/02/23*/
void utbi_xtoi(unt *x, char *ss)
{

	char *p, *pp;
	extern int yousosuu;
	int i;
	unt tmp;

	pp = p = (char *)malloc(sizeof(char)*yousosuu*8+5);
	if(p==NULL){
		exit(1);
	}
	strncpy(p, ss, (size_t)(8*yousosuu));

	utbi_shokika(x);

	i=0;
	while(*p!='\0'&&i<8*yousosuu){
		*p = tolower(*p);
		p++;
		i++;
	}
	p = pp;
	for(;(*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f');p++){
		utbi_bitzurashi_h4(x, x);
		if(*p >= '0' && *p <= '9'){
			tmp = *p - '0';
		}else{
			tmp = *p - 'a' + 0xa;
		}
		utbi_wa_ui(x, x, tmp);
	}

	p = pp;
	while(*p!='\0'){
		*p='\0';
		p++;
	}

	free(pp);

	p = pp = NULL;
}
