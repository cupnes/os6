#ifndef _EFI_H_
#define _EFI_H_

#define MAX_IMG_BUF	4194304	/* 4MB */

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

#define EVT_TIMER	0x80000000
#define EVT_RUNTIME	0x40000000
#define EVT_NOTIFY_WAIT	0x00000100
#define EVT_NOTIFY_SIGNAL	0x00000200
#define EVT_SIGNAL_EXIT_BOOT_SERVICES	0x00000201
#define EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE	0x60000202

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

enum EFI_TIMER_DELAY {
	TimerCancel,
	TimerPeriodic,
	TimerRelative
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
		unsigned long long (*CreateEvent)(
			unsigned int Type,
			unsigned long long NotifyTpl,
			void (*NotifyFunction)(void *Event, void *Context),
			void *NotifyContext,
			void *Event);
		unsigned long long (*SetTimer)(void *Event,
					       enum EFI_TIMER_DELAY Type,
					       unsigned long long TriggerTime);
		unsigned long long (*WaitForEvent)(
			unsigned long long NumberOfEvents,
			void *Event,
			unsigned long long *Index);
		unsigned long long _buf4[3];

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

struct EFI_CPU_PHYSICAL_LOCATION {
	unsigned int Package;
	unsigned int Core;
	unsigned int Thread;
};

struct EFI_PROCESSOR_INFORMATION {
	unsigned long long ProcessorId;
	unsigned int StatusFlag;
	struct EFI_CPU_PHYSICAL_LOCATION Location;
};

struct EFI_MP_SERVICES_PROTOCOL {
	unsigned long long (*GetNumberOfProcessors)(
		struct EFI_MP_SERVICES_PROTOCOL *This,
		unsigned long long *NumberOfProcessors,
		unsigned long long *NumberOfEnabledProcessors);
	unsigned long long (*GetProcessorInfo)(
		struct EFI_MP_SERVICES_PROTOCOL *This,
		unsigned long long ProcessorNumber,
		struct EFI_PROCESSOR_INFORMATION *ProcessorInfoBuffer);
	unsigned long long (*StartupAllAPs)(
		struct EFI_MP_SERVICES_PROTOCOL *This,
		void (*Procedure)(void *ProcedureArgument),
		unsigned char SingleThread,
		void *WaitEvent,
		unsigned long long TimeoutInMicroSeconds,
		void *ProcedureArgument,
		unsigned long long **FailedCpuList);
	unsigned long long (*StartupThisAP)(
		struct EFI_MP_SERVICES_PROTOCOL *This,
		void (*Procedure)(void *ProcedureArgument),
		unsigned long long ProcessorNumber,
		void *WaitEvent,
		unsigned long long TimeoutInMicroseconds,
		void *ProcedureArgument,
		unsigned char *Finished);
	unsigned long long (*SwitchBSP)(
		struct EFI_MP_SERVICES_PROTOCOL *This,
		unsigned long long ProcessorNumber,
		unsigned char EnableOldBSP);
	unsigned long long (*EnableDisableAP)(
		struct EFI_MP_SERVICES_PROTOCOL *This,
		unsigned long long ProcessorNumber,
		unsigned char EnableAP,
		unsigned int *HealthFlag);
	unsigned long long (*WhoAmI)(
		struct EFI_MP_SERVICES_PROTOCOL *This,
		unsigned long long *ProcessorNumber);
};

extern struct EFI_SYSTEM_TABLE *SystemTable;
extern struct EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
extern struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfsp;
extern struct EFI_MP_SERVICES_PROTOCOL *msp;
extern unsigned char img_buf[MAX_IMG_BUF];

void efi_init(struct EFI_SYSTEM_TABLE *_SystemTable);
void blt(unsigned char img[], unsigned int img_width, unsigned int img_height);

#endif /* _EFI_H_ */
