/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that apps can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_KMDFDriver1,
    0x5da3b817,0x4a53,0x4fc8,0xaf,0x8a,0x10,0xcd,0x00,0xfd,0x61,0xba);
// {5da3b817-4a53-4fc8-af8a-10cd00fd61ba}
