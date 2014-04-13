/*
./LIBSRC/MATH.C
*/

#include <MATH.H>
#include <STRING.H>

int exponent(int base, unsigned int exp)
{
    int i, result = 1;
    for (i = 0; i < exp; i++)
        result *= base;
    return result;
}

unsigned int textTOhex(char* s)
{
	unsigned int ret = 0;
	while ( *s != 0 )
	{
		if (*s >= '0' && *s <= '9') ret = (ret * 16) + (*s - '0');
		else if (*s >= 'A' && *s <= 'F') ret = (ret * 16) + (*s - 'A' + 10);
		else if (*s >= 'a' && *s <= 'f') ret = (ret * 16) + (*s - 'a' + 10);
		else return 0;
		*s++;
	}
	return ret;
}

char* hexTOtext(int in)
{
	char* out;
	itoa_s (in, 16, out);
	return out;
}

char* intTOchar(int in)
{
	char* out;
	itoa_s (in, 10, out);
	return out;
}

unsigned int charTOint(char* in)
{
	unsigned int ret = 0;
	while (*in != 0)
	{
		if (*in < '0' || *in > '9') return -1;
		ret = ret * 10 + *in - '0';
		*in++;
	}
	return ret;
}

uint64_t BIN2INT(unsigned char* in)  // it's backwards
{
	int i = strlen((const char*)in) -1;
	uint64_t ret = 0;
	while (i >= 0) {
		ret = ret * 256 + in[i];
		i--;
	}
	//ret = (uint64_t) in;
	return ret;
}

bool isEven(uint64_t in)
{
	if (in%2 == 0) return true;
	return false;
}
