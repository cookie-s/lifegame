#include "Uefi.h"
#include "Protocol/GraphicsOutput.h"

#include "common.h"
#include "debug.h"

const int WIDTH = 100;
const int HEIGHT= 100;

EFI_SYSTEM_TABLE *gST;
EFI_BOOT_SERVICES *gBS;

EFI_STATUS EFIAPI EfiMain (
        IN EFI_HANDLE ImageHandle,
        IN EFI_SYSTEM_TABLE *SystemTable
        )
{
    gST = SystemTable;
    gBS = gST->BootServices;

    gST->ConOut->OutputString(gST->ConOut, L"Hello.\r\n\r\n");

    EFI_GUID graphics_proto_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

    EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphOut;
    gBS->LocateProtocol(&graphics_proto_guid, NULL, (VOID**)&GraphOut);

    if(!GraphOut) {
        gST->ConOut->OutputString(gST->ConOut, L"EFI_GRAPHICS_OUTPUT_PROTOCOL not found.\r\n\r\n");
        while(1);
    }

    print_uint(GraphOut->Mode->Mode);
    puts("");
    print_uint(GraphOut->Mode->MaxMode);
    puts("");

    const UINT32 HR = GraphOut->Mode->Info->HorizontalResolution;
    const UINT32 VR = GraphOut->Mode->Info->VerticalResolution;
    const UINT32 PSL = GraphOut->Mode->Info->PixelsPerScanLine;

    char *map, *cnt;
    gBS->AllocatePool(AllocateAnyPages, (HR + 2) * (VR + 2), (VOID**)&map);
    gBS->AllocatePool(AllocateAnyPages, (HR + 2) * (VR + 2), (VOID**)&cnt);
    if(!map || !cnt) {
        gST->ConOut->OutputString(gST->ConOut, L"Allocate failed...\r\n\r\n");
        while(1);
    }

    const char donguri[3][7] = {
        {0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0},
        {1, 1, 0, 0, 1, 1, 1}
    };

    for(int i=0; i<HR+2; i++)
        for(int j=0; j<VR+2; j++)
            map[j*HR + i] = 0;

    for(int i=0; i<7; i++)
        for(int j=0; j<3; j++)
            map[(500 + j) * HR + 500 + i] = donguri[j][i];

    while(1) {
        for(int i=0; i<HR+2; i++)
            for(int j=0; j<VR+2; j++)
                cnt[j*HR + i] = 0;

        for(int i=1; i<=HR; i++)
            for(int j=1; j<=VR; j++)
                if(map[j*HR + i]) {
                    cnt[(j-1)*HR + i-1]++;
                    cnt[(j-1)*HR + i  ]++;
                    cnt[(j-1)*HR + i+1]++;
                    cnt[(j  )*HR + i-1]++;
                    cnt[(j  )*HR + i+1]++;
                    cnt[(j+1)*HR + i-1]++;
                    cnt[(j+1)*HR + i  ]++;
                    cnt[(j+1)*HR + i+1]++;
                }

        for(int i=1; i<=HR; i++)
            for(int j=1; j<=VR; j++)
                if(map[j*HR + i]) {
                    if(cnt[j*HR + i] <= 1 || cnt[j*HR + i] >= 4)
                        map[j*HR + i] = 0;
                } else {
                    if(cnt[j*HR + i] == 3)
                        map[j*HR + i] = 1;
                }

        for(int i=1; i<=HR; i++)
            for(int j=1; j<=VR; j++)
                ((UINT32*)GraphOut->Mode->FrameBufferBase)[j * PSL + i] = map[j*HR + i] ? 0xFFFFFF : 0;

    }
    while(1);
    return EFI_SUCCESS;
}

void list_modes(EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphOut) {
    for(int mode=0; mode<GraphOut->Mode->MaxMode*0; mode++) {
        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
        UINTN info_sz;
        print_uint(GraphOut->QueryMode(GraphOut, mode, &info_sz, &info));
        print_uint(mode);
        print(": ");
        print_uint((unsigned int)info->HorizontalResolution);
        print(", ");
        print_uint((unsigned int)info->VerticalResolution);
        print(", " );
        print_uint((unsigned int)info->PixelFormat);
        print(", " );
        print_uint(info->PixelInformation.RedMask);
        print(" " );
        print_uint(info->PixelInformation.GreenMask);
        print(" " );
        print_uint(info->PixelInformation.BlueMask);
        print(", " );
        print_uint((unsigned int)info->PixelsPerScanLine);
        puts("");
    }
}
