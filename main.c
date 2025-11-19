#include <linux/init.h>
#include <linux/module.h>
#include <linux/tcp.h>
#include "trampoline/trampoline.h"


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Amit Barzilai");
MODULE_DESCRIPTION("Trampoline hook");


static int (*orig_tcp_v4_rcv)(struct sk_buff *);

static int hooked_tcp_v4_rcv(struct sk_buff *skb)
{
    unsigned char *payload;
    unsigned int payload_len;
    void *tcp_header = tcp_hdr(skb);
    
    payload = (unsigned char *)tcp_header + tcp_hdrlen(skb);
    payload_len = skb->len - tcp_hdrlen(skb);

    if (payload_len > 5 || payload_len < 1)
        goto ret;

    if (*payload == 'a')
        printk(KERN_ALERT "Get hooked douchebag!\n");

ret:
    return orig_tcp_v4_rcv(skb);
}

static const hook_t tcp_v4_rcv_hook = {
    .target_name = "tcp_v4_rcv",
    .original_function = (void *)&orig_tcp_v4_rcv,
    .function = hooked_tcp_v4_rcv
};

static int captain_init(void)
{
    int err;
    err = tramp_hook_install(&tcp_v4_rcv_hook);
    if (err) {
        printk(KERN_ALERT "Failed installing hook\n");
        return err;
    }
    printk(KERN_INFO "Installed captain hook\n");
	return 0;
}

static void captain_exit(void)
{
    tramp_hook_uninstall(&tcp_v4_rcv_hook);
	printk(KERN_INFO "Removed captain hook\n");
}

module_init(captain_init);
module_exit(captain_exit);
