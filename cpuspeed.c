#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/pci.h>
#include <asm/uaccess.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");

unsigned long long  stamp1;
unsigned long long  stamp2;
unsigned long long  cycles;

static struct proc_dir_entry* proc;

static ssize_t my_info(struct file *file, char *buf, size_t _len, loff_t *ppos)
{
	int	 len = 0;
	volatile int start = jiffies;
	volatile int timeout = start + HZ/10 ;

	// asm(" cli \n rdtsc ");
	asm("rdtsc");
	asm(" mov %%eax, stamp1 \n mov %%edx, stamp1+4 " : : : "ax", "dx" );

	while(jiffies < timeout) {} // Go for 1/10 of a second

	//asm(" rdtsc \n sti ");
	asm("rdtsc");
	asm(" mov %%eax, stamp2 \n mov %%edx, stamp2+4 " : : : "ax", "dx" );

	cycles = stamp2 - stamp1;

	volatile int finish = jiffies;

	len += sprintf( buf+len, "\n Elapsed cpu-cycles = %llu\n", cycles );
	len += sprintf( buf+len, "\n Jiffies = %i\n", (finish - start) );
	len += sprintf( buf+len, "\n Elapsed cpu-cycles = %iMhz\n", (int) cycles/100000 );

	len += sprintf( buf+len, "\n" );

	return	len;
}

static const struct file_operations proc_fops =
{
 .owner = THIS_MODULE,
 .read  = my_info,

};

static int my_init (void)
{

	proc = proc_create( "cpuspeed",  0777, NULL, &proc_fops );

	if (!proc) {
		return -ENOMEM;
	}

	return	0;

}

static void my_exit (void)
{
	remove_proc_entry( "cpuspeed", NULL );

}

module_init (my_init);
module_exit (my_exit);
