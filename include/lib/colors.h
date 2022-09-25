#ifndef LIB_COLORS_H
#define LIB_COLORS_H

#define CUDEF

#define CMNORMAL "00"
#define CMBOLD "01"
#define CMDEFAULT CMBOLD
#define CMUNDERLINED "04"
#define CMFLASHING "05"

#define CCNONE "00"
#define CCBLACK "30"
#define CCRED "31"
#define CCGREEN "32"
#define CCORANGE "33"
#define CCBLUE "34"
#define CCPURPLE "35"
#define CCCYAN "36"
#define CCWHITE "37"
#define CCBRIGHT_BLACK "90"
#define CCBRIGHT_RED "91"
#define CCBRIGHT_GREEN "92"
#define CCBRIGHT_ORANGE "93"
#define CCBRIGHT_BLUE "94"
#define CCBRIGHT_PURPLE "95"
#define CCBRIGHT_CYAN "96"
#define CCBRIGHT_WHITE "97"

#define CBNONE "00"
#define CBBLACK "40"
#define CBRED "41"
#define CBORANGE "43"
#define CBBLUE "44"
#define CBPURPLE "45"
#define CBCYAN "46"
#define CBWHITE "47"
#define CBBRIGHT_BLACK "100"
#define CBBRIGHT_RED "101"
#define CBBRIGHT_ORANGE "103"
#define CBBRIGHT_BLUE "104"
#define CBBRIGHT_PURPLE "105"
#define CBBRIGHT_CYAN "106"
#define CBBRIGHT_WHITE "107"

#define CRESET "\e[0;0;0m"

#define CGETCOLOR3(type, fore, back)                                           \
	"\e[" CM##type ";" CC##fore ";" CB##back "m"
#define CGETCOLOR2(fore, back) "\e[" CMDEFAULT ";" CC##fore ";" CB##back "m"
#define CGETCOLOR1(fore) "\e[" CMDEFAULT ";" CC##fore "m"

#define GET_MACRO(_1, _2, _3, NAME, ...) NAME
#define CGETCOLOR(...)                                                         \
	GET_MACRO(__VA_ARGS__, CGETCOLOR3, CGETCOLOR2, CGETCOLOR1)(__VA_ARGS__)

#define CTGETCOLOR(type, fore) "\e[" CM##type ";" CC##fore "m"

#endif
