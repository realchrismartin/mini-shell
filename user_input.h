//user_input.h
//Manages creation of user input structs
//Struct which represents a series of user commands to be piped together
//Has a static max # of 10 commands each with 10 args (can be changed here)

typedef struct input{
    char* commands[10][10];
    int commandCount;
    int commandCapacity;
    int argCount;
}input_t;

//Create a new input struct
input_t* new_input() {

    input_t* i = (input_t*)malloc(sizeof(input_t));

    if(i == NULL) {
        return NULL;
    } else {
        
        int x=0;
        int y=0;
        
        for(x=0;x < MAX_COMMANDS;x++) {
            for(y=0; y < MAX_ARGS ;y++) {
                (i->commands)[x][y] = (char*)malloc(MAX_ARG_LENGTH * sizeof(char));
            }
        }

        i->argCount = MAX_ARGS;
        i->commandCount = 0;
        i->commandCapacity = MAX_COMMANDS;
        return i;
   }
}

//Add an input entry to an input struct.
//This will usually consist of a single command and arguments
int add_input_entry(input_t* input, char* cmd) {

   if(input == NULL) {
      return 1;
   } else {

       if(input->commandCount >= input->commandCapacity) {
           return -1;
       } else {
           
           char entry[MAX_ARG_LENGTH * MAX_ARGS];
           strcpy(entry,cmd);
           
           int args = 0;

           //Remove newline
           char *newline = strchr( entry, '\n' );
           if(newline) { 
                *newline = 0;
           }

           int tn = 0;
           int index = input->commandCount;
           char* tok = strtok(entry," ");

           while(tok != NULL) {     
              
              if(tn > 0) {
                stat_tracker->args++;
              }

              if(tn >= input->argCount) {
                return 1;
              } else {
                if(tok != NULL) {
                    strcpy((input->commands)[index][tn],tok); 
                }
                tn++;
                tok = strtok(NULL," ");
              }
           }
            
           int it=tn;
           
           //Free and NULL out all unused argument pointers
           for(it;it<input->argCount;it++) {
                 free((input->commands)[index][it]);
                 (input->commands)[index][it] = NULL;
           }

           input->commandCount = input->commandCount + 1;
        }

        return 0;
    }
}

//Wait for input, returning an input struct based on
//the user's entries
input_t* get_user_input() {
      
        printf("mini-shell> ");
        char buf[MAX_ARG_LENGTH * MAX_ARGS];
        fgets(buf, sizeof buf, stdin);
     
        //Remove newline
        char *newline = strchr( buf, '\n' );
        if(newline) {
            *newline = 0;
        }

        input_t* i = new_input();
        char* tok = strtok(buf,"|");
        int in = 0;
        
        char toks[MAX_ARGS][MAX_ARG_LENGTH];

        while(tok != NULL) {
            
            if(in > 0) {
                stat_tracker->pipes++;
            }
            
            in++;

            if(in > MAX_ARGS) {
                printf("Too many pipe statements!");
                return NULL;
            }

            strcpy(toks[in],tok);   
           
            tok = strtok(NULL,"|");
        }

        //Add tokens
        int it=1;
        for(it;it<=in;it++) {
           add_input_entry(i,toks[it]);
        }

        return i;
}

//Free an input struct
void free_input(input_t* i) {

    int x=0;
    int y=0;

    for(x=0;x<MAX_COMMANDS;x++) {
        for(y=0;y<MAX_ARGS;y++) {
            free((i->commands)[x][y]);
            
        }
    }
    
    free(i);
}
