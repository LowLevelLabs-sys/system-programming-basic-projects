# Raw Syscall File Copy Tool — 2026-07-07

## Goal
Start a systems programming journey in C by building a beginner project: a file copy tool using raw syscalls (`open`, `read`, `write`, `close`) instead of standard library functions like `fopen`/`fread`/`fwrite`.

## Progress
Built the file copy tool through four milestones in one session:
- v1: basic copy loop using `open()`, `read()`, `write()`, `close()`
- v2: proper error handling for each syscall, fixed a resource leak, replaced the magic number `100` with `#define BUFFER_SIZE`
- v3: preserved the source file's permissions on the destination file using `fstat()` and `fchmod()`
- v4: benchmarked different buffer sizes (64 / 4096 / 65536 bytes) on a 10MB file to see the effect on copy speed

All verified working using `diff` (to confirm the copy was byte-identical) and `ls -l` (to confirm permissions matched).

## What I Learned
- The difference between a syscall and a library function: a syscall (`open`, `read`, `write`) involves the CPU actually switching from user mode to kernel mode and back, while a library function (like `strlen()`) runs entirely in user mode with no kernel involvement.
- `fopen()` is a wrapper around `open()` — it translates string modes like `"r"`/`"w"` into flags, then adds buffering on top.
- `read()` doesn't need to be called only once. It can be called repeatedly in a loop, and each call continues from where the last one left off, until it returns 0 (EOF).
- File I/O at the syscall level is purely byte-oriented — `read()` has no concept of "lines" or "words," only bytes.
- Buffer size directly affects performance because each syscall has a fixed overhead cost (the user↔kernel mode switch). Smaller buffers mean more syscalls, so more overhead. Going from a 64-byte to a 4096-byte buffer gave a ~24x speedup, but going from 4096 to 65536 only gave about 4-5x — because once syscall overhead becomes small enough, actual disk I/O time starts to dominate instead, and buffer size can't speed that part up further.
- `mode` in `open()` (e.g. `0644`) is written in octal because Unix permissions map naturally onto 3-bit groups (owner/group/others), each representing read/write/execute.
- WSL2's `/mnt/c/...` paths (DrvFs, backed by NTFS) don't support real Unix permission bits — testing `chmod`/`fchmod` there gave misleading results (`-rwxrwxrwx` no matter what). Switching to the Linux home directory (`~`, on ext4) showed permission changes actually working.

## Biggest Challenge
Understanding why the buffer-size benchmark results looked the way they did (v4) — specifically why the jump from 64→4096 bytes was such a huge speedup (~24x) while 4096→65536 was a much smaller jump (~4-5x), even though both were similar-sized increases in buffer size. This was tied into learning about user mode vs. kernel mode for the first time.

## Debugging Story
This wasn't a bug in the traditional sense, but a conceptual sticking point. After running the benchmark and seeing the timing numbers (2.481s at 64 bytes, 0.103s at 4096 bytes, 0.022s at 65536 bytes), the pattern itself was confusing — why would doubling the buffer size in the second case not give another dramatic speedup?

The explanation that made it click was calculating the actual number of `read()`/`write()` calls needed for the 10MB file at each buffer size (156,250 calls at 64 bytes vs. just 153 calls at 65536 bytes), then separating the total time into two components: syscall overhead (a fixed cost per call, which shrinks fast as buffer size grows) and actual disk I/O time (proportional to total bytes moved, which doesn't change no matter the buffer size). At small buffer sizes, overhead dominates the total time, so increasing the buffer size helps a lot. Once the number of calls gets small enough, overhead stops being the bottleneck and disk I/O time takes over — so further buffer increases stop mattering as much. This was also the first real exposure to what user mode and kernel mode actually are, and why a syscall specifically involves the CPU switching privilege levels, not just a "worse case of a function call."

## Technical Decisions
Chose to hardcode the source/destination filenames directly in the code for this version, rather than accepting them as command-line arguments — kept the scope focused on the syscall/I/O concepts for this project instead of also handling `argv` parsing.

## Reflection
Made a couple of wrong guesses before understanding the real explanation:
- After seeing the copied output get cut off mid-word and printed on separate lines, first assumed this meant something like "every 100 words starts a new line." The real reason was that `read()` only cares about byte counts — the newline characters seen in the output were the file's own original `\n` characters that happened to fall inside a 100-byte chunk, not any word/line-based logic.
- Initially thought the buffer still needed the extra `+1` byte for a null terminator, out of habit from when the code used `%s`/`strlen()`. Once the code switched to using `bytes_read` explicitly everywhere (`write()` and `printf("%.*s", ...)`), the `+1` was no longer necessary since there's no more reliance on a `\0` terminator.

## Next Steps
Plans to read more into the documentation (man pages, syscall references) to go deeper on the concepts covered today. The next major project is a simple Unix shell (v1: single command execution using `fork()`/`exec()`/`wait()`), which will introduce the process syscall category.

