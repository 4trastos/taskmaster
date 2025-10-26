#include "taskmaster.h"

void set_default_config(t_program_config* config){
    config->command = "";
    config->stderr_path = "";
    config->stdout_path = "";
    config->umask = "";
    config->env = NULL;
}