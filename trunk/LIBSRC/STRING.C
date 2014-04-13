/*
./LIBSRC/STRING.C
*/

#include <STRING.H>
#include <STDIO.H>

char ASCII[128] =
{
'\0', 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
'\b', '\t', '\n', '\v', '\f', '\r', 0x0E, 0x0F,
0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
' ', '!', '\"', '#', '$', '%', '&', '\'',
'(', ')', '*', '+', ',', '-', '.', '/',
'0', '1', '2', '3', '4', '5', '6', '7',
'8', '9', ':', ';', '<', '=', '>', '?',
'@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
'`', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
'x', 'y', 'z', '{', '|', '}', '~', 0x7F
};

int ASCII2int(char A)
{
	int ret = 0;
	int done = 0;
	while (done == 0)
	{
		if (ASCII[ret] == A) done = 1;
		ret ++;
	}
	ret --;
	return ret;
}
char int2ASCII(int A)
{
	return ASCII[A];
}

bool arrayAppend(char* out, const char in)
{
	bool ret = TRUE;
	int i = strlen(out) - 1;
	if (!(out[i] = in) )
	{
		ret = FALSE;
	}
	if ((out[i+1] = '\0') )
	{
		ret = FALSE;
	}
	//if (!i >= 1) if (out[0] = in) if (out[1] = '\0') ret = TRUE;
	return ret;
	
}
bool arrayRemove(char* out, const int cs)
{
	bool ret = TRUE;
	int i = strlen(out) - 1;
	if ( i <= 0 ) return FALSE;
	for (int i = strlen(out) - 1 - cs; i < strlen(out) - 1; i++)
		if ((out[i] = '\0'))
			ret = FALSE;
	return ret;
}
int searchChar(char find, char *in)
{
	int ret;
	int done;
	for (ret = 0; done == 0; ret++)
	{
		if (in[ret] == find) done = 1;
		if (ret == strlen(in))
		{
			done = 1;
			ret = 0;
		}
	}
	return ret;
}
int strlen(const char *str)
{
	size_t ret = 0;
	while (str[ret++]);
	return ret;
}
int explode( char out[50][100], const char *s1, const char s2 )
{
	int i = 0;
	int z = 0;
	memset(out, 0, sizeof out);
	while (*s1 != 0)
	{
		//if(!streql(*s1, s2))
		if(*s1 != s2)
		{
			out[i][z] = *s1;
			z++;
		}
		else
		{
			out[i][z] = 0;
			out[i][z+1] = 0;
			i++;
			z = 0;
		}
		*s1++;
	}
	return i;
}
bool streql( const char *s1, const char *s2 )
{
	int temp = strcmp (s1, s2);
	if (temp == 0) return TRUE;
	else return FALSE;
}

int strcmp( const char * s1, const char * s2 )
{
	int ret = 0;
	while (!(ret = *(unsigned char*)s1 - *(unsigned char*)s2) && *s2)
    {
        ++s1;
        ++s2;
    }
	if (ret < 0) ret = -1;
	if (ret > 0) ret = 1;
    return	ret;
}

char *strcpy(char *s1, const char *s2)
{
	char *s1_p = s1;
	while ( (*s1++ = *s2++) );
	return s1_p;
}

char* StringtoUpper(char* in)
{
	char* out;
	int i = 0;
	while (*in != 0) {
		out[i] = ChartoUpper(*in);
		*in++;
		i++;
	}
	out[i] = '\0';
	return out;
}

char* StringtoLower(char* in)
{
	char* out;
	int i = 0;
	while (*in != 0) {
		//out[i] = ChartoLower(*in);
		if(*in >= 'A' && *in <= 'Z')
			out[i] = *in + 32;
		else
			out[i] = *in;
		*in++;
		i++;
	}
	out[i] = '\0';
	return out;
}

char ChartoUpper(char in)
{
	char out;
	if(in >= 'a' && in <= 'z')
		out = in - 32;
	else
		out = in;
	return out;
}

char ChartoLower(char in)
{
	char out;
	if(in >= 'A' && in <= 'Z')
		out = in + 32;
	else
		out = in;
	return out;
}


char* strchr (char * str, int character )
{
	do {
		if ( *str == character )
			return (char*)str;
	}
	while (*str++);
	return 0;
}

char* charchange(const char in)
{
	char *ret;
	ret[0] = in;
	ret[1] = '\0';
	return ret;
}

int stringf(char *str, const char *format, ...)
{
	va_list ap;
	va_start(ap,format);
	size_t loc=0;
	size_t i;
	for (i=0 ; i<=strlen(format);i++, loc++)
	{
		switch (format[i]) {
			case '%':
				switch (format[i+1])
				{
					/*** characters ***/
					case 'c': {
						char c = va_arg (ap, char);
						str[loc] = c;
						i++;
						break;
					}
					/*** integers ***/
					case 'd':
					case 'i':
					{
						int c = va_arg (ap, int);
						char s[32]={0};
						itoa_s (c, 10, s);
						strcpy (&str[loc], s);
						loc+= strlen(s) - 2;
						i++;		// go to next character
						break;
					}
					/*** display in hex ***/
					case 'X':
					case 'x':
					{
						int c = va_arg (ap, int);
						char s[32]={0};
						itoa_s (c,16,s);
						strcpy (&str[loc], s);
						i++;		// go to next character
						loc+=strlen(s) - 2;
						break;
					}
					/*** strings ***/
					case 's':
					{
						char *c = va_arg (ap, char*);
						char s[32]={0};
						strcpy (s,(const char*)c);						
						strcpy (&str[loc], s);
						i++;		// go to next character
						loc+=strlen(s) - 2;
						break;
					}
				}
				break;
			default:
				str[loc] = format[i];
				break;
		}
	}
	return i;
}



char tbuf[32];
char bchars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void itoa(unsigned i,unsigned base,char* buf)
{
   int pos = 0;
   int opos = 0;
   int top = 0;

   if (i == 0 || base > 16)
   {
      buf[0] = '0';
      buf[1] = '\0';
      return;
   }

   while (i != 0)
   {
      tbuf[pos] = bchars[i % base];
      pos++;
      i /= base;
   }
   top=pos--;
   for (opos=0; opos<top; pos--,opos++)
   {
      buf[opos] = tbuf[pos];
   }
   buf[opos] = 0;
   return;
}

void itoa_s(int i,unsigned base,char* buf)
{
   if (base > 16) return;
   if (i < 0)
   {
      *buf++ = '-';
      i *= -1;
   }
   itoa(i,base,buf);
   return;
}