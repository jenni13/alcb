/*
Nom : MANFREDO
Prenom : Jennifer
Compte examen : camsi10
*/

/* Includes */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/mutex.h>
#include <linux/device.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/blk_types.h>

#define LICENCE "GPL"
#define AUTEUR "Manfredo_Jennifer"
#define DESCRIPTION "Exemple de module Master CAMSI"
#define DEVICE "My device"
#define KERNEL_SECTOR_SIZE 512

static int hardsect_size = 512;
static int nsectors = 1024; 
static int major_num = 0;
static struct request_queue *Queue;

static int rb_close(struct inode *inode,struct file *filp);

static int rb_open(struct block_device *dev, fmode_t mode);
static int rb_getgeo(struct block_device *dev,struct hd_geometry *geo);

char *NomUtilisateur="Jennifer";
module_param(NomUtilisateur, charp, S_IRUGO);

static struct rb_device
{
	unsigned int size;
	spinlock_t lock;
	struct request_queue *rb_queue;
	struct gendisk *rb_disk;
}rb_dev;

static struct block_device_operations rb_fops =
{
	.owner = THIS_MODULE,
	.open = rb_open,
	.getgeo = rb_getgeo,
	.release = rb_close,
};

static int rb_open(struct block_device *dev, fmode_t mode)
{
	
	return 0;
}
static int rb_close(struct inode *inode,struct file *filp)
{
	printk(KERN_ALERT "Appel de rb_close");
	return 0;

}
static int rb_getgeo(struct block_device *dev,struct hd_geometry *geo)
{
	return 0;
}


int sample_init(void) 
{
	printk(KERN_ALERT "Hello %s !\n", NomUtilisateur);

	// Dynamic allocation for (major,minor) 
	/*if (alloc_chrdev_region(&dev,0,2,"sample") == -1)
	{
		printk(KERN_ALERT ">>> ERROR alloc_chrdev_region\n");
		return -EINVAL;
	}
	// Print out the values 
	printk(KERN_ALERT "Init allocated (major, minor)=(%d,%d)\n",MAJOR(dev),MINOR(dev));
	*/
	// Structures allocation 
	rb_dev.rb_disk->major = major_num;
	rb_dev.rb_disk->first_minor = 0;
	rb_dev.rb_disk->fops = &rb_fops;
	rb_dev.rb_disk->private_data = &rb_dev;
    strcpy (rb_dev.rb_disk->disk_name, "sbd0");

	spin_lock_init(&rb_dev.lock);

	major_num = register_blkdev(major_num,"blc");

	if(major_num<=0)
	{
		printk(KERN_WARNING "blc, unable to get major number\n");
		goto out;
	}
	rb_dev.rb_disk = alloc_disk(16);
	if(!rb_dev.rb_disk)
	{
		goto out_unreg;
		
	}
	set_capacity(rb_dev.rb_disk,nsectors*(hardsect_size/KERNEL_SECTOR_SIZE));
	rb_dev.rb_disk->queue = Queue;
	add_disk(rb_dev.rb_disk);

	return 0;

	out:
			vfree(rb_dev.rb_queue);
			return -ENOMEM;
	out_unreg:
			unregister_blkdev(major_num,"blc");
			return -ENOMEM;
	
}


static void sample_cleanup(void) 
{
    printk(KERN_ALERT "Goodbye %s\n", NomUtilisateur);
    
	// Unregister 
	//unregister_chrdev_region(dev,2);
	// Delete cdev 
	//cdev_del(my_cdev);
	//blk
	del_gendisk(rb_dev.rb_disk);
	put_disk(rb_dev.rb_disk);
	blk_cleanup_queue(rb_dev.rb_queue);
	vfree(rb_dev.rb_queue);
}

module_exit(sample_cleanup);
module_init(sample_init);


MODULE_LICENSE(LICENCE);
MODULE_AUTHOR(AUTEUR);
MODULE_DESCRIPTION(DESCRIPTION);
MODULE_SUPPORTED_DEVICE(DEVICE);

