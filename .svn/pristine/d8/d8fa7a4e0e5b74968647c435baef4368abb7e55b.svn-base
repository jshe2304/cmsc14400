#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <fcntl.h>

#define MAX_LEN 512

char WHITESPACE[] = " \t\n\v\f\r";
char ERROR_MESSAGE[] = "An error has occurred\n";

void print(char *msg) {
    write(STDOUT_FILENO, msg, strlen(msg));
}

void throw_error() {
    write(STDOUT_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE));
}

void throw_error_exit() {
    write(STDOUT_FILENO, ERROR_MESSAGE, 22);
    exit(0);
}

void print_pwd() {
    char* pwd = getcwd(NULL, 0);
    print(pwd);
    print("\n");
    free(pwd);
}

// Split command by redirect symbol
// Original string truncated to just before redirect
// Returns pointer to redirect address, NULL if no redirect
char* redirect_where(char* cmd, bool* advanced) {
    // Loop through cmd until EOS or '>' found
    while(*cmd && *cmd != '>') cmd++;

    // No redirect symbol
    if (*cmd == '\0') return NULL;

    // Check if advanced. 
    *(cmd++) = '\0';
    if (*cmd == '+') {
        *cmd = '\0';
        *advanced = true;
        cmd++;
    }

    // Move to first non-whitespace character;
    while (*cmd == ' ' || *cmd == '\t' || *cmd == '\n') *(cmd++) = '\0';
    
    // Return pointer
    return cmd;
}

char** parse_cmd(char* cmd, int* argc) {
    char** args = (char**) malloc(sizeof(char*) * 512);

    *argc = 0;

    char* ptr;
    char* arg = strtok_r(cmd, WHITESPACE, &ptr);
    while (arg != NULL) {
        args[(*argc)++] = arg;
        arg = strtok_r(NULL, WHITESPACE, &ptr);
    }

    args[*argc] = NULL;

    return args;
}

int append(int from_f, int to_f, off_t start) {
    if (lseek(from_f, start, SEEK_SET) == -1) return -1;

    // Buffer
    char buf[MAX_LEN];

    // Read chunks and write to outfile
    int bytes_read;
    do {
        bytes_read = read(from_f, buf, MAX_LEN);
        if (bytes_read == -1) return -1;
        if (write(to_f, buf, bytes_read) == -1) return -1;
    } while (bytes_read > 0);

    return 0;
}

int valid_redirect(char* path) {
    if (*path == '\0') return 0;

    while(*path) {
        if (*path == '>') return 0;
        path++;
    }
    
    return 1;
}

int run_redirect(char** cmd_args, char* path) {
    path = strtok(path, WHITESPACE);

    // Open file
    int f = open(path, O_WRONLY | O_CREAT | O_EXCL, 0664);
    if (f == -1) return -1;

    // I/O redirect
    if (dup2(f, STDOUT_FILENO) == -1) return -1;

    // Execute command
    if (execvp(cmd_args[0], cmd_args) == -1) return -1;

    close(f);

    return 0;
}

int run_advanced_redirect(char** cmd_args, char* path) {
    path = strtok(path, WHITESPACE);

    // Open redirect file
    // If file does not exist, run normal redirect
    int f = open(path, O_RDWR | O_APPEND);
    if (f == -1) return run_redirect(cmd_args, path);

    // Check number of bytes in redirect file
    off_t initial_offset = lseek(f, 0, SEEK_END);
    if (initial_offset == -1) return -1;

    // Create child process for command
    pid_t pid;
    if ((pid = fork()) == -1) throw_error();
    
    // Execute command in child process
    if (pid == 0) {
        // I/O redirect to file
        if (dup2(f, STDOUT_FILENO) == -1) return -1;
        // Execute command
        if (execvp(cmd_args[0], cmd_args) == -1) return -1;
    }

    wait(NULL);

    // Create temporary file
    char tmp[] = "./tmp-redirection-XXXXXX";
    int tmpf = mkstemp(tmp);
    if (tmpf == -1) return -1;

    // Move redirected text to temporary file
    if (lseek(f, initial_offset, SEEK_SET) == -1) return -1;
    if (append(f, tmpf, initial_offset) == -1) return -1;
    if (ftruncate(f, initial_offset)) return -1;

    // Move original text to temporary file
    if (append(f, tmpf, 0) == -1) return -1;
    
    // Close files
    close(f);
    close(tmpf);

    // Replace old redirect file with temporary file
    if (remove(path) == -1) return -1;
    if (rename(tmp, path) == -1) return -1;

    return 0;
}

// Run an individual command
int run(char* cmd) {

    // Check if redirect or advanced redirect needed. 
    bool advanced_redirect = false;
    char* redirect_path = redirect_where(cmd, &advanced_redirect);

    // Parse command arguments
    int argc;
    char** cmd_args = parse_cmd(cmd, &argc);
    if (cmd_args[0] == NULL && redirect_path == NULL) {
        free(cmd_args);
        return 0;
    }

    // Run built-in commands
    if (cmd_args[0] != NULL) {
        if (strcmp(cmd_args[0], "cd") == 0) {
            char* path = cmd_args[1];
            free(cmd_args);
            if (path == NULL) path = getenv("HOME");
            if (argc > 2 || redirect_path || chdir(path) != 0) return -1;
            return 0;
        } else if (strcmp(cmd_args[0], "exit") == 0) {
            free(cmd_args);
            if (argc > 1 || redirect_path) return -1;
            exit(0);
        } else if (strcmp(cmd_args[0], "pwd") == 0) {
            free(cmd_args);
            if (argc > 1 || redirect_path) return -1;
            print_pwd();
            return 0;
        }
    }

    // Run forked commands

    // Fork
    pid_t pid;
    if ((pid = fork()) == -1) return -1;

    // Child Fork
    else if (pid == 0) {
        if (advanced_redirect) {
            if (!valid_redirect(redirect_path)) throw_error();
            else if (run_advanced_redirect(cmd_args, redirect_path) == -1)  throw_error();
        } else if (redirect_path) {
            if (!valid_redirect(redirect_path)) throw_error();
            else if (run_redirect(cmd_args, redirect_path) == -1) throw_error();
        } else {
            if (execvp(cmd_args[0], cmd_args) == -1) throw_error();
        }

        exit(0);

    // Parent Fork
    } else wait(NULL);

    free(cmd_args);

    return 0;
}

// Run a line of colon-separated commands
void run_line(char* line) {

    char* ptr;
    char* cmd;

    // Loop through tokenized line and run commands
    cmd = strtok_r(line, ";", &ptr);
    while (cmd != NULL) {
        if (run(cmd) == -1) throw_error();
        cmd = strtok_r(NULL, ";", &ptr);
    }
}

// Run commands from batch file
int run_batch_mode(char* batch_file) {
    FILE* f;

    char line[MAX_LEN + 2];

    // Open file
    f = fopen(batch_file, "r");

    // Check if file successfully opened
    if (f == NULL) return -1;

    // Run each line in the batch file
    while (fgets(line, MAX_LEN, f) != NULL) {
        if (strchr(line, '\n') == NULL && !feof(f)) {
            print(line);
            print(fgets(line, MAX_LEN, f));
            throw_error();
            continue;
        }
        // Skip empty line
        char* ptr = line;
        while (*ptr == ' ' || *ptr == '\t') ptr++;
        if (*ptr == '\n') continue;

        print(line);
        run_line(line);
    }
    
    // Close file
    fclose(f);

    return 0;
}

// Run commands from interactive user input
int run_interactive_mode() {
    char line[MAX_LEN + 2];

    // Interactive loop
    while (1) {
        print("myshell> ");

        // Get line from stdin
        if (!fgets(line, MAX_LEN + 1, stdin)) return -1;

        run_line(line);
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc > 2) throw_error();
    else if (argc == 2) {
        if (run_batch_mode(argv[1]) == -1) throw_error();
    } else {
        if (run_interactive_mode() == -1) throw_error();
    }
}