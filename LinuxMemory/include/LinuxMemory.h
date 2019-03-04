#ifndef LINUXMEMORY_H
#define LINUXMEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LinuxProc_s {
    const char*   ProcessName;
    pid_t         ProcessID;
    void*         ProcessBaseAddress;
} LinuxProc_t;

/*
 * Attach to a process, needed to write, read in his memory
 */
int attach(LinuxProc_t target);

int detach(LinuxProc_t target);

/*
 * Read the memory of a process
 */
void Read(LinuxProc_t lpProcess, void *vpAddress, void *vpBuffer, size_t nSize);

/*
 * Write the memory of a process
 */
void Write(LinuxProc_t lpProcess, void *vpAddress, void *vpBuffer, size_t nSize);

pid_t getPidByName(const char *ccpProcessName);

void *getModuleBaseAddress(pid_t ProcessID, const char *ProcessName);

LinuxProc_t fillProcessStructbyName(const char *ProcessName);

int IsNumeric(const char *ccpCharacterList);

#ifdef __cplusplus
}
#endif
#endif // LINUXMEMORY_H