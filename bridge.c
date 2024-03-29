#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#include "bridge.h"


int bridge_major = BRIDGE_MAJOR;
int bridge_minor = 0;
int bridge_nr_devs = BRIDGE_NR_DEVS; /* number of bare bridge devices */

module_param(bridge_major, int, S_IRUGO);
module_param(bridge_minor, int, S_IRUGO);
module_param(bridge_nr_devs, int, S_IRUGO);

MODULE_AUTHOR("Jheisson Argiro Lopez Restrepo");
MODULE_LICENSE("Dual BSD/GPL");

struct list_head current_list;
struct list_head high_q;
struct list_head mid_q;
struct list_head low_q;
LIST_HEAD(list_1);
LIST_HEAD(list_2);


// LIST_HEAD(stack);
// LIST_HEAD(stack_2);
// LIST_HEAD(high_q);
// LIST_HEAD(mid_q);
// LIST_HEAD(low_q);

static void create_list(void) {
	INIT_LIST_HEAD(&current_list);
} 

static void add_element_to_list(char *node_element_msg)
{
	struct string_node *tmp_element;
	tmp_element = kmalloc(sizeof(struct string_node), GFP_KERNEL);
	strcpy(tmp_element->message, node_element_msg);
	INIT_LIST_HEAD(&tmp_element->list);
	list_add_tail(&(tmp_element->list), &current_list);
}

static void destroy_list(void)
{
	struct string_node *tmp_element;
	struct list_head *watch, *next;
	list_for_each_safe(watch, next, &current_list)
	{
		tmp_element = list_entry(watch, struct string_node, list);
		list_del(&(tmp_element->list));
		kfree(tmp_element);
	}
	kfree(&current_list);
}

static void randomize_list(void) {
	struct string_node *tmp_element;
	struct list_head *watch, *next;
	unsigned rand;
	
	list_for_each_safe(watch, next, &current_list)
	{
		tmp_element = list_entry(watch, struct string_node, list);

		get_random_bytes(&rand, sizeof(rand));
		rand %= 2;

		list_del(&(tmp_element->list));
		if ( rand == 0 ) {
			list_add(&(tmp_element->list), &current_list);
		}
		else {
			list_add_tail(&(tmp_element->list), &current_list);
		}
		// printk(KERN_INFO "message %n\n", rand);
		kfree(tmp_element);
	}
}

static void create_queue(void) {
	INIT_LIST_HEAD(&current_list);
	high_q = current_list;
	mid_q = current_list;
	low_q = current_list;
}

static void add_element_to_queue(char *node_element_msg, int prior)
{
	struct string_node *tmp_element;
	tmp_element = kmalloc(sizeof(struct string_node), GFP_KERNEL);
	strcpy(tmp_element->message, node_element_msg);
	INIT_LIST_HEAD(&tmp_element->list);

	switch ( prior )
	{
	case 0:
		list_add(&(tmp_element->list), &high_q);
		high_q = tmp_element->list;
		break;

	case 1:
		list_add_tail(&(tmp_element->list), &low_q);
		mid_q = tmp_element->list;
		break;

	case 2:
		list_add_tail(&(tmp_element->list), &current_list);
		if (&low_q == &current_list) low_q = tmp_element->list;
		break;
	}

	kfree(tmp_element);

}

static void read_element_from_queue(unsigned long arg, int prior) {
	// struct string_node *tmp_element;
	// switch ( prior )
	// {
	// 	case 0:
	// 		if ( high_q != current_list && high_q != NULL)
	// 		{
	// 			tmp_element = list_first_entry(&current_list, struct string_node, list);
	// 			list_del(&(tmp_element->list));
	// 			raw_copy_to_user((char *)arg, tmp_element->message, 100);
	// 			kfree(tmp_element);
	// 		}
	// 		break;

	// 	case 1:
	// 		if ( mid_q != current_list && mid_q != NULL)
	// 		{
	// 			tmp_element = list_first_entry(&high_q, struct string_node, list);
	// 			list_del(&(tmp_element->list));
	// 			raw_copy_to_user((char *)arg, tmp_element->message, 100);
	// 			kfree(tmp_element);
	// 		}
	// 		break;

	// 	case 2:
	// 		if ( low_q != current_list && low_q != NULL)
	// 		{
	// 			tmp_element = list_entry(&low_q, struct string_node, list);
	// 			low_q = list_first_entry(&low_q, struct string_node, list)->list;
	// 			list_del(&(tmp_element->list));
	// 			raw_copy_to_user((char *)arg, tmp_element->message, 100);
	// 			kfree(tmp_element);
	// 		}
	// 		break;
	// }

	// tmp_element = list_entry(&mid_q, struct string_node, list);
	// list_del(&(tmp_element->list));
	// raw_copy_to_user((char *)arg, tmp_element->message, 100);
	// kfree(tmp_element);
	// raw_copy_from_user(message, (char *)arg, 100);
	
}

static void invert_list(void)
{
	struct string_node *tmp_element;
	struct list_head *watch, *next;
	list_for_each_safe(watch, next, &current_list)
	{
		tmp_element = list_entry(watch, struct string_node, list);
		printk(KERN_INFO "%s", tmp_element->message);
		list_del(&(tmp_element->list));
		list_add(&(tmp_element->list), &current_list);
	}

	list_for_each_safe(watch, next, &current_list)
	{
		tmp_element = list_entry(watch, struct string_node, list);
		printk(KERN_INFO "%s", tmp_element->message);
	}
	printk(KERN_INFO "\n");
}

void concat_lists(void) {

	struct string_node *tmp_element;
	struct list_head *watch, *next;
	list_for_each_safe(watch, next, &list_1)
	{
		tmp_element = list_entry(watch, struct string_node, list);
		list_del(&(tmp_element->list));
		list_add_tail(&(tmp_element->list), &current_list);
	}
	kfree(&list_1);
	list_for_each_safe(watch, next, &list_2)
	{
		tmp_element = list_entry(watch, struct string_node, list);
		list_del(&(tmp_element->list));
		list_add_tail(&(tmp_element->list), &current_list);
	}
	kfree(&list_2);
}

static void rotate_right(void)
{
	struct string_node *tmp_element;
	tmp_element = list_last_entry(&current_list, struct string_node, list);
	list_del(&(tmp_element->list));
	list_add(&(tmp_element->list), &current_list);
}

void clean_list(void) 
{
	struct string_node *tmp_element;
	struct string_node *tmp_element_2;
	struct list_head *watch, *next, *watch2, *next2;
	list_for_each_safe(watch, next, &current_list)
	{
		tmp_element = list_entry(watch, struct string_node, list);
		list_for_each_prev_safe(watch2, next2, &current_list)
		{
			tmp_element_2 = list_entry(watch2, struct string_node, list);
			if (&(tmp_element->list) != &(tmp_element_2->list) && strcmp(tmp_element->message, tmp_element_2->message) == 0)
			{
				printk("Message: %p vs %p \n", &(tmp_element->list),  &(tmp_element_2->list));
				printk("Message: %s vs %s \n", tmp_element->message, tmp_element_2->message);
				list_del(&(tmp_element->list));
				kfree(tmp_element);
			}
		}
	}
}

void find_highest_val(unsigned long arg) {
	struct string_node *tmp_element;
	char highest_str[100] = "\0";
	char tmp_str[100];
	struct list_head *watch, *next;
	list_for_each_safe(watch, next, &current_list)
	{
		tmp_element = list_entry(watch, struct string_node, list);
		strcpy(tmp_str, tmp_element->message);
		if (strcmp(tmp_str, highest_str) > 0) strcpy(highest_str, tmp_str);
	}

	raw_copy_to_user((char *)arg, highest_str, 100);
}





void mylist_exit(void)
{
	struct string_node *tmp_element;
	struct list_head *watch, *next;
	list_for_each_safe(watch, next, &current_list)
	{
		tmp_element = list_entry(watch, struct string_node, list);
		list_del(&(tmp_element->list));
		kfree(tmp_element);
	}
	// kfree(&stack);
}

struct bridge_dev *bridge_devices; /* allocated in bridge_init_module */

static long bridge_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	int return_value = 0;
	int data;
	char message[100];
	struct string_node *tmp_element;
	switch (cmd)
	{
	case BRIDGE_CREATE_Q:
		printk(KERN_INFO "message %s\n", "bla");
		create_queue();
		// Return a posituve value indicating the state of the queue
		return_value = 1;
		break;

	case BRIDGE_W_HIGH_PRIOR_Q:
		raw_copy_from_user(message, (char *)arg, 100);
		add_element_to_queue(message, 0);
		break;

	case BRIDGE_W_MIDDLE_PRIOR_Q:
		raw_copy_from_user(message, (char *)arg, 100);
		add_element_to_queue(message, 1);
		break;

	case BRIDGE_W_LOW_PRIOR_Q:
		raw_copy_from_user(message, (char *)arg, 100);
		add_element_to_queue(message, 2);
		break;

	case BRIDGE_R_HIGH_PRIOR_Q:
		// tmp_element = list_first_entry(&high_q, struct string_node, list);
		// list_del(&(tmp_element->list));
		// raw_copy_to_user((char *)arg, tmp_element->message, 100);
		// kfree(tmp_element);
		break;

	case BRIDGE_R_MIDDLE_PRIOR_Q:
		// tmp_element = list_first_entry(&mid_q, struct string_node, list);
		// list_del(&(tmp_element->list));
		// raw_copy_to_user((char *)arg, tmp_element->message, 100);
		// kfree(tmp_element);
		break;

	case BRIDGE_R_LOW_PRIOR_Q:
		// tmp_element = list_first_entry(&low_q, struct string_node, list);
		// list_del(&(tmp_element->list));
		// raw_copy_to_user((char *)arg, tmp_element->message, 100);
		// kfree(tmp_element);
		break;

	case BRIDGE_STATE_Q:
		printk(KERN_INFO "message %s\n", "bla6");
		break;

	case BRIDGE_DESTROY_Q:
		printk(KERN_INFO "message %s\n", "bla7");
		break;

	case BRIDGE_CREATE_S:
		create_list();
		break;

	case BRIDGE_W_S:
		raw_copy_from_user(message, (char *)arg, 100);
		add_element_to_list(message);
		break;

	case BRIDGE_R_S:
		tmp_element = list_last_entry(&current_list, struct string_node, list);
		list_del(&(tmp_element->list));
		raw_copy_to_user((char *)arg, tmp_element->message, 100);
		kfree(tmp_element);
		break;

	case BRIDGE_STATE_S:
		if (list_empty(&current_list) != 0)
		{
			return_value = 0;
		}
		else
		{
			return_value = 1;
		}
		break;

	case BRIDGE_DESTROY_S:
		printk(KERN_INFO "message %s\n", "bla12");
		break;

	case BRIDGE_CREATE_L:
		create_list();
		break;

	case BRIDGE_W_L:
		raw_copy_from_user(message, (char *)arg, 100);
		add_element_to_list(message);
		break;

	case BRIDGE_R_L:
		tmp_element = list_first_entry(&current_list, struct string_node, list);
		list_del(&(tmp_element->list));
		raw_copy_to_user((char *)arg, tmp_element->message, 100);
		kfree(tmp_element);
		break;

	case BRIDGE_INVERT_L:
		invert_list();
		break;

	case BRIDGE_ROTATE_L:
		rotate_right();
		break;

	case BRIDGE_CLEAN_L:
		clean_list();
		break;

	case BRIDGE_GREATER_VAL_L:
		find_highest_val(arg);
		break;

	case BRIDGE_END_L:
		printk(KERN_INFO "message %s\n", "bla21");
		break;

	case BRIDGE_CONCAT_L:
		printk(KERN_INFO "message %s\n", "bla22");
		break;

	case BRIDGE_STATE_L:
		printk(KERN_INFO "message %s\n", "bla23");
		break;

	case BRIDGE_DESTROY_L:
		destroy_list();
		break;
	
	
	case BRIDGE_RANDOMIZE_L:
		randomize_list();
		break;
	
	}


	return return_value;
}

struct file_operations bridge_fops = {
		.owner = THIS_MODULE,
		.unlocked_ioctl = bridge_ioctl};

/*
 * Finally, the module stuff
 */

/*
 * The cleanup function is used to handle initialization failures as well.
 * Thefore, it must be careful to work correctly even if some of the items
 * have not been initialized
 */
void bridge_cleanup_module(void)
{
	int i;
	dev_t devno = MKDEV(bridge_major, bridge_minor);
	mylist_exit();
	/* Get rid of our char dev entries */
	if (bridge_devices)
	{
		for (i = 0; i < bridge_nr_devs; i++)
		{
			cdev_del(&bridge_devices[i].cdev);
		}
		// Be aware of clean memory completely
		kfree(bridge_devices);
	}

	/* cleanup_module is never called if registering failed */
	unregister_chrdev_region(devno, bridge_nr_devs);
}

/*
 * Set up the char_dev structure for this device.
 */
static void bridge_setup_cdev(struct bridge_dev *dev, int index)
{
	int err, devno;

	devno = MKDEV(bridge_major, bridge_minor + index);
	cdev_init(&dev->cdev, &bridge_fops);
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add(&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding bridge%d", err, index);
}

int bridge_init_module(void)
{
	int result, i;
	dev_t dev = 0;

	/*
	 * Get a range of minor numbers to work with, asking for a dynamic
	 * major unless directed otherwise at load time.
	 */
	if (bridge_major)
	{
		dev = MKDEV(bridge_major, bridge_minor);
		result = register_chrdev_region(dev, bridge_nr_devs, "bridge");
	}
	else
	{
		result = alloc_chrdev_region(&dev, bridge_minor, bridge_nr_devs, "bridge");
		bridge_major = MAJOR(dev);
		bridge_devices = kmalloc(bridge_nr_devs * sizeof(struct bridge_dev), GFP_KERNEL);
	}

	if (result < 0)
	{
		printk(KERN_WARNING "bridge: can't get major %d\n", bridge_major);
		return result;
	}

	/*
	 * allocate the devices -- we can't have them static, as the number
	 * can be specified at load time
	 */
	bridge_devices = kmalloc(bridge_nr_devs * sizeof(struct bridge_dev), GFP_KERNEL);

	if (!bridge_devices)
	{
		result = -ENOMEM;
		goto fail; /* Make this more graceful */
	}

	memset(bridge_devices, 0, bridge_nr_devs * sizeof(struct bridge_dev));

	/* Initialize each device. */
	for (i = 0; i < bridge_nr_devs; i++)
	{
		bridge_setup_cdev(&bridge_devices[i], i);
	}

	return 0; /* succeed */

fail:
	bridge_cleanup_module();
	return result;
}

module_init(bridge_init_module);
module_exit(bridge_cleanup_module);
