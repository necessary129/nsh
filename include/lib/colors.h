#ifndef LIB_COLORS_H
#define LIB_COLORS_H

#define CUDEF


#define CMNORMAL "00"
#define CMBOLD "01"
#define CMDEFAULT CMBOLD
#define CMUNDERLINED "04"
#define CMFLASHING "05"

#define CCBLACK "30"
#define CCRED "31"
#define CCGREEN "32"
#define CCORANGE "33"
#define CCBLUE "34"
#define CCPURPLE "35"
#define CCCYAN "36"
#define CCGREY "37"

#define CBBLACK "40"
#define CBRED "41"
#define CBORANGE "43"
#define CBBLUE "44"
#define CBPURPLE "45"
#define CBCYAN "46"
#define CBGREY "47"



#define COLOR_RESET "\e[0;0;0m"




#define CGETCOLOR3(type, fore, back) "\e[" CM##type ";" CC##fore ";" CB##back "m"
#define CGETCOLOR2(fore, back) "\e[" CMDEFAULT ";" CC##fore ";" CB##back "m"
#define CGETCOLOR1(fore) "\e[" CMDEFAULT ";" CC##fore "m"

#define GET_MACRO(_1,_2,_3, NAME,...) NAME
#define CGETCOLOR(...) GET_MACRO(__VA_ARGS__, CGETCOLOR3, CGETCOLOR2, CGETCOLOR1)(__VA_ARGS__)

#endif
