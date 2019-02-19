#include <stdio.h>
#include <stdlib.h>

#include "./LinuxMemory.h"

int main()
{
    char buffer[128];
    int value = 321;
    printf("---- STARTED ----\n");
    LinuxProc_t cpp_mem_edit;
    /* Fill structure */
    // cpp_mem_edit = LinuxProcFromID((pid_t) GetPIDbyName("cpp_mem_edit",0,0));
    // cpp_mem_edit = LinuxProcFromID((pid_t) 1497);
    cpp_mem_edit = LinuxProcFromID((pid_t) getPidByNameTEST("cpp_mem_edit"));
    printf("Process ID : %i\n", cpp_mem_edit.ProcId);
    // void* address =(void *) 0x7FFF8850A784; //TEST
    attach(cpp_mem_edit);
    read_int(cpp_mem_edit, 0,(void *) 0x7FFE6B9F41F4, 0);
    // read_char(cpp_mem_edit, 5, (void *) 0x7FFE6B9F4230, 0);
    write_int(cpp_mem_edit, 0, (void *) 0x7FFE6B9F41F4, (void *) 321);
    read_int(cpp_mem_edit, 0,(void *) 0x7FFE6B9F41F4, 0);
    detach(cpp_mem_edit);

    return 0;
}