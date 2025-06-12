#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

/**
	Constants Declaration
*/
#define VSSHL_RL_BUFSIZE 1024
#define VSSHL_TOK_BUFSIZE 64
#define VSSHL_TOK_DELIM " \t\r\n\a"

/** 
	Declaration for builtin commands
*/
int vsshl_cd(char **args);
int vsshl_help(char **args);
int vsshl_exit(char **args);

/**
	Function Declaration
*/
int vsshl_launch(char **args);
int vsshl_execute(char **args);
char *vsshl_read_line(void);
char **vsshl_split_line(char *line);
void vsshl_loop(void);

/**
	List of builtin commands
*/
char *builtin_str[] = { "cd", "help", "exit"};

/** 
	Builtin command functions
*/
int (*builtin_func[]) (char **) = {
  &vsshl_cd,
  &vsshl_help,
  &vsshl_exit
};

int vsshl_num_builtins()
{
  return sizeof(builtin_str) / sizeof(char *);
}

/**
	Builtin Function Implementation
*/

/**
	@brief Builtin command: change directory
	@param args List of args, args[0] is "cd", args[1] is the directory
	@return Always returns 1 in order to continue executing
*/
int vsshl_cd(char **args)
{
  if (args[1] == NULL)
  {
    fprintf(stderr, "vsshl: expected argument to \"cd\"\n");
  }
  else
  {
    if (chdir(args[1]) != 0)
    {
      perror("vsshl");
    }
  }

  return 1;
}

/**
	@brief Builtin command: print help
	@param args list of args. Not examined.
	@return Always returns 1 in order to continue executing 
*/
int vsshl_help(char **args)
{
  int i;
  printf("Little shell: vsshl\n");
  printf("Type program names and arguments, then hit enter\n");
  printf("The following commands are built in: \n");

  for (int i = 0; i < vsshl_num_builtins(); i++)
  {
    printf(" %s\n", builtin_str[i]);
  }

  printf("Use the man command for information of other programs\n");
  return 1;
}

/**
	@brief Builtin command: exit
	@param args List of args, not examined
	@return Always returns 0, to terminate execution
*/
int vsshl_exit(char **args)
{
  return 0;
}

/**
	@brief Launch a program and wait for it to terminate
	@param args Null terminated list of arguments (including program)
	@return Always returns 1, to continue the execution
*/
int vsshl_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();

  if (pid == 0)
  {
    // Child process
    if (execvp(args[0], args) == -1)
    {
      perror("vsshl");
    }
    exit(EXIT_FAILURE);
  }
  else if (pid < 0)
  {
    // Error forking
    perror("vsshl");
  }
  else
  {
    // Parent process
    do
    {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  
  return 1;
}

/**
	@brief Execute shell built-in or launch program.
	@param args Null terminated list of arguments
	@return 1 if the shell should continue running, 0 if it should terminate
*/
int vsshl_execute(char **args)
{
  int i;

  if (args[0] == NULL)
  {
    return 1;
  }

  for (i = 0; i < vsshl_num_builtins(); i++)
  {
    if (strcmp(args[0], builtin_str[i]) == 0)
    {
      return (*builtin_func[i])(args);
    }
  }

  return vsshl_launch(args);
}

/**
	@brief Read a line of input from stdin
	@return The line from stdin
*/
char *vsshl_read_line()
{
  int bufsize = VSSHL_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer)
  {
    fprintf(stderr, "vsshl: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1)
  {
    // Read the character
    c = getchar();

    // If we hit EOF, replace it with a null character and return
    if (c == EOF || c == '\n')
    {
      buffer[position] = '\0';
      return buffer;
    }
    else
    {
      buffer[position] = c;
    }
    position++;

    // If we exceed the buffer, reallocate
    if (position >= bufsize)
    {
      bufsize += VSSHL_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer)
      {
	fprintf(stderr, "vsshl: allocation error\n");
	exit(EXIT_FAILURE);
      }
    }
  }
}

/**
	@brief Split a line into tokens
	@param line The line
	@return Null-terminated array of tokens
*/
char **vsshl_split_line(char *line)
{
  int bufsize = VSSHL_TOK_BUFSIZE;
  int position = 0;
  char **tokens = malloc(sizeof(char) * bufsize);
  char *token;

  if (!tokens)
  {
    fprintf(stderr, "vsshl: allocation failed\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, VSSHL_TOK_DELIM);

  while (token != NULL)
  {
    tokens[position] = token;
    position++;

    if (position > bufsize)
    {
      bufsize += VSSHL_TOK_BUFSIZE;
      tokens = realloc(tokens, sizeof(char*) * bufsize);

      if (!tokens)
      {
	fprintf(stderr, "vsshl: allocation failed\n");
	exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, VSSHL_TOK_DELIM);
  }

  tokens[position] = NULL;
  return tokens;
}

/**
	@brief Loop that gets the input and executes it
*/
void vsshl_loop()
{
  char *line;
  char **args;
  int status;

  do
  {
    printf("> ");
    line = vsshl_read_line();
    args = vsshl_split_line(line);
    status = vsshl_execute(args);

    free(line);
    free(args);
  } while (status);
}

/**
	@brief Main entry point
	@param argc Argument count
	@param argv Argument vector
	@return status code
*/
int main(int argc, char **argv)
{
  // Load config files

  // Run command loop
  vsshl_loop();

  // Shutdown/cleanup

  return EXIT_SUCCESS;
}
