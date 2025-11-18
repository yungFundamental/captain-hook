# Captain Hook

## Introduction
A Linux Kernel module that can hook kernel functions with a custom trampoline hook mechanism.
> Note that the custom hook was developed in my [LKMs repository](https://github.com/yungFundamental/LKMs), but this is where I decided to create one main LKM.




## Dependencies
Linux Kernel - version 5.18

## Trampoline Hooks
In this project, I learned how to implement hooks.

At the beginning I used xcellerators ftrace helper. A simple header file that uses ftrace to install hooks in functions.
Later, I felt like this was cheating. Plus ftrace is easily detectable by checking /sys/kernel/tracing.

To solve this myself, I implemented my own hooking mechanism, based off trampoline hooking.
When disassembling the Linux Kernel, I noticed that the first 5 bytes of each function are reserved.
During runtime, those 5 bytes either filled with `0x0f1f440000` (a 5 byte NOP) or a CALL NEAR instruction to the ftrace logic.
> [!NOTE]
> CALL NEAR is one opcode byte and 4 bytes for the relative address of the function.

Since the function will always have those 5 bytes reserved, we can replace them in runtime to a `jmp` instruction to a custom function - and that is exactly what the `trampoline` module enables us to do!
Implementing your own hooks has never been easier, and harder to detect!

In addition, hook implementers can decide when the original function should be called or not. We can just call the original function, which will run the function from the address **after** the 5 bytes changed.

### Version Proofing
The trampoline module works only for Kernel versions that don't include Intel Control-flow Enforcement Technology (CET), meaning kernel versions below 5.18. 

While the module doesn't support CET just yet, adding support should be relatively simple. All we need to do is alter the 5 bytes that are after the `endbr64` instruction.


