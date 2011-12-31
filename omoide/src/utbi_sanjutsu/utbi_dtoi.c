/* utbi_dtoi.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

void utbi_dtoi(unt *dtoi, wchar_t *wss)
{
	wchar_t *p = wss;

	utbi_shokika(dtoi);

	while(L'0' <= *p && *p <= L'9'){
		utbi_seki_ui(dtoi, dtoi, 10);
		utbi_wa_ui(dtoi, dtoi, (unt)(*p - L'0'));
		p++;
	}
}
