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

// 2. Message Queues
// Unlike FIFOs, which act as a continuous stream of raw bytes, Message Queues allow processes to exchange discrete,
// formatted packets of data called "messages."
// How it works: The kernel maintains a linked list of messages identified by a unique queue ID. Processes can attach to this
// queue to drop off or pick up messages.
// Data Flow: Each message is defined by a user-created struct that includes a specific Message Type (an integer) and the
// message payload.
// Synchronization: Message Queues are highly flexible because they allow asynchronous communication. A sender can drop
// off a message and immediately continue its work without waiting for the receiver. Additionally, receivers can filter the queue
// by requesting only messages of a specific "Type," leaving other messages in the queue untouched.

#include <sys/ipc.h>
#include <sys/msg.h>

key = ftok("progfile", 65); // ftok to generate unique key
msgid = msgget(key, 0666 | IPC_CREAT); // msgget creates a message queue and returns identifier
msgsnd(msgid, &message, sizeof(message), 0); // msgsnd to send message
msgrcv(msgid, &message, sizeof(message), 1, 0); // msgrcv to receive message
msgctl(msgid, IPC_RMID, NULL); // msgctl to destroy the message queue
  
// 1. FIFOs (Named Pipes)
// A standard pipe provides a unidirectional (one-way) stream of data between two related processes (like a parent and child). A
// FIFO, or Named Pipe, extends this concept by existing as a special file on the physical file system.
// How it works: Because it has a physical path on the file system, completely unrelated processes can connect to it. One
// process opens the file for writing, and the other opens it for reading.
// Data Flow: Data flows through the pipe in a first-in, first-out sequence. The data is buffered in the kernel; it is not actually
// written to the hard drive.
// Synchronization: FIFOs have built-in blocking synchronization. A process attempting to read from an empty FIFO will
// naturally pause (block) until another process writes data into it. Furthermore, both ends of the FIFO must be opened
// simultaneously before any data can be transferred.

#include <fcntl.h>
#include <sys/stat.h>

mkfifo(const char *pathname, mode_t mode);
