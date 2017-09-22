#include "Uefi.h"
#include "Guid/FileInfo.h"
#include "Library/FileHandleLib.h"

#include "common.h"

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
    EFI_GUID simpl_fs_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

    EFI_HANDLE *handles;
    UINTN no_handles;
    gBS->LocateHandleBuffer(ByProtocol, &simpl_fs_guid, NULL, &no_handles, &handles);

    for(int i=0; i<no_handles; i++) {
        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *simpl_fs_proto;
        gBS->OpenProtocol(handles[i], &simpl_fs_guid, (VOID**)&simpl_fs_proto, ImageHandle, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

        if(!simpl_fs_proto) {
            gST->ConOut->OutputString(gST->ConOut, L"EFI_SIMPLE_FILE_SYSTEM_PROTOCOL not found.\r\n\r\n");
            continue;
        }

        EFI_FILE_PROTOCOL *root;
        simpl_fs_proto->OpenVolume(simpl_fs_proto, &root);

        if(!root) {
            gST->ConOut->OutputString(gST->ConOut, L"Couldn't OpenVolume.\r\n\r\n");
            continue;
        }

        gST->ConOut->OutputString(gST->ConOut, L"/\r\n");
        gST->ConOut->OutputString(gST->ConOut, L"\r\n");
        root->Close(root);
        gBS->CloseProtocol(handles[i], &simpl_fs_guid, ImageHandle, NULL);
    }
    gBS->FreePool(handles);

    while(1);
    return EFI_SUCCESS;
}
