#include <common.h>
#include <efi.h>

#define MAX_LINE_SIZE	512
#define MAX_FILE_BUF	1024
#define MAX_IMG_BUF	4194304	/* 4MB */
#define MAX_ARGS	100
#define MAX_FILE_LEN	32

#define UNICODE_BS	0x0008

enum {
	ECHO,
	SHOWHWPARAM,
	LS,
	CAT,
	VIEW,
	SH,
#ifdef DEBUG
	TEST,
	MULTITEST,
	KBDTEST,
#endif /* DEBUG */
	COMMAND_NUM
} _COMMAND_SET;

struct EFI_SYSTEM_TABLE *SystemTable;
struct EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfsp;
struct EFI_MP_SERVICES_PROTOCOL *msp;
unsigned char img_buf[MAX_IMG_BUF];
int cursor_x = 0;

void execute_line(unsigned short *buf);

static void put_char(unsigned short c)
{
	unsigned short str[2] = L" ";

	str[0] = c;
	SystemTable->ConOut->OutputString(SystemTable->ConOut, str);
}

volatile unsigned char lock_conout = 0;
static void put_str(unsigned short *str)
{
	while (lock_conout);
	lock_conout = 1;
	SystemTable->ConOut->OutputString(SystemTable->ConOut, str);
	lock_conout = 0;
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

void ap_main(void *_SystemTable)
{
	unsigned short str[1024];
	struct EFI_SYSTEM_TABLE *SystemTable = _SystemTable;

	struct EFI_GUID msp_guid = {0x3fdda605, 0xa76e, 0x4f46, {0xad, 0x29, 0x12, 0xf4, 0x53, 0x1b, 0x3d, 0x08}};
	struct EFI_MP_SERVICES_PROTOCOL *msp;
	unsigned long long status;
	status = SystemTable->BootServices->LocateProtocol(&msp_guid, NULL, (void **)&msp);
	if (status) {
		put_str(L"error: SystemTable->BootServices->LocateProtocol\r\n");
		while (1);
	}
	unsigned long long pnum;
	status = msp->WhoAmI(msp, &pnum);
	if (status) {
		put_str(L"error: msp->WhoAmI\r\n");
		while (1);
	}

	put_str(int_to_unicode(pnum, 1, str));
	put_str(L"\r\n");
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
static int command_multitest(unsigned short *args __attribute__ ((unused)))
{
	unsigned long long status;

	status = msp->StartupAllAPs(msp, ap_main, 0, NULL, 0, SystemTable, NULL);
	if (status) {
		put_str(L"error: msp->StartupAllAPs\r\n");
		while (1);
	}

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
	if (!str_compare(command, L"multitest")) {
		return MULTITEST;
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
	case MULTITEST:
		command_multitest(args);
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
	struct EFI_GUID msp_guid = {0x3fdda605, 0xa76e, 0x4f46, {0xad, 0x29, 0x12, 0xf4, 0x53, 0x1b, 0x3d, 0x08}};

	SystemTable = _SystemTable;

	SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);

	SystemTable->BootServices->LocateProtocol(&gop_guid, NULL, (void **)&gop);
	SystemTable->BootServices->LocateProtocol(&sfsp_guid, NULL, (void **)&sfsp);
	SystemTable->BootServices->LocateProtocol(&msp_guid, NULL, (void **)&msp);

	shell();
}
