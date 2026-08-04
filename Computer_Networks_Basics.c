// What is an API?
// An Application Programming Interface (API) is a set of functions, rules, and protocols that allows one software application to communicate with another software component without knowing its internal implementation.

// What is a Socket?
// A socket is one endpoint of a two-way communication link between two processes over a network. It acts as an interface between an application and the network, allowing programs to send and receive data.
// In simple terms: A socket is like a telephone endpoint.
// ARPANET was the first computer network but with no programming interface; so UC, Berkeley shipped the Berkeley Software Distribution Version of Unix with Sockets

// What is a Socket Descriptor?
// A socket descriptor is an integer value returned by the socket() system call. It uniquely identifies an open socket and is used by the operating system to perform operations on that socket.

// What is a Socket Address?
// A socket address specifies the network location of a socket. It consists of: IP Address, Port Number, & Address Family

// struct sockaddr: This is a generic socket address structure. Used by all address families.
struct sockaddr {
    sa_family_t sa_family;
    char sa_data[14];
};

// struct sockaddr_in: This structure is specifically designed for IPv4.
struct sockaddr_in {
    short sin_family;
    unsigned short sin_port;
    struct in_addr sin_addr;
    char sin_zero[8];
};

// What is an Address Family (AF)?
// An Address Family (AF) specifies the type or format of addresses that a socket uses. It tells the operating system how to interpret the addresses associated with the socket.

// What is a Protocol Family (PF)?
// A Protocol Family (PF) specifies the communication protocol suite used by the socket.

// AF defines the format of addresses; used to interpret socket addresses. PF defines the protocol suite used for communication; used to identify the protocol family.
// In modern Unix/Linux systems: They have the same numeric values, so using either usually produces the same behavior.

// AF_UNIX, AF_LOCAL: Local Communication
// AF_INET: IPv4 Internet Protocols
// AF_INET6: IPv6 Internet Protocols
// AF_NETLINK: Kernel user interface device
// AF_PACKET: Low Level Packet Interface 

// Byte Order: When a value occupies more than one byte (such as an int or short), the bytes can be arranged in memory in different orders. This arrangement is called byte order.
// In Little Endian, the Least Significant Byte (LSB) is stored at the lowest memory address.
// In Big Endian, the Most Significant Byte (MSB) is stored at the lowest memory address.
// Host Byte Order: The host byte order is the byte order used internally by the computer. So different computers may have different host byte orders. May be Little or Big Endian (depends on system architecture). Used by applications internally.
// Network Byte Order: The network byte order is the standard byte order used for transmitting data over a network. Network byte order is always Big Endian. This standard ensures that all computers interpret transmitted data in the same way, regardless of their internal architecture. Used when sending and receiving data.

