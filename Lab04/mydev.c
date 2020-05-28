#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>


MODULE_LICENSE("GPL");


/*Buffer to store data */
char *tx_buffer;   /*data to write in*/
char *rx_buffer;   /*data to read out*/

int BUFFER_SIZE=8;
int actual_rx_size = 0;
/* file operation */

static ssize_t my_read(struct file *filp,char *buf, size_t count, loff_t *fops){

	printk("call read\n");

	/*Transfering data to user space*/
	int retval = copy_to_user(buf,rx_buffer,8);

	return count;

}



static ssize_t my_write(struct file *fp,const char *buf, size_t count, loff_t *fops){

	printk("call write\n");

	/*Zero the input buffer*/
	//memset(tx_buffer,0,BUFFER_SIZE);
	//memset(rx_buffer,0,BUFFER_SIZE);

	/* Transfering data from user space */
	int ret = copy_from_user(tx_buffer,buf,8);

	/* tx_buffer copy to rx_buffer */
	memcpy(rx_buffer,tx_buffer,8);


	return count;
} 



static int my_open(struct inode *inode, struct file *fp){

	printk("call open\n");

	return 0;
}

static int my_close(struct inode *inode, struct file *fp){

	printk("call close\n");

	return 0;
}

struct file_operations my_fops = {
	read: my_read,
	write: my_write,
	open: my_open,
        release: my_close
};


#define MAJOR_NUM 244
#define DEVICE_NAME "my_dev"



static int my_init(void) {

	printk("call init\n");
	if (register_chrdev(MAJOR_NUM, DEVICE_NAME, &my_fops)<0){
		printk("Can not get major %d\n",MAJOR_NUM);
		return (-EBUSY);
	}

	printk("My device is started and the major is %d\n",MAJOR_NUM);

	/*Allocating memory for the buffers*/    /*Allocate buffers*/
	
	tx_buffer = kmalloc(BUFFER_SIZE,GFP_KERNEL);
	rx_buffer = kmalloc(BUFFER_SIZE,GFP_KERNEL);

	/*Check allocation was ok*/
	if ( !tx_buffer || !rx_buffer){
	   return -1;
	}

	/*Reset the buffers*/
	memset(tx_buffer,0,BUFFER_SIZE);
	memset(rx_buffer,0,BUFFER_SIZE);

	printk("Inserting Memory module\n");
	return 0;
}



static void my_exit(void) {
	unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
	printk("call exit\n");

	/*Free the buffers*/
	kfree(tx_buffer);
	kfree(rx_buffer);
}

module_init(my_init);
module_exit(my_exit);






