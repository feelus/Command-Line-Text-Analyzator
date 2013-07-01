/*
 *  Text analysis program
 * 
 *  File: cp1250_ctype.c
 *  Contains CP1250 specific character functions. Similar to ctype.c.
 * 
 *  Author: Martin Kucera, 2012
 */

#include <ctype.h>

#include "cp1250_ctype.h"

/* array of CP1250 specific characters */
unsigned short dict[128] = {
    _PUNCT, /* 0x80 € */
    _EMPTY, /* 0x81 UNUSED */
    _PUNCT, /* 0x82 ‚ */
    _EMPTY, /* 0x83 UNUSED */
    _PUNCT, /* 0x84 „ */
    _PUNCT, /* 0x85 … */
    _PUNCT, /* 0x86 † */
    _PUNCT, /* 0x87 ‡ */
    _EMPTY, /* 0x88 UNUSED */
    _PUNCT, /* 0x89 ‰ */
    _UPPER, /* 0x8A Š */
    _PUNCT, /* 0x8B ‹ */
    _UPPER, /* 0x8C Ś */
    _UPPER, /* 0x8D Ť */
    _UPPER, /* 0x8E Ž */
    _UPPER, /* 0x8F Ź */
    
    _EMPTY, /* 0x90 UNUSED */
    _PUNCT, /* 0x91 ‘ */
    _PUNCT, /* 0x92 ’ */
    _PUNCT, /* 0x93 “ */
    _PUNCT, /* 0x94 ” */
    _PUNCT, /* 0x95 • */
    _PUNCT, /* 0x96 – */
    _PUNCT, /* 0x97 — */
    _EMPTY, /* 0x98 UNUSED */
    _PUNCT, /* 0x99 ™ */
    _LOWER, /* 0x9A š */
    _PUNCT, /* 0x9B › */
    _LOWER, /* 0x9C ś */
    _LOWER, /* 0x9D ť */
    _LOWER, /* 0x9E ž */
    _LOWER, /* 0x9F ź */
    
    _SPACE, /* 0xA0 NBSP */
    _PUNCT, /* 0xA1 ˇ */
    _PUNCT, /* 0xA2 ˘ */
    _UPPER, /* 0xA3 Ł */
    _PUNCT, /* 0xA4 ¤ */
    _UPPER, /* 0xA5 Ą */
    _PUNCT, /* 0xA6 ¦ */
    _PUNCT, /* 0xA7 § */
    _PUNCT, /* 0xA8 ¨ */
    _PUNCT, /* 0xA9 © */
    _UPPER, /* 0xAA Ş */
    _PUNCT, /* 0xAB « */
    _PUNCT, /* 0xAC ¬ */
    _PUNCT, /* 0xAD SHY */
    _PUNCT, /* 0xAE ® */
    _UPPER, /* 0xAF Ż */
    
    _PUNCT, /* 0xB0 ° */
    _PUNCT, /* 0xB1 ± */
    _PUNCT, /* 0xB2 ˛ */
    _LOWER, /* 0xB3 ł */
    _PUNCT, /* 0xB4 ´ */
    _LOWER, /* 0xB5 µ */
    _PUNCT, /* 0xB6 ¶ */
    _PUNCT, /* 0xB7 · */
    _PUNCT, /* 0xB8 ¸ */
    _LOWER, /* 0xB9 ą */
    _LOWER, /* 0xBA ş */
    _PUNCT, /* 0xBB » */
    _UPPER, /* 0xBC Ľ */
    _PUNCT, /* 0xBD ˝ */
    _LOWER, /* 0xBE ľ */
    _LOWER, /* 0xBF ż */
    
    _UPPER, /* 0xC0 Ŕ */
    _UPPER, /* 0xC1 Á */
    _UPPER, /* 0xC2 Â */
    _UPPER, /* 0xC3 Ă */
    _UPPER, /* 0xC4 Ä */
    _UPPER, /* 0xC5 Ĺ */
    _UPPER, /* 0xC6 Ć */
    _UPPER, /* 0xC7 Ç */
    _UPPER, /* 0xC8 Č */
    _UPPER, /* 0xC9 É */
    _UPPER, /* 0xCA Ę */
    _UPPER, /* 0xCB Ë */
    _UPPER, /* 0xCC Ě */
    _UPPER, /* 0xCD Í */
    _UPPER, /* 0xCE Î */
    _UPPER, /* 0xCF Ď */
    
    _UPPER, /* 0xD0 Đ */
    _UPPER, /* 0xD1 Ń */ 
    _UPPER, /* 0xD2 Ň */
    _UPPER, /* 0xD3 Ó */
    _UPPER, /* 0xD4 Ô */
    _UPPER, /* 0xD5 Ő */
    _UPPER, /* 0xD6 Ö */
    _PUNCT, /* 0xD7 × */
    _UPPER, /* 0xD8 Ř */
    _UPPER, /* 0xD9 Ů */
    _UPPER, /* 0xDA Ú */
    _UPPER, /* 0xDB Ű */
    _UPPER, /* 0xDC Ü */
    _UPPER, /* 0xDD Ý */
    _UPPER, /* 0xDE Ţ */
    _UPPER, /* 0xDF ß */
    
    _LOWER, /* 0xE0 ŕ */
    _LOWER, /* 0xE1 á */
    _LOWER, /* 0xE2 â */
    _LOWER, /* 0xE3 ă */
    _LOWER, /* 0xE4 ä */
    _LOWER, /* 0xE5 ĺ */
    _LOWER, /* 0xE6 ć */
    _LOWER, /* 0xE7 ç */
    _LOWER, /* 0xE8 č */
    _LOWER, /* 0xE9 é */
    _LOWER, /* 0xEA ę */
    _LOWER, /* 0xEB ë */
    _LOWER, /* 0xEC ě */
    _LOWER, /* 0xED í */
    _LOWER, /* 0xEE î */
    _LOWER, /* 0xEF ď */
    
    _LOWER, /* 0xF0 đ */
    _LOWER, /* 0xF1 ń */
    _LOWER, /* 0xF2 ň */
    _LOWER, /* 0xF3 ó */
    _LOWER, /* 0xF4 ô */
    _LOWER, /* 0xF5 ő */
    _LOWER, /* 0xF6 ö */
    _PUNCT, /* 0xF7 ÷ */
    _LOWER, /* 0xF8 ř */
    _LOWER, /* 0xF9 ů */
    _LOWER, /* 0xFA ú */
    _LOWER, /* 0xFB ű */
    _LOWER, /* 0xFC ü */
    _LOWER, /* 0xFD ý */
    _LOWER, /* 0xFE ţ */
    _PUNCT  /* 0xFF ˙ */
};

/**
 *  int cp1250_isalpha(int c)
 * 
 *  Checks whether c is an alphabetic letter
 */
int cp1250_isalpha(int c) {
    if(c < _DICTOFFSET) {
	return isalpha(c);
    }
    
    return dict[c - _DICTOFFSET] & (_UPPER | _LOWER);
}

/**
 *  int cp1250_isupper(int c)
 * 
 *  Checks whether c is an uppercase alphabetic letter
 */
int cp1250_isupper(int c) {
    if(c < _DICTOFFSET) {
	return isupper(c);
    }
    
    return dict[c - _DICTOFFSET] & _UPPER;
}

/**
 *  int cp1250_islower(int c)
 * 
 *  Checks whether c is an lowercase alphabetic letter
 */
int cp1250_islower(int c) {
    if(c < _DICTOFFSET) {
	return islower(c);
    }
    
    return dict[c - _DICTOFFSET] & _LOWER;
}

/**
 *  int cp1250_tolower(int c)
 * 
 *  Converts an uppercase letter to it's lowercase equivalent if such exists.
 */
int cp1250_tolower(int c) {
    int diff = 0;
    
    if(c < _DICTOFFSET) {
	return tolower(c);
    }
    
    if(dict[c - _DICTOFFSET] & _UPPER) {
	if(c <= 191) {
	    if(c == 165) {
		diff = 20;
	    }
	    else if(c == 188) {
		diff = 2;
	    }
	    else {
		diff = 16;
	    }
	}
	else {
	    if(c == 223) {
		diff = 0;
	    }
	    else {
		diff = 32;
	    }
	}
    }
    
    return c + diff;
}

/**
 *  int cp1250_isspace(int c)
 * 
 *  Checks wheter c is a space character.
 */
int cp1250_isspace(int c) {
    if(c < _DICTOFFSET) {
        return isspace(c);
    }
    
    return dict[c] & _SPACE;
}
