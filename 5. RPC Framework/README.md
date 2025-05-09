# Program 5 - RPC - Remote Procedure Call

## Overview

This project implements a Remote Procedure Call (RPC) system that allows a client to interact with a remote file system via specific commands such as opening, reading, writing, seeking, and closing files. The program consists of a server (`rserver.c`) and two client programs (`rclient1.c` and `rclient2.c`), which demonstrate the functionality of the RPC system.

---

## Files Included

- **`rserver.c`**: Implements the RPC server that processes client requests and interacts with the file system.
- **`rclient.c`**: Provides core RPC communication functions used by both client programs.
- **`rclient.h`**: Header file for `rclient.c`.
- **`rclient1.c`**: A sample client program that demonstrates file operations using the RPC system.
- **`rclient2.c`**: Another sample client program showcasing different RPC operations.
- **`Makefile`**: Contains rules for compiling and cleaning the project.
- **`README.md`**: Instructions for compiling, running, and testing the program.

---

## Compilation

Use the provided `Makefile` for building the project. Supported commands:

- **Compile all programs**: 
```bash
make all
```

- **Clean up generated files**: 
```bash
make clean
```

---

## Running the Server

Start the server using the following command:
```bash
./rserver
```

The server will output the port number it is running on (e.g., `Port Number: 46657`). Make a note of this port for use with the clients.

---

## Running the Clients

- **Basic File Transfer with `rclient1`**:
    ```bash
    echo "This is a test file." > test_files/input.txt
    ./rclient1 localhost 46657 test_files/input.txt test_files/output.txt
    ```
   - Verify:
    ```bash
    cat test_files/output.txt
    ```
