#ifndef NSH_RAWGETLINE_H
#define NSH_RAWGETLINE_H
#include <stddef.h>
#define C_LF 0x0A
#define C_BS 0x08
#define C_DEL 0x7F
#define C_TAB 0x09
#define C_CR 0x0D
#define C_ETX 0x03
#define C_SUB 0x1A
#define C_EOT 0x04

#define C_ESC 0x1B

size_t myGetline(char **line, size_t *maxLen);

#endif