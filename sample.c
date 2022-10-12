#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>

#define DEV_NAME "sample"

struct sample_dev_t
{
	dev_t id;
	struct cdev cdev;
	struct class *class;
	struct device *dev;
} *sample_dev;

static int sample_open(struct inode *inode, struct file *filp)
{
	pr_info("open\n");
	return 0;
}

static int sample_close(struct inode *inode, struct file * filp)
{
	pr_info("close\n");
	return 0;
}

static ssize_t sample_read(struct file *filp, char __user *buf,
                size_t size, loff_t *offset)
{
	pr_info("read\n");
	return 0;
}

static ssize_t sample_write(struct file *filp, const char __user *buf,
                size_t size, loff_t *offset)
{
	pr_info("write\n");
	return 0;
}

static long sample_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	pr_info("ioctl cmd %d arg %ld\n", cmd, arg);
	return 0;
}

struct file_operations sample_fops = {
	.owner          = THIS_MODULE,
	.open           = sample_open,
	.release        = sample_close,
	.read           = sample_read,
	.write          = sample_write,
	.unlocked_ioctl = sample_ioctl,
};

static int __init sample_init(void)
{
	int err;

	pr_info("%s:init\n", DEV_NAME);

	sample_dev = kmalloc(sizeof(struct sample_dev_t), GFP_KERNEL);
	if (!sample_dev) {
		pr_err("kmalloc err\n");
		return -ENOMEM;
	}

	err = alloc_chrdev_region(&sample_dev->id, 0, 1, DEV_NAME);
	if (err) {
		pr_err("alloc_chrdev_region err %d\n", err);
		goto out_free;
	}

	cdev_init(&sample_dev->cdev, &sample_fops);
	sample_dev->cdev.owner = THIS_MODULE;

	err = cdev_add(&sample_dev->cdev, sample_dev->id, 1);
	if (err) {
		pr_err("cdev_add err %d\n", err);
		goto out_unregister;
	}

	sample_dev->class = class_create(THIS_MODULE, DEV_NAME);
	if (IS_ERR(sample_dev->class)) {
		err = PTR_ERR(sample_dev->class);
		pr_err("class_create err %d\n", err);
		goto out_cdev_del;
	}

	sample_dev->dev = device_create(sample_dev->class, NULL,
			sample_dev->id, NULL, DEV_NAME);
	if (IS_ERR(sample_dev->dev)) {
		err = PTR_ERR(sample_dev->dev);
		pr_err("device_create err %d\n", err);
		goto out_class_destroy;
	}

	return 0;

out_class_destroy:
	class_destroy(sample_dev->class);

out_cdev_del:
	cdev_del(&sample_dev->cdev);

out_unregister:
	unregister_chrdev_region(sample_dev->id, 1);

out_free:
	kfree(sample_dev);

	return err;
}

static void __exit sample_exit(void)
{
	pr_info("exit\n");

	device_destroy(sample_dev->class, sample_dev->id);
	class_destroy(sample_dev->class);
	cdev_del(&sample_dev->cdev);
	unregister_chrdev_region(sample_dev->id, 1);
	kfree(sample_dev);
}

module_init(sample_init);
module_exit(sample_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("kernel module sample");

