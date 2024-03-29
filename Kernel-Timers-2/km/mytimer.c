#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <asm/system_misc.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>
#include <linux/list.h>
#include <linux/proc_fs.h>
#include <linux/proc_fs.h> 
#include <linux/seq_file.h>
#include <linux/time.h> 
#include <linux/sched/signal.h>
#include <linux/sched.h> 
#include <linux/jiffies.h>
#include <linux/string.h>

MODULE_LICENSE("Dual BSD/GPL");
#define DEVICE_NAME "MY_TIMER"
#define MAX_TIMERS 1

static ssize_t nibbler_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
static int fasync_fasync(int fd, struct file *filp, int mode);
static int mytimer_proc_open(struct inode *inode, struct file *file);
static int mytimer_proc_show(struct seq_file *m, void *v);
static void my_timer_callback(struct timer_list *timer);
static void cleanup(void); //for kmod_exit
static void kmod_exit(void);
static int kmod_init(void);


/* fops, proc, and timer structs */
static struct proc_dir_entry *mytimer_proc_entry;
struct fasync_struct *async_queue;
struct file_operations nibbler_fops = {
    write: nibbler_write,
    fasync: fasync_fasync
};
struct file_operations mytimer_proc_fops = {
    .owner = THIS_MODULE,
    .open = mytimer_proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
  	.release = single_release,
};
static struct mytimer_info {
    char module_name[256];
    unsigned long msec_since_load;
    pid_t pid;
    char cmd[TASK_COMM_LEN];
    unsigned long sec_until_expiration;
    char* user_msg;
} timer_info;
struct timer_list my_timer;



/* chrdev and timer setup */
static int nibbler_major=61;
static unsigned capacity = 256;
static unsigned bite = 256;
/* Buffer to store data */
static char *nibbler_buffer;
/* length of the current message */
static int nibbler_len;
int active_timers = 0; 
int m_limit = 1; //current user defined timer limit
static unsigned long initial_jiffies;

/* km base functions */
static int kmod_init(void) {
    int result;
    initial_jiffies = jiffies;

    /* Registering chr device */
    result = register_chrdev(nibbler_major, DEVICE_NAME, &nibbler_fops);
    if (result < 0)
    {
        printk(KERN_ALERT
            "Cannot obtain major number (failed to register) %d\n", nibbler_major);
        return result;
    }

    /* Allocating nibbler for the buffer */
    nibbler_buffer = kmalloc(capacity, GFP_KERNEL); 
    if (!nibbler_buffer)
    { 
        printk(KERN_ALERT "Insufficient kernel memory\n"); 
        result = -ENOMEM;
        goto fail; 
    } 
    memset(nibbler_buffer, 0, capacity);
    nibbler_len = 0;

    printk(KERN_ALERT "Inserting nibbler module\n"); 

    /* Create procfs entry */
    mytimer_proc_entry = proc_create("mytimer", 0644, NULL, &mytimer_proc_fops);
    if (!mytimer_proc_entry) {
        return -ENOMEM; // Return error if creation failed
    }
    printk(KERN_ALERT "Inserting procfs entry\n");

    /* Initialize single timer */
    timer_setup(&my_timer, my_timer_callback, 0);

    /* Initialize timer_info structure */
    strncpy(timer_info.module_name, "mytimer", sizeof(timer_info.module_name) - 1);
    timer_info.msec_since_load = jiffies_to_msecs(jiffies - initial_jiffies);
    timer_info.pid = 0; // Indicate no timer has been registered
    timer_info.cmd[0] = '\0'; 
    timer_info.sec_until_expiration = 0;
    timer_info.user_msg = "0000";
    return 0;

fail: 
    kmod_exit(); 
    return result;
}

static void kmod_exit(void) {
        
        /* Freeing the major number */
        unregister_chrdev(nibbler_major, DEVICE_NAME);
        
        /* Freeing buffer memory */
        if (nibbler_buffer)
        {
            kfree(nibbler_buffer);
        }

        cleanup();
        
        printk(KERN_ALERT "Removing timer module\n");

        proc_remove(mytimer_proc_entry);
}

static int fasync_fasync(int fd, struct file *filp, int mode) {
	return fasync_helper(fd, filp, mode, &async_queue);                                                                                                                                                      
}  

static ssize_t nibbler_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos) {
    int sec;
    char msg[256]; // Assuming the maximum length of the message is 255 characters
    int temp;
    char tbuf[256], *tbptr = tbuf;
  

    /* end of buffer reached */
    if (*f_pos >= capacity)
    {
        printk(KERN_INFO
            "write called: process id %d, command %s, count %d, buffer full\n",
            current->pid, current->comm, count);
        return -ENOSPC;
    }

    /* do not eat more than a bite */
    if (count > bite) count = bite;

    /* do not go over the end */
    if (count > capacity - *f_pos)
        count = capacity - *f_pos;

    /* Copy data from user buffer to kernel buffer */
    if (copy_from_user(nibbler_buffer + *f_pos, buf, count)) {
        return -EFAULT;  // Error copying data from user space
    }

    for (temp = *f_pos; temp < count + *f_pos; temp++)
    {
        tbptr += sprintf(tbptr, "%c", nibbler_buffer[temp]);
    }                    

    // printk(KERN_INFO "TBUF STORED INFO: %s\n", tbuf);

    *f_pos += count;
    nibbler_len = *f_pos;

    
    /* Parse the flag and associated values */
    if (sscanf(tbuf, "-s %d %s", &sec, msg) == 2) {
        // Handle the case for flag -s
        // printk(KERN_INFO "Received flag -s: sec = %d, msg = %s\n", sec, msg);
        // Here you can use sec and msg as needed
        
        if (strcmp(timer_info.user_msg, msg) == 0) {
        	mod_timer(&my_timer, jiffies + msecs_to_jiffies(sec*1000));
        	timer_info.pid = current->pid;
	        timer_info.sec_until_expiration = sec;
        	printk(KERN_INFO "The timer %s was updated!\n", msg);
        	return count;
        } 

        else 
        {
        	if (active_timers >= 1)
	        {
		        printk(KERN_ALERT "Cannot add another timer!\n");
		        return count;
	        }
	        mod_timer(&my_timer, jiffies + msecs_to_jiffies(sec*1000));
	        active_timers++;
	        timer_info.pid = current->pid;
	        timer_info.sec_until_expiration = sec;
	        timer_info.user_msg = msg;
	        get_task_comm(timer_info.cmd, current);
    	}
    } 
    else 
    {
        printk(KERN_INFO "Invalid input\n");
        return -EINVAL; // Invalid input
    }

    return count;
}

static int mytimer_proc_open(struct inode *inode, struct file *file) {
    return single_open(file, mytimer_proc_show, NULL);
}


/*km helper functions */
static int mytimer_proc_show(struct seq_file *m, void *v) {
	unsigned long elapsed_msec = jiffies_to_msecs(jiffies - initial_jiffies);
	unsigned long remaining_sec;
	timer_info.msec_since_load = elapsed_msec;
 
    /* Check if the timer has been initialized or has valid information */
    if (timer_info.pid == 0) {
        // Assuming pid is 0 if no timer is registered
        seq_printf(m, "[MODULE_NAME]: %s\n", timer_info.module_name);
        seq_printf(m, "[MSEC]: %lu\n", timer_info.msec_since_load);
    } else {
    	remaining_sec =  jiffies_to_msecs(my_timer.expires - jiffies) * 1000;
    	timer_info.sec_until_expiration = remaining_sec;
        seq_printf(m, "[MODULE_NAME]: %s\n", timer_info.module_name);
        seq_printf(m, "[MSEC]: %lu\n", timer_info.msec_since_load);
        seq_printf(m, "[PID]: %d\n", timer_info.pid);
        seq_printf(m, "[CMD]: %s\n", timer_info.cmd);
        seq_printf(m, "[SEC]: %lu\n", timer_info.sec_until_expiration);
    }
    return 0;
}

static void my_timer_callback(struct timer_list *my_timer) {
	struct pid *pid_struct;
    struct task_struct *task;
    
    // Assuming you have stored the pid of the user-space process in timer_info.pid
    pid_struct = find_get_pid(timer_info.pid);
    task = pid_task(pid_struct, PIDTYPE_PID);
    if(task != NULL) {
        send_sig(SIGIO, task, 0);
    }
    
    // printk(KERN_INFO "mytimer: Timer expired and SIGIO signal sent\n");
    timer_info.pid = 0;
    timer_info.user_msg = "0000";
    active_timers = 0;
}
static void cleanup(void) {   
    del_timer(&my_timer);   
}

module_init(kmod_init);
module_exit(kmod_exit);