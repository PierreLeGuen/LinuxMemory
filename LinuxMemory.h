#define _GNU_SOURCE

#ifndef LINUXMEMORY_H
#define LINUXMEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include <fcntl.h>

#define PROC_DIRECTORY "/proc/"
#define CASE_SENSITIVE    1
#define CASE_INSENSITIVE  0
#define EXACT_MATCH       1
#define INEXACT_MATCH     0

typedef struct LinuxProc_s{

    char*    ProcMemPath;
    pid_t    ProcId;

} LinuxProc_t;

int attach(LinuxProc_t target);
int detach(LinuxProc_t target);
int read_int(LinuxProc_t Process, int32_t nsize, void* address, void* buffer);
int write_int(LinuxProc_t Process, int32_t nsize, void* address, void* value);
int read_char(LinuxProc_t Process, int32_t nsize, void *address, char *buffer);
int Write(int32_t nsize, void* address, void* buffer);
pid_t getPidByNameTEST(char* task_name);
pid_t GetPIDbyName(const char* cchrptr_ProcessName, int intCaseSensitiveness, int intExactMatch);
int IsNumeric(const char* ccharptr_CharacterList);
LinuxProc_t LinuxProcFromID(pid_t pid);

#endif // LINUXMEMORY_H