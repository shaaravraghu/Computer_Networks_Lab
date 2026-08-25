// Background: Inter-Process Communication (IPC)
// In modern operating systems, a process is an executing instance of a program. To ensure system stability and security, the
// operating system strictly isolates each process within its own private virtual address space. This means that a process cannot
// accidentally or maliciously access the memory variables, data structures, or code of another process.
// While memory isolation is crucial for stability, processes frequently need to collaborate to perform complex tasks. For
// example, in a Client-Server Architecture, a server process must receive requests from client processes, compute the results,
// and send data back. Because they cannot directly share variables, they must rely on the operating system kernel to mediate
// their data exchange.
// Inter-Process Communication (IPC) is the set of standard mechanisms provided by the operating system kernel that safely
// bridges this gap, allowing independent processes to exchange data, synchronize their actions, and coordinate tasks.
// There are several IPC mechanisms available in POSIX/System V Linux environments, each with distinct performance
// characteristics and use cases. For this assignment, we focus on three primary mechanisms:
// 1. FIFO's (Named Pipes)
// 2. Message Queues
// 3. Shared Memory & Semaphores
  
// 3. Shared Memory & Semaphores
// Shared Memory is the fastest form of IPC available. Instead of copying data back and forth between the user process and the
// kernel (as seen in FIFOs and Message Queues), the operating system allocates a specific segment of physical RAM and maps
// it directly into the address spaces of multiple processes.
// How it works: Once attached, the shared memory segment behaves exactly like a global variable. When Process A writes to
// the memory, Process B can see the changes instantly, completely bypassing the kernel.
// Synchronization (The Catch): Because the kernel is no longer mediating the data transfer, there is no built-in protection
// against race conditions. If Process A and Process B attempt to modify the shared memory at the exact same fraction of a
// second, the data will become corrupted.
// Semaphores: To solve this, Shared Memory is almost always paired with a Semaphore. A semaphore acts as a locking
// mechanism (a "padlock" for the memory). Before a process can read or write to the shared memory, it must lock the
// semaphore. If another process tries to access the memory, it will be forced to wait until the first process unlocks the
// semaphore, ensuring safe, synchronized access.

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

// ftok() — key_t ftok(const char *pathname, int proj_id); generates a unique key.
// shmget() — int shmget(key_t key, size_t size, int shmflg); returns an identifier for the shared memory segment.
// shmat() — void *shmat(int shmid, void *shmaddr, int shmflg); attaches the process to the shared memory segment (pass shmaddr = NULL and the OS chooses the address).
// shmdt() — int shmdt(void *shmaddr); detaches the process from the shared memory segment once it is no longer needed.
// shmctl() — shmctl(int shmid, IPC_RMID, NULL); destroys the shared memory segment (detaching alone does not destroy it).
// semget() — int semget(key_t key, int nsems, int semflg); creates or accesses a semaphore set, used here to protect the shared memory segment.
// semop() — int semop(int semid, struct sembuf *sops, size_t nsops); performs wait/signal operations on the semaphore so that only one process accesses the shared memory segment at a time.
// semctl() — semctl(int semid, int semnum, int cmd, ...); used with IPC_RMID to remove the semaphore set once you are done.
