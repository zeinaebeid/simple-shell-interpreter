# Simple Shell Interpreter (SSI)

**CSc 360: Operating Systems (Fall 2024)**  
**Programming Assignment 1 (P1)**  
**Author**: Zeina Ebeid  
**Date**: October 4, 2024

---

## 1. Overview

This project implements a **Simple Shell Interpreter (SSI)** in C, which provides basic shell functionalities such as:
- Foreground execution of commands.
- Changing directories with the `cd` command.
- Background execution of commands using `&`.
- Listing and managing background processes with `bglist`.

The SSI displays a prompt in the format:
```
username@hostname: /current/directory >
```

The shell accepts user input, executes commands, and manages background processes, continuing to accept input while background jobs run.

---
