#include <stdio.h>
#include <stdlib.h>

#include "./LinuxMemory.h"

int main()
{
    int recv_buf[128] = {0};
    int send_buf[1] = {0};
    send_buf[0] = 73351;

    printf("---- STARTED ----\n");
    LinuxProc_t process;
    /* Fill structure */
    process = LinuxProcFromID((pid_t) getPidByName("dummyMemProgram"));
    printf("Process ID : %i\n", process.ProcId);
    void* address =(void *) 0x7FFD1E363EA4; // address of var
    attach(process);
    Write(process, address, send_buf, sizeof(send_buf));
    detach(process);

    return 0;
}