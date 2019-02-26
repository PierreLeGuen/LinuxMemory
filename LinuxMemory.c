#include "LinuxMemory.h"

int IsNumeric(const char* ccharptr_CharacterList)
{
    for ( ; *ccharptr_CharacterList; ccharptr_CharacterList++)
        if (*ccharptr_CharacterList < '0' || *ccharptr_CharacterList > '9')
            return 0;
    return 1;
}

int strcmp_ws(const char *s1, const char *s2, int intCaseSensitive)
{
    if (intCaseSensitive)
        return !strcmp(s1, s2);
    else
        return !strcasecmp(s1, s2);
}

int strstr_ws(const char* haystack, const char* needle, int intCaseSensitive)
{
    if (intCaseSensitive)
        return (int) strstr(haystack, needle);
    else
        return (int) strcasestr(haystack, needle);
}

#define BUF_SIZE 64

pid_t getPidByName(char *task_name) {
    DIR *dir;
    struct dirent *ptr;
    FILE *fp;
    char filepath[64];
    char cur_task_name[64];//大小随意，能装下要识别的命令行文本即可
    char buf[BUF_SIZE];
    int ipid = -1;
    dir = opendir(PROC_DIRECTORY); //打开路径
    if (dir == NULL)
    {
        perror("Couldn't open the " PROC_DIRECTORY " directory") ;
        return -2;
    }
    if (NULL != dir)
    {
        while ((ptr = readdir(dir)) != NULL) //循环读取路径下的每一个文件/文件夹
        {
            // Skip non numeric entries
            if (ptr->d_type == DT_DIR) {
                if (IsNumeric(ptr->d_name)) {
                    //如果读取到的是"."或者".."则跳过，读取到的不是文件夹名字也跳过
                    if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
                        continue;
                    if (DT_DIR != ptr->d_type)
                        continue;

                    sprintf(filepath, "/proc/%s/status", ptr->d_name);//生成要读取的文件的路径
                    fp = fopen(filepath, "r");//打开文件
                    if (NULL != fp) {
                        if (fgets(buf, BUF_SIZE - 1, fp) == NULL) {
                            fclose(fp);
                            continue;
                        }
                        sscanf(buf, "%*s %s", cur_task_name);

                        //如果文件内容满足要求则打印路径的名字（即进程的PID）
                        if (!strcmp(task_name, cur_task_name)) {
                            ipid = (int) strtol(ptr->d_name, (char **) NULL, 10);
                            fclose(fp);
                            closedir(dir);//关闭路径
                            return ipid;
                        }
                        fclose(fp);
                    }
                }
            }

        }
        closedir(dir);//关闭路径
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

int Read(LinuxProc_t Process, void *address, void *buf, size_t size) {
    if (Process.ProcId == 0)
        return 1;
    if (size == 0)
        return 1;

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

    if (sSize == (ssize_t) size)
    {
        //Success
        return 0;
    }
    else if (sSize == 0)
    {
        //Failure
        return 1;
    }

    //Partial read, data might be corrupted
    return 1;
}


int write_int(LinuxProc_t Process, int32_t nsize, void *address, void *value) {
    ptrace(PTRACE_POKEDATA,Process.ProcId,address, value);
    printf("ptrace wrote : (0x%lx)\n", (long) address); // currently just reads one word, but later i will add more data types.

    return 1;
}

int read_char(LinuxProc_t Process, int32_t nsize, void *address, char *buffer) {
    char ptrace_result;
    char tiny_buffer[64];
    int i=0, z=0;
    do{
        ptrace_result=(char) ptrace(PTRACE_PEEKTEXT, Process.ProcId, address + (z), 0);
        strcat(tiny_buffer,&ptrace_result);
        ++z;
    }while (ptrace_result);

    do{
        printf("%c", tiny_buffer[i]);
        i++;
    }while(i!=z);
    printf("\n");
    return 1;
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