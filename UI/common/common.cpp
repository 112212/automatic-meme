#include "common.hpp"
#include <iostream>

namespace ng {
void printChars(char* str, int start, int end) {
	for(int i=start; i < end-start; i++) {
		std::cout << str[i];
	}
}

/*
inline unsigned int binary_search_log10( unsigned int num ) {
	// fast log10
	if(num >= 100000u) { // 4 zeros (9,8,7,6,5)
		if(num >= 10000000u) { // 7 zeros (7,8,9)
			if(num >= 1000000000u) // 9 zeros
				return 9;
			else if( num >= 100000000u ) // 10 zeros
				return 8;
			else 
				return 7;
		} else { // 6,5
			if(num >= 1000000u) { // 6 zeros
				return 6;
			} else {
				return 5;
			}
		}
	} else { // 4,3,2,1
		if( num >= 1000u ) { // 4, 3
			if( num >= 10000u )
				return 4;
			else
				return 3;
		} else { // 2, 1
			if( num >= 100u )
				return 2;
			else if( num >= 10u )
				return 1;
			else 
				return 0;
		}
	}
	return 0;
}

int old_bcd_to_binary( const char* str, int start, int end ) {
	int buf=0;
	int len = end-start;
	int retval = 0;
	int mask = 0;
	int i;
	for(i=0; i < len; i++) {
		buf |= (str[start+i]-0x30) << ((len-i-1)<<2);
	}
	
	len*=4;
	for(i=0; i < len; i++) {
		if(buf & 1)
			retval |= 1 << i;
			
		mask = buf & 0x01111110;
		if(mask) {
			buf ^= mask;
			mask >>= 4;
			mask *= 10;
			buf += mask;
		}
		buf >>= 1;
	}
	return retval;
}

void old_basic_binary_to_bcd( int binary, char* str, int start, int end ) {
	// [stri] <<
	int minus = 0;
	int zeros = 1;
	int i;
	unsigned num, n, q, r;
	
	if( binary < 0 ) {
		minus = 1;
		num = (unsigned)(-binary);
	} else {
		num = (unsigned)binary;
	}
	
	zeros = binary_search_log10( num );
	
	start = start + zeros + minus;
	
	if( end - start  < 0 )
		start = end;
	
	for(i=end; i > start; i--)
		str[i] = ' ';
	
	if(num == 0)
		str[start] = '0';
	else
	for(i=start; num > 0; i--) {
		str[i] = '0' | (num % 10);
		num /= 10;
	}
	
	if(minus)
		str[i] = '-';
}



void binary_to_bcd( int binary, char* bcd, int start, int length ) {
	unsigned int num = binary > 0 ? binary : -binary;
	unsigned int num_digits = binary_search_log10( num ) + 1;
	unsigned int end = start + std::min<int>( num_digits+(binary < 0 ? 1 : 0), length );
	unsigned int end2 = start+length;
	for(int i=end; i < end2; ++i) {
		bcd[i] = ' ';
	}
	for(int i=1; i < num_digits+1; ++i) {
		bcd[end-i] = 0x30 | (num % 10); 
		num /= 10;
	}
	if(binary < 0 && length >= num_digits+1)
		bcd[end-num_digits-1] = '-';
}

int bcd_to_binary( const char* bcd, int start, int length ) {
	int end = start + length;
	bool minus = false;
	int retval = 0;
	if(bcd[start] == '-') { minus = true; start++; }
	for(int i=start; i < end; ++i) {
		retval = (retval << 3) + (retval << 1) + (bcd[i] & 0xf);
	}
	return minus ? -retval : retval;
}

unsigned int hex_to_binary( const char* str, int start, int length ) {
	int end = start + length;
	unsigned int retval = 0;
	for(int i=start; i < end; ++i) {
		retval <<= 4;
		if(str[i] >= '0' && str[i] <= '9')
			retval |= (str[i] & 15);
		else if(str[i] >= 'a' && str[i] <= 'f')
			retval |= (str[i] - 'a' + 10);
		else if(str[i] >= 'A' && str[i] <= 'F')
			retval |= (str[i] - 'A' + 10);
	}
	return retval;
}
*/

#ifdef USE_SDL
const char sdl_code_conversion_table[] = 
"................................" // 32 dots
" !\"#$%&\'()*+,-./0123456789:;<=>?@" // 33 chars
".........................." // 26 dots
"[\\]^_`abcdefghijklmnopqrstuvwxyz"
;
const char sdl_code_conversion_table_shift[] = 
"................................" // 32 dots
" !\"#$%&\"()*+<_>?)!@#$%^&*(::<+>?@" // 33 chars
".........................." // 26 dots
"{|}^_~ABCDEFGHIJKLMNOPQRSTUVWXYZ" // 32 chars
;
char SDLCodeToChar( int code, bool shift ) {
	if(code <= 122)
		return (shift ? sdl_code_conversion_table_shift[code] : sdl_code_conversion_table[code]);
	else
		return ' ';
}

#endif

#ifdef USE_SFML
const char sf_code_conversion_table[] =
"abcdefghijklmnopqrstuvwxyz0123456789..........[];,.'/\\`=- l\b\t......+-*/....0123456789................";
const char sf_code_conversion_table_shift[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ)!@#$%^&*(..........{}:<>\"?|~+_ l\b\t......+-*/....0123456789................";
char SFMLCodeToChar( unsigned char sf_code, bool shift ) {
	return (shift ? sf_code_conversion_table_shift[sf_code] : sf_code_conversion_table[sf_code]);
}
#endif
}
