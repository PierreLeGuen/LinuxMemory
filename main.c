#include <stdio.h>
#include <stdlib.h>

#include "./LinuxMemory.h"

int main()
{
    char buffer[128];
    printf("---- STARTED ----\n");
    LinuxProc_t cpp_mem_edit;
    /* Fill structure */
    cpp_mem_edit = LinuxProcFromID((pid_t) GetPIDbyName("cpp_mem_edit",0,0));
    // cpp_mem_edit = LinuxProcFromID((pid_t) 1513);
    printf("Process ID : %i\n", cpp_mem_edit.ProcId);
    void* address =(void *) 0x7FFC2A6393D4; //TEST
    attach(cpp_mem_edit);
    read_int(cpp_mem_edit, 0, address, 0);
    // read_string(cpp_mem_edit, 4, 0x7FFC9EADC230, 0);
    detach(cpp_mem_edit);

    return 0;
}