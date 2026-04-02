// Example.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <DMALibrary/Memory/Memory.h>

int main()
{
    std::cout << "[DMALibrary] Starting Assault Cube test...\n\n";

    // === 1. Initialize DMA (targeting Assault Cube) ===
    if (!mem.Init("ac_client.exe", true, true))   // true = use memory map, true = debug output
    {
        std::cout << "[!] Failed to initialize DMA\n"
                  << "    → Is Assault Cube running?\n"
                  << "    → Is your Cap75T connected and flashed correctly?\n"
                  << "    → Check that vmm.dll, leechcore.dll, FTD3XX.dll are in the executable folder.\n";
        system("pause");
        return 1;
    }
    std::cout << "[+] DMA initialized successfully!\n\n";

    // === 2. Initialize kernel keyboard (optional but very useful) ===
    if (!mem.GetKeyboard()->InitKeyboard())
    {
        std::cout << "[!] Keyboard init failed (winlogon signature issue possible on newer Windows)\n"
                  << "    → We'll improve this later. Continuing without hotkeys...\n\n";
    }
    else
    {
        std::cout << "[+] Kernel keyboard ready! (press A in Assault Cube to continue)\n\n";
    }

    // === 3. Get base address ===
    uintptr_t base = mem.GetBaseDaddy("ac_client.exe");
    if (!base)
    {
        std::cout << "[!] Could not find ac_client.exe base address!\n";
        system("pause");
        return 1;
    }
    std::cout << "[+] Assault Cube base address: 0x" << std::hex << base << std::dec << "\n\n";

    // === 4. Simple read/write test (educational) ===
    int testValue = mem.Read<int>(base + 0x10);   // harmless small offset for testing
    std::cout << "[+] Test read (base + 0x10): " << testValue << "\n";

    mem.Write<int>(base + 0x10, 1337);
    std::cout << "[+] Wrote 1337 to (base + 0x10)\n";
    std::cout << "[+] New read value: " << mem.Read<int>(base + 0x10) << "\n\n";

    // === 5. Wait for keyboard input (demonstrates InputManager) ===
    if (mem.GetKeyboard()->InitKeyboard())  // only wait if keyboard worked
    {
        std::cout << "Press 'A' in Assault Cube to continue...\n";
        while (!mem.GetKeyboard()->IsKeyDown(0x41))  // VK_A
        {
            Sleep(50);
        }
        std::cout << "[+] 'A' key detected via kernel!\n\n";
    }

    // === 6. Scatter read/write example (very important for performance) ===
    std::cout << "[+] Testing scatter read/write...\n";
    auto handle = mem.CreateScatterHandle();

    int scatterValue = 0;
    mem.AddScatterReadRequest(handle, base + 0x10, &scatterValue, sizeof(scatterValue));
    mem.ExecuteReadScatter(handle);
    std::cout << "[+] Scatter read: " << scatterValue << "\n";

    scatterValue = 9999;
    mem.AddScatterWriteRequest(handle, base + 0x10, &scatterValue, sizeof(scatterValue));
    mem.ExecuteWriteScatter(handle);

    mem.CloseScatterHandle(handle);   // ALWAYS clean up!
    std::cout << "[+] Scatter operations complete.\n\n";

    std::cout << "=== All basic DMA features tested successfully! ===\n";
    std::cout << "Next step: We'll build the ImGui dashboard.\n";
    system("pause");
    return 0;
}