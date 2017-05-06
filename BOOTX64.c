#define NULL		(void *)0
#define MAX_LINE_SIZE	512
#define MAX_FILE_BUF	1024
#define MAX_IMG_BUF	4194304	/* 4MB */
#define MAX_ARGS	100
#define MAX_FILE_LEN	32

#define UNICODE_BS	0x0008

//*******************************************************
// Open Modes
//*******************************************************
#define EFI_FILE_MODE_READ	0x0000000000000001
#define EFI_FILE_MODE_WRITE	0x0000000000000002
#define EFI_FILE_MODE_CREATE	0x8000000000000000
//*******************************************************
// File Attributes
//*******************************************************
#define EFI_FILE_READ_ONLY	0x0000000000000001
#define EFI_FILE_HIDDEN	0x0000000000000002
#define EFI_FILE_SYSTEM	0x0000000000000004
#define EFI_FILE_RESERVED	0x0000000000000008
#define EFI_FILE_DIRECTORY	0x0000000000000010
#define EFI_FILE_ARCHIVE	0x0000000000000020
#define EFI_FILE_VALID_ATTR	0x0000000000000037

enum {
	ECHO,
	SHOWHWPARAM,
	LS,
	CAT,
	VIEW,
	SH,
#ifdef DEBUG
	TEST,
	KBDTEST,
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
		char _buf2[32];
		unsigned long long (*ClearScreen)(struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *);
	} *ConOut;
	char _buf3[24];
	struct EFI_BOOT_SERVICES {
		char _buf1[24];

		//
		// Task Priority Services
		//
		unsigned long long _buf2[2];

		//
		// Memory Services
		//
		unsigned long long _buf3[5];

		//
		// Event & Timer Services
		//
		unsigned long long _buf4[6];

		//
		// Protocol Handler Services
		//
		unsigned long long _buf5[9];

		//
		// Image Services
		//
		unsigned long long _buf6[5];

		//
		// Miscellaneous Services
		//
		unsigned long long _buf7[2];
		unsigned long long (*SetWatchdogTimer)(
			unsigned long long Timeout,
			unsigned long long WatchdogCode,
			unsigned long long DataSize,
			unsigned short *WatchdogData);

		//
		// DriverSupport Services
		//
		unsigned long long _buf8[2];

		//
		// Open and Close Protocol Services
		//
		unsigned long long _buf9[3];

		//
		// Library Services
		//
		unsigned long long _buf10[2];
		unsigned long long (*LocateProtocol)(struct EFI_GUID *, void *, void **);
		unsigned long long _buf11[2];

		//
		// 32-bit CRC Services
		//
		unsigned long long _buf12;

		//
		// Miscellaneous Services
		//
		unsigned long long _buf13[3];
	} *BootServices;
};

struct EFI_TIME {
	unsigned short Year; // 1900 – 9999
	unsigned char Month; // 1 – 12
	unsigned char Day; // 1 – 31
	unsigned char Hour; // 0 – 23
	unsigned char Minute; // 0 – 59
	unsigned char Second; // 0 – 59
	unsigned char Pad1;
	unsigned int Nanosecond; // 0 – 999,999,999
	unsigned short TimeZone; // -1440 to 1440 or 2047
	unsigned char Daylight;
	unsigned char Pad2;
};

struct EFI_FILE_INFO {
	unsigned long long Size;
	unsigned long long FileSize;
	unsigned long long PhysicalSize;
	struct EFI_TIME CreateTime;
	struct EFI_TIME LastAccessTime;
	struct EFI_TIME ModificationTime;
	unsigned long long Attribute;
	unsigned short FileName[];
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

struct EFI_FILE_PROTOCOL {
	unsigned long long Revision;
	unsigned long long (*Open)(struct EFI_FILE_PROTOCOL *This,
				   struct EFI_FILE_PROTOCOL **NewHandle,
				   unsigned short *FileName,
				   unsigned long long OpenMode,
				   unsigned long long Attributes);
	unsigned long long (*Close)(struct EFI_FILE_PROTOCOL *This);
	unsigned long long (*Delete)(struct EFI_FILE_PROTOCOL *This);
	unsigned long long (*Read)(struct EFI_FILE_PROTOCOL *This,
				   unsigned long long *BufferSize,
				   void *Buffer);
	unsigned long long (*Write)(struct EFI_FILE_PROTOCOL *This,
				    unsigned long long *BufferSize,
				    void *Buffer);
	unsigned long long (*GetPosition)(struct EFI_FILE_PROTOCOL *This,
					  unsigned long long *Position);
	unsigned long long (*SetPosition)(struct EFI_FILE_PROTOCOL *This,
					  unsigned long long Position);
	unsigned long long (*GetInfo)(struct EFI_FILE_PROTOCOL *This,
				      struct EFI_GUID *InformationType,
				      unsigned long long *BufferSize,
				      void *Buffer);
	unsigned long long (*SetInfo)(struct EFI_FILE_PROTOCOL *This,
				      struct EFI_GUID *InformationType,
				      unsigned long long BufferSize,
				      void *Buffer);
	unsigned long long (*Flush)(struct EFI_FILE_PROTOCOL *This);
};

struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
	unsigned long long Revision;
	unsigned long long (*OpenVolume)(
		struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *This,
		struct EFI_FILE_PROTOCOL **Root);
};

struct EFI_SYSTEM_TABLE *SystemTable;
struct EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfsp;
unsigned char img_buf[MAX_IMG_BUF];
int cursor_x = 0;

void execute_line(unsigned short *buf);

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

void blt(unsigned char img[], unsigned int img_width, unsigned int img_height)
{
	unsigned char *fb;
	unsigned int i, j, k, vr, hr, ofs = 0;

	fb = (unsigned char *)gop->Mode->FrameBufferBase;
	vr = gop->Mode->Info->VerticalResolution;
	hr = gop->Mode->Info->HorizontalResolution;

	for (i = 0; i < vr; i++) {
		if (i >= img_height)
			break;
		for (j = 0; j < hr; j++) {
			if (j >= img_width) {
				fb += (hr - img_width) * 4;
				break;
			}
			for (k = 0; k < 4; k++)
				*fb++ = img[ofs++];
		}
	}
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

static int command_ls(unsigned short *args __attribute__ ((unused)))
{
	unsigned long long status;
	unsigned long long buf_size;
	char file_buf[MAX_FILE_BUF];
	struct EFI_FILE_INFO *efi;
	struct EFI_FILE_PROTOCOL *root;

	status = sfsp->OpenVolume(sfsp, &root);
	if (status) {
		put_str(L"error: sfsp->OpenVolume\r\n");
		return 1;
	}

	while (1) {
		buf_size = MAX_FILE_BUF;
		status = root->Read(root, &buf_size, (void *)file_buf);
		if (status) {
			put_str(L"error: root->Read\r\n");
			break;
		}
		if (!buf_size)
			break;

		efi = (struct EFI_FILE_INFO *)file_buf;
		put_str(efi->FileName);
		put_str(L" ");
	}
	put_str(L"\r\n");

	status = root->Close(root);
	if (status)
		put_str(L"root->Close\r\n");

	return 0;
}

static int command_cat(unsigned short *args)
{
	unsigned long long buf_size = MAX_FILE_BUF;
	unsigned short file_buf[MAX_FILE_BUF / 2];
	unsigned long long status;
	unsigned short str[1024];
	struct EFI_FILE_PROTOCOL *root;
	struct EFI_FILE_PROTOCOL *file;

	status = sfsp->OpenVolume(sfsp, &root);
	if (status) {
		put_str(L"error: sfsp->OpenVolume\r\n");
		return 1;
	}

	status = root->Open(root, &file, args, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if (status) {
		put_str(L"error: root->Open(status:0x");
		put_str(int_to_unicode_hex(status, 16, str));
		put_str(L")\r\n");
		return 1;
	}

	status = file->Read(file, &buf_size, (void *)file_buf);
	if (status) {
		put_str(L"error: file->Read(status:0x");
		put_str(int_to_unicode_hex(status, 16, str));
		put_str(L")\r\n");
	} else
		put_str(file_buf);

	status = file->Close(file);
	if (status) {
		put_str(L"error: file->Close(status:0x");
		put_str(int_to_unicode_hex(status, 16, str));
		put_str(L")\r\n");
		put_str(L"file->Close\r\n");
	}

	status = root->Close(root);
	if (status)
		put_str(L"root->Close\r\n");

	return 0;
}

#define SKIP_SIZE	5
unsigned short img_name_buf[MAX_ARGS][MAX_FILE_LEN];
static int command_view(unsigned short *args)
{
	unsigned long long buf_size = MAX_IMG_BUF;
	unsigned long long status;
	unsigned short str[1024];
	struct EFI_FILE_PROTOCOL *root;
	struct EFI_FILE_PROTOCOL *file;
	struct EFI_INPUT_KEY efi_input_key;
	unsigned short other[MAX_FILE_LEN];
	int line_len;
	int i;
	int num_args;
	unsigned char quit = 0;

	status = sfsp->OpenVolume(sfsp, &root);
	if (status) {
		put_str(L"error: sfsp->OpenVolume\r\n");
		return 1;
	}

	for (i = 0; ; i++) {
		str_get_first_entry(args, img_name_buf[i], other);
		if (img_name_buf[i][0] == L'\0') {
			num_args = i;
			break;
		}

		line_len = str_get_len(other);
		str_copy(other, args, line_len);
	}

	i = 0;
	while (!quit) {
		status = root->Open(root, &file, img_name_buf[i], EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
		if (status) {
			put_str(L"error: root->Open(status:0x");
			put_str(int_to_unicode_hex(status, 16, str));
			put_str(L")\r\n");
			break;
		}

		status = file->Read(file, &buf_size, (void *)img_buf);
		if (status) {
			put_str(L"error: file->Read(status:0x");
			put_str(int_to_unicode_hex(status, 16, str));
			put_str(L")\r\n");
			break;
		} else
			blt(img_buf, 640, 480);

		while (SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &efi_input_key));
		switch (efi_input_key.UnicodeChar) {
		case L'j':
			if (i < num_args - 1)
				i++;
			break;
		case L'J':
			if (i < num_args - 1)
				i += SKIP_SIZE;
			if (i >= num_args)
				i = num_args - 1;
			break;
		case L'k':
			if (i > 0)
				i--;
			break;
		case L'K':
			if (i > 0)
				i -= SKIP_SIZE;
			if (i < 0)
				i = 0;
			break;
		case L'h':
			i = 0;
			break;
		case L'l':
			i = num_args - 1;
			break;
		case L'q':
			quit = 1;
			break;
		}

		status = file->Close(file);
		if (status) {
			put_str(L"error: file->Close(status:0x");
			put_str(int_to_unicode_hex(status, 16, str));
			put_str(L")\r\n");
			put_str(L"file->Close\r\n");
			break;
		}
	}

	status = root->Close(root);
	if (status)
		put_str(L"root->Close\r\n");

	SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
	cursor_x = 0;

	return 0;
}

static int command_sh(unsigned short *args)
{
	unsigned long long buf_size = MAX_FILE_BUF;
	unsigned short file_buf[MAX_FILE_BUF / 2];
	unsigned long long status;
	unsigned short str[1024];
	struct EFI_FILE_PROTOCOL *root;
	struct EFI_FILE_PROTOCOL *file;

	status = sfsp->OpenVolume(sfsp, &root);
	if (status) {
		put_str(L"error: sfsp->OpenVolume\r\n");
		return 1;
	}

	status = root->Open(root, &file, args, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if (status) {
		put_str(L"error: root->Open(status:0x");
		put_str(int_to_unicode_hex(status, 16, str));
		put_str(L")\r\n");
		return 1;
	}

	status = file->Read(file, &buf_size, (void *)file_buf);
	if (status) {
		put_str(L"error: file->Read(status:0x");
		put_str(int_to_unicode_hex(status, 16, str));
		put_str(L")\r\n");
	} else
		execute_line(file_buf);

	status = file->Close(file);
	if (status) {
		put_str(L"error: file->Close(status:0x");
		put_str(int_to_unicode_hex(status, 16, str));
		put_str(L")\r\n");
		put_str(L"file->Close\r\n");
	}

	status = root->Close(root);
	if (status)
		put_str(L"root->Close\r\n");

	return 0;
}

#ifdef DEBUG
static int command_test(unsigned short *args __attribute__ ((unused)))
{
	put_str(L"test\r\n");

	return 0;
}
static int command_kbdtest(unsigned short *args __attribute__ ((unused)))
{
	unsigned short c;
	unsigned short str[8];

	while (1) {
		c = get_char();
		put_str(int_to_unicode_hex((long long)c, 4, str));
		put_str(L"\r\n");
	}

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

	if (!str_compare(command, L"ls")) {
		return LS;
	}

	if (!str_compare(command, L"cat")) {
		return CAT;
	}

	if (!str_compare(command, L"view")) {
		return VIEW;
	}

	if (!str_compare(command, L"sh")) {
		return SH;
	}

#ifdef DEBUG
	if (!str_compare(command, L"test")) {
		return TEST;
	}
	if (!str_compare(command, L"kbdtest")) {
		return KBDTEST;
	}
#endif /* DEBUG */

	return COMMAND_NUM;
}

void execute_line(unsigned short *buf)
{
	unsigned short command[256], args[256];
	unsigned char command_id;

	str_get_first_entry(buf, command, args);
	command_id = get_command_id(command);

	switch (command_id) {
	case ECHO:
		command_echo(args);
		break;
	case SHOWHWPARAM:
		command_showhwparam(args);
		break;
	case LS:
		command_ls(args);
		break;
	case CAT:
		command_cat(args);
		break;
	case VIEW:
		command_view(args);
		break;
	case SH:
		command_sh(args);
		break;
#ifdef DEBUG
	case TEST:
		command_test(args);
		break;
	case KBDTEST:
		command_kbdtest(args);
		break;
#endif /* DEBUG */
	default:
		put_str(L"Command not found.\r\n");
		break;
	}
}

void shell(void)
{
	unsigned short buf[MAX_LINE_SIZE];

	while (1) {
		put_str(L"OS6> ");
		if (get_line(buf, MAX_LINE_SIZE) <= 0) {
			continue;
		}

		execute_line(buf);
	}
}

void efi_main(void *ImageHandle __attribute__ ((unused)), struct EFI_SYSTEM_TABLE *_SystemTable)
{
	struct EFI_GUID gop_guid = {0x9042a9de, 0x23dc, 0x4a38, {0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a}};
	struct EFI_GUID sfsp_guid = {0x0964e5b22, 0x6459,0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};

	SystemTable = _SystemTable;

	SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);

	SystemTable->BootServices->LocateProtocol(&gop_guid, NULL, (void **)&gop);
	SystemTable->BootServices->LocateProtocol(&sfsp_guid, NULL, (void **)&sfsp);

	shell();
}
