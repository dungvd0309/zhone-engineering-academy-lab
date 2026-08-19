# Section 2

- **Section**: Linux System Programming
- **Topic**: C Programming Language Review
- **Duration**: 3 working days (8 hours/day) 

## 1. C Syntax Essentials

### 1.1. Variables

### Syntax

``` C
// Declare
// type variableName;
// type variableName = value;

int age = 10; 
char ch = 'a';
float temperature;

// Assign 
// variableName = value;

age = 99;
temperature = 20.04;
```

### Data types

|Type|Meaning|Size
|-|-|-|
|int|Integer|4 bytes|
|float|Decimal numbers|4 bytes|
|double|Decimal numbers, more precision than float|8 bytes|
|char|Single character|1 byte|
|bool|`true`/`false`|1 byte|

### 1.2. Control flow

#### Conditionals (`if-else`, `switch case`)
```C
if(gpa >= 3.6) 
    printf("Xuất sắc");
else if(gpa >= 3.2)
    printf("Giỏi");
else 
    printf("Fail CV!");
```
```C
switch (grade):
    case 'A':
        printf("Giỏi");
        break; 
    case 'B':
        printf("Khá");
        break;
    default:
        printf("Fail CV");
```

#### Loop (`for, while, do-while`):
```C
for (int i = 0; i < 5; i++) {
    printf("%d ", i); // 0 1 2 3 4 
}
```

```C
int count = 0;
while (count < 5) {
    count++;
}
```

```C
int x = 10;
do {
    x++;
} while (x < 5);
```

### 1.3. Functions

#### Syntax

Function declaration:
```
return_type function_name(parametersType parameters);
```
```C
void hello(void);
int addNumbers(int a, int b);
```

Function definition:
```
return_type function_name(parametersType parameters) {
  // function body
}
```
```C
void hello(void)
{
    printf("Hello, world!\n");
}
int addNumbers(int a, int b) {
    return a + b;
}
```
Function call:
```
function_name(args);
```
```C
hello(); // Prints "Hello, world!"
int result = addNumbers(5, 3); 
printf("%d", result) // 8
```

- return_type: The data type of the value the function returns (`int`, `float`). `void` if the function does not return any value

## 2. Pointers and arrays

### 2.1. Pointers

Pointers are **variables** that store memory address of another variable.

Size of pointers are depended on the system architecture.
- 32-bit Systems uses 4 bytes pointers.
- 64-bit Systems uses 8 bytes pointers.

Example:

```C
int num = 90;
int *p = &num; // assuming 64-bit Systems, pointer p will take 8 bytes
```
[![pointer1.png](http://10.0.0.220:9090/uploads/images/gallery/2026-08/scaled-1680-/pointer1.png)](http://10.0.0.220:9090/uploads/images/gallery/2026-08/pointer1.png)

### 2.2. Pointer manipulation

#### Basic manipulation

```C
int x = 10;
int *p = &x; 

printf("%d\n", *p);  // 10

*p = 20;             
printf("%d\n", x);   // 20
```
- `int *p`: initializes `p` as a int pointer (`int*` type)
- `&x`: gets the address of `x`
- `*p`: accesses the value of the memory that `p` is holding

#### Pointer arithmetic

Pointer arithmetic can move a pointer to a type to the next item of that type.
```C
// assuming 64-bit Systems
int16_t a[5] = {11, 22, 33, 44, 55};
int16_t *p = &a[0];
printf("%d\n", *p); // 11
```
[![pointer2.png](http://10.0.0.220:9090/uploads/images/gallery/2026-08/scaled-1680-/pointer2.png)](http://10.0.0.220:9090/uploads/images/gallery/2026-08/pointer2.png)

```C
p++;
printf("%d\n", *p); // 22
```

[![pointer3.png](http://10.0.0.220:9090/uploads/images/gallery/2026-08/scaled-1680-/pointer3.png)](http://10.0.0.220:9090/uploads/images/gallery/2026-08/pointer3.png)

```C
p += 2;
printf("%d\n", *p); // 44
```
[![pointer4.png](http://10.0.0.220:9090/uploads/images/gallery/2026-08/scaled-1680-/pointer4.png)](http://10.0.0.220:9090/uploads/images/gallery/2026-08/pointer4.png)

### 2.3. Pointers to Pointers

Pointers to Pointers are when **pointers hold the addresses of other pointers**.

```C
char c = 'a';  // Type: char
char *p = &c;   // Type: pointer to a char
char **q = &p;  // Type: pointer to pointer to char

printf("%c %c\n", *p, **q);  // a a
```

[![pointer5.png](http://10.0.0.220:9090/uploads/images/gallery/2026-08/scaled-1680-/pointer5.png)](http://10.0.0.220:9090/uploads/images/gallery/2026-08/pointer5.png)

```C
**q = 'b' // change the value of the variable c through a two-star pointer

printf("%c %c\n", *p, **q);  // b b
```

## 3. Structs, unions, and typedefs

### 3.1. Structs
- A `struct` is a user-defined type that holds multiple pieces of data.
- To bundle multiple variables into a single object.

#### Syntax

Declare `struct car` type:
```C
struct car {
    char *name;
    float price;
    int speed;
};
```

Declare a variable of `struct car` type:
```C
struct car vinfast; 
```

Initialize the variable:
```C
vinfast.name = "VinFast VF3";
vinfast.price = 300.3;
vinfast.speed = 175;
```

Initializers:
```C
struct car vinfast = {"VinFast VF3", 300.3, 175};
```

#### Passing structs by value

The original struct won't be modified, but an independent copy will be created.
```C
void print_price(struct car c) {
  printf("%f", c.price); // 300.3
}
```

But this won't work if the data needed to be modified:
```C
void set_price(struct car c, float new_price) {
    c.price = new_price; 
}
```
```C
printf("%f", c.price); // 300.3

set_price(vinfast, 999);

printf("%f", c.price); // Still 300.3
```

#### Passing structs by pointer

Used for modifying the original struct or avoiding copying a large struct.
```C
void set_price(struct car *c, float new_price) {
    c->price = new_price; 
    // use '->' instead of '.' when accessing data from a struct pointer
}
```
```C
printf("%f", c.price); // 300.3

set_price(&vinfast, 999); // pass the struct address

printf("%f", c.price); // 999
```

### 3.2. Unions
- Basically just like structs, except the fields overlap in memory.
```C
union foo {
    int a, b, c, d, e, f;
    float g, h;
    char i, j, k, l;
};
```
- If this were a struct, it would took 36 bytes to hold it all. But it's a union, all those fields overlap in teh same strech of memory => only take 4 bytes.
- Downside: can only portably use one of those fields at a time 

### 3.3. Typedefs
Used for making new types as getting new names for existing types.

```C
typedef int antelope;  // Make "antelope" an alias for "int"

antelope x = 10;       // Type "antelope" is the same as type "int"
```

typedef and structs:
```C
struct animal {
    char *name;
    int leg_count, speed;
};

//  original name      new name
//            |         |
//            v         v
//      |-----------| |----|
typedef struct animal animal;

struct animal y;  // This works
animal z;         // This also works because "animal" is an alias
```

```C
//  original name
//            |
//            v
//      |-----------|
typedef struct animal {
    char *name;
    int leg_count, speed;
} animal;                         // <-- new name

struct animal y;  // This works
animal z;         // This also works because "animal" is an alias
```

```C
//  Anonymous struct! It has no name!
//         |
//         v
//      |----|
typedef struct {
    char *name;
    int leg_count, speed;
} animal;                         // <-- new name

//struct animal y;  // ERROR: this no longer works--no such struct!
animal z;           // This works because "animal" is an alias
```

## 4. Dynamic memory management

### 4.1. malloc()

- `malloc()` (memory allocation) is used to allocate a single block of contiguous memory on the heap at runtime.
- The allocated memory is **uninitialized**, meaning it contains garbage values.
- The function returns **a pointer to the allocated memory**. On failure, it returns `NULL`.

```C
int *arr = malloc(5 * sizeof(int));
if (x == NULL) {
  printf("Error allocating\n")
}
```

### 4.2. calloc()

- `calloc()` (contiguous allocation) allocates memory for multiple elements.
- It initializes the allocated memory to **zero**.
- The function returns **a pointer to the allocated memory**. On failure, it returns `NULL`.

```C
int *arr = calloc(5, sizeof(int));
if (x == NULL) {
  printf("Error allocating\n")
}
```

### 4.3. realloc()

- `realloc()` is used to resize a previously allocated memory block.
- The function returns **a pointer to the allocated memory**. On failure, it returns `NULL`.

```C
int *ptr = (int *)malloc(5 * sizeof(int));

// Reallocation
int *temp = (int *)realloc(ptr, 10 * sizeof(int));

// Only update the pointer if reallocation is successful
if (temp == NULL)
    printf("Memory Reallocation Failed\n");
else
    ptr = temp;
```


### 4.4. free()

- `free()` releases dynamically allocated memory by `malloc()` and `calloc()`

```C
int *ptr = (int *)calloc(5, sizeof(int));
    
for (int i = 0; i < 5; i++)
  printf("%d ", ptr[i]); 
    
// Free the memory after completing operations
free(ptr);
ptr = NULL; 
```

### 4.5. Common pitfalls 
#### Leaks
Occurs when dynamically allocated memory is not released properly.
```C
int *p = malloc(sizeof(int));

p = NULL; // We lost the memory address
        // => The memory can no longer be accessed or freed
```

#### Double-free
Occurs when a pointer got free twice.
```C
int *ptr = (int *)calloc(5, sizeof(int));
free(ptr); // OK
free(ptr); // Aborted (core dumped)
```

#### Dangling pointers
Free the memory without setting the pointer to NULL
```C
int *ptr = (int *)calloc(5, sizeof(int));
    
for (int i = 0; i < 5; i++)
  printf("%d ", ptr[i]); // 0 0 0 0 0 since we use calloc()
    
free(ptr); 
// no assigning NULL to ptr => ptr still holds the old address
// => Dangling pointers

for (int i = 0; i < 5; i++)
  printf("%d ", ptr[i]); // -1504568300 5 -1778696097 -377398520 0 
```

## 5. Compilation pipeline

`Source File -> Preprocessor -> Compiler -> Assembler -> Linker`

```
.c (Source File)
 ↓  preprocess
.i (Preprocessed File)
 ↓  compile
.s (Assembly code)
 ↓  assemble
.o (Object code)
 ↓  link
Executable File
```

```bash
gcc -E main.c -o main.i   # Preprocess

gcc -S main.i -o main.s   # Compile

gcc -c main.s -o main.o   # Assemble

gcc main.o -o main        # Link
```

### Preprocessor

### Compiler proper

### Assembler

### Linker

## 6. Basic data structures
## 7. Function pointers and callback-style APIs