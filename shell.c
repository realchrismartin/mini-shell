#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "builtin_commands.h"
#include "user_input.h"
#include "shell_processes.h"

int main(){
    
    printf("_____________________________\n");
    printf("| [ Welcome to Mini-Shell ] |\n");
    printf("[Ctrl+C to Quit][Help][Exit]\n");  
	
    //Setup sigint
    signal(SIGINT, sigint_handler);
    
    //Init builtin commands
    initialize_builtin_commands();
    
    //Init stat tracker
    initialize_stat_tracker();

    int running = 0;

    char b[BUFSIZE];
    
    //Run until a child process spawned by 'run' returns code 99
    while(running != 1){
        
        //Reset the buffer
        memset(b,0,sizeof b);

        //Get user input
        input_t* in = get_user_input(); 

        if(in == NULL) {           
            printf("failed to get user input, exiting...");
            exit(1);
        } 

        //Run the specified command(s)
        running = run(in,b,sizeof b);
       
        //If exit code 99 is returned (exit command), exit.
        if(running == 99) {
            free_input(in);
            free_stat_tracker();
            free_builtin_commands();
            printf("see you later space cowboy ... ");	
            return 0;
        }

        //Print the buffer
        printf("%s\n",b);

        //Dump old input
        free_input(in);
    }
}
