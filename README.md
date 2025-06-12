# Very Simple Shell

VSSHL (very simple shell) is a minimalistic command-line shell written in C. It has a basic funcionality of a Unix shell: command parsing, launching programs and handling a few builtin commands.

This project is intended for educational purposes, demonstrating process creation with fork and exec and handling input. 

## Builtin Commands:

- cd -> Changes the current working directory
- help -> Prints information about the shell
- exit -> Quits the program

## Compiling and Running:

To compile the shell, use gcc:
```bash
gcc -o vsshl main.c
```

Once compiled, run the shell with:
```bash
./vsshl
```

You should be prompted with "> " and you can start typing commands.

## Limitations:

- No piping, redirection or background execution
- Basic parsing with no support for quotes or escaping
- None of the advanced shell features

## Licence

This project was created for educational purposes. Attribution is given to the original author, Stephen Bernnan. Feel free to study, modify and improve upon it.

