# Section 3

- **Section**: Linux System Programming
- **Topic**: Process Fundamentals, Memory Layout 
- **Duration**: 4 working days (8 hours/day) 

---

## Table of Contents

1. [Processes, Process Identifiers](#1-processes-process-identifiers)
1. [Process creation (fork/vfork)](#2-process-creation-forkvfork)

---
## 1. Processes, Process Identifiers

### 1.1. Processes and Programs
- A *process* is an instance of an executing program.
- A *program* is a file containing a range of information that describes how to construct a process at run time.
- Multiple processes can execute the same program.

or simply:

- A *program* is like a recipe.
- A *process* is like an actual cooking session using that recipe. Multiple chefs can cook multiple dishes with the same recipe.

### 1.2. Process ID
- Each process has a unique integer *process identifier* (PID).
- Each process also has a
*parent process identifier* (PPID) attribute, which identifies the process that requested
the kernel to create this process.
- Use `getpid()` to returns the process ID of the calling process.
- Use `getppid()` to returns the process ID of the parent of the calling process.

---
## 2. Process creation (fork/vfork) 

### 2.1. fork()

A process can create a new process using the `fork()` system call.

```c
#include <unistd.h>

pid_t fork(void);

/* In parent: returns process ID of child on success, or –1 on error; 
in successfully created child: always returns 0 */
```

```
process A
  │
fork()
  ├─ process A: parent process ─────────▶ 
  │
  └─ process B: child process  ─────────▶   
```
- The child obtains copies of the parent’s stack, data, heap, and text segments.

- Both processes continue running independently.

**Example 1:**
```c
int main(void) {
    printf("Before fork\n");
    fork();
    printf("Hello, World!\n");
}
```
Output:
```
Before fork
Hello, World!
Hello, World!
```
*There were 2 processes did the printf()*

**Example 2:**
```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
    pid_t pid = fork();

    if (pid < 0) 
    {
        perror("fork failed");
        return 1;
    } 
    else if (pid == 0) 
    {
        printf("This is the child process with PID: %d and Parent PID: %d\n", getpid(), getppid());
    }
    else 
    {
        printf("This is the parent process with PID: %d and Child PID: %d\n", getpid(), pid);
    }

    return 0;
}
```
Output:
```
This is the parent process with PID: 22286 and Child PID: 22287
This is the child process with PID: 22287 and Parent PID: 2185
```

### 2.2. vfork()

`vfork()` is a version of `fork()`, operated with slightly different semantic.

```c
#include <unistd.h>

pid_t fork(void);

/* In parent: returns process ID of child on success, or –1 on error;
in successfully created child: always returns 0 */
```

```
process A
  │
vfork()
  ├─ process A: parent process (suspended)
  │
  └─ process B: child process ─────────▶             
```

- No duplication of virtual memory pages or page tables is done for the child process. Instead, the child shares the parent’s memory.
- Execution of the parent process is suspended until the child has performed an
`exec()` or `_exit()`.

**Example:**
```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
    printf("Before vfork\n");

    pid_t pid = vfork();

    if (pid == 0) 
    {
        printf("Child process\n");
        _exit(0);
    }

    printf("Parent process\n");

    return 0;
}
```
Output:
```
Before vfork
Child process
Parent process
```

---
## 3. Process Termination (_exit/exit)

**Two ways of process termination**:

- *Abnormal termination*: caused by the delivery of a signal whose default action is to terminate the process.
- *Normal termination*: using the `_exit()` system call.

**Termination status**:
- only the bottom 8 bits of status are actually made available to the parent.
- `EXIT_SUCCESS` (0) and `EXIT_FAILURE` (1).
    ```c
    _exit(EXIT_SUCCESS);
    ```

### 3.1. _exit()
```c
#include <unistd.h>

void _exit(int status);
```
`_exit()` is a system call that terminates the calling process immediately


### 3.2. exit()
```c
#include <stdlib.h>

void exit(int status);
```
`exit()` is a C library function layered on top of `_exit()`.

The following actions are performed by `exit()`:
1. Exit handlers (functions registered with `atexit()` and `on_exit()`) are called, in reverse order of their registration.
1. Flush `stdio` stream buffers.
1. Invoke `_exit()` system call.


### 3.3. Return from main()
```c
int main()
{
    return 0;
}
```
The `main()` function returns first. As the function returns, its stack frame disappears, meaning ***all local variables declared within `main()` are immediately destroyed.*** After that, the C runtime startup code calls `exit()`.

**To differentiate a "Return from `main()`" from an "`exit()` inside main", let's run an experiment:**

```c
#include <stdio.h>
#include <stdlib.h>

char **g_key_ptr = NULL; /* A global pointer to char* */

/* Function that prints the value pointed to by the global pointer */
void key_backup(void) 
{
    if (g_key_ptr) {
        printf("Program shutting down...\n");
        printf("Please backup this code: %s\n", *g_key_ptr);
    }
}

int main(void)
{
    char* super_important_key = "sixseven"; /* Main local variable */
    g_key_ptr = &super_important_key; /* The global pointer to the local variable */

    atexit(key_backup); /* Register key_backup as an exit handler */

    return 0; /* Let's try returning from main() */
}
```
Output:
```
Program shutting down...
Please backup this code: @�,��
```
> Some weird characters appeared!

Explanation the `return 0;` workflow:

1. The `main()` function returns. The local variable `super_important_key` stored in `main()` stack frame disappears.
1. `g_key_ptr` becomes a dangling pointer since it points to a freed address.
1. `exit()` automatically occurs after `main()` returns.
1. `exit()` called registerd exit handlers, which is `key_backup` in this case.
1. The `key_backup` function performs a printf from `g_key_ptr`, resulting in undefined behaviors.

**Let's make a simple change:**
```c
#include <stdio.h>
#include <stdlib.h>

char **g_key_ptr = NULL;

void key_backup(void)
{
    if (g_key_ptr) {
        printf("Program shutting down...\n");
        printf("Please backup this code: %s\n", *g_key_ptr);
    }
}

int main(void)
{
    char* super_important_key = "sixseven";
    g_key_ptr = &super_important_key; 

    atexit(key_backup);

    /* return 0; */
    exit(0); /* Use exit() instead */
}
```
Output
```
Program shutting down...
Please backup this code: sixseven
```
> Works properly!

Now, the exit handler can access data from `main()` local variables with ease because calling `exit(0)` directly inside `main()` triggers the handler before the `main()` stack frame is destroyed.


---
## 4. Monitoring Child Processes (wait/waitpid/waitid)

### 4.1. wait()

The `wait()` system call waits for one of the children of the calling process to terminate.

```c
#include <sys/wait.h>

pid_t wait(int *status); 
/* Returns process ID of terminated child, or –1 on error */
```

The `wait()` system call does the following:
1. `wait()` blocks until one of the children terminates.
If a child has already terminated by the time of the call, `wait()` returns immediately.
1. `status` is an output parameter, `wait()` writes information about how the child terminated.  
1. The kernel adds the process CPU times and resource usage statistics to running totals for all children of this parent process.
1. `wait()` returns the PID of the child that has terminated.

```
parent
  │
fork()
  ├─ parent ────────|wait() blocks |───────▶
  │                                │
  └─ child  ────────────────exit()─┘   
```

### 4.2. waitpid()

`wait()` waits for any one of the children, while `waitpid()` waits for a specific one by specifying its process ID or its stopped signal.

```c
#include <sys/wait.h>

pid_t waitpid(pid_t pid, int *status, int options);

/* Returns process ID of child, 0 (WNOHANG if no child has changed state), or –1 (error) */
```
The return value and `status` arguments of `waitpid()` are the same as for `wait()`.

```
parent
  │
fork()
  ├─ parent ────|wait(123, NULL, 0) blocks |───────▶
  │                                        │
  ├─ child (PID 123) ───────────────exit()─┘   
  │                                
  └─ child (PID 139) ──────────────────────────────▶
```


|`pid` arguments|Description|
|-|-|
|> 0|Wait for the child whose process ID == `pid`.|
|= 0|Wait for any child in the same *process group* as the caller (parent).|
|< -1|Wait for any child whose *process group* identifier equals the absolute value of `pid`.|
|= -1|Wait for any child. `wait(&status)` is equivalent to `waitpid(–1, &status, 0)`.|

If no children that match the specification in `pid`, `waitpid()` fails with the error `ECHILD`.

|`options` arguments|Description|
|-|-|
|`WUNTRACED`|Wait for children that have been stopped by a signal.|
|`WCONTINUED`|Wait for children that have been resumed by a SIGCONT signal.|
|`WNOHANG`|If no child has yet changed state, then return `0` immediately, instead of blocking.

### 4.3. waitid()
`waitid()` is like `waitpid()` but provides extra functionality.

```c
#include <sys/wait.h>

int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);

/* Returns 0 on success or if WNOHANG was specified and
there were no children to wait for, or –1 on error */
```

|`idtype` arguments|Description|
|-|-|
|`P_ALL`|Wait for any child; `id` is ignored|
|`P_PID`|Wait for the child whose *process ID* equals `id`|
|`P_PGID`|Wait for any child whose *process group ID* equals `id`|

|`options` arguments|Description|
|-|-|
|`WEXITED`|Wait for children that have terminated, either normally or abnormally.|
|`WSTOPPED`|Wait for children that have been stopped by a signal.|
|`WCONTINUED`|Wait for children that have been resumed by a `SIGCONT` signal.|
|`WNOHANG`|If no child has yet changed state, then return `0` immediately, instead of blocking.|
|`WNOWAIT`|The child status is returned, but the child remains in a waitable state, and we can later wait for it again to retrieve the same information|

|`siginfo_t` struct|Description|
|-|-|
|`si_code`|This field contains one of the following values:<br>- `CLD_EXITED`, indicating that the child terminated by calling `_exit()`; <br>- `CLD_KILLED`, indicating that the child was killed by a signal. <br>- `CLD_STOPPED`, indicating that the child was stopped by a signal; <br>- `CLD_CONTINUED`, indicating that the (previously stopped) child resumed execution as a consequence of receiving a (`SIGCONT`) signal.|
|`si_signo`|This field is always set to `SIGCHLD`.|
|`si_status`|This field is always set to `SIGCHLD`.|
|`si_uid`|This field contains the real user ID of the child. Most other UNIX implementations don’t set this field.|

### 4.4. Orphans and Zombies

#### Orphans
- Child processes that lost their parents (parent processes got terminated, while the children didn't). 
- The orphans are adopted by `init` process, the ancestor of all processes, whose PID is 1.


#### Zombies
-  A zombie is a child process that already exited, but whose parent hasn't called `wait()` yet to collect the result.

---
## 5. Program Execution (exec)

`execve()` system call loads a new program into a process’s memory.

During this operation, the old program is discarded, and the process’s stack, data, and heap are replaced by those of the new program.

`exec()` library functions are layered on top of `execve()`, provide alternative APIs.

```c
#include <unistd.h>

int execve(const char *pathname, char *const argv[], char *const envp[]);
int execle(const char *pathname, const char *arg, ...
/* , (char *) NULL, char *const envp[] */ );
int execlp(const char *filename, const char *arg, ...
/* , (char *) NULL */);
int execvp(const char *filename, char *const argv[]);
int execv(const char *pathname, char *const argv[]);
int execl(const char *pathname, const char *arg, ...
/* , (char *) NULL */);

/* None of the above returns on success; all return –1 on error */
```

|Function|Specification of program file <br> *(–, p)*|Specification of arguments <br> *(v, l)*|Source of environment <br> *(e, –)*|
|-|-|-|-|
|`execve()`| pathname| array| *envp* argument
|`execle()`| pathname| list| *envp* argument
|`execlp()`| filename + PATH|  list| caller’s *environ*
|`execvp()`| filename + PATH|  array| caller’s *environ*
|`execv()`| pathname| array| caller’s *environ*
|`execl()`| pathname| list| caller’s *environ*

**Specification of program file (–, p)**

- `-`: You provide the pathname yourself
  ```c
  execl("/bin/echo", "echo", "Hello", NULL);
  ```

- `p`: Search for the program using $PATH
  ```c
  execlp("echo", "echo", "Hello", NULL);
  ```

**Specification of arguments (v, l)**

- `v`: vector/array
  ```c
  char *args[] = {"ls", "-la", "~", NULL};
  execvp("ls", args);
  ```

- `l`: list
  ```c
  execlp("ls", "ls", "-la", "~", NULL);
  ```

**Source of environment (e, –)**

- `-`: Inherit the caller's environment <br>
  For example, if the current process has:
  ```c
  PATH=/usr/bin:/bin
  HOME=/home/user
  USER=dungvd
  ```
  the new program receives those environment variables.

- `e`: Explicitly provide a new environment
  ```c
  char *envp[] = {
      "MY_NAME=Dung Vu",
      "LANG=C",
      NULL
  };

  execve("/usr/bin/env", argv, envp);
  ```


---
## 6. Race Conditions between fork/exec/wait

After `fork()`, parent and child run independently.

We *cannot assume which process runs first* since different UNIX systems and Linux kernel versions may schedule differently:
- Some versions tend to run the parent first. (Linux 2.2.19, Linux 2.6.32)
- Others tend to run the child first. (adopted in Linux 2.6)

To guarantee a particular order, we must use some kind of synchronization technique, such as semaphores, file locks, IPC, signals. 

---
## 7. system Function
`system()`: allows the calling program to execute an arbitrary shell command

```c
#include <stdlib.h>

int system(const char *command);
```
Examples:
```c
system("clear");
system("echo Hello!");
system("ls *.c > files.txt");
```

`system()` function ***creates a child process*** that invokes a shell to execute command.

**Advantages of `system()`:**

- We don’t need to handle the details of calling `fork()`, `exec()`, `wait()`, and `exit()`, or handle errors and signal.
- `system()` uses the shell to execute command => All of the *shell processing, substitutions, redirections* are performed on command before it is executed.

**Disadvantage of `system()`:**

- Ineffciency. Executing a command using `system()`
requires the creation of at least 2 processes:
  - one for the shell,
  - one or more for the command(s) its execute.

**Return value of `system()`:**

- If `command` is a `NULL` pointer:
  - returns a non zero value if a shell is available,
  - returns `0` if no shell is available.
- If a child process couldn't be created / its termination couldn't be retrieved:
  - returns `-1`.
- If a shell could not be execed in the child process:
  - returns a value as though the child shell had terminated with the call `_exit(127)`.
- If all system calls succeed:
  - returns the termination status of the child shell used to execute `command`.

---
## 8. Process Groups, Sessions, Controlling Terminal & Job Control

```
Terminal session
       │
       ▼
    Session
       │
       ├── Job 1 → Process Group
       │             ├── process
       │             └── process
       │
       ├── Job 2 → Process Group
       │             └── process
       │
       └── Job 3 → Process Group
                     ├── process
                     └── process
```

### 8.1. Process Groups

A *process group* is a collection of related processes.

Identified by a *process group identifier* (PGID).

**Process group leader**:
- The process that creates the group.
- Whose PID become PGID of the group.

**Process group lifetime**:
- Begins when the leader creates the group.
- Ends when the last member leaves the group (by terminating or joining another process group).
- The process group leader need not be the last member of a process group.

***Foreground process group*** is for the terminal, the others are ***Background process groups***.

```c
#include <unistd.h>

pid_t getpgrp(void);

/* Always successfully returns process group ID of calling process */
```

```c
#include <unistd.h>

int setpgid(pid_t pid, pid_t pgid);

/* Returns 0 on success, or –1 on error */
/* If pid and pgid match, the specified process becomes the leader of a new process group. */
```

### 8.2. Sessions
A *session* is a collection of process groups. 

Identified by a *session identifier* (SID).

**Session leader**:
- The process that creates a new session.
- Whose process ID becomes the session ID
- A new process inherits its parent’s session ID

### 8.3. Controlling Terminal

A *controlling terminal* is a terminal device associated with a session that manages terminal-based processes and handles job control, signals, and session lifetimes.

### 8.4. Job Control

*Job control* a shell feature.

Job control permits user to simultaneously execute multiple commands (jobs), one in the foreground and the others in the background.

Jobs can be stopped and resumed, and moved between the foreground and background.

#### Using Job Control Within the Shell

When using `&` at the end of a command, it's run as a background job.

Creating background jobs:
```bash
$ sleep 60 & 
[1] 37546   # Job 1 with PID 37546
$ sleep 100 &
[2] 37621   # Job 2 with PID 37621
```

List all background jobs:
```bash
& jobs
[1]-  Running         sleep 60 &
[2]+  Running         sleep 100 &
```

Move a background job to the foreground:
```bash
$ fg %1
sleep 60
```

Suspend a job (`Control-Z`-`SIGTSTP` signal):
```bash
^Z
[1]+  Stopped                 sleep 60
```

Resume a job in the foreground (`fg`) or in the background (`bg`):
```bash
$ bg %1
[1]+ sleep 60 &
```

![job_control_state.png](./img/job_control_state.png)

---
## 9. Command-Line Arguments and Environment List

### 9.1. Command-Line Arguments

When the program is executed, the command-line arguments (the separate words parsed by the shell) are made available via two arguments to the function `main()`.

```c
int main(int argc, char *argv[])
{
  // ...
}
```
- `int argc`: how many command-line arguments there are.
- `char *argv[]`: an array of pointers to the command-line arguments.
  
Examples:

```c
/* program.c */
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("argument count: %d\n", argc);
    for (int i = 0; i < argc; i++) 
    {
        printf("argv[%d]: %s\n", i, argv[i]);
    }
    return 0;
}
```
```bash
$ ./program Hello World !
argument count: 4
argv[0]: ./program
argv[1]: Hello
argv[2]: World
argv[3]: !
```

### 9.2. Environment List

The **environment list** (or simply **environment**) is an array of strings in a process's user-space memory consisting of `name=value` pairs known as environment variables.

A new process inherits a copy of its parent's env => Environment changes from either process are not seen by the other process.

Adding a value to the environment using `export` command in `bash` shell:

```bash
$ export MY_PATH=~/Documents
$ echo $MY_PATH
/home/dungvd/Documents
```

#### Accessing the environment from a program

1. Using the global variable `char **environ`:

Example:
```c
#include <stdio.h>

extern char **environ;

int main(int argc, char *argv[])
{
    char **ep;

    for (ep = environ; *ep != NULL; ep++) 
    {
        puts(*ep);
    }
    return 0;
}
```
Output:
```
LINENO=19
SHELL=/bin/bash
SESSION_MANAGER=local/dungvd-asus:@/tmp/.ICE-unix/2453,unix/dungvd-asus:/tmp/.ICE-unix/2453
QT_ACCESSIBILITY=1
COLORTERM=truecolor
...
```

2. Declaring a third argument `char *envp[]` to `main()`:
  ```c
  int main(int argc, char *argv[], char *envp[])
  ```
- This argument can then be treated in the same way as `environ`, with the difference that its scope is local to main().

3. Using `getenv()` function:
```c
#include <stdlib.h>

char *getenv(const char *name);

/* Returns pointer to (value) string, or NULL if no such variable */
```

#### Modifying the environment from a program

1. `putenv()` function:
```c
#include <stdlib.h>

int putenv(char *string);

/* Returns 0 on success, or nonzero on error */
```
- `string` arg: a pointer to a strign of the form `name=value`
- After the `putenv()` call, this string is part of the environment => Modifying the bytes pointed by `string` will affect the process environment.

Example:
```c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    printf("MY_VAR: %s\n", getenv("MY_VAR"));
    putenv("MY_VAR=my_value");
    printf("MY_VAR: %s\n", getenv("MY_VAR"));
    return 0;
}
```
Output:
```
MY_VAR: (null)
MY_VAR: my_value
```

2. `setenv()` function:
```c
#include <stdlib.h>

int setenv(const char *name, const char *value, int overwrite);

/* Returns 0 on success, or –1 on error */
```
- `name`: name of the new environment variable
- `value`: value of the new environment variable
- `overvalue`: 
  - `0`: doesn't change the env if the variable with `name` already exists
  - nonzero: always change the env.
- `setenv()` copies its arguments => we can subsequently modify the contents of the strings pointed to by `name` and `value` without affecting the environment.

3. `unsetenv()` function:
```c
#include <stdlib.h>

int unsetenv(const char *name);

/* Returns 0 on success, or –1 on error */
```
- `unsetenv()`removes the variable identified by name from the environment.

4. Erasing the entire environment:

By assigning `NULL` to `environ`
```c
environ = NULL;
```
or using `clearenv()` function
```c
#define _BSD_SOURCE /* Or: #define _SVID_SOURCE */
#include <stdlib.h>

int clearenv(void)

/* Returns 0 on success, or –1 on error */
```

## 10. Memory Layout of a C Program (text/data/bss/heap/stack)

![memory_management.png](./img/memory_management.png)

|Segment|Description|
|-|-|
|Text|- Program instructions/code <br>- Read-only|
|Data|- Initialized global/static variables|
|BSS|- Uninitialized global/static variables|
|Heap|- is an area for dynamic allocation at run time. <br>- `malloc()`, `calloc()`, `realloc()`|
|Stack|- is a dynamically growing and shrinking segment containing stack frame. <br>- One stack frame is allocated for each currently called function. <br> - function's local variables, arguments, and return value|

```c
#include <stdio.h>
#include <stdlib.h>
char globBuf[65536]; /* Uninitialized data segment */
int primes[] = { 2, 3, 5, 7 }; /* Initialized data segment */

static int square(int x) /* Allocated in frame for square() */
{
  int result; /* Allocated in frame for square() */

  result = x * x;
  return result; /* Return value passed via register */
}

static void doCalc(int val) /* Allocated in frame for doCalc() */
{
  printf("The square of %d is %d\n", val, square(val));

  if (val < 1000) {
    int t; /* Allocated in frame for doCalc() */

    t = val * val * val;
    printf("The cube of %d is %d\n", val, t);
  }
}
int main(int argc, char *argv[]) /* Allocated in frame for main() */
{
  static int key = 9973; /* Initialized data segment */
  static char mbuf[10240000]; /* Uninitialized data segment */
  char *p; /* Allocated in frame for main() */

  p = malloc(1024); /* Points to memory in heap segment */

  doCalc(key);
  exit(EXIT_SUCCESS);
}
```

## 11. Shared Libraries: static vs. dynamic linking

> **section 41**

Shared libraries are a technique for placing library functions into a single unit that can be shared by multiple processes at run time.

This technique can save both disk space and RAM.

## 12. getrlimit and setrlimit Functions (resource limits)

> **section 36**

```c
#include <sys/resource.h>

int getrlimit(int resource, struct rlimit *rlim);
int setrlimit(int resource, const struct rlimit *rlim);

/* Both return 0 on success, or –1 on error */
```
```c
struct rlimit {
  rlim_t rlim_cur; /* Soft limit (actual process limit) */
  rlim_t rlim_max; /* Hard limit (ceiling for rlim_cur) */
};
```

| resource | Limit on | SUSv3 |
|---|---|:---:|
| `RLIMIT_AS` | Process virtual memory size (bytes) | • |
| `RLIMIT_CORE` | Core file size (bytes) | • |
| `RLIMIT_CPU` | CPU time (seconds) | • |
| `RLIMIT_DATA` | Process data segment (bytes) | • |
| `RLIMIT_FSIZE` | File size (bytes) | • |
| `RLIMIT_MEMLOCK` | Locked memory (bytes) | |
| `RLIMIT_MSGQUEUE` | Bytes allocated for POSIX message queues for real user ID (since Linux 2.6.8) | |
| `RLIMIT_NICE` | Nice value (since Linux 2.6.12) | |
| `RLIMIT_NOFILE` | Maximum file descriptor number plus one | • |
| `RLIMIT_NPROC` | Number of processes for real user ID | |
| `RLIMIT_RSS` | Resident set size (bytes; not implemented) | |
| `RLIMIT_RTPRIO` | Realtime scheduling priority (since Linux 2.6.12) | |
| `RLIMIT_RTTIME` | Realtime CPU time (microseconds; since Linux 2.6.25) | |
| `RLIMIT_SIGPENDING` | Number of queued signals for real user ID (since Linux 2.6.8) | |
| `RLIMIT_STACK` | Size of stack segment (bytes) | • |

## 13. ELF format, symbol table, relocation - supplementary linking concepts

> Section 45.9: "IPC Hardware Limits and IPC_INFO" (or nonstandard IPC control operations) under Chapter 45: Introduction to System V IPC

## 14. Stack frame layout (System V AMD64 ABI): prologue/epilogue, %rbp/%rsp, register- vs. stack-passed arguments, the 128-byte red zone

> The Stack frame layout (System V AMD64 ABI)—including prologue/epilogue, %rbp/%rsp register usage, argument passing (register vs. stack), and the 128-byte red zone—is not detailed in the provided book excerpt.  The text briefly introduces basic stack concept concepts on x86-32 architecture in Section 6.5: "The Stack and Stack Frames" (page 121), but it does not cover the System V AMD64 ABI details, register calling conventions, or the red zone

## 15. Reading compiler-generated Assembly (gcc -S / objdump -d) to identify a function's own stack frame

> The topic of inspecting compiler-generated Assembly (using gcc -S or objdump -d) to examine a function's stack frame is briefly mentioned in Section 6.8: "Performing a Nonlocal Goto: setjmp() and longjmp()" (specifically in the callout/note on page 136).  It appears under the section discussing how compiler optimizations affect local variables during a longjmp(), where the text notes:  "It is instructive to look at the assembler output produced when compiling the setjmp_vars.c program both with and without optimization. The cc -S command produces a file with the extension .s containing the generated assembler code for a program."  

## 16. Lab 

### Lab 1
Write a program using fork/exec/wait/waitpid; observe zombie and orphan processes 

[Lab 1 link](./lab/lab_1.c)

**Diagram**:

![lab_1_diagram.png](./img/lab_1_diagram.png)

**Observation**:

At first, the parent process `fork()` to create a child process, then the child `fork()` to create a grandchild.

```bash
$ ./lab_1
[Parent process] PID: 36160, PPID: 7611, Time: 0
[Child process] PID: 36161, PPID: 36160, Time: 0
[Grandchild] PID: 36162, PPID: 36161, Time: 0
...
```

```bash
$ ps aux | grep lab_1
dungvd     36160  0.0  0.0   2692  1732 pts/1    S+   10:13   0:00 ./lab_1
dungvd     36161  0.0  0.0   2692  1000 pts/1    S+   10:13   0:00 ./lab_1
dungvd     36162  0.0  0.0   2692  1000 pts/1    S+   10:13   0:00 ./lab_1
```

After 5 seconds, the child exited then became a zombie. Since its parent (`PID 36160`) hadn't waited for it yet. The grandchild became an orphan because its parent (`PID 36161`) exited, then got adopted by the *user systemd* (`PID 2217`):
```
...
[Grandchild] PID: 36162, PPID: 36161, Time: 6
Child process - PID 36161 exiting
[Parent process] PID: 36160, PPID: 7611, Time: 8
[Grandchild] PID: 36162, PPID: 2217, Time: 8
...
```

```bash
$ ps aux | grep lab_1
dungvd     36160  0.0  0.0   2692  1732 pts/1    S+   10:13   0:00 ./lab_1
dungvd     36161  0.0  0.0      0     0 pts/1    Z+   10:13   0:00 [lab_1] <defunct>
dungvd     36162  0.0  0.0   2692  1000 pts/1    S+   10:13   0:00 ./lab_1

$ ps aux | grep 2217
dungvd      2217  0.0  0.1  21332  9876 ?        Ss   08:22   0:00 /usr/lib/systemd/systemd --user
```

After sleep for 30 seconds, the grandchild did a `execlp()` to run `echo` command then exited. The *user systemd* handled the grandchild exit:

```
...
Bye!!! Grandchild process using execlp to echo
[Parent process] PID: 36160, PPID: 7611, Time: 32
[Parent process] PID: 36160, PPID: 7611, Time: 34
[Parent process] PID: 36160, PPID: 7611, Time: 36
...
```
```bash
$ ps aux | grep lab_1
dungvd     36160  0.0  0.0   2692  1732 pts/1    S+   10:13   0:00 ./lab_1
dungvd     36161  0.0  0.0      0     0 pts/1    Z+   10:13   0:00 [lab_1] <defunct>
```

After that, the parent waited for the child. The child was no longer a zombie:
```
...
[Parent process] PID: 36160, PPID: 7611, Time: 58
parent waited!
[Parent process] PID: 36160, PPID: 7611, Time: 0
[Parent process] PID: 36160, PPID: 7611, Time: 2
...
```

```bash
$ ps aux | grep lab_1
dungvd     36160  0.0  0.0   2692  1732 pts/1    S+   10:13   0:00 ./lab_1
```

Finally, the parent exited:
```
...
[Parent process] PID: 36160, PPID: 7611, Time: 4
Parent process - PID 36160 exiting
```

```bash
$ ps aux | grep lab_1

```

### Lab 2

### Lab 3