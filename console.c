#include <console.h>
#include <efi.h>

int cursor_x = 0;

void put_char(unsigned short c)
{
	unsigned short str[2] = L" ";

	str[0] = c;
	SystemTable->ConOut->OutputString(SystemTable->ConOut, str);
}

volatile unsigned char lock_conout = 0;
void put_str(unsigned short *str)
{
	while (lock_conout);
	lock_conout = 1;
	SystemTable->ConOut->OutputString(SystemTable->ConOut, str);
	lock_conout = 0;
}

unsigned short get_char(void)
{
	struct EFI_INPUT_KEY efi_input_key;

	while (SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &efi_input_key));

	return efi_input_key.UnicodeChar;
}

unsigned int get_line(unsigned short *buf, unsigned int buf_size)
{
	unsigned int i;

	cursor_x = 0;
	for (i = 0; i < buf_size - 1;) {
		buf[i] = get_char();
		if ((buf[i] == UNICODE_BS) && (cursor_x <= 0))
			continue;
		put_char(buf[i]);
		if (buf[i] == L'\r') {
			put_char(L'\n');
			break;
		}
		if (buf[i] == UNICODE_BS) {
			i--;
			cursor_x--;
		} else {
			i++;
			cursor_x++;
		}
	}
	buf[i] = L'\0';

	return i;
}
