/*
./INCLUDE/STRING.H
*/

#include <FORMATTING.H>
#include <stdarg.h>

#ifndef				STRING_H_INCLUDED
	#define				STRING_H_INCLUDED
	
	bool arrayAppend(char* out, const char in);
	bool arrayRemove(char* out, const int cs);
	int searchChar(char find, char *in);
	int explode( char out[50][100], const char *s1, const char s2 );
	bool streql( const char *s1, const char *s2 );
	int strcmp( const char * s1, const char * s2 );
	char *strcpy(char *s1, const char *s2);
	char* StringtoUpper(char* in);
	char* StringtoLower(char* in);
	char ChartoUpper(char in);
	char ChartoLower(char in);
	char* strchr (char * str, int character );
	char* charchange(const char in);
	int strlen(const char *str);
	int stringf(char *str, const char *format, va_list ap);
	void itoa(unsigned i,unsigned base,char* buf);
	void itoa_s(unsigned i,unsigned base,char* buf);
	
#endif