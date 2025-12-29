#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include "queue_ioctl.h"


#define DEVICE_NAME "swaraj_testing"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Swaraj");
MODULE_DESCRIPTION("Dynamic Circular Queue Driver with IOCTL");
MODULE_VERSION("1.0");


static dev_t dev;
static struct cdev cdev;
static struct class *cls;
static char *queue;
static int q_size;
static int head, tail, count;
static DEFINE_MUTEX(q_lock);
static DECLARE_WAIT_QUEUE_HEAD(read_wq);


static int queue_push(const char *buf, int len)
{
    int i;

    if (!queue || len > q_size - count)
        return -ENOMEM;

    for (i = 0; i < len; i++) {
        queue[tail] = buf[i];
        tail = (tail + 1) % q_size;
    }
    count += len;

    pr_info("queue_push: pushed %d bytes, count=%d, head=%d, tail=%d\n",
            len, count, head, tail);
    return 0;
}


static int queue_pop(char *buf, int len)
{
    int i;

    if (!queue || len > count)
        return -EINVAL;

    for (i = 0; i < len; i++) {
        buf[i] = queue[head];
        head = (head + 1) % q_size;
    }
    count -= len;

    pr_info("queue_pop: popped %d bytes, count=%d, head=%d, tail=%d\n",
            len, count, head, tail);
    return 0;
}


static long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct data d;
    char *kbuf;
    int ret = 0;

    switch (cmd) {
    case SET_SIZE_OF_QUEUE: {
        int size;
        if (copy_from_user(&size, (int __user *)arg, sizeof(int)))
            return -EFAULT;
        if (size <= 0)
            return -EINVAL;

        mutex_lock(&q_lock);
        kfree(queue);
        queue = kmalloc(size, GFP_KERNEL);
        if (!queue) {
            mutex_unlock(&q_lock);
            return -ENOMEM;
        }

        q_size = size;
        head = tail = count = 0;
        mutex_unlock(&q_lock);

        pr_info("SET_SIZE_OF_QUEUE: initialized queue with size %d\n", size);
        break;
    }

    case PUSH_DATA:
        if (copy_from_user(&d, (void __user *)arg, sizeof(d)))
            return -EFAULT;

        if (d.length <= 0 || d.length > q_size)
            return -EINVAL;

        kbuf = kmalloc(d.length, GFP_KERNEL);
        if (!kbuf)
            return -ENOMEM;

        if (copy_from_user(kbuf, d.data, d.length)) {
            kfree(kbuf);
            return -EFAULT;
        }

        mutex_lock(&q_lock);
        ret = queue_push(kbuf, d.length);
        mutex_unlock(&q_lock);
        kfree(kbuf);

        if (!ret)
            wake_up_interruptible(&read_wq);

        break;

    case POP_DATA:
        if (copy_from_user(&d, (void __user *)arg, sizeof(d)))
            return -EFAULT;

        if (d.length <= 0 || d.length > q_size)
            return -EINVAL;

        ret = wait_event_interruptible(read_wq, count >= d.length);
        if (ret)
            return ret;

        kbuf = kmalloc(d.length, GFP_KERNEL);
        if (!kbuf)
            return -ENOMEM;

        mutex_lock(&q_lock);
        ret = queue_pop(kbuf, d.length);
        mutex_unlock(&q_lock);

        if (!ret) {
            if (copy_to_user(d.data, kbuf, d.length))
                ret = -EFAULT;
        }
        kfree(kbuf);
        break;

    default:
        return -EINVAL;
    }

    return ret;
}


static int dev_open(struct inode *inode, struct file *file)
{
    pr_info("Device opened by process %d\n", current->pid);
    return 0;
}


static int dev_release(struct inode *inode, struct file *file)
{
    pr_info("Device closed by process %d\n", current->pid);
    return 0;
}


static ssize_t dev_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    pr_info("Read called by process %d\n", current->pid);
    return 0;
}


static ssize_t dev_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    pr_info("Write called by process %d\n", current->pid);
    return len;
}


static const struct file_operations fops = {
    .owner          = THIS_MODULE,
    .unlocked_ioctl = device_ioctl,
    .open           = dev_open,
    .release        = dev_release,
    .read           = dev_read,
    .write          = dev_write,
};


static int __init queue_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret)
        return ret;

    cdev_init(&cdev, &fops);
    ret = cdev_add(&cdev, dev, 1);
    if (ret)
        goto unregister_chrdev;

    cls = class_create(DEVICE_NAME);  // Linux 6.x fix
    if (IS_ERR(cls)) {
        ret = PTR_ERR(cls);
        goto del_cdev;
    }

    if (IS_ERR(device_create(cls, NULL, dev, NULL, DEVICE_NAME))) {
        ret = -EINVAL;
        goto destroy_class;
    }

    pr_info("swaraj_testing: module loaded\n");
    return 0;

destroy_class:
    class_destroy(cls);
del_cdev:
    cdev_del(&cdev);
unregister_chrdev:
    unregister_chrdev_region(dev, 1);
    return ret;
}


static void __exit queue_exit(void)
{
    kfree(queue);
    device_destroy(cls, dev);
    class_destroy(cls);
    cdev_del(&cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("swaraj_testing: module unloaded\n");
}


module_init(queue_init);
module_exit(queue_exit);
