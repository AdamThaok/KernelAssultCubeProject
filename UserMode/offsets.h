#ifndef OFFSETS_H
#define OFFSETS_H

namespace off {
    constexpr uintptr_t ModuleBase = 0x400000;
    constexpr const wchar_t* ProcessName = L"ac_client.exe";

                 // FOV [ac_client.exe + 0x18A7CC]

    // Base Addresses
    constexpr uintptr_t LocalPlayer = 0x0017E0A8;   // LocalPlayer [ac_client.exe + 0x0017E0A8]
    constexpr uintptr_t EntityList = 0x18AC04;      // Entity List [ac_client.exe + 0x18AC04]
    constexpr uintptr_t FOV = 0x18A7CC;             // FOV [ac_client.exe + 0x18A7CC]
    constexpr uintptr_t PlayerCount = 0x18AC0C;     // PlayerCount [ac_client.exe + 0x18AC0C]
    constexpr uintptr_t vMatrix = 0x17DFD0;         // View Matrix [ac_client.exe + 0x17DFD0]
    constexpr const wchar_t* WindowName = L"AssaultCube";         // View Matrix [ac_client.exe + 0x17DFD0]





    constexpr uintptr_t PlayerStride = 0x4;      // Distance between players in the list
    constexpr uintptr_t  PositionOffset = 0x4;  // Offset for X position in each player
    constexpr uintptr_t  Align = 0x4;  // Offset for X position in each player









    // Position Offsets
    constexpr uintptr_t PositionX = 0x2C;           // Position X
    constexpr uintptr_t PositionY = 0x30;           // Position Y
    constexpr uintptr_t PositionZ = 0x28;           // Position Z

    // Head Position Offsets
    constexpr uintptr_t HeadPositionX = 0x4;        // Head Position X
    constexpr uintptr_t HeadPositionY = 0xC;        // Head Position Y
    constexpr uintptr_t HeadPositionZ = 0x8;        // Head Position Z

    // Player Camera Offsets
    constexpr uintptr_t PlayerCameraX = 0x34;       // Player Camera X
    constexpr uintptr_t PlayerCameraY = 0x38;       // Player Camera Y

    // Ammo Offsets
    constexpr uintptr_t AssaultRifleAmmo = 0x140;   // Assault Rifle Ammo
    constexpr uintptr_t SMGAmmo = 0x138;            // Submachine Gun Ammo
    constexpr uintptr_t SniperAmmo = 0x13C;         // Sniper Ammo
    constexpr uintptr_t ShotgunAmmo = 0x134;        // Shotgun Ammo
    constexpr uintptr_t PistolAmmo = 0x12C;         // Pistol Ammo
    constexpr uintptr_t GrenadeAmmo = 0x144;        // Grenade Ammo

    // Fast Fire Offsets
    constexpr uintptr_t FastFireAR = 0x164;         // Fast fire Assault Rifle
    constexpr uintptr_t FastFireSniper = 0x160;     // Fast fire Sniper
    constexpr uintptr_t FastFireShotgun = 0x158;    // Fast fire Shotgun

    // Misc Offsets
    constexpr uintptr_t AutoShoot = 0x204;          // Auto shoot
    constexpr uintptr_t HealthValue = 0xEC;         // Health Value
    constexpr uintptr_t ArmorValue = 0xF0;          // Armor Value
    constexpr uintptr_t PlayerName = 0x205;         // Player Name

}

#endif // OFFSETS_H
