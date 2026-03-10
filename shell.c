#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>


#define MAX_CMD 4096
#define MAX_ARGS 256

//vai remover os espaços do inicio e fim das strings
char *trim(char *s){
    while (*s== ' ' || *s=='\t') s++;
    char *e=s+strlen(s)-1;
    while(e>s&& (*e== ' '||*e=='\t'|| *e=='\n'||*e=='\r'))
    *e--='\0';
    return s;
}

//vai separar a linha em argumentos
void parse(char *line,char **args,char **in,char **out,char **app){
    
    *in=NULL;
    *out=NULL;
    *app=NULL;
    int i=0;
    char *token =strtok(line," \t\n");
   
    while (token !=NULL){
        if(!strcmp(token, "<")){
            *in=strtok(NULL, " \t\n");
        }else if(!strcmp(token, ">>")){
            *app=strtok(NULL, " \t\n");
        }else if(!strcmp(token, ">")){
            *out=strtok(NULL, " \t\n");
        }else{
            args[i++]=token;
        }
        token=strtok(NULL," \t\n");
    }
    args[i]=NULL;
}

int tem_pipe(char *cmd){
    return strchr(cmd,'|')!=NULL;
}

//vai execultar apenas um comando
void execultar(char *cmd){
    char *args[MAX_ARGS];
    char *in, *out, *app;
    parse(cmd, args, &in, &out, &app);

    if(args[0]==NULL)return;

    //printf("DEBUG args[0] = '%s'\n", args[0]); 

    if(strcmp(args[0],"cd")==0){
        char *dir=args[1];
        if(dir==NULL)dir=getenv("HOME");
        if(chdir(dir)<0){
            perror("cd");
        }
        return;
    }
    
    pid_t pid=fork();
    if(pid<0){
        perror("fork");
        return;
    }
    if(pid==0){
        if(in!=NULL){
            int fd=open(in,O_RDONLY);
            if(fd<0){perror(in);exit(1);}
            dup2(fd,0);
            close(fd);
        }
        if(app!=NULL){
            int fd=open(app,O_WRONLY|O_CREAT|O_APPEND,0644);
            if(fd<0){perror(app);exit(1);}
            dup2(fd,1);
            close(fd);
        }
        else if(out !=NULL){
            int fd=open(out,O_WRONLY|O_CREAT|O_TRUNC,0644);
            if(fd<0){perror(out);exit(1);}
            dup2(fd,1);
            close(fd);
        }

        execvp(args[0],args);
        fprintf(stderr,"shell: %s: comando não encontrado\n",args[0]);
        exit(1);
    }
    else{
        wait(NULL);
    }

}
void execultar_pipe(char *cmd){
    char *saveptr;
    char *cmd1=__strtok_r(cmd,"|",&saveptr);
    char *cmd2=__strtok_r(NULL,"|",&saveptr);

    cmd1=trim(cmd1);
    cmd2=trim(cmd2);

    int fd[2];
    if(pipe(fd)<0){
        perror("pipe");
        return;
    }
    pid_t pid1=fork();
    if(pid1==0){
        dup2(fd[1],1);
        close(fd[0]);
        close(fd[1]);
        execultar(cmd1);
        exit(0);
    }
    pid_t pid2=fork();
    if(pid2==0){
       dup2(fd[0],0);
        close(fd[0]);
        close(fd[1]);
        execultar(cmd2);
        exit(0);
    }
    close(fd[0]);
    close(fd[1]);
    waitpid(pid1,NULL,0);
    waitpid(pid2,NULL,0);
}



int main(){
    char line[MAX_CMD];
    //char *args[MAX_ARGS];

    while(1){
        //mostrar prompt
        printf("myPrompt seq> ");
        fflush(stdout);

        //vai ler a linha digitada
        if (!fgets(line,sizeof(line),stdin)){
            //ctrl-d vai encerrar o shell
            printf("\n");
            break;
        
        }
       //printf("DEBUG linha: '%s'\n", line);

        //vai ignorar a linha vazia
        if (line[0]=='\n') continue;
        if (strncmp(line,"exit",4)==0)exit(0);

        //divide a linha pelo ;
        char*saveptr;
        char *cmd=strtok_r(line, ";",&saveptr);
        while(cmd!=NULL){
            char *t=trim(cmd);
            if (strlen(t)>0){
                execultar_pipe(t);
            }else{
                execultar(t);
            }
            cmd=strtok_r(NULL,";",&saveptr);
        }
    }
    return 0;
}