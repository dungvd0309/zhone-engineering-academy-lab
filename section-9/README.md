# Section 9

- **Section**: Linux Kernel Programming  
- **Topic**: Linux User-Kernel Communication: Syscall, IOCTL, procfs & Netlink Socket  
- **Purpose**: Understand and use the four standard interfaces for userspace-kernel communication, with an emphasis relevant to network applications.
- **Expectation**: Four small C demo programs (syscall, ioctl, procfs, netlink).

---

## 1. User-kernel communication interfaces

**Linux kernels** are normally initiating any actions with outside effects, but rely on requests
from user space to perform these actions.

**User space programs** running without invoking kernel services have no visible effect out of its internal computations.

=> Both need to interact to produce results
=> A program execution involve alternating between user space code and kernel space code

But how the user space and the kernel space communicate? Via some kinds of User-kernel communication interfaces!

Some of communication interfaces:

- **System call**: the kernel’s fundamental, provide the *formal API* for user-space applications.

    Use for: Core, general-purpose OS services: processes, memory, files, sockets, time, signals.

- **`ioctl` (I/O Control)**: a “catch-all” control call on an open file descriptor (fd)

    Use for: Device-specific control that doesn’t fit the stream model of `read`/`write` (*e.g.: configuring a UART baud rate, setting a camera format, starting DMA, querying hardware state*)

- **`/proc` file system**: a virtual file system exposing kernel and process state (`/proc/cpuinfo`, `/proc/<pid>/status`, `/proc/interrupts`). as text files.

    Use it for: Read-mostly status, statistics, and debugging info that humans or scripts read with `cat`.

- **Netlink socket**: A socket family (`AF_NETLINK`) for structured messaging between the kernel and userspace. 

## 2. System Call 

### 2.1. How a System Call is Called?

![system_call.png](./img/system_call.png)

1. **User → Shell and the binaries**

    When we type a command `echo "Hello"`, the shell prints `Hello`.

    It seems simple. But deep down the program calls `printf()` function in the C library.

2. **C library functions** (`printf()`, `fgets()`, etc.)

    `printf()` is not a system call. It is a regular function in the glibc that runs entirely in user space. It:

    - formats the text (turns `"Hello"` and any `%d`, `%s` into the final bytes),
    - copies the result into a user-space buffer (stdio buffering),
    - calls `write()` only when the buffer needs to be flushed: on a newline if stdout is a terminal, when the buffer is full if it is a pipe or file, or on `fflush()` / program exit.

    This buffering means many small `printf()` calls can result in just one system call, which is much cheaper. At this point we are still in user space.

3. **Low-level functions**

    When the library needs the kernel to do real work, it calls the thin wrapper functions for system calls: `write()`, `read()`, `open()`, `creat()`, `close()`. Each wrapper does:

    - Loads the syscall number into `rax` register.
    - Loads the arguments into the argument registers (`rdi`, `rsi`, `rdx`, `r10`, `r8`, `r9`)
    - Executes the trap instruction `syscall`.

4. **Mode switching: User mode → kernel mode**

    The CPU: 

    - switches from low privilege (ring 3 / EL0) to high privilege (ring 0 / EL1)
    - saves the user's registers and return address.
    - jumps to the kernel's entry point.

    The kernel then reads the syscall number from `rax` and uses it as an index into the [system call table](https://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/)

*(assuming the system is x86-64, more on [syscall(2) man page](https://man7.org/linux/man-pages/man2/syscall.2.html#NOTES))*


### 2.2. syscall() - indirect system call

```c
#include <sys/syscall.h>      /* Definition of SYS_* constants */
#include <unistd.h>

long syscall(long number, ...);
```

- `number`: system call number


`syscall()`: a low-level function that invokes a system call by [its number](https://man7.org/linux/man-pages/man2/syscall.2.html#NOTES). 

The caller might need to handle the **architecture-dependent** details - Why you may ask?

**Example:**

```c
// x86-64: off_t is 64-bit, fit 1 register. Normal passing.
syscall(SYS_readahead, fd, offset, count);
```

|Register|Value|
|-|-|
|`rax`|`SYS_readahead`|
|`rdi`|`fd`|
|`rsi`|`offset` (whole 64-bit)|
|`rdx`|`count`|

```c
// ARM 32-bit: off_t is 64-bit needs 2 registers (r2/r3), and r1 is ignored for alignment.
syscall(SYS_readahead, fd, 0, (unsigned)(offset & 0xFFFFFFFF),
                              (unsigned)(offset >> 32), count);
```

|Register|Value|
|-|-|
|`r7`|`SYS_readahead`|
|`r0`|`fd`|
|`r1`|0 (padding)|
|`r2`|lo32 (lower half of `offset`)|
|`r3`|hi32 (higher half of `offset`)|
|`r4`|`count`|

When employing `syscall()` instead of other system call wrappers (`write()`, `getpid()`, etc.):

- Pros:
    - Access to system calls with no wrapper.
    - Work with minimal or old C libraries.
    - Stable kernel ABI (Application Binary Interface) since syscall numbers are kept stable by the kernel.

- Cons:
    - No type safety.
    - Architecture-specific numbers and conventions.
    - Less readable and portable.

## 3. ioctl Interface



## 4. proc file system interface

## Netlink Socket interface

## Why Netlink matters for network applications