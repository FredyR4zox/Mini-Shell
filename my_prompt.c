//////////////////////////////////////////////////////////
//                                                      //
//               Project I: Mini-Shell                  //
//                                                      //
// Compilation: gcc my_prompt.c -lreadline -o my_prompt //
// Usage: ./my_prompt                                   //
//                                                      //
//////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <errno.h>

#define MAXARGS 100
#define PIPE_READ 0
#define PIPE_WRITE 1

#define MAXLINE 64

typedef struct command {
    char* cmd;              // string with just the command
    int argc;               // number of arguments
    char* argv[MAXARGS+1];  // vector of arguments of the command
    struct command *next;   // pointer to the next command
} COMMAND;

// global variables
char* inputfile = NULL;     // filename (in case of input redirection)
char* outputfile = NULL;    // filename (in case of output redirection)
int background_exec = 0;    // execute in the background (0/1)

// function declarations
COMMAND* parse(char *);
void print_parse(COMMAND *);
void execute_commands(COMMAND *);
void free_commlist(COMMAND *);

// include for the code contained in parser.c (parser of the command line)
#include "parser.c"

int main(int argc, char const *argv[]){
    char* linha;
    COMMAND* com;

    while (1){
        if ((linha = readline("my_prompt$ ")) == NULL)
            exit(0);
        else if(!strncmp(linha, "exit", 5))
            exit(0);
        else if(!strncmp(linha, "cd ", 3))
            chdir(linha + 3);
        else if (strlen(linha) != 0){
            add_history(linha);
            com = parse(linha);
            if (com){
                print_parse(com);
                execute_commands(com);
                free_commlist(com);
            }
        }
        free(linha);
    }
}


void print_parse(COMMAND* commlist){
    int n, i;

    printf("---------------------------------------------------------\n");
    printf("BG: %d IN: %s OUT: %s\n", background_exec, inputfile, outputfile);
    n = 1;
    while (commlist != NULL) {
        printf("#%d: cmd '%s' argc '%d' argv[] '", n, commlist->cmd, commlist->argc);
        i = 0;
        while (commlist->argv[i] != NULL) {
            printf("%s,", commlist->argv[i]);
            i++;
        }
        printf("%s'\n", commlist->argv[i]);
        commlist = commlist->next;
        n++;
    }
    printf("---------------------------------------------------------\n");
}


void free_commlist(COMMAND* commlist){
    if(commlist->next != NULL)
        free_commlist(commlist->next);

    free(commlist);
}

void execute_commands(COMMAND* commlist){

    unsigned int nCommands = 0;
    COMMAND* count = commlist;
    while(count != NULL){
        nCommands++;
        count = count->next;
    }


    pid_t pid[nCommands];
    int fd_prev[2], fd_next[2];

    
    if(pipe(fd_prev) < 0){
        perror("Error");
        exit(1);
    }
    
    for(unsigned int i=0; commlist != NULL; i++){

        if(pipe(fd_next) < 0){
            perror("Error");
            exit(1);
        }

        if((pid[i] = fork()) < 0){
            perror("Error");
            exit(1);
        }
        else if(pid[i] == 0){
            dup2(fd_prev[0], STDIN_FILENO);
            close(fd_prev[0]);
            close(fd_prev[1]);
            close(fd_next[0]);

            if(commlist->next == NULL && outputfile != NULL){
                int fileFD = open(outputfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
                if(fileFD == -1){
                    perror("Error");
                    exit(0);
                }

                dup2(fileFD, STDOUT_FILENO);
                close(fileFD);
            }
            else if(commlist->next != NULL)
                dup2(fd_next[1], STDOUT_FILENO);

            close(fd_next[1]);

            execvp(commlist->argv[0], commlist->argv);

            perror("Error");
            exit(1);
        }
        else{
            close(fd_prev[0]);

            if(inputfile != NULL && i==0){
                int fileFD = open(inputfile, O_RDONLY);
                
                if(fileFD == -1){
                    perror("Error");
                    exit(1);
                }
                
                int n;
                char buf[MAXLINE];
                while((n = read(fileFD, buf, MAXLINE)) > 0)
                    write(fd_prev[1], buf, n);

                close(fileFD);
            }

            close(fd_prev[1]);
        }

        commlist = commlist->next;
        fd_prev[0] = fd_next[0];
        fd_prev[1] = fd_next[1];
    }

    close(fd_next[0]);
    close(fd_next[1]);
    
    if(background_exec == 0){
        for(unsigned int i=0; i<nCommands; i++)
            waitpid(pid[i], NULL, 0);
    }
}
