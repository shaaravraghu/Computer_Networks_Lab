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
  
