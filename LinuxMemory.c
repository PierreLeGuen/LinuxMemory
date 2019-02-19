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

#define BUF_SIZE 1024

pid_t getPidByNameTEST(char *task_name) {
    DIR *dir;
    struct dirent *ptr;
    FILE *fp;
    char filepath[50];//大小随意，能装下cmdline文件的路径即可
    char cur_task_name[50];//大小随意，能装下要识别的命令行文本即可
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
            //如果读取到的是"."或者".."则跳过，读取到的不是文件夹名字也跳过
            if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
                continue;
            if (DT_DIR != ptr->d_type)
                continue;

            sprintf(filepath, "/proc/%s/status", ptr->d_name);//生成要读取的文件的路径
            fp = fopen(filepath, "r");//打开文件
            if (NULL != fp)
            {
                if( fgets(buf, BUF_SIZE-1, fp)== NULL ){
                    fclose(fp);
                    continue;
                }
                sscanf(buf, "%*s %s", cur_task_name);

                //如果文件内容满足要求则打印路径的名字（即进程的PID）
                if (!strcmp(task_name, cur_task_name)){
                    ipid = (int)strtol(ptr->d_name, (char **)NULL, 10);
                    fclose(fp);
                    closedir(dir);//关闭路径
                    return ipid;
                }
                fclose(fp);
            }

        }
        closedir(dir);//关闭路径
        return ipid;
    }
}

pid_t GetPIDbyName(const char* cchrptr_ProcessName, int intCaseSensitiveness, int intExactMatch)
{
    int ipid = -1;
    char chrarry_CommandLinePath[800]  ;
    struct dirent* de_DirEntity = NULL ;
    DIR* dir_proc = NULL ;
    char chrarry_NameOfProcess[500] = {0}; // ICIIIII
    char* chrptr_StringToCompare = NULL ; // ICIIIII

    int (*CompareFunction) (const char*, const char*, int) ;

    if (intExactMatch)
        CompareFunction = &strcmp_ws;
    else
        CompareFunction = &strstr_ws;

    dir_proc = opendir(PROC_DIRECTORY) ;
    if (dir_proc == NULL)
    {
        perror("Couldn't open the " PROC_DIRECTORY " directory") ;
        return -2;
    }

    // Loop while not NULL
    while ((de_DirEntity = readdir(dir_proc)) )
    {
        // Skip non numeric entries
        if (de_DirEntity->d_type == DT_DIR)
        {
            if (IsNumeric(de_DirEntity->d_name))
            {
                strcpy(chrarry_CommandLinePath, PROC_DIRECTORY) ;
                strcat(chrarry_CommandLinePath, de_DirEntity->d_name) ;
                strcat(chrarry_CommandLinePath, "/cmdline") ;
                FILE* fd_CmdLineFile = fopen (chrarry_CommandLinePath, "rt") ;  // open the file for reading text*

                if (fd_CmdLineFile)
                {
                    fscanf(fd_CmdLineFile, "%s", chrarry_NameOfProcess) ; // read from /proc/<NR>/cmdline
                    fclose(fd_CmdLineFile);  // close the file prior to exiting the routine

                    if (strrchr(chrarry_NameOfProcess, '/'))
                        chrptr_StringToCompare = strrchr(chrarry_NameOfProcess, '/') +1 ; //Compare after ./
                    else
                        chrptr_StringToCompare = chrarry_NameOfProcess ;

                    if ( CompareFunction(chrptr_StringToCompare, cchrptr_ProcessName, intCaseSensitiveness) )
                    {
                        ipid = atoi(de_DirEntity->d_name);
                        // ipid = (int)strtol(de_DirEntity->d_name, (char **)NULL, 10); //atoi deprecated, use strtol
                        printf("PID in func : %i",ipid);
                        closedir(dir_proc) ;
                        return ipid;
                    }
                }
            }
        }
    }
    closedir(dir_proc);
    return ipid ;
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

int read_int(LinuxProc_t Process, int32_t nsize, void* address, void* buffer)
{
    long _DEBUGINT  = 0;

    _DEBUGINT = ptrace(PTRACE_PEEKDATA,Process.ProcId,address,0);
    printf("Output from ptrace (0x%lx): %i\n", (long) address,(int) _DEBUGINT); // currently just reads one word, but later i will add more data types.

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