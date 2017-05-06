#include <common.h>

void str_copy(const unsigned short *src, unsigned short *dst, unsigned int size)
{
	for (; size > 0; size--)
		*dst++ = *src++;
}

int str_get_len(const unsigned short *src)
{
	int len;
	for (len = 0; src[len] != L'\0'; len++);
	return len + 1;
}

int str_find_char(const unsigned short *src, unsigned short key)
{
	int i;

	for (i = 0; src[i] != key; i++) {
		if (src[i] == L'\0') {
			i = -1;
			break;
		}
	}

	return i;
}

void str_get_first_entry(const unsigned short *line, unsigned short *first, unsigned short *other)
{
	int line_len, first_len, other_len;

	line_len = str_get_len(line);
	first_len = str_find_char(line, L' ');
	if (first_len < 0) {
		str_copy(line, first, line_len);
		first_len = line_len;
		other_len = 0;
		other[other_len] = L'\0';
	} else {
		str_copy(line, first, first_len);
		first[first_len] = L'\0';
		first_len++;
		other_len = line_len - first_len;
		str_copy(line + first_len, other, other_len);
	}
}

int str_compare(const unsigned short *src, const unsigned short *dst)
{
	char is_equal = 1;

	for (; (*src != L'\0') && (*dst != L'\0'); src++, dst++) {
		if (*src != *dst) {
			is_equal = 0;
			break;
		}
	}

	if (is_equal) {
		if (*src != L'\0') {
			return 1;
		} else if (*dst != L'\0') {
			return -1;
		} else {
			return 0;
		}
	} else {
		return (int)(*src - *dst);
	}
}

unsigned short *int_to_ascii(long long val, unsigned char num_digits, unsigned short str[])
{
	unsigned char digits_base = 0;
	char i;

	if (val < 0) {
		str[digits_base++] = L'-';
		val *= -1;
	}

	for (i = num_digits - 1; i >= 0; i--) {
		str[digits_base + i] = L'0' + (val % 10);
		val /= 10;
	}

	str[digits_base + num_digits] = L'\0';

	return str;
}

unsigned short *int_to_ascii_hex(unsigned long long val, unsigned char num_digits, unsigned short str[])
{
	short i;
	unsigned short v;

	for (i = num_digits - 1; i >= 0; i--) {
		v = (unsigned short)(val & 0x0f);
		if (v < 0xa)
			str[i] = L'0' + v;
		else
			str[i] = L'A' + (v - 0xa);
		val >>= 4;
	}

	str[num_digits] = L'\0';

	return str;
}

unsigned short *int_to_unicode(long long val, unsigned char num_digits, unsigned short str[])
{
	unsigned char digits_base = 0;
	char i;

	if (val < 0) {
		str[digits_base++] = L'-';
		val *= -1;
	}

	for (i = num_digits - 1; i >= 0; i--) {
		str[digits_base + i] = L'0' + (val % 10);
		val /= 10;
	}

	str[digits_base + num_digits] = L'\0';

	return str;
}

unsigned short *int_to_unicode_hex(unsigned long long val, unsigned char num_digits, unsigned short str[])
{
	short i;
	unsigned short v;

	for (i = num_digits - 1; i >= 0; i--) {
		v = (unsigned short)(val & 0x0f);
		if (v < 0xa)
			str[i] = L'0' + v;
		else
			str[i] = L'A' + (v - 0xa);
		val >>= 4;
	}

	str[num_digits] = L'\0';

	return str;
}

unsigned short *ascii_to_unicode(char ascii[], unsigned char num_digits, unsigned short str[])
{
	unsigned char i;

	for (i = 0; i < num_digits; i++) {
		if (ascii[i] == '\0') {
			break;
		}

		if ('0' <= ascii[i] && ascii[i] <= '9')
			str[i] = L'0' + (ascii[i] - '0');
		else if ('A' <= ascii[i] && ascii[i] <= 'Z')
			str[i] = L'A' + (ascii[i] - 'A');
		else if ('a' <= ascii[i] && ascii[i] <= 'z')
			str[i] = L'a' + (ascii[i] - 'a');
		else {
			switch (ascii[i]) {
			case ' ':
				str[i] = L' ';
				break;
			case '-':
				str[i] = L'-';
				break;
			case '+':
				str[i] = L'+';
				break;
			case '*':
				str[i] = L'*';
				break;
			case '/':
				str[i] = L'/';
				break;
			case '&':
				str[i] = L'&';
				break;
			case '|':
				str[i] = L'|';
				break;
			case '%':
				str[i] = L'%';
				break;
			case '#':
				str[i] = L'#';
				break;
			case '!':
				str[i] = L'!';
				break;
			case '\r':
				str[i] = L'\r';
				break;
			case '\n':
				str[i] = L'\n';
				break;
			}
		}
	}

	str[i] = L'\0';
	return str;
}
