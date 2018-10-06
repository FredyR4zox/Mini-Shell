# Mini-Shell
First project for the subject Operating Systems, which consisted in developing a basic shell.


### Compilation
``` bash
$ gcc my_prompt.c -lreadline -o my_prompt
```

### Usage
The following commands where implemented:
* cd
* exit
* input/output redirection - |, <, >
* background execution - &
#### Example:
``` bash
$ ./my_prompt
my_prompt$ cat < my_prompt.c | grep fork > test.txt
```

### Files
* my_prompt.c - Program made by me.
* parser.h - Parser given by the teacher.


### Authors
* Frederico Emanuel Almeida Lopes - up201604674 - [FredyR4zox](https://www.github.com/FredyR4zox)
