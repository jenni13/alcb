/*
Nom : MANFREDO
Prenom : Jennifer
Compte examen : camsi10*/

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
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/hdreg.h>
#include <linux/bio.h>

#define LICENCE "GPL"

#define KERNEL_SECTOR_SIZE 512

static int hardsect_size = 512;
static int nsectors = 1024;
static int major_num = 0;
static struct request_queue *Queue;

//static int rb_open(struct block_device *dev, fmode_t mode);
//static int rb_getgeo(struct block_device *dev,struct hd_geometry *geo);

char *NomUtilisateur="Jennifer";
module_param(nsectors, int, 0);

static struct rb_device
{
	unsigned long size;
	spinlock_t lock;
	struct request_queue *rb_queue;
	struct gendisk *rb_disk;
	uint8_t *data;
}rb_dev;

static int rb_open(struct block_device *dev, fmode_t mode)
{
        printk(KERN_ALERT "Appel rb_open\n");
        return 0;
}
static int rb_release(struct inode *inode, struct file *filp,unsigned int cmd,unsigned long arg)
{
	printk(KERN_ALERT "Appel rb_release\n");
	return 0;
}

static int rb_getgeo(struct block_device *dev, struct hd_geometry *geo)
{
	printk(KERN_ALERT "Appel de rb_getgeo\n");
        return 0;
}

int rb_ioctl(struct inode *inode,struct file *filp,unsigned int cmd,unsigned long arg)
{
	long size;
	struct hd_geometry *geo;
	
	switch(cmd){
		case HDIO_GETGEO:
			printk(KERN_ALERT "Appel de rb_getgeo\n");
			size =  rb_dev.size*(hardsect_size / KERNEL_SECTOR_SIZE);
        		geo -> cylinders = (size & ~0x3f) >> 6;
       			geo -> heads = 4;
        		geo -> sectors = 16;
        		geo -> start = 0;
        		if(copy_to_user((void * ) arg,&geo,sizeof(geo)))
                		return -EFAULT;
			return 0;
		default:
			return -ENOTTY;
	}
}


static struct block_device_operations rb_fops =
{
	.owner = THIS_MODULE,
	.open = rb_open,
	.release = rb_release,
        .getgeo = rb_getgeo,
        .ioctl = rb_ioctl,

};

static void rb_transfert(struct rb_device *dev, unsigned long sector,
						unsigned long nsect, char *buffer, int write)
{
	unsigned long offset = sector*hardsect_size;
	unsigned long nbytes = nsect*hardsect_size;

	if((offset + nbytes) > dev -> size)
	{
		printk(KERN_NOTICE "probleme transfert\n");
		return;
	}
	if(write)
		memcpy(dev->data+offset,buffer,nbytes);
	else
		memcpy(buffer,dev->data+offset,nbytes);
}

static void rb_request(struct request_queue *q)
{
	struct request *req;
	while((req = blk_fetch_request(q)) != NULL)
	{
		if(req == NULL || (req -> cmd_type != REQ_TYPE_FS))
		{
			printk(KERN_NOTICE "Skip non - fs request\n");
			blk_end_request_all(req,0);
			continue;
		}

		rb_transfert(&rb_dev,blk_rq_pos(req), blk_rq_cur_sectors(req),
					bio_data(req -> bio), rq_data_dir(req));
		if(! __blk_end_request_cur(req,0))
			req = blk_fetch_request(q);
	}
}

int blk_init(void)
{
	printk(KERN_ALERT "Hello %s !\n", NomUtilisateur);

	rb_dev.size = nsectors*hardsect_size;
	spin_lock_init(&rb_dev.lock);
	rb_dev.data = vmalloc(rb_dev.size);
	if(rb_dev.data == NULL)
		return -ENOMEM;
	Queue = blk_init_queue(rb_request,&rb_dev.lock);
	if(Queue == NULL)
		goto out;
	//blk_queue_hardsect_size(Queue,hardsect_size);


	major_num = register_blkdev(major_num,"blc");

	if(major_num<=0)
	{
		printk(KERN_WARNING "blc, unable to get major number\n");
		goto out;
	}

	rb_dev.rb_disk = alloc_disk(16);
	if(!rb_dev.rb_disk)
		goto out_unreg;

	// Structures allocation
	rb_dev.rb_disk->major = major_num;
	rb_dev.rb_disk->first_minor = 0;
	rb_dev.rb_disk->fops = &rb_fops;
	rb_dev.rb_disk->private_data = &rb_dev;
	strcpy (rb_dev.rb_disk->disk_name, "sbd0");
	rb_dev.size = nsectors*hardsect_size;
	set_capacity(rb_dev.rb_disk,nsectors*(hardsect_size/KERNEL_SECTOR_SIZE));
	rb_dev.rb_disk->queue = Queue;
	add_disk(rb_dev.rb_disk);

	return 0;

	out:
			vfree(rb_dev.data);
			return -ENOMEM;
	out_unreg:
			unregister_blkdev(major_num,"sbd0");
			return -ENOMEM;

}


static void blk_cleanup(void)
{
    printk(KERN_ALERT "Goodbye %s\n", NomUtilisateur);

	del_gendisk(rb_dev.rb_disk);
	put_disk(rb_dev.rb_disk);
	unregister_blkdev(major_num,"sdb0");
	blk_cleanup_queue(Queue);
	vfree(rb_dev.data);
}

module_exit(blk_cleanup);
module_init(blk_init);


MODULE_LICENSE(LICENCE);
//MODULE_AUTHOR(AUTEUR);
//MODULE_DESCRIPTION(DESCRIPTION);
//MODULE_SUPPORTED_DEVICE(DEVICE);

