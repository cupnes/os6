#define NULL		(void *)0
#define MAX_LINE_SIZE	512

enum {
	ECHO,
	SHOWHWPARAM,
#ifdef DEBUG
	TEST,
#endif /* DEBUG */
	COMMAND_NUM
} _COMMAND_SET;

struct EFI_INPUT_KEY {
	unsigned short ScanCode;
	unsigned short UnicodeChar;
};

struct EFI_GUID {
	unsigned int Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char Data4[8];
};

struct EFI_SYSTEM_TABLE {
	char _buf1[44];
	struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
		void *_buf;
		unsigned long long (*ReadKeyStroke)(struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL *, struct EFI_INPUT_KEY *);
	} *ConIn;
	void *_buf2;
	struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
		void *_buf;
		unsigned long long (*OutputString)(struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *, unsigned short *);
	} *ConOut;
	char _buf3[24];
	struct EFI_BOOT_SERVICES {
		char _buf1[24];
		char _buf2[296];
		unsigned long long (*LocateProtocol)(struct EFI_GUID *, void *, void **);
	} *BootServices;
};

struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL {
	unsigned char Blue;
	unsigned char Green;
	unsigned char Red;
	unsigned char Reserved;
};

enum EFI_GRAPHICS_OUTPUT_BLT_OPERATION {
	EfiBltVideoFill,
	EfiBltVideoToBltBuffer,
	EfiBltBufferToVideo,
	EfiBltVideoToVideo,
	EfiGraphicsOutputBltOperationMax
};

enum EFI_GRAPHICS_PIXEL_FORMAT {
	PixelRedGreenBlueReserved8BitPerColor,
	PixelBlueGreenRedReserved8BitPerColor,
	PixelBitMask,
	PixelBltOnly,
	PixelFormatMax
};

struct EFI_GRAPHICS_OUTPUT_PROTOCOL {
	void *_buf;
	unsigned long long (*SetMode)(struct EFI_GRAPHICS_OUTPUT_PROTOCOL *, unsigned int);
	unsigned long long (*Blt)(struct EFI_GRAPHICS_OUTPUT_PROTOCOL *,
				  struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL *,
				  enum EFI_GRAPHICS_OUTPUT_BLT_OPERATION,
				  unsigned long long SourceX, unsigned long long SourceY,
				  unsigned long long DestinationX, unsigned long long DestinationY,
				  unsigned long long Width, unsigned long long Height,
				  unsigned long long Delta);
	struct EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE {
		unsigned int MaxMode;
		unsigned int Mode;
		struct EFI_GRAPHICS_OUTPUT_MODE_INFORMATION {
			unsigned int Version;
			unsigned int HorizontalResolution;
			unsigned int VerticalResolution;
			enum EFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
			struct EFI_PIXEL_BITMASK {
				unsigned int RedMask;
				unsigned int GreenMask;
				unsigned int BlueMask;
				unsigned int ReservedMask;
			} PixelInformation;
			unsigned int PixelsPerScanLine;
		} *Info;
		unsigned long long SizeOfInfo;
		unsigned long long FrameBufferBase;
		unsigned long long FrameBufferSize;
	} *Mode;
};

struct EFI_SYSTEM_TABLE *SystemTable;
struct EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

static void str_copy(const unsigned short *src, unsigned short *dst, unsigned int size)
{
	for (; size > 0; size--)
		*dst++ = *src++;
}

static void put_char(unsigned short c)
{
	unsigned short str[2] = L" ";

	str[0] = c;
	SystemTable->ConOut->OutputString(SystemTable->ConOut, str);
}

static void put_str(unsigned short *str)
{
	SystemTable->ConOut->OutputString(SystemTable->ConOut, str);
}

static unsigned short get_char(void)
{
	struct EFI_INPUT_KEY efi_input_key;

	while (SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &efi_input_key));

	return efi_input_key.UnicodeChar;
}

static unsigned int get_line(unsigned short *buf, unsigned int buf_size)
{
	unsigned int i;

	for (i = 0; i < buf_size - 1;) {
		buf[i] = get_char();
		put_char(buf[i]);
		if (buf[i] == L'\r') {
			put_char(L'\n');
			break;
		}
		i++;
	}
	buf[i] = L'\0';

	return i;
}

static int str_get_len(const unsigned short *src)
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

static void str_get_first_entry(const unsigned short *line, unsigned short *first, unsigned short *other)
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

static int str_compare(const unsigned short *src, const unsigned short *dst)
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

static int command_echo(unsigned short *args)
{
	put_str(args);
	put_str(L"\r\n");

	return 0;
}

static int command_showhwparam(unsigned short *args __attribute__ ((unused)))
{
	unsigned short str[32];

	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"MaxMode: ");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, int_to_ascii(gop->Mode->MaxMode, 10, str));
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\r\n");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Mode: ");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, int_to_ascii(gop->Mode->Mode, 10, str));
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\r\n");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Version: ");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, int_to_ascii(gop->Mode->Info->Version, 10, str));
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\r\n");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"HorizontalResolution: ");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, int_to_ascii(gop->Mode->Info->HorizontalResolution, 10, str));
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\r\n");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"VerticalResolution: ");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, int_to_ascii(gop->Mode->Info->VerticalResolution, 10, str));
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\r\n");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"PixelFormat: ");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, int_to_ascii(gop->Mode->Info->PixelFormat, 10, str));
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\r\n");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"PixelInformation:\r\n");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, int_to_ascii_hex(gop->Mode->Info->PixelInformation.RedMask, 8, str));
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\r\n");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, int_to_ascii_hex(gop->Mode->Info->PixelInformation.GreenMask, 8, str));
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\r\n");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, int_to_ascii_hex(gop->Mode->Info->PixelInformation.BlueMask, 8, str));
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\r\n");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, int_to_ascii_hex(gop->Mode->Info->PixelInformation.ReservedMask, 8, str));
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\r\n");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"SizeOfInfo: ");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, int_to_ascii(gop->Mode->SizeOfInfo, 10, str));
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\r\n");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"FrameBufferBase: ");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, int_to_ascii_hex(gop->Mode->FrameBufferBase, 16, str));
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\r\n");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"FrameBufferSize: ");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, int_to_ascii_hex(gop->Mode->FrameBufferSize, 16, str));
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\r\n");

	return 0;
}

#ifdef DEBUG
static int command_test(unsigned short *args __attribute__ ((unused)))
{
	put_str(L"test\r\n");

	return 0;
}
#endif /* DEBUG */

static unsigned char get_command_id(const unsigned short *command)
{
	if (!str_compare(command, L"echo")) {
		return ECHO;
	}

	if (!str_compare(command, L"showhwparam")) {
		return SHOWHWPARAM;
	}

#ifdef DEBUG
	if (!str_compare(command, L"test")) {
		return TEST;
	}
#endif /* DEBUG */

	return COMMAND_NUM;
}

void shell(void)
{
	unsigned short buf[MAX_LINE_SIZE];
	unsigned short command[256], args[256];
	unsigned char command_id;

	while (1) {
		put_str(L"shell> ");
		if (get_line(buf, MAX_LINE_SIZE) <= 0) {
			continue;
		}

		str_get_first_entry(buf, command, args);
		command_id = get_command_id(command);

		switch (command_id) {
		case ECHO:
			command_echo(args);
			break;
		case SHOWHWPARAM:
			command_showhwparam(args);
			break;
#ifdef DEBUG
		case TEST:
			command_test(args);
			break;
#endif /* DEBUG */
		default:
			put_str(L"Command not found.\r\n");
			break;
		}
	}
}

void efi_main(void *ImageHandle __attribute__ ((unused)), struct EFI_SYSTEM_TABLE *_SystemTable)
{
	struct EFI_GUID gop_guid = {0x9042a9de, 0x23dc, 0x4a38, {0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a}};

	SystemTable = _SystemTable;
	SystemTable->BootServices->LocateProtocol(&gop_guid, NULL, (void **)&gop);

	shell();
}
