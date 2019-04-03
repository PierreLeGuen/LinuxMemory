#include <iostream>
#include <vector>
#include "LinuxMemory.h"

/*
 * Short example of how to use LinuxMemory.
 * 
 * Game : Assault Cube
 * OS : Linux, ARCH64
 */

uintptr_t FindDMAAddy(LinuxProc_t process, uintptr_t ptr, std::vector<unsigned int> offsets);

int main() {
    uintptr_t offsetPlayerBase = 0x00143218; //offset 2019.03.04
    int newAmmo = 1337;

    LinuxProc_t AssaultCube; // the handle
    AssaultCube = fillProcessStructbyName("ac_client"); //Fills the process struct
    std::cout<<"PID of Proc : " << AssaultCube.ProcessID << std::endl;
    uintptr_t PlayerBase = (uintptr_t) AssaultCube.ProcessBaseAddress + (uintptr_t)offsetPlayerBase;

    uintptr_t addressAmmo = FindDMAAddy(AssaultCube, PlayerBase, {0x168});    //Resolving the pointer chain
    WriteProcessMemory(AssaultCube,(void*) addressAmmo, &newAmmo, 4, nullptr); //Write the new value
    return 0;
}

uintptr_t FindDMAAddy(LinuxProc_t process, uintptr_t ptr, std::vector<unsigned int> offsets) {
    uintptr_t addr = ptr;
    for (unsigned int offset : offsets) {
        ReadProcessMemory(process,(void*) addr, &addr, sizeof(addr), nullptr);
        addr += offset;
    }
    return addr;
}
