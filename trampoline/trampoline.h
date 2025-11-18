#ifndef TRAMPOLINE_HOOKS
#define TRAMPOLINE_HOOKS

typedef struct hook {
    const char *target_name;
    void *function;
    void **original_function;

}hook_t;

// This would potentially increase on systems with CET
#define TRAMPOLINE_BYTES_REPLACED 5


int tramp_hook_install(const hook_t *hook);
void tramp_hook_uninstall(const hook_t *hook);


#endif // !TRAMPOLINE_HOOKS
#define TRAMPOLINE_HOOKS

