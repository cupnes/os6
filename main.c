#include <common.h>
#include <efi.h>
#include <console.h>
#include <shell.h>

struct EFI_SYSTEM_TABLE *SystemTable;
struct EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfsp;
struct EFI_MP_SERVICES_PROTOCOL *msp;
unsigned char img_buf[MAX_IMG_BUF];

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
