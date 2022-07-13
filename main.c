#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_MAX 3000
#define COMMAND_GET_PROCESSES "wmic.exe path Win32_Process where \"name like 'msedge.exe'\" get Commandline"

//settings
int P = 0;

//TODO
const char* searchengines []= {
    "google",
    "yahoo"
};
const char* prefixes [] = {
    "https://www.google.com/search?q=",
    "https://search.yahoo.com/search?p="
};
const int preflen = sizeof(prefixes)/sizeof(char*);


void edgeradicate(char* query){
    printf("<%s>\n", query);
    system("taskkill /IM msedge.exe /F");

    const int LEN = 6 + strlen(prefixes[P]) + strlen(query) + 1;
    char startcmd[2000];
    strncpy(startcmd, "start ", LEN);
    strcat(startcmd, prefixes[P]);
    strcat(startcmd, query);
    printf("%s", startcmd);
    system(startcmd);
}

int check(){
    FILE *fp;
    int status;
    char path[PATH_MAX];

    char* query;

    fp = popen(COMMAND_GET_PROCESSES, "r");
    if (fp == NULL){
        printf("fp error");
        return 1;
    }

    while (fgets(path, PATH_MAX, fp) != NULL){
        if(strstr(path, "Microsoft.Windows.Search")){
            char* urlstart   = strstr(path, "url=") ;
            query            = strstr(path, "%3D" ) + 3 * sizeof(char);
            char* queryend   = strstr(path, "%26" ) ;

            // error checks
            if( !query ){ printf("an error occured: url"); return 1;}
            if( !queryend   ){ printf("an error occured: &"); return 1;}

            *queryend = '\0';
            
            edgeradicate(query);
            
            break;
        }
    }
    
    status = pclose(fp);

    return 0;        
}


int main(int argc, char const *argv[]){
    system("taskkill /IM msedge.exe /F");

    printf("%d\n", argc);
    if(argc == 3){
        printf("| %s %s", argv[1], argv[2]);
        //TODO
    }

    while( 1 ){
        check();
    }
}
