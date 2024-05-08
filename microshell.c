#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stddef.h>
#include <ctype.h>
#include <string.h>

#define BLUE "\x1b[94m"
#define RED "\e[0;31m"
#define GREEN "\e[0;32m"
#define ORANGE "\e[0;33m"
#define YELLOW "\x1b[93m"
#define RESET "\x1b[0m"
#define SIZE_BUFFER_KOMENDA 2048
#define SIZE_BUFFER_ARG 2048
#define HISTORY_SIZE 10
char help_dir[256];
char old_dir[256];
char cwd[256];
char history[HISTORY_SIZE][2048];
int liczba_history = 0;

int other_program(char **args){
    pid_t pid;

    pid= fork();
    if (pid == 0) {
        execvp(args[0], args);
        if (execvp(args[0], args) == -1) {
            perror("error");
        }
        
    } else if (pid < 0) {

        perror("error");
    } else {
        wait(NULL);
    }

    return 1;
}

void *wczytywanie (){
    
    static char buffer1[SIZE_BUFFER_KOMENDA];
    char *znak;

    znak=fgets(buffer1, SIZE_BUFFER_KOMENDA, stdin);
    return znak;
}

char **parsowanie (char *line){
   
    int position = 0;
    static char *buffer2[SIZE_BUFFER_ARG];
    char *seperatedword;

    seperatedword = strtok(line, " \n");
    while (seperatedword!= NULL) {
        buffer2[position] = seperatedword;
        position++;
        seperatedword = strtok(NULL, " \n");// funkcja strtok kontynuuje działanie, w miejscu w którym przestało poprzednie wywołanie strtok
    }
    buffer2[position] = NULL;
    return buffer2;
}

void add_history(char*input){
    if (liczba_history < HISTORY_SIZE) {
        strcpy(history[liczba_history], input);//kopiowanie input na pierwsze wolne miejsce w historii 
        liczba_history++;
    } else {
        for (int i = 0; i < HISTORY_SIZE - 1; i++) {
            strcpy(history[i], history[i + 1]);// jeśli osiągnęliśmy maksymalny rozmiar historii, to kopiujemy komendy i zmniejszamy indeks o 1 (usuwany najstarsza komendę) 
            }
        strcpy(history[HISTORY_SIZE - 1], input);//dodawanie nowej komendy
    }
    }


void history_command(){
    for (int i = 0; i < liczba_history; i++) {
        printf("%d: %s\n", i + 1, history[i]);// wyświetlamy historie (i+1, żeby zacząć od 1.)
    }
    }

void mkdir_command (char**args){
    if (args[1]==NULL){
        fprintf(stderr,"komenda potrzebuje argumentu\n");
        return;
    }
    int katalog;
    katalog= mkdir(args[1],0777);
    if(katalog==0){
        printf("utworzono katalog");
        } else {
            fprintf(stderr,"error\n");
            return;
        }
    }
    
void exit_command(char **args){
    exit(0);
}

void mv_command(char *from, char* to){
    if (from==NULL){
        fprintf(stderr,"brak pliku zrodlowego\n");
        return;
         }
        if (to==NULL){
            fprintf(stderr,"brak pliku docelowego\n");
            return ;
         }
    int katalog;
    katalog = rename(from, to);

    if(katalog!=0){
        fprintf(stderr,"error\n");
        return ;
         } else {
            printf("katalog zostal przeniesiony\n");
            }
}

void cp_command (char *from, char *to){
    char z;
    FILE *source, *target;
    source = fopen(from, "r");
    target = fopen(to, "w");//tworzony jest nowy plik do pisania, jeżeli plik o tej nazwie istnieje, to wszystkie dane zostają wymazane 
    if( source == NULL ){
        fprintf(stderr, "brak pliku zrodlowego\n");
        return ;
        }
        if( target == NULL ){        
            fprintf(stderr,"brak pliku docelowego\n");
            return ;
            }

    while ((z = fgetc(source)) != EOF) {
        fputc(z, target);
    }

 
    fclose(source);
    fclose(target);

    printf("plik zostal skopiowany\n");
}
void help_command(char **args){
    if (args[1]==NULL){
    printf(BLUE "projekt microshell autor: Sonia Ołubczyńska\n");
    printf(ORANGE "wbudowane funkcje:\n");
    printf(GREEN "cd PATH (przeniesienie do wybranego katalogu)\n");
    printf(RED "cd ~ (przeniesienie do katalogu domowego)\n");
    printf(RED "cd . (przeniesienie do katalogu, w ktorym sie znajdujemy)\n");
    printf(BLUE "cd.. (przeniesienie do katalogu o jeden wyższego)\n");
    printf(BLUE "cd - (przeniesienie do katalogu poprzedniego)\n");
    printf(ORANGE "cp PLIK1 PLIK2 (kopiowanie pliku)\n");
    printf(GREEN "mkdir NAZWA KATALOGU (utworzenie nowego katalogu)\n");
    printf(RED "mv KATALOG1 KATALOG2 (przenniesienie katalogu)\n");
    printf(BLUE "history (pokazanie ostatnich komend)\n");
    printf(ORANGE "exit (wyjscie z microshella)\n" RESET );
    }
    else if (strcmp(args[1],"cd")==0){
    printf(GREEN "cd PATH (przeniesienie do wybranego katalogu)\n");
    printf(RED "cd ~ (przeniesienie do katalogu domowego)\n");
    printf(BLUE "cd - (przeniesienie do katalogu poprzedniego)\n");
    printf(RED "cd . (przeniesienie do katalogu, w ktorym sie znajdujemy)\n");
    printf(BLUE "cd.. (przeniesienie do katalogu o jeden wyższego)\n" RESET);   
    }
    else if(strcmp(args[1],"cp")==0){
    printf(ORANGE "cp PLIK1 PLIK2 (kopiowanie pliku)\n" RESET);  
    }
    else if(strcmp(args[1],"mkdir")==0){
    printf(GREEN "mkdir NAZWA KATALOGU (utworzenie nowego katalogu)\n" RESET);
    }
    else if (strcmp(args[1],"mv")==0){
    printf(RED "mv KATALOG1 KATALOG2 (przenniesienie katalogu)\n" RESET);
    }
    else if(strcmp(args[1],"history")==0){
    printf(BLUE "history (pokazanie ostatnich komend)\n"RESET);
    }
    else if(strcmp(args[1],"exit")){
    printf(ORANGE "exit (wyjscie z microshella)\n" RESET);
    }
    
}
void cd_command(char **args){
    if (args[1] == NULL) {
        fprintf(stderr, "komenda potrzebuje argumentu\n");
        return;
        }
   
    else if (strcmp(args[1],"~")==0){
        chdir(getenv("HOME"));
        }
    else if (strcmp(args[1],"-")==0){
        getcwd(help_dir, sizeof(help_dir));
        chdir(old_dir);
        memcpy(old_dir, help_dir, 256);//kopiujemy help_dir do old_dir
             }
    else {
    if (access(args[1], F_OK) == -1) {
        fprintf(stderr, "Katalog nie istnieje\n");
        return;
    }
    getcwd(old_dir, sizeof(old_dir));
    chdir(args[1]);
}
}
void hello (){
    printf(YELLOW"╔╗─╔╦═══╦╗──╔╗──╔═══╗╔╗\n");
    printf("║║─║║╔══╣║──║║──║╔═╗║║║\n");
    printf("║╚═╝║╚══╣║──║║──║║─║║║║\n");
    printf("║╔═╗║╔══╣║─╔╣║─╔╣║─║║╚╝\n");
    printf("║║─║║╚══╣╚═╝║╚═╝║╚═╝║╔╗\n");
    printf("╚╝─╚╩═══╩═══╩═══╩═══╝╚╝\n"RESET);
    }
    
int main(int argc, char** argv) {
    
    char *input;
    char **args;
    time_t tm;
    time(&tm);
    char *login;
    login = getenv("USER");
    hello();
    
    while(1) {
        printf(ORANGE"\n%s", ctime(&tm));
        printf(RED "[%s>" GREEN "%s]<$" ,login, getcwd(cwd,sizeof(cwd)));
        input= wczytywanie();
        add_history(input);
        args= parsowanie(input);

        if(strcmp(args[0],"cd")==0){
            cd_command(args);
        }
        else if(strcmp(args[0], "help")==0){
            help_command(args);
        }
        else if(strcmp(args[0], "exit")==0){
            exit_command(args);
        }
        else if(strcmp(args[0], "cp")==0){
            cp_command(args[1], args[2]);
        }
        else if(strcmp(args[0], "mv")==0){
            mv_command(args[1], args[2]);
        }
        else if(strcmp(args[0], "mkdir")==0){
            mkdir_command(args);
        }
        else if(strcmp(args[0], "history")==0){
            history_command();
        }
        else other_program(args);
    }
    return 0;
}
