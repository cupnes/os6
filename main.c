#include <common.h>
#include <efi.h>
#include <console.h>
#include <shell.h>

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
	efi_init(_SystemTable);
	shell();
}
