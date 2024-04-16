//builtin_commands.h
//Defines and initializes the global builtin command struct
//Contains all builtin command functions

//Builtin command struct defs
typedef struct command{
    char* name;
    int (*fn)(char*);
}command_t;

typedef struct builtin_commands{
    command_t* commands[10];
    int capacity;
    int count;
}builtin_commands_t;

typedef struct stat_tracker{
    int commands;
    int args;
    int pipes;
    int help;
    int procs;
    int fails;
}stat_tracker_t;

//Global command struct
//Note: This is global so that it can be cleared in SIGINT handler
builtin_commands_t* builtin_commands; 
stat_tracker_t* stat_tracker;

int BUFSIZE = 10000;
int MAX_COMMANDS = 10;
int MAX_ARGS = 10;
int MAX_ARG_LENGTH = 100; //Also applies to commands

//Forward declarations
builtin_commands_t* new_builtin_commands();
int add_command(char* cmd, void* fn_pointer);

//=================================================
//Define Builtin command functions here
//Builtin commands run sandboxed in child processes

//help builtin command
//Shows a help dialog
int cmd_help(char* input) {
    printf("| cd: change directory to arg 0\n");
    printf("| stats: see your stats for this session\n");
    printf("| help: see this menu\n");
    printf("| exit: exit Mini-Shell\n");
    return 0;
}

//exit builtin command
//Exits the shell
int cmd_exit(char* input) {
    return 99;
}

//cd builtin command
//Unlike other commands, runs in parent process
//(See override in run func (shell_processes.h) )
int cmd_cd(char* input) {

    chdir(input);
    
    char b[BUFSIZE];
    
    printf("Changed directory to %s\n",getcwd(b,BUFSIZE));
    
    return 0;
}

//Print shell user stats
int cmd_stats(char* input) {
    
    printf("| YOU HAVE ENTERED [%d] COMMANDS\n",stat_tracker->commands);
    printf("| YOU HAVE USED [%d] ARGUMENTS\n",stat_tracker->args);
    printf("| YOU HAVE FLUSHED [%d] PIPES\n",stat_tracker->pipes);
    printf("| YOU HAVE YELLED FOR HELP [%d] TIMES\n",stat_tracker->help);
    printf("| YOU HAVE WITNESSED [%d] PROCESSES FAIL\n",stat_tracker->fails);

    return 0;
}


//Initialize the global builtin_commands struct
void initialize_builtin_commands() {
    //Init builtin_commands
    builtin_commands = new_builtin_commands();

    //Add new commands here
    add_command("help",&cmd_help);
    add_command("exit",&cmd_exit);
    add_command("cd",&cmd_cd);
    add_command("stats",&cmd_stats);
}

// =============================================

//Make a new builtin commands struct
builtin_commands_t* new_builtin_commands() {
    
    builtin_commands_t* c = malloc(sizeof(builtin_commands_t));
    c->capacity = 10;
    c->count = 0;

    int i=0;
    for(i;i<(c->capacity);i++) {
        (c->commands)[i] = malloc(sizeof(command_t));
    }
    return c;
}

//Add a builtin command with name / function pointer to builtin_commands
int add_command(char* cmd_name, void* fn_pointer) {

    builtin_commands_t* c = builtin_commands;

    if(c == NULL) {
        return 1;
    } else {
       if(c->count == c->capacity) {
          return 1;
       } else {
          ((c->commands)[c->count])->name = cmd_name;
          ((c->commands)[c->count])->fn = fn_pointer;
          c->count = c->count + 1;
          return 0;
       }
    }
}

//Get a command for the specified name (if it exists as a builtin)
command_t* get_builtin_command(char* cmd_name) {
    
    builtin_commands_t* c = builtin_commands;

    if(c == NULL) {
        return NULL;
    } else {
        int i=0;

        for(i;i<(c->count);i++) {
            
            char* itcn = (c->commands)[i]->name;
            if(strcmp(itcn,cmd_name) == 0) {
                return (c->commands)[i];
            }
        }
     
        return NULL; //Return null if not found
    }
}

//Frees the global builtin commands struct
void free_builtin_commands() {
    
    int i=0;
    for(i;i<(builtin_commands->capacity);i++) {
        free((builtin_commands->commands)[i]);
    }
    
    free(builtin_commands);
}

//Initialze the global stat tracker struct
int initialize_stat_tracker() {
    stat_tracker = (stat_tracker_t*)malloc(sizeof(stat_tracker_t));

    if(stat_tracker == NULL) {
        return 1;
    } else {
        stat_tracker->commands=0;
        stat_tracker->args=0;
        stat_tracker->pipes=0;
        stat_tracker->help=0;
        stat_tracker->fails=0;

        return 0;
    }
}

//Free the global stat tracker struct
void free_stat_tracker(){
    free(stat_tracker);
}
