#include <stdio.h>
#include <stdlib.h>

#include "./LinuxMemory.h"

int main()
{
    char buffer[128];
    int value = 123456;
    printf("---- STARTED ----\n");
    LinuxProc_t process;
    /* Fill structure */
    process = LinuxProcFromID((pid_t) getPidByName("dummyMemProgram"));
    printf("Process ID : %i\n", process.ProcId);
    void* address =(void *) 0x7FFD57C7FFE0; //TEST
    attach(process);
    Read(process, address, buffer, 32);
    for (int i = 0; i < 32 ; ++i) {
        printf("buf[%d] = %c\n", i, buffer[i]);
    }
    detach(process);

    return 0;
}