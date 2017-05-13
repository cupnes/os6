#include <shell.h>
#include <common.h>
#include <efi.h>
#include <console.h>

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
