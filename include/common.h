#ifndef _COMMON_H_
#define _COMMON_H_

#define NULL		(void *)0

void str_copy(const unsigned short *src, unsigned short *dst, unsigned int size);
int str_get_len(const unsigned short *src);
int str_find_char(const unsigned short *src, unsigned short key);
void str_get_first_entry(const unsigned short *line, unsigned short *first, unsigned short *other);
int str_compare(const unsigned short *src, const unsigned short *dst);
unsigned short *int_to_ascii(long long val, unsigned char num_digits, unsigned short str[]);
unsigned short *int_to_ascii_hex(unsigned long long val, unsigned char num_digits, unsigned short str[]);
unsigned short *int_to_unicode(long long val, unsigned char num_digits, unsigned short str[]);
unsigned short *int_to_unicode_hex(unsigned long long val, unsigned char num_digits, unsigned short str[]);
unsigned short *ascii_to_unicode(char ascii[], unsigned char num_digits, unsigned short str[]);

#endif /* _COMMON_H_ */
