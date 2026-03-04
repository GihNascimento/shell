#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


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
void parse(char *line,char**args){
    int i=0;
    char *token =strtok(line," \t\n");
   
    while (token !=NULL){
        args[i++]=token;
        token= strtok(NULL," \t\n");
    }
    args[i]=NULL;
}

//vai execultar apenas um comando
void execultar(char *cmd){
    char *args[MAX_ARGS];
    parse(cmd,args);

    if(args[0]==NULL)return;
    
    pid_t pid=fork();
    if(pid<0){
        perror("fork");
        return;
    }
    if(pid==0){
        execvp(args[0],args);
        fprintf(stderr,"shell: %s: comando não encontrado\n",args[0]);
        exit(1);
    }
    else{
        wait(NULL);
    }

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
       // printf("DEBUG linha: '%s'\n", line);

        //vai ignorar a linha vazia
        if (line[0]=='\n') continue;
        if (strncmp(line,"exit",4)==0)exit(0);

        //divide a linha pelo ;
        char*saveptr;
        char *cmd=strtok_r(line, ";",&saveptr);
        while(cmd!=NULL){
            char *t=trim(cmd);
            if (strlen(t)>0){
                execultar(t);
            }
            cmd=strtok_r(NULL,";",&saveptr);
        }
    }
    return 0;
}