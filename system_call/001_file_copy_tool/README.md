# Raw Syscall File Copy Tool

A file copy utility built using raw Linux syscalls (`open`, `read`, `write`, `close`, `fstat`, `fchmod`) instead of the C standard library's buffered I/O (`fopen`, `fread`, `fwrite`). Built as an introductory project to understand how file I/O actually works at the OS level.

## What it does

Copies the contents of a source file to a destination file, byte-for-byte, while preserving the source file's permission bits.

## Why raw syscalls instead of `fopen`/`fread`/`fwrite`?

Standard library functions like `fopen()` are wrappers around syscalls — they call `open()` internally, then add buffering on top. Using the raw syscalls directly makes that hidden layer visible: how the kernel actually reads/writes data, how permissions work, and why buffer size affects performance.

## Features

- Copies file contents in chunks using a fixed-size buffer
- Handles errors from each syscall individually (`open`, `read`, `write`) with proper cleanup of file descriptors
- Preserves the source file's permission bits on the destination file via `fstat()`/`fchmod()`
- Buffer size is configurable via `#define BUFFER_SIZE`

## Build

```bash
gcc main.c -o main
```

## Usage

Edit the source and destination filenames directly in `main.c` (currently hardcoded), then run:

```bash
./main
```

This reads from the source file and writes an identical copy to the destination file, matching its permissions.

## Verify it worked

```bash
diff source.txt destination.txt   # should output nothing if identical
ls -l source.txt destination.txt  # compare permission bits
```

## Benchmarking buffer size

To see how `BUFFER_SIZE` affects performance, generate a large test file first:

```bash
head -c 10000000 /dev/urandom > bigfile.txt
```

This creates a 10MB file of random bytes. Point the source path to `bigfile.txt`, try different `BUFFER_SIZE` values (e.g. 64, 4096, 65536), recompile for each, and time the copy:

```bash
time ./main
```

Smaller buffers mean more `read()`/`write()` calls, and each call has a fixed syscall overhead — so smaller buffers are noticeably slower on large files.

## What I learned building this

- The difference between a **syscall** (a request to the kernel, involving a CPU mode switch from user mode to kernel mode) and a **library function** (runs entirely in user mode, no kernel involved)
- File I/O at the syscall level is purely byte-oriented — there's no concept of "lines" or "words," only raw bytes
- Buffer size directly affects performance: smaller buffers mean more syscalls, and each syscall has a fixed overhead cost from the user↔kernel mode switch. Benchmarked buffer sizes of 64B / 4KB / 64KB copying a 10MB file and saw dramatic speedups (64→4096 bytes: ~24x faster) that leveled off once syscall overhead stopped dominating over actual disk I/O time
- How to preserve file permissions programmatically using `fstat()` (read metadata) and `fchmod()` (apply permission bits)
- WSL2's `/mnt/c/...` paths (DrvFs, backed by NTFS) don't support real Unix permission bits the way native Linux filesystems (ext4) do — permission changes only actually took effect when testing from the Linux home directory (`~`)

## Possible future improvements

- Accept source/destination paths as command-line arguments instead of hardcoding them
- Add an option to fail instead of overwrite if the destination already exists (`O_EXCL`)
