#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/slab.h>

#define DRIVER_AUTHOR "Arthur Pellegrin"
#define DRIVER_DESC "Chenillard Module"
#define DRIVER_LICENSE "GPL"
#define NUMBER_OF_LED 10

static int delay;
static struct timer_list my_timer;
int pattern = 1;
static char fileName[50];
int led_on = 0;

module_param(delay,int, 0);
MODULE_PARM_DESC(delay, "Vitesse du chenillard en ms");

static void generate_file_name_led_x(int x){
    char* base_url = "/sys/class/leds/fpga_led%d/brightness";
    int j = scnprintf(fileName, strlen(base_url), base_url , x);
}

static void turn_on_led_x(int x){
    struct file * output_file = filp_open(fileName, O_WRONLY|O_CREAT, 0644);
    mm_segment_t oldfs;
    if (IS_ERR(output_file)) {
        return PTR_ERR(output_file);
    }
    oldfs = get_fs();
    set_fs(KERNEL_DS);
    vfs_write(output_file, "1", strlen("1"), &output_file->f_pos);
    set_fs(oldfs);
    filp_close(output_file, NULL);  
}

static void turn_off_led_x(int x){
    struct file * output_file = filp_open(fileName, O_WRONLY|O_CREAT, 0644);
    mm_segment_t oldfs;
    if (IS_ERR(output_file)) {
        return PTR_ERR(output_file);
    }
    oldfs = get_fs();
    set_fs(KERNEL_DS);
    vfs_write(output_file, "0", strlen("0"), &output_file->f_pos);
    set_fs(oldfs);
    filp_close(output_file, NULL);  
}


static ssize_t my_read(struct file *file, char __user *buf, size_t count, loff_t *ppos){
    return count;
};

static ssize_t my_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos){
    char msg[10];
    msg[count] = '\0';
    if (copy_from_user(msg, buf, count)) 
    {
        return -EFAULT;
    }    
    if(msg[0] == '0'){
        pattern = 0;
    }
    else pattern = 1;
    printk(KERN_INFO "Writed %c in chenillard \n", msg[0]);

    return count;
};

void my_timer_callback(struct timer_list  *timer){
    printk(KERN_INFO "TIMER CALLBACK");

    if(pattern == 1){
        generate_file_name_led_x(led_on);
        turn_off_led_x(led_on);
        led_on = (led_on + 1) % NUMBER_OF_LED;
        generate_file_name_led_x(led_on);
        turn_on_led_x(led_on);
    }
};


static const struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .read = my_read,
    .write = my_write,
};

static int my_init(void)
{
    printk(KERN_INFO "Initialising Chenillard Module with speed = %d\n", delay);
    if(proc_create("chenillard", 0, NULL, &my_fops) == NULL) {
        return -ENOMEM;
    };
    setup_timer(&my_timer, my_timer_callback, 0);
    mod_timer(&my_timer, jiffies + delay);
    return 0;
};

static void my_exit(void)
{
    del_timer(&my_timer);
    printk(KERN_INFO "Exiting Chenillard Module\n");
    remove_proc_entry("chenillard", NULL);
};

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");