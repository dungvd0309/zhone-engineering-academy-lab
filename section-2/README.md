# Section 2

- Section: Linux System Programming
- Topic: C Programming Language Review
- Duration: 3 working days (8 hours/day) 

## 1. C Syntax Essentials

### 1.1. Variables

Syntax:

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

Data type:

|Type|Meaning|
|-|-|
|int|dwqdw
|float|

### 1.2. Control flow

Conditionals (`if-else`, `switch case`)
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

Loop (`for, while, do-while`):
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

Functions

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
<img src="./img/pointer1.png">

### 2.2. Pointer arithmetic

