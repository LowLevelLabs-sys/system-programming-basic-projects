# Systems & Network Programming Roadmap — Faried
Last updated: July 7, 2026

## Current Level Summary
Faried is a self-taught D3 Informatics graduate coming from a JS/web background (Express, Node, React) who restarted from fundamentals. Prior C experience: built Quary Notetaker (CLI note-taking app in C). This session marked his first real systems programming project — a raw syscall file copy tool — built from scratch through v1-v4, fully completed, with genuine conceptual understanding, not pattern-copying. He asks good follow-up questions when something doesn't fully click (e.g. pushed back on `+1` buffer sizing once he understood the byte-explicit approach; asked to slow down when v4 syscall-overhead explanation moved too fast) rather than just accepting explanations at face value. He also connects new concepts to prior experience unprompted (e.g. linked buffer concept to `scanf`/stdin leftover input, linked syscall batching to an HTTP request/response analogy). Comfortable in WSL2/Ubuntu, uses Vim, familiar with `sed`/`grep` basics for generating test data.

## Confirmed Strengths
- **Byte-oriented I/O model**: correctly reasoned that `read()` has no concept of "words" or "lines," only bytes — arrived at this himself after seeing output cut mid-word.
- **Resource management instinct**: independently identified the resource leak (destination fd left open if source `open()` failed) once introduced to the ownership concept — didn't need it repeated.
- **Syscall vs library distinction**: correctly reasoned that `fopen()` wraps `open()` after being walked through it once.
- **Willingness to test assumptions empirically**: diagnosed the WSL2 DrvFs vs ext4 permission issue by testing in `~` after being prompted, understood why `/mnt/c/...` behaved differently.
- **Self-correction**: revised buffer sizing logic (`+1` no longer needed) after reasoning through the change from string-based to byte-explicit I/O.

## Active Gaps
- New to POSIX syscall signatures generally (flags, `mode_t`, octal literals) — needs the "why" spelled out each time so far, but retains it well once explained (e.g. picked up octal-for-permissions reasoning quickly).
- Hasn't yet independently used man pages to look up a syscall signature (was pointed to `man open` but hasn't reported back using it unprompted yet) — worth watching whether this becomes a habit.
- No exposure yet to `fork()`/`exec()`/`wait()` or process concepts — next major concept area.

## Recently Completed
- **File Copy Tool (raw syscall) — FULLY COMPLETE (v1-v4)**:
  - v1: basic copy loop with `open()`/`read()`/`write()`/`close()`
  - v2: error handling + resource-leak fix (self-identified) + `#define BUFFER_SIZE`
  - v3: permission preservation via `fstat()`/`fchmod()`; diagnosed WSL2 DrvFs vs ext4 permission quirk himself after being prompted to test in `~` instead of `/mnt/c/...`
  - v4: benchmarked buffer sizes (64 / 4096 / 65536 bytes) on a 10MB file using `time`, saw firsthand that going from 64→4096 gave a ~24x speedup while 4096→65536 gave only ~4-5x (diminishing returns as syscall overhead stops dominating vs. disk I/O time). Connected this to the general **batching** principle (DB bulk inserts, HTTP request batching).
  - All verified via `diff` and `ls -l` across two filesystems, plus empirical timing data.

## Recommended Next Concepts
1. **`fork()` / `exec()` / `wait()`** — the process model, next major concept area, needed for the shell project already agreed on as the next project.
2. **`man` pages as first resort** — encourage checking `man <syscall>` before asking; introduced this session but not yet self-initiated by Faried. Worth nudging toward this habit early in the shell project.
3. Reinforce **batching** as a recurring theme — will resurface in network programming (packet batching) later.

## Project Ideas (progressively harder)
1. **Simple Unix Shell v1** — NEXT PROJECT, already agreed. Single command execution only (no pipes/redirection yet). Introduces `fork()`, `exec()`, `wait()`.
2. **Shell v2-v4** — incrementally add pipes, I/O redirection, background jobs (`&`) — natural progression once shell v1 is solid.
3. **(Stretch, later)** Toy `malloc`/`free` implementation — heavier conceptually (heap layout, free lists), better suited after process fundamentals are comfortable.
