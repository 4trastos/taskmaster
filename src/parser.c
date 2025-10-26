#include "taskmaster.h"
#include "parse_utils.h"
#include "ft_printf.h"

static int check_is_name(char *str){
    if (str[0] != ' '){
        return 1;
    } else if (str[1] != ' '){
        return 1;
    } else if (str[2] == ' ' ){
        return 1;
    } else {
        return 0;
    }
}

static char *get_field(char *str){
    int start = 0;
    int end;

    if (ft_strlen(str) < 8){
        return NULL;
    }

    for (int i = 0; str[i] == ' '; i++){
        start++;
    }

    if (start != 4){
        return NULL;
    }
    
	end = start;
    for (int i = start; str[i] != ':'; i++){
        end++;
    }

    return ft_substr(str, start, end - start);
}

static char *get_field_value(char *str) {
    //ft_printf("LINE: %s", str);
    int start = 0;
    int end;

    for (int i = 0; str[i] != ':'; i++){
        start++;
    }
    start += 2;
    if (str[start] == '"'){
        start++;
        
		end = start;
        for (int i = start; str[i] != '"'; i++){
            end++;
        }

        if (start == end){
            return NULL;
        }
        //ft_printf("inicio: char[%i] = %c, fin: char[%i] = %c\n", start, str[start], end, str[end]);
        return ft_substr(str, start, end - start);
    }

	end = start;
    for (int i = start; str[i] != '\n'; i++){
        end++;
    }
    //ft_printf("inicio: char[%i] = %c, fin: char[%i] = %c\n", start, str[start], end, str[end]);
    
    if (start == end){
        return NULL;
    }

    return ft_substr(str, start, end - start);
}

static int check_line_is_useful(char *line){
	int i = 0;
	while (line[i] == ' '){
        i++;
        if (line[i] == '\0')
            break;
		if (line[i] == '#')
			return 1;
		if ((line[i] >= 'a' && line[i] <= 'z') || (line[i] >= 'A' && line[i] <= 'Z'))
			return 0;
	}
	return 1;
}

int get_number_of_program(char *filename) {
    int     fd;
    int     n;
    char    *line;
    
    fd = open(filename, O_RDONLY);

    n = 0;
    while (get_next_line(fd, &line)){
        if (line[0] != '#' && line[0] != '\n' && ft_strlen(line) > 1 && check_line_is_useful(line) == 0){
            if (check_is_name(line) == 0){
                n++;
            }
        }
    }
    close(fd);
    
    return n;
}

static t_autorestart get_autorestart_value(char *str) {
    if (ft_strcmp(str, "always") == 0) {
        return AR_ALWAYS;
    } else if (ft_strcmp(str, "never") == 0) {
        return AR_NEVER;
    } else {
        return AR_UNEXPECTED;
    }
}

static void fill_field(t_program_config *config, char *field, char *field_value){
    if (ft_strcmp(field, "cmd") == 0) {
        config->command = ft_strdup(field_value);
    } else if (ft_strcmp(field, "numprocs") == 0) {
        config->numprocs = ft_atoi(field_value);
    } else if (ft_strcmp(field, "autostart") == 0) {
        config->autostart = ft_atob(field_value);
    } else if (ft_strcmp(field, "autorestart") == 0) {
        config->autorestart = get_autorestart_value(field_value);
    } else if (ft_strcmp(field, "exitcodes") == 0) {
        config->exitcodes = get_exit_codes(field_value);
    } else if (ft_strcmp(field, "startretries") == 0) {
        config->startretries = ft_atoi(field_value);
    } else if (ft_strcmp(field, "starttime") == 0) {
        config->starttime = ft_atoi(field_value);
    } else if (ft_strcmp(field, "stopsignal") == 0) {
        config->stopsignal = get_signal_from_str(field_value);
    } else if (ft_strcmp(field, "stoptime") == 0) {
        config->stoptime = ft_atoi(field_value);
    } else if (ft_strcmp(field, "stdout") == 0) {
        config->stdout_path = ft_strdup(field_value);
    } else if (ft_strcmp(field, "stderr") == 0) {
        config->stderr_path = ft_strdup(field_value);
    } else if (ft_strcmp(field, "workingdir") == 0) {
        config->workingdir = ft_strdup(field_value);
    } else if (ft_strcmp(field, "umask") == 0) {
        //printf("umask: %s\n", field_value);
        config->umask = ft_strdup(field_value); //TODO parsear
    }
}

static void get_env_block(int fd, int program_index, t_program_config* config){
    char    *line;
    int     i;

    while (get_next_line(fd, &line)){
        if (line[0] != '#' && line[0] != '\n' && ft_strlen(line) > 1 && check_line_is_useful(line) == 0){
            if (check_is_name(line) == 0){
                i++;
            } else if (i = program_index + 1) {
            }
            if (i > program_index + 1){
                break;
            }
        }
    }
}

t_program_config init_program_config_structs(char * filename, int program_index){
    int                 fd;
    int                 i;
    char                *line;
    char                *field;
    char                *field_value;
    t_program_config    config;

    fd = open(filename, O_RDONLY);
    i = 0;
    set_default_config(&config);
    while (get_next_line(fd, &line)){
        if (line[0] != '#' && line[0] != '\n' && ft_strlen(line) > 1 && check_line_is_useful(line) == 0){
			//ft_printf("Line: %s\n", line);
            if (check_is_name(line) == 0){
                    i++;
            } else if (i == program_index + 1){
                field = get_field(line);
                if ((ft_strcmp(field, "env") == 0)){
                    get_env_block(fd, program_index, &config);
                } else {
                    field_value = get_field_value(line);
                    //ft_printf("Field: %s, Field Value: %s\n", field, field_value);
                    if (field != NULL){
                        //ft_printf("FIELD: %s\n", field);
                        fill_field(&config, field, field_value);
                    }
                    free(field_value);
                }
                free(field);
            }
            if (i > program_index + 1){
                break;
            }
        }
    }
	close(fd);

    return config;
}