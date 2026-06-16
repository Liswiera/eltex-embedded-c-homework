#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/rwlock.h>
#include <linux/proc_fs.h>
#include <linux/sysfs.h>
#include <linux/string.h>
#include <linux/kobject.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

#define HW20_BUFF_LEN 32

static int major = 0;
static struct proc_dir_entry *proc_dir_ent = NULL;
static struct kobject *kobj = NULL;
static rwlock_t lock;
static char mod_buff[HW20_BUFF_LEN] = "Hello!\n\0";

static ssize_t dev_read(struct file *fd, char __user *buff, size_t size, loff_t *off) {
    pr_info("hw20_mod: Trying to read from buf.\n");
    pr_info("hw20_mod: read args: buff=*0x%p; buf_size = %zu.\n", buff, size);

    read_lock(&lock);
    ssize_t bytes_read = simple_read_from_buffer(buff, size, off, mod_buff, HW20_BUFF_LEN);
    read_unlock(&lock);

    if (bytes_read < 0) {
        pr_info("hw20_mod2: read error: %zd\n", bytes_read);
    }

    return bytes_read;
}

static ssize_t dev_write(struct file *fd, const char __user *buff, size_t size, loff_t *off) {
    pr_info("hw20_mod: Trying to write into buf.\n");
    pr_info("hw20_mod: write args: buff=*0x%p; buf_size = %zu.\n", buff, size);

    if (size > HW20_BUFF_LEN) {
        return -EINVAL;
    }

    write_lock(&lock);
    ssize_t bytes_written = simple_write_to_buffer(mod_buff, HW20_BUFF_LEN, off, buff, size);
    write_unlock(&lock);

    return bytes_written;
}

static ssize_t proc_read(struct file *fd, char __user *buff, size_t size, loff_t *off) {
    return dev_read(fd, buff, size, off);
}

static ssize_t proc_write(struct file *fd, const char __user *buff, size_t size, loff_t *off) {
    return dev_write(fd, buff, size, off);
}

static loff_t proc_lseek(struct file *fd, loff_t off, int whence) {
    return fixed_size_llseek(fd, off, whence, HW20_BUFF_LEN);
}

static ssize_t mod_buff_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    memcpy(buf, mod_buff, HW20_BUFF_LEN);
    return strnlen(buf, HW20_BUFF_LEN);
}

static ssize_t mod_buff_store(struct kobject *kobj, struct kobj_attribute *attr, char const *buf, size_t count) {
    memcpy(mod_buff, buf, HW20_BUFF_LEN);
    return strnlen(buf, HW20_BUFF_LEN);
}


static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = dev_read,
    .write = dev_write,
};

static const struct proc_ops pops = {
    .proc_read = proc_read,
    .proc_write = proc_write,
    .proc_lseek = proc_lseek,
};

static struct kobj_attribute string_attribute = __ATTR_RW(mod_buff);

static struct attribute *attrs[] = {
    &string_attribute.attr,
    NULL,
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

int init_module(void) {
    rwlock_init(&lock);

    major = register_chrdev(major, "hw20_mod", &fops);
    if (major < 0) {
        return major;
    }

    proc_dir_ent = proc_create("hw20_mod", S_IRUGO | S_IWUGO, NULL, &pops);
    if (proc_dir_ent == NULL) {
        return -ENOMEM;
    }

    kobj = kobject_create_and_add("hw20_kobj", kernel_kobj);
    if (kobj == NULL) {
        return -ENOMEM;
    }

    int retval = sysfs_create_group(kobj, &attr_group);
    if (retval != 0) {
        kobject_put(kobj);
        return retval;
    }

    pr_info("hw20_mod: Module loaded. Major number is %d.\n", major);
    return 0;
}

void cleanup_module(void) {
    unregister_chrdev(major, "hw20_mod");
    proc_remove(proc_dir_ent);
    kobject_put(kobj);

    pr_info("hw20_mod: Module unloaded.\n");
}
