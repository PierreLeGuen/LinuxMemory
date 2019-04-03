#ifndef LINUXMEMORY_H
#define LINUXMEMORY_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A Structure that contains all the information needed to work with a process
 *
 * @param ProcessName A pointer to a string that contains the process name.
 * @param ProcessID PID of the process in struct.
 * @param ProcessBaseAddress A pointer to the process base address.
 */
typedef struct LinuxProc_s {
    const char *ProcessName;
    pid_t ProcessID;
    void *ProcessBaseAddress;
} LinuxProc_t;

/**
 * @brief Reads data from an area of memory in a specified process. The entire area to be read must be accessible or the operation fails.
 *
 * @param lpProcess : A handle to the process with memory that is being read. The handle must have PROCESS_VM_READ access to the process.
 * @param vpBaseAddress : A pointer to the base address in the specified process from which to read. Before any data transfer occurs, the system verifies that all data in the base address and memory of the specified size is accessible for read access, and if it is not accessible the function fails.
 * @param vpBuffer : A pointer to a buffer that receives the contents from the address space of the specified process.
 * @param nSize : The number of bytes to be read from the specified process.
 * @param lpNumberOfBytesRead : A pointer to a variable that receives the number of bytes transferred into the specified buffer. If lpNumberOfBytesRead is NULL, the parameter is ignored.
*/
void ReadProcessMemory(LinuxProc_t lpProcess, void *vpBaseAddress, void *vpBuffer, size_t nSize, size_t *lpNumberOfBytesRead);

/**
 * @brief Writes data to an area of memory in a specified process. The entire area to be written to must be accessible or the operation fails.
 *
 * @param lpProcess : A handle to the process memory to be modified. The handle must have PROCESS_VM_WRITE and PROCESS_VM_OPERATION access to the process.
 * @param vpBaseAddress : A pointer to the base address in the specified process to which data is written. Before data transfer occurs, the system verifies that all data in the base address and memory of the specified size is accessible for write access, and if it is not accessible, the function fails.
 * @param vpBuffer : A pointer to the buffer that contains data to be written in the address space of the specified process.
 * @param nSize : The number of bytes to be written to the specified process.
 * @param lpNumberOfBytesRead : A pointer to a variable that receives the number of bytes transferred into the specified process. This parameter is optional. If lpNumberOfBytesWritten is NULL, the parameter is ignored.
 */
void WriteProcessMemory(LinuxProc_t lpProcess, void *vpBaseAddress, void *vpBuffer, size_t nSize,
                        size_t *lpNumberOfBytesRead);

/**
 * @brief Gets the PID of a given process
 *
 * @param ccpProcessName : String containing the name of the desired process
 * @return  pid of the process
 */
pid_t getPidByName(const char *ccpProcessName);

/**
 * @brief Gets the module base address (or process base address) of a process
 * @param ProcessID : PID of the process.
 * @param ProcessName : A pointer to the string that contains the name of the process.
 * @return module base address
 */
void *getModuleBaseAddress(pid_t ProcessID, const char *ProcessName);

/**
 * @brief Function to fill completely a LinuxProc_t struct.
 * @param ProcessName A pointer to the string containing the process name.
 * @return A fulfilled LinuxProc_t struct.
 */
LinuxProc_t fillProcessStructbyName(const char *ProcessName);

/**
 * @brief Permits attaching to a given process
 * 
 * @param target A LinuxProc_t struct containing the target
 * @return int 
 */
int attach(LinuxProc_t target);

/**
 * @brief Permits detaching from a given process
 * 
 * @param target A LinuxProc_t struct containing the target
 * @return int 
 */
int detach(LinuxProc_t target);

#ifdef __cplusplus
}
#endif
#endif // LINUXMEMORY_H