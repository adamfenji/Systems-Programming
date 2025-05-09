# Program 4 - Husky Script (hscript)

## Overview
Husky Script (`hscript`) is a program that captures and logs the standard input, output, and error of a child process. Output is stored in files named `0`, `1`, and `2` under a specified directory:
- `0`: Logs standard input.
- `1`: Logs standard output.
- `2`: Logs standard error.

---

## Compilation
To compile the program, run:
   - `make all` compiles the program.
   - `make clean` removes generated files.
   - `make submission` packages the project for submission.

---

## Running the Program
```bash
./hscript <program_name> <arguments> <directory>
```

- `<program_name>`: The full path to the program to execute.
- `<arguments>`: Arguments for the program.
- `<directory>`: Directory where log files will be created.

---

## Tests
Here are the tests to verify the functionality of `hscript`:

1. **Compile the program:**
   - `make hscript`

2. **Check usage message:**
   - Run `./hscript` with no arguments to ensure the correct usage message is displayed.

3. **Test execution of a simple program:**
   - Run `./hscript /bin/ls -l /tmp/testdir` to capture the output of the `ls` command.
   - Check the contents of:
     - `/tmp/testdir/0`: Should be empty (no input).
     - `/tmp/testdir/1`: Should contain the directory listing.
     - `/tmp/testdir/2`: Should be empty (no errors).

4. **Test a program that requires input:**
   - Run `./hscript /bin/cat /tmp/testdir` and type some input.
   - Check the contents of:
     - `/tmp/testdir/0`: Should contain the input typed.
     - `/tmp/testdir/1`: Should echo the input.
     - `/tmp/testdir/2`: Should be empty (no errors).

5. **Test error logging:**
   - Run `./hscript /bin/ls /nonexistentdir /tmp/testdir`.
   - Check the contents of `/tmp/testdir/2`: Should contain the error message `ls: cannot access '/nonexistentdir': No such file or directory`.

6. **Test output redirection:**
   - Run `./hscript /bin/echo "Hello, world!" /tmp/testdir > redirected_output.txt 2>&1`.
   - Check the contents of:
     - `/tmp/testdir/1`: Should contain `Hello, world!`.
     - `redirected_output.txt`: Should also contain `Hello, world!`.
