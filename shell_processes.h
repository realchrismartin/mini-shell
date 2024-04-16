//Signal Interrupt Handler
void sigint_handler(int sig){
    free_builtin_commands(); //Frees global builtins
    free_stat_tracker(); //Frees global stat tracker
	exit(0);
}

//Run all commands specified in input_t commands
//Pipe commands together
//Write final command output to out_buffer
int run(input_t* in, void* out_buffer, int buffer_size) {
    
    int res=0; //Result of this function
    int argCount = in->argCount;
    int count = in->commandCount;
    
    //Initialize pipes for all child processes
    int pipes[count-1][2];

    int i=0;
    for(i=0;i<count;i++) {
        pipe(pipes[i]);
    }

    //Create a subprocess for each child command,
    //redirecting I/O using pipes
    for(i=0;i<count;i++) {
        
       int pid;
       int dorun=0; 
        //Create a subprocess for each child command,
        //redirecting I/O using pipes
        char* pr = in->commands[i][0];
        stat_tracker->commands++;

        //Exception: 'cd' needs to occur in the parent thread
        if(strcmp(pr,"cd") == 0) {
            command_t* b = get_builtin_command(pr);
                
                if(b != NULL) {
                    return (b->fn)(in->commands[i][1]);    
                }
        } else if(strcmp(pr,"help") == 0) {
           stat_tracker->help++; 
        }
        
        //Create Child process
        switch(pid = fork()) {
            int status;
            //Child case: Execute the ith command
            case 0: 

                if(i == 0) {
                    close(pipes[i][0]); // Close read end of initial pipe
                    dup2(pipes[i][1],1); //Output to write end
                } else {
                    dup2(pipes[i-1][0],0); //Input to read end from prior
                    dup2(pipes[i][1],1); //Output to write end
                }
                

                int c_res = 0; // Child result

                //Try to find a builtin with the specified name
                //If not found, try to execute something from /bin
                command_t* b = get_builtin_command(pr);
                
                if(b != NULL) {
                    if(i == 0) {
                        //First command case: Pass argument 1 to builtin
                        c_res = (b->fn)(in->commands[i][1]);    
                    } else {
                        //Pipe case: pass stdout to builtin
                        char argbuffer[9000];
                        if(read(0,argbuffer,9000) > 0) {
                            c_res = (b->fn)(argbuffer);
                        } else {
                            c_res = 1;
                        }
                    }   
                } else {
                    //Attempt to execute the command from /bin
                    char* path = (char*)malloc(9000*sizeof(char));
                    path[0] = '\0'; //Null terminate path
                    strcat(path,"/bin/");
                    strcat(path,pr);
                    
                    execve(path,(in->commands)[i],NULL);
                    free(path);
                    c_res = 1;
                }
                
                //Free child's copy of parent structs
                free_stat_tracker();
                free_builtin_commands();
                free_input(in);
                
                //Exit with result code
                exit(c_res);

            //Parent Process Case: Write final output to output buffer and close pipes
            default:

                //Wait on children, return 2 if any failed
                waitpid(pid, &status, 0); 
  
                if(WIFEXITED(status)) { 
                    int stat = WEXITSTATUS(status); 
                    //Only return 2 if exit command is executed
                    if(stat > 1) {
                        res = stat;
                        stat_tracker->fails++;
                    }
                }

                if(i > 0) {
                  close(pipes[i-1][0]); //Close read end of prior
                }
                
                close(pipes[i][1]); //Close write end of current
                

                //If this is the last process, close read of current and return
                if(i == (count - 1)) {
                    close(pipes[i][1]);
                    read(pipes[i][0],out_buffer,buffer_size);
                   
                    //Return the function result
                    return res;
                }
                
                break;
            case -1:
                perror("Error on fork");
                return 1;
        }
    }
}


