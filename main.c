#include <windows.h>
#include <stdio.h>

#pragma pack(push, 1) 
typedef struct
{
	BYTE BS_jmpBoot[3];				// 0x00
	BYTE BS_OEMName[8];				// 0x03
	WORD BPB_BytesPerSec;			// 0x0B
	BYTE BPB_SectorsPerCluster;		// 0x0D
	WORD BPB_ReservedSectors;		// 0x0E
	BYTE BPB_NumOfCopiesFAT;		// 0x10
	WORD BPB_MaxRootDirEntries;		// 0x11
	WORD BPB_NumOfSectors32MB;		// 0x13
	BYTE BPB_MediaDescriptor;	 	// 0x15
	WORD BPB_SectorsPerFAT;			// 0x16
	WORD BPB_SectorsPerTrack;		// 0x18
	WORD BPB_NumOfHeads;			// 0x1A
	DWORD BPB_NumOfHiddenSectors;	// 0x1C
	DWORD BPB_NumOfSectors;			// 0x20
	DWORD BPB_FATSz32;				// 0x24
	WORD BPB_ExtFlags;				// 0x28
	WORD BPB_FSVer;					// 0x2A
	DWORD BPB_RootClus;				// 0x2C
	WORD BPB_FSInfo;				// 0x30
	WORD BPB_BkBootSec;				// 0x32
	BYTE BPB_Reserved[12];			// 0x34
	BYTE BS_DrvNum;					// 0x40
	BYTE BS_Reserved1;				// 0x41
	BYTE BS_BootSig;				// 0x42
	DWORD BS_VolID;					// 0x43
	BYTE BS_VolLab[11];				// 0x47
	BYTE BS_FilSysType;				// 0x52		
}FAT32_BOOTSECTOR, *PFAT32_BOOTSECTOR;
#pragma pack(pop)

int ReadSector(char* drive, int readPoint, BYTE sector[512])
{
    int retCode = 0;
    DWORD bytesRead;
    HANDLE device = NULL;

    device = CreateFile(drive,    // Drive to open
        GENERIC_READ,           // Access mode
        FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
        NULL,                   // Security Descriptor
        OPEN_EXISTING,          // How to create
        0,                      // File attributes
        NULL);                  // Handle to template

    if (device == INVALID_HANDLE_VALUE) // Open Error
    {
        printf("CreateFile: %u\n", GetLastError());
        return 1;
    }

    SetFilePointer(device, readPoint, NULL, FILE_BEGIN);//Set a Point to Read

    if (!ReadFile(device, sector, 512, &bytesRead, NULL))
    {
        printf("ReadFile: %u\n", GetLastError());
    }
    else
    {
        printf("\n");
        printf("Success!\n");
    }
}


void printName(BYTE BS_OEMName[8]) {
    printf("Name: ");
    for (int i = 0; i< 9;i++) {
        printf("%c", BS_OEMName[i]);
    }
    printf("\n");
}

int main(int argc, char ** argv)
{
    BYTE sector[512];
    
    ReadSector("\\\\.\\E:",0, sector);
    int i;
    for (i = 0; i < 512; i++)
    {
        if (i > 0) printf(":");
        printf("%2X", sector[i]);
    }
    PFAT32_BOOTSECTOR bs = (PFAT32_BOOTSECTOR)sector;

	printf("Jump instruction to boot code: %X %X %X\n", bs->BS_jmpBoot[0], bs->BS_jmpBoot[1], bs->BS_jmpBoot[2]);
	// make sure string is zero-terminated
	// printf("Name: %c%c%c\n", (char)bs->BS_OEMName[0], bs->BS_OEMName[1], bs->BS_OEMName[2]);
	printName(bs->BS_OEMName);

    //Common FAT Info
    printf("Bytes Per Sector: 0x%X (%d)\n", (WORD)bs->BPB_BytesPerSec, bs->BPB_BytesPerSec);
	printf("Sectors Per Cluster: 0x%X (%d)\n", bs->BPB_SectorsPerCluster, bs->BPB_SectorsPerCluster);
	printf("# FAT Data Structures: 0x%X (%d)\n", bs->BPB_NumOfCopiesFAT, bs->BPB_NumOfCopiesFAT);
	printf("# 32-byte directory entries: 0x%X (%d)\n", bs->BPB_MaxRootDirEntries, bs->BPB_MaxRootDirEntries);
	printf("# Total sectors 16-Bit : 0x%X (%d)\n", bs->BPB_NumOfSectors32MB, bs->BPB_NumOfSectors32MB);
	printf("Media Descriptor: 0x%X (%d)\n", bs->BPB_MediaDescriptor, bs->BPB_MediaDescriptor);
	printf("# Sectors occupied by 1 FAT: 0x%X (%d)\n", bs->BPB_SectorsPerFAT, bs->BPB_SectorsPerFAT);
	printf("# Sectors per Track (int 0x13): 0x%X (%d)\n", bs->BPB_SectorsPerTrack, bs->BPB_SectorsPerTrack);
	printf("# Heads (int 0x13): 0x%X (%d)\n", bs->BPB_NumOfHeads, bs->BPB_NumOfHeads);
	printf("# Hidden Sectors: 0x%X (%d)\n", bs->BPB_NumOfHiddenSectors, bs->BPB_NumOfHiddenSectors);
	printf("# Sectors: 0x%X\n", bs->BPB_NumOfSectors, bs->BPB_NumOfSectors);


    //FAT32 Info
    printf("Int0x13 Drive Number: 0x%X\n", bs->BS_DrvNum, bs->BS_DrvNum);
	printf("Extended Boot Signature: 0x%X\n", bs->BS_BootSig, bs->BS_BootSig);
	printf("Volume Serial Number: 0x%X\n", bs->BS_VolID, bs->BS_VolID);

    printf("# Sectors occupied by 1 FAT (32Bit): 0x%X (%d)\n", bs->BPB_FATSz32, bs->BPB_FATSz32);
	printf("Extended Flags: 0x%X (%d)\n", bs->BPB_ExtFlags, bs->BPB_ExtFlags);
	printf("FS Version: 0x%X \n", bs->BPB_FSVer);
	printf("Root Cluster: 0x%X \n", bs->BPB_RootClus);
	printf("FS Info structure: 0x%X \n", bs->BPB_FSInfo);
	printf("Sector number of boot record copy: 0x%X (%d) \n", bs->BPB_BkBootSec, bs->BPB_BkBootSec);
	// from here on same as FAT12/16, but remember that the values are stored at a different position
	// inside FAT32 structure
	printf("Int0x13 Drive Number: 0x%X\n", bs->BS_DrvNum, bs->BS_DrvNum);
	printf("Extended Boot Signature: 0x%X\n", bs->BS_BootSig, bs->BS_BootSig);
	printf("Volume Serial Number: 0x%X\n", bs->BS_VolID, bs->BS_VolID);
    return 0;
}