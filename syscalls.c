#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/syscalls.h>

#define PROC_NAME "syscalls"

MODULE_AUTHOR("Joseph Connor");
MODULE_LICENSE("GPL");

static void **my_sys_call_table = NULL;
static int (*show_value_func)(void);

struct syscalls_iter {
    int show_value;
};

static void *syscall_seq_start(struct seq_file *s, loff_t *pos) {
    loff_t *spos;

    if (*pos > __NR_syscall_max) {
        return NULL;
    }
    else {
        spos = kmalloc(sizeof(loff_t), GFP_KERNEL);
        *spos = *pos;
        return spos;
    }
}

static void *syscall_seq_next(struct seq_file *s, void *v, loff_t *pos) {
    loff_t *spos = (loff_t*)v;

    (*spos)++;
    (*pos)++;

    if (*spos > __NR_syscall_max) {
        kfree(spos);
        return NULL;
    }
    
    return v;
}

static void syscall_seq_stop(struct seq_file *s, void *v) {
    loff_t *spos = (loff_t*)v;

    if (spos) {
        kfree(spos);
    }
}

static int syscall_seq_show(struct seq_file *s, void *v) {
    loff_t *spos = (loff_t *) v;
	struct syscalls_iter *iter = (struct syscalls_iter *)s->private;

    void *handler = my_sys_call_table[*spos];

    void *ptr = iter->show_value ? handler : NULL;
    
    seq_printf(s, "%3Ld %px %pf\n", *spos, ptr, handler);
    return 0;
}


static struct seq_operations syscall_seq_ops = {
    .start = syscall_seq_start,
    .next  = syscall_seq_next,
    .stop  = syscall_seq_stop,
    .show  = syscall_seq_show
};


static int syscalls_open(struct inode *inode, struct file *file) {
	struct syscalls_iter *iter;

	iter = __seq_open_private(file, &syscall_seq_ops, sizeof(*iter));
	if (!iter)
		return -ENOMEM;

	iter->show_value = show_value_func();

	return 0;
}


static const struct file_operations syscalls_ops = {
	.open = syscalls_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release_private,
};


int init_module(void) {
    my_sys_call_table = (void*)kallsyms_lookup_name("sys_call_table");
    show_value_func = (void*)kallsyms_lookup_name("kallsyms_show_value");
    
    if (!my_sys_call_table) {
        printk("unable to obtain offset for sys_call_table\n"); 
        return -ENOENT;
    }

    if (!show_value_func) {
        printk("unable to obtain offset for kallsyms_show_value\n"); 
        return -ENOENT;
    }

    proc_create(PROC_NAME, 0444, NULL, &syscalls_ops);
    
    return 0;
}

void cleanup_module(void) {
    remove_proc_entry(PROC_NAME, NULL);
}
