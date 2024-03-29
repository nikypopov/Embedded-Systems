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

MODULE_LICENSE("Dual BSD/GPL");
#define DEVICE_NAME "MY_TIMER"
#define MAX_TIMERS 5


static LIST_HEAD(timer_list_head);

struct my_timer_info {
    struct timer_list timer;
    unsigned long expires;
    char msg[128];
    struct list_head list;
};

static ssize_t nibbler_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
static ssize_t nibbler_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

struct file_operations nibbler_fops = {
    read: nibbler_read,
    write: nibbler_write,
};


static void timer_callback(struct timer_list *t); //called on timer completion 
void add_timer_to_list(unsigned long expires, char *msg); //add to new timer to linked list
void remove_timer_from_list(char *msg); //once timer is expired
void list_timers(void); //when -l flag called from ul
static void cleanup_timers(void); //for mod exit
static void kmod_exit(void);
static int kmod_init(void);


static int nibbler_major=61;
static unsigned capacity = 256;
static unsigned bite = 256;

/* Buffer to store data */
static char *nibbler_buffer;
/* length of the current message */
static int nibbler_len;

int active_timers = 0; 
int m_limit = 1; //current user defined timer limit


static int kmod_init(void) {
    int result;

    /* Registering device */
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

        cleanup_timers();
        
        printk(KERN_ALERT "Removing timer module\n");
}


static void cleanup_timers(void) 
{   
    // Free all the allocated memory from the timers and program
    
}


void add_timer_to_list(unsigned long expires, char *msg) 
{
   
}


static void timer_callback(struct timer_list *t) 
{
   
    
}


void remove_timer_from_list(char *msg) 
{
    
}


void list_timers(void) 
{
    
}



static ssize_t nibbler_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
    char flag[3];  // Assuming the maximum length of the flag is 3 characters
    int sec;
    char msg[256]; // Assuming the maximum length of the message is 255 characters
    int count_value;
    int sscanf_result;
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

    printk(KERN_INFO "TBUF STORED INFO: %s\n", tbuf);

    *f_pos += count;
    nibbler_len = *f_pos;

    sscanf_result = sscanf(tbuf, "-s %d %s", &sec, msg);
    printk(KERN_INFO "sscanf result: %d\n", sscanf_result);


     /* Parse the flag and associated values */
    if (sscanf_result == 2) {
        // Handle the case for flag -s
        printk(KERN_INFO "Received flag -s: sec = %d, msg = %s\n", sec, msg);
        // Here you can use sec and msg as needed
    } else if (strcmp(tbuf, "-l\n") == 0) {
        // Handle the case for flag -l
        printk(KERN_INFO "Received flag -l\n");
        // Here you can handle the -l flag
    } else if (sscanf(tbuf, "-m %d", &count_value) == 1) {
        // Handle the case for flag -m
        printk(KERN_INFO "Received flag -m: count = %d\n", count_value);
        // Here you can use count_value as needed
    } else {
        printk(KERN_INFO "Invalid input\n");
        return -EINVAL; // Invalid input
    }



    // printk(KERN_ALERT "Buffer written too\n");
    /* Return the number of bytes written */
    return count;

}

static ssize_t nibbler_read(struct file *filp, char *buf, size_t length, loff_t *offset)
{
    printk(KERN_ALERT "Buffer read from\n");
    return length;
}




module_init(kmod_init);
module_exit(kmod_exit);
