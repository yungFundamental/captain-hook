# Captain Hook

## Introduction
A Linux kernel module that intercepts inbound TCP network traffic, and renders itself invisible in accordance to commands sent to the system from the network.

When a TCP segment that starts with the string "Hokus Pokus" is sent to the machine, the module will render itself invisible to `lsmod` and `rmmod` commands.
When a TCP segment that starts with the string "Lumos" is sent to the machine, the module will reappear.

This project introduces a custom hooking mechanism and is mainly for demonstration purposes.

## Dependencies
This module will compile and run successfully on Linux kernels built with `ftrace` support but without IBT support.

IBT was introduced in kernel version 5.18, and `ftrace` is added by default, therefore versions between 5.17 and 2.6.27 should be fine.


## Hooking Mechanism
When researching kernel level hooks, you can find plenty of simple strategies.
A good one for beginners is [xcellerators ftrace helper](https://gist.github.com/xcellerator/ac2c039a6bbd7782106218298f5e5ac1#file-ftrace_helper-h). 
A simple header file that uses `ftrace` to install hooks in functions. `ftrace` is a built-in Linux module for tracing.

> [!WARNING] 
> The `ftrace` hooks assume that the kernel was built with `ftrace` support. This is safe to assume because it is the default option, but it is important to note that our custom hooking mechanism assumes the same.

Since `ftrace` is built-in, it easily detectable and abstract - systems can check for hooks by accessing `/sys/kernel/tracing/` and `ftrace` abstracts the brass tacks of hooking linux functions.

To solve this, I implemented my own hooking mechanism, based off trampoline hooking and binary instrumentation.

When disassembling the Linux Kernel, I noticed that the first 5 bytes of each function are reserved.
During runtime, those 5 bytes either filled with `0x0f1f440000` (a 5 byte NOP) or a CALL NEAR instruction to the `__fentry__` function.
This function handles `ftrace` logic, which potentially contains user set hooks if `ftrace` is utilized.
> [!NOTE]
> CALL NEAR is one opcode byte and 4 bytes for the relative address of the function.

Since each function will always have those 5 bytes reserved, we can replace them in runtime to a `JMP` instruction to a custom function - and that is exactly what the `trampoline` module enables us to do!
The custom hooking mechanism finds the requested function, calculates the relative address from the given function to the hook function, and replaces the `CALL NEAR` with a `JMP NEAR` and sets the destination to our custom hook.
Implementing your own hooks has never been easier, and harder to detect!

In addition, hook implementers can decide when the original function should be called or not. After installing a hook, the hook can access the `original_function` field to call the original function. 
Of course this pointer points at the instruction **after** the `JMP` instruction, to prevent infinite recursion.

### Indirect Branch Tracking Support
This mechanism hasn't been updated to support the Indirect Branch Tracking feature introduced in Linux version 5.18. 

With the introduction of Intel Control-flow Enforcement Technology (CET), Linux added support for building with IBT enabled. 
In kernels with IBT enabled, kernel functions will start with the `endbr64` instruction, and only after will they call `__fentry__` (or the 5 byte NOP mentioned earlier).

The `trampoline` module doesn't support kernels built with IBT support. But on older kernel versions (like version 4.4 in Ubuntu 14.04), we don't even need to consider this.

### Handling Kernels Without Ftrace

The hooking mechanism assumes that the kernel was built with `ftrace` support, it will not work without it.

In kernel compiled without `ftrace` support, the first 5 bytes are not reserved.
This means we can't just replace them without the risk of removing or splitting valid and important multi-byte instructions.

This could potentially be solved by copying the hooked function to a new address before mutating it.

