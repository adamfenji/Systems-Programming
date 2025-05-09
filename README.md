## 📘 **Project Overview**  
This repository documents my work from **CS3411: Systems Programming**, a foundational systems-level course I completed at **Michigan Technological University** in **Fall 2024**. The class was taught by **Dr. Soner Onder** and focused on **UNIX system programming** using **C**, including low-level system calls, memory management, inter-process communication, file systems, scripting, and remote procedure calls.

All projects were coded from scratch based on the textbooks:

**Advanced Programming in the UNIX Environment, Third Edition, by W. Richard Stevens and Stephen A. Rago** and **C: A Reference Manual, Fifth Edition, by Samuel P. Harbison, III and Guy L. Steele, Jr. UNIX Network Programming, Volume 1—Networking APIs: Sockets and XTI, Second Edition, by W. Richard Stevens**

## 📂 **Projects & Concepts Implemented**

| #  | **Topic**                      | **Description** |
|----|-------------------------------|-----------------|
| 1  | [**Custom Memory Allocator – hmalloc**](1.%20Husky%20malloc) | Built a simplified `malloc`/`free` system using `sbrk()` and a single free list. Implemented `hmalloc`, `hfree`, `hcalloc`, and a `traverse` function to debug memory usage. |
| 2  | [**Tar Archiver (ctar / utar)**](2.%20Tar%20Archiver) | Developed a file archiver (`ctar`) and extractor (`utar`) using custom header blocks, supporting append/delete/extract operations on regular files, with strict validation using `lstat()` and no `stdio.h`. |
| 3  | [**Bit-level File Compressor (czy/dzy)**](3.%20File%20Compressor) | Created a symbol stream compressor (`czy`) and decompressor (`dzy`) using 4- and 9-bit encoding schemes with only `read()`/`write()` syscalls and bit-level abstractions. |
| 4  | [**System Call Script Logger (hscript)**](4.%20System%20Call%20Script%20Logger) | Implemented a script-like utility to log a program’s I/O activity into files `0`, `1`, and `2` (stdin, stdout, stderr) using **pipes**, **select()**, and **non-blocking I/O**. |
| 5  | [**Mini RPC Framework (client/server)**](5.%20RPC%20Framework) | Designed a Remote Procedure Call system enabling `rp_open`, `rp_read`, `rp_write`, `rp_seek`, and `rp_close` across networked clients and servers via **TCP sockets**, **marshalling**, and **forked server handlers**. |

## 🧠 **Key Learnings**
- Developed **low-level systems software** in **pure C** without relying on standard libraries (`stdio.h` not allowed)  
- Mastered **UNIX system calls** (`sbrk`, `fork`, `pipe`, `select`, `socket`, `lstat`, `exec`, `read`, `write`) for robust system-level development  
- Designed a **custom memory allocator** with a free list and relative offsets for internal structure  
- Built a **tar-like archiver** from scratch with header chaining and overflow handling  
- Compressed files using **bit-level algorithms** with custom encoding schemes and direct I/O  
- Simulated **terminal logging** and program execution tracing using **hscript** with strict file I/O redirection  
- Created a working **RPC system** that transparently redirects system calls over a network, using **marshalling/unmarshalling**, client entry wrapping, and socket-based communication  
- Reinforced debugging using **GDB**, **valgrind**, and structured trace output
