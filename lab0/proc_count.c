#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>

struct proc_dir_entry *entry;

int count_procs(struct seq_file *s, void *v){
	int proc_count = 0;
 	struct task_struct *task;
 	for_each_process(task){
 		proc_count++;
	}
 	seq_printf(s, "%d\n", proc_count);
 	return 0;
}

static int __init proc_count_init(void)
{
	pr_info("proc_count: init\n");
    entry = proc_create_single("count", 0644, NULL, count_procs);
	return 0;
}

static void __exit proc_count_exit(void)
{
	proc_remove(entry);
	pr_info("proc_count: exit\n");
}

module_init(proc_count_init);
module_exit(proc_count_exit);

MODULE_AUTHOR("Matthew Yu");
MODULE_DESCRIPTION("Counts number of running processes and inserts count into /proc/count");
MODULE_LICENSE("GPL");
