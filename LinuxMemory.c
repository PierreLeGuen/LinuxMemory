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

pid_t GetPIDbyName(const char* cchrptr_ProcessName, int intCaseSensitiveness, int intExactMatch)
{
    int ipid = -1;
    char chrarry_CommandLinePath[800]  ;
    char chrarry_NameOfProcess[500]  ;
    char* chrptr_StringToCompare = NULL ;
    struct dirent* de_DirEntity = NULL ;
    DIR* dir_proc = NULL ;

    int (*CompareFunction) (const char*, const char*, int) ;

    if (intExactMatch)
        CompareFunction = &strcmp_ws;
    else
        CompareFunction = &strstr_ws;

    dir_proc = opendir(PROC_DIRECTORY) ;
    if (dir_proc == NULL)
    {
        perror("Couldn't open the " PROC_DIRECTORY " directory") ;
        return  -2 ;
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
                FILE* fd_CmdLineFile = fopen (chrarry_CommandLinePath, "rt") ;  // open the file for reading text
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
                        ipid = (int)strtol(de_DirEntity->d_name, (char **)NULL, 10); //atoi deprecated, use strtol
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

int read_string(LinuxProc_t Process, int32_t nsize, void *address, char *buffer) {
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