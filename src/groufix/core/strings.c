/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#include <stdlib.h>
#include <string.h>

/******************************************************/
int _gfx_contains_string(

		const char*  haystack,
		const char*  needle)
{
	/* Get needle length */
	size_t len = strlen(needle);
	if(!haystack || !len) return 0;

	/* Try to find a complete match */
	char* found = strstr(haystack, needle);
	while(found)
	{
		char* end = found + len;
		if(
			(*end == ' ' || *end == '\0') &&
			(found == haystack || *(found - 1) == ' '))
		{
			return 1;
		}
		found = strstr(end, needle);
	}

	return 0;
}

/******************************************************/
char* _gfx_unformat_string(

		const char* str)
{
	/* Get required size */
	size_t size = 1;
	size_t s;

	for(s = 0; str[s]; ++s)
		size += str[s] == '%' ? 2 : 1;

	/* Format the string */
	char* form = malloc(size);
	if(!form) return NULL;

	size_t f = 0;
	for(s = 0; str[s]; ++s)
	{
		if(str[s] != '%')
			form[f++] = str[s];
		else
			form[f++] = '%', form[f++] = '%';
	}

	form[size - 1] = 0;

	return form;
}
