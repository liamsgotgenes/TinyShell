#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE 80
#define true 1
#define false 0

int main(){
	char *args[MAX_LINE/2+1];
	while (1){
        char in[256];
        int bg=false;
        pid_t pid;
        int piped=false;
        int index;
        int redirect=false;

		printf("shell> ");
        fgets(in,MAX_LINE,stdin);
        int i=0;
        char* tok=strtok(in," ");
        while (tok!=NULL){
            args[i++]=tok;
            if (!strcmp(tok,"|")){
                piped=true;
                index=i-1;
                args[i-1]=NULL;
            }
            if (!strcmp(tok,">")){
                redirect=true;
                index=i-1;
                args[i-1]=NULL;
            }
            tok=strtok(NULL," ");
        }
        strtok(args[i-1],"\n");
        if (!strcmp(args[i-1],"&")){
            bg=true;
            i--;
        }
        args[i]=NULL;
        if (!strcmp(args[0],"fg")){wait(NULL);}
        if (!strcmp(args[0],"exit")){
            exit(0);
        }
        if (!strcmp("cd",args[0])){
            chdir(args[1]);
            continue;
        }
		if ((pid=fork())==0){
            if (redirect){
                int filed=open(args[index+1], O_CREAT | O_WRONLY, 0644);
                if (filed<0){
                    perror("open");
                }
                dup2(filed,1);
                if ((execvp(args[0],args))==-1){
                    exit(0);
                }
            }
            else if (piped){
                int fd[3];
                pipe(fd);
                pid=fork();
                if (pid==0){
                    dup2(fd[1],1);
                    close(fd[0]);
                    if ((execvp(args[0],args))==-1){
                        exit(0);
                    }
                }
                else{
                    char *p=&args[index+1];
                    dup2(fd[0],0);
                    close(fd[1]);
                    if ((execvp(args[index+1],p))==-1){
                        exit(0);
                    }
                }
            }
            else{
                if ((execvp(args[0],args))==-1){
                    exit(0);
                }
            }
		}
        else{
            if (!bg){wait(-1);}
            else{
                int j=0;
                while (args[j]!=NULL){printf("%s ",args[j++]);}
                printf("& (PID: %d)\n",pid);
            }
        }
        fflush(stdout);
	}
}
