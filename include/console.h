#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#define UNICODE_BS	0x0008

extern int cursor_x;

void put_char(unsigned short c);
void put_str(unsigned short *str);
unsigned short get_char(void);
unsigned int get_line(unsigned short *buf, unsigned int buf_size);

#endif /* _CONSOLE_H_ */
