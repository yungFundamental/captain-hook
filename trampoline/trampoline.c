#include <linux/string.h>
#include <linux/mman.h>
#include <asm/cacheflush.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kallsyms.h>
#include <linux/types.h>
#include <linux/mm.h>
#include <asm/tlbflush.h>
#include <asm/pgtable.h>

#include "trampoline.h"


static int make_page_writable(unsigned long address) 
{
    unsigned int level;
    pte_t *pte;
    
    pte = lookup_address(address, &level);
    if (!pte) 
	    return -EINVAL;
    
    set_pte_atomic(pte, pte_mkwrite(*pte));
    __flush_tlb_one(address);
    return 0;
}

static int make_page_readonly(unsigned long address) 
{
    unsigned int level;
    pte_t *pte;
    
    pte = lookup_address(address, &level);
    if (!pte)
        return -EINVAL;
    
    // Clear the writable bit
    set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));
    __flush_tlb_one(address);
    return 0;
}

int tramp_hook_install(const hook_t *hook)
{
    u32 relative_hooker_address;
    void *target;
    int err;

    static const u8 jmp_opcode = 0xe9;

    target = (void *)kallsyms_lookup_name(hook->target_name);
    if (!target) {
        printk(KERN_ALERT "Failed looking for symbol %s while installing hook", hook->target_name);
        return -EINVAL;
    }

    err = make_page_writable((long unsigned)target);
    if (err) {
        printk(KERN_ALERT "Unable to make the page of address %p writable", target);
        return err;
    }

    relative_hooker_address = hook->function - (target + 5);
    memcpy(target, &jmp_opcode, sizeof(jmp_opcode));
    memcpy(target + 1, &relative_hooker_address, sizeof(relative_hooker_address));

    make_page_readonly((long unsigned)target);

    *(hook->original_function) = target + TRAMPOLINE_BYTES_REPLACED;  // Must be equal to sizeof(jmp_opcode) + sizeof(relative_hooker_address)
    return 0;
}


void tramp_hook_uninstall(const hook_t *hook)
{
    void *target;
    static const u8 nop_opcodes[TRAMPOLINE_BYTES_REPLACED] = {0x0f, 0x1f, 0x44, 0x00, 0x00};

    target = *(hook->original_function) - TRAMPOLINE_BYTES_REPLACED;
    make_page_writable((long unsigned)target);

    memcpy(target, nop_opcodes, sizeof(nop_opcodes));

    make_page_readonly((long unsigned)target);
}
