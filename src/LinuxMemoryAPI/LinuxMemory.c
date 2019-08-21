#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

#include <sys/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include <fcntl.h>

#include "LinuxMemory.h"

int IsNumeric(const char *characterList) {
    for (; *characterList; characterList++)
        if (*characterList < '0' || *characterList > '9')
            return 0;
    return 1;
}

pid_t getPidByName(const char *processName) {
    DIR *dir;
    dir = opendir("/proc");
    if (dir == NULL) {
        perror("Couldn't open the PROC directory");
        return -2;
    }
    if (NULL != dir) {
        struct dirent *ptr;
        int ipid = -1;
        while ((ptr = readdir(dir)) != NULL) {
            // Skip non numeric entries
            if (ptr->d_type == DT_DIR) {
                if (IsNumeric(ptr->d_name)) {
                    FILE *fp;
                    char filepath[64];
                    //Skip "." and ".." files
                    if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
                        continue;
                    if (DT_DIR != ptr->d_type)
                        continue;
                    sprintf(filepath, "/proc/%s/status", ptr->d_name);//parse status file
                    fp = fopen(filepath, "r");//open in read mode
                    if (NULL != fp) {
                        char cur_task_name[64];
                        char buf[64];
                        if (fgets(buf, sizeof(buf) - 1, fp) == NULL) {
                            fclose(fp);
                            continue;
                        }
                        sscanf(buf, "%*s %s", cur_task_name);

                        //compare task_name and cur_task_name)
                        if (!strcmp(processName, cur_task_name)) {
                            ipid = (int) strtol(ptr->d_name, (char **) NULL, 10);
                            fclose(fp);
                            closedir(dir);//close everything
                            return ipid;
                        }
                        fclose(fp);
                    }
                }
            }

        }
        closedir(dir);//close the dir
        return ipid;
    }
}

void *getModuleBaseAddress(pid_t processID, const char *processName) {
    void *vProcBaseAddress = NULL;      //ptr that will store the ModuleBaseAddres
    char *stringToBase = malloc(12);    //Temp var (char* to void*)
    char mapsFilePath[256];

    FILE *mapsFile;
    char *currentLine = NULL;
    size_t lengthLine = 0;
    sprintf(mapsFilePath, "/proc/%d/maps", processID);
    mapsFile = fopen(mapsFilePath, "r");

    if (mapsFile == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    /*
     * Reads each line of the maps file until processName found
     */
    while ((getline(&currentLine, &lengthLine, mapsFile)) != -1) {
        if (strstr(currentLine, processName)) {
            memcpy(stringToBase, &currentLine[0],
                   12);  //Base address is between [0] and [11] in string, and base 0x (hex)
            vProcBaseAddress = (void *) strtol(stringToBase, (char **) NULL, 16);   //fill the ptr
            break;
        }
    }

    free(currentLine);
    free(stringToBase);
    fclose(mapsFile);
    return vProcBaseAddress;
}

void ReadProcessMemory(LinuxProc_t lpProcess, void *vpBaseAddress, void *vpBuffer, size_t nSize,
                       size_t *lpNumberOfBytesRead) {
    struct iovec iovLocalAddressSpace[1];
    struct iovec iovRemoteAddressSpace[1];
    iovLocalAddressSpace[0].iov_base = vpBuffer; //Store data in this buffer
    iovLocalAddressSpace[0].iov_len = nSize; //which has this size.

    iovRemoteAddressSpace[0].iov_base = vpBuffer; //The data comes from here
    iovRemoteAddressSpace[0].iov_len = nSize; //and has this size.

    ssize_t sSize = process_vm_readv(lpProcess.ProcessID, //Remote process id
                                     iovLocalAddressSpace,  //Local iovec array
                                     1, //Size of the local iovec array
                                     iovRemoteAddressSpace,  //Remote iovec array
                                     1, //Size of the remote iovec array
                                     0); //Flags, unused
    if (sSize < 0) {
        printf("READ %d -> ", lpProcess.ProcessID);
        switch (errno) {
            case EINVAL:
                printf("ERROR: INVALID ARGUMEnNTS.\n");
                break;
            case EFAULT:
                printf("ERROR: UNABLE TO ACCESS TARGET MEMORY ADDRESS.\n");
                break;
            case ENOMEM:
                printf("ERROR: UNABLE TO ALLOCATE MEMORY.\n");
                break;
            case EPERM:
                printf("ERROR: INSUFFICIENT PRIVILEGES TO TARGET PROCESS.\n");
                break;
            case ESRCH:
                printf("ERROR: PROCESS DOES NOT EXIST.\n");
                break;
            default:
                printf("ERROR: AN UNKNOWN ERROR HAS OCCURRED.\n");
        }
    }
}

void WriteProcessMemory(LinuxProc_t lpProcess, void *vpBaseAddress, void *vpBuffer, size_t nSize,
                        size_t *lpNumberOfBytesRead) {
    struct iovec iovLocalAddressSpace[1];
    struct iovec iovRemoteAddressSpace[1];
    iovLocalAddressSpace[0].iov_base = vpBuffer; //The data comes from here
    iovLocalAddressSpace[0].iov_len = nSize; //which has this size.

    iovRemoteAddressSpace[0].iov_base = vpBaseAddress; //Store data in this buffer
    iovRemoteAddressSpace[0].iov_len = nSize; //and has this size.

    ssize_t sSize = process_vm_writev(lpProcess.ProcessID, //Remote process id
                                      iovLocalAddressSpace,  //Local iovec array
                                      1, //Size of the local iovec array
                                      iovRemoteAddressSpace,  //Remote iovec array
                                      1, //Size of the remote iovec array
                                      0); //Flags, unused
    if (sSize < 0) {
        printf("WRITE %d -> ", lpProcess.ProcessID);
        switch (errno) {
            case EINVAL:
                printf("ERROR: INVALID ARGUMENTS.\n");
                break;
            case EFAULT:
                printf("ERROR: UNABLE TO ACCESS TARGET MEMORY ADDRESS.\n");
                break;
            case ENOMEM:
                printf("ERROR: UNABLE TO ALLOCATE MEMORY.\n");
                break;
            case EPERM:
                printf("ERROR: INSUFFICIENT PRIVILEGES TO TARGET PROCESS.\n");
                break;
            case ESRCH:
                printf("ERROR: PROCESS DOES NOT EXIST.\n");
                break;
            default:
                printf("ERROR: AN UNKNOWN ERROR HAS OCCURRED.\n");
        }
    }
}

LinuxProc_t fillProcessStructbyName(const char *processName) {
    LinuxProc_t processStruct;
    processStruct.ProcessName = processName;
    processStruct.ProcessID = getPidByName(processName);
    processStruct.ProcessBaseAddress = getModuleBaseAddress(processStruct.ProcessID, processName);
    return processStruct;
}

int attach(LinuxProc_t target) {
    int status = -1;
    /* attach, to the target application, which should cause a SIGSTOP */
    if (ptrace(PTRACE_ATTACH, target.ProcessID, NULL, NULL) == -1L) {
        fprintf(stderr, "error: failed to attach to %d, %s, Try running as root\n", target.ProcessID,
                strerror(errno));
        return 0;
    }

    /* wait for the SIGSTOP to take place. */
    if (waitpid(target.ProcessID, &status, 0) == -1 || !WIFSTOPPED(status)) {
        fprintf(stderr,
                "error: there was an error waiting for the target to stop.\n");
        fprintf(stdout, "info: %s\n", strerror(errno));
        return 0;
    }

    /* everything looks okay */
    return 1;

}

int detach(LinuxProc_t target) {
    return ptrace(PTRACE_DETACH, target, NULL, 0) == 0;
}
