#include "LinuxMemory.h"

int IsNumeric(const char* ccharptr_CharacterList)
{
    for ( ; *ccharptr_CharacterList; ccharptr_CharacterList++)
        if (*ccharptr_CharacterList < '0' || *ccharptr_CharacterList > '9')
            return 0;
    return 1;
}

pid_t getPidByName(char *task_name) {
    DIR *dir;
    struct dirent *ptr;
    FILE *fp;
    char filepath[64];
    char cur_task_name[64];
    char buf[64];
    int ipid = -1;
    dir = opendir("/proc");
    if (dir == NULL)
    {
        perror("Couldn't open the PROC directory") ;
        return -2;
    }
    if (NULL != dir)
    {
        while ((ptr = readdir(dir)) != NULL)
        {
            // Skip non numeric entries
            if (ptr->d_type == DT_DIR) {
                if (IsNumeric(ptr->d_name)) {
                    //Skip "." and ".." files
                    if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
                        continue;
                    if (DT_DIR != ptr->d_type)
                        continue;

                    sprintf(filepath, "/proc/%s/status", ptr->d_name);//parse status file
                    fp = fopen(filepath, "r");//open in read mode
                    if (NULL != fp) {
                        if (fgets(buf, sizeof(buf) - 1, fp) == NULL) {
                            fclose(fp);
                            continue;
                        }
                        sscanf(buf, "%*s %s", cur_task_name);

                        //compare task_name and cur_task_name)
                        if (!strcmp(task_name, cur_task_name)) {
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

int attach(LinuxProc_t target)
{
    int status;
    /* attach, to the target application, which should cause a SIGSTOP */
    if (ptrace(PTRACE_ATTACH, target.ProcId, NULL, NULL) == -1L) {
        fprintf(stderr, "error: failed to attach to %d, %s, Try running as root\n", target.ProcId,
                strerror(errno));
        return 0;
    }

    /* wait for the SIGSTOP to take place. */
    if (waitpid(target.ProcId, &status, 0) == -1 || !WIFSTOPPED(status)) {
        fprintf(stderr,
                "error: there was an error waiting for the target to stop.\n");
        fprintf(stdout, "info: %s\n", strerror(errno));
        return 0;
    }

    /* everything looks okay */
    return 1;

}

int detach(LinuxProc_t target)
{
    return ptrace(PTRACE_DETACH, target, NULL, 0) == 0;
}

void Read(LinuxProc_t Process, void *address, void *buf, size_t size) {
    struct iovec iovLocalAddressSpace[1];
    struct iovec iovRemoteAddressSpace[1];
    iovLocalAddressSpace[0].iov_base = buf; //Store data in this buffer
    iovLocalAddressSpace[0].iov_len = size; //which has this size.

    iovRemoteAddressSpace[0].iov_base = address; //The data comes from here
    iovRemoteAddressSpace[0].iov_len = size; //and has this size.

    ssize_t sSize = process_vm_readv(Process.ProcId, //Remote process id
                                     iovLocalAddressSpace,  //Local iovec array
                                     1, //Size of the local iovec array
                                     iovRemoteAddressSpace,  //Remote iovec array
                                     1, //Size of the remote iovec array
                                     0); //Flags, unused
    if (sSize < 0) {
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

void Write(LinuxProc_t Process, void *address, void *buf, size_t size);

void Write(LinuxProc_t Process, void *address, void *buf, size_t size) {
    struct iovec iovLocalAddressSpace[1];
    struct iovec iovRemoteAddressSpace[1];
    iovLocalAddressSpace[0].iov_base = buf; //Store data in this buffer
    iovLocalAddressSpace[0].iov_len = size; //which has this size.

    iovRemoteAddressSpace[0].iov_base = address; //The data comes from here
    iovRemoteAddressSpace[0].iov_len = size; //and has this size.

    ssize_t sSize = process_vm_writev(Process.ProcId, //Remote process id
                                      iovLocalAddressSpace,  //Local iovec array
                                      1, //Size of the local iovec array
                                      iovRemoteAddressSpace,  //Remote iovec array
                                      1, //Size of the remote iovec array
                                      0); //Flags, unused
    if (sSize < 0) {
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

LinuxProc_t LinuxProcFromID(pid_t pid)
{
    char mem_file_name[1000];
    LinuxProc_t ProcStruct;


    sprintf(mem_file_name, "/proc/%d/mem", pid);

    ProcStruct.ProcId = pid;
    ProcStruct.ProcMemPath = mem_file_name;

    return ProcStruct;
}