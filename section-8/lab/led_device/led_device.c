#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>


#define DRV_NAME       "dungvd_led_device"
#define LED_DEVNAME    "dungvd_led"
#define BTN_DEVNAME    "dungvd_btn"

#define DEV_NUMS        2   /* LED and Button */

/* Device number: major/minor */
static dev_t devno;   
static dev_t led_devno;
static dev_t btn_devno;          
/* Character device structure */
static struct cdev led_cdev;     
static struct cdev btn_cdev;   
/* Device class  */
static struct class *devclass; 

/* ====================== 
 * GPIO LED and Button 
 * ====================== */
#define GPIO_BASE   512
#define NUM_LEDS    4
static int led_pins[NUM_LEDS] = {5, 6, 13, 19};
static int btn_pin = 26;

static inline int led_gpio(int i) { return GPIO_BASE + led_pins[i]; }
static inline int btn_gpio(void) { return GPIO_BASE + btn_pin; }

static int all_led_test(void)
{
    for(int i = 0; i < NUM_LEDS; i++) {
        gpio_set_value(led_gpio(i), 1);  
        msleep(50);
        gpio_set_value(led_gpio(i), 0);  
        msleep(50);
    }
    return 0;
}

static int gpio_setup(void)
{
    int ret;

    ret = gpio_request(btn_gpio(), "BTN1");
    if(ret) {
        pr_err("Failed to request GPIO %d\n", btn_pin);
        return ret;
    }
    gpio_direction_input(btn_gpio());

    for(int i = 0; i < NUM_LEDS; i++) {
        char led_name[6];
        snprintf(led_name, sizeof(led_name), "LED%d", i + 1);
        ret = gpio_request(led_gpio(i), led_name);
        if(ret) {
            pr_err("Failed to request GPIO %d\n", led_pins[i]);
            return ret;
        }
        gpio_direction_output(led_gpio(i), 0);
    }
    all_led_test();
    return 0;
}

static int gpio_cleanup(void)
{
    all_led_test();
    
    gpio_free(btn_gpio());
    for(int i = 0; i < NUM_LEDS; i++) {
        gpio_set_value(led_gpio(i), 0);
        gpio_free(led_gpio(i));
    }
    return 0;
}

/* ====================== 
 * File operations
 * ====================== */

/*
 * Read the LED status
 */
static ssize_t led_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    char s[NUM_LEDS + 2];
    int i;

    for (i = 0; i < NUM_LEDS; i++)
        s[i] = gpio_get_value(led_gpio(i)) ? '1' : '0';
    s[NUM_LEDS] = '\n';
    s[NUM_LEDS + 1] = '\0';

    return simple_read_from_buffer(buf, len, off, s, sizeof(s));
}

/*
 * Write the LED status
 */
static ssize_t led_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    char kbuf[NUM_LEDS];
    if(copy_from_user(kbuf, buf, NUM_LEDS))
        return -EFAULT;

    for(int i = 0; i < NUM_LEDS; i++) {
        if(kbuf[i] == '1')
            gpio_set_value(led_gpio(i), 1);
        else if(kbuf[i] == '0')
            gpio_set_value(led_gpio(i), 0);
        else
            return -EINVAL;
    }

    return len;
}

static const struct file_operations led_fops = {
    .owner   = THIS_MODULE,
    .read    = led_read,
    .write   = led_write,
};

/*
 * Read the button status
 */
static ssize_t btn_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    char s[3];

    s[0] = gpio_get_value(btn_gpio()) ? '1' : '0';
    s[1] = '\n';
    s[2] = '\0';

    return simple_read_from_buffer(buf, len, off, s, sizeof(s));
}

static const struct file_operations btn_fops = {
    .owner   = THIS_MODULE,
    .read    = btn_read,
};

/* ====================== 
 * init/exit functions
 * ====================== */

static int __init my_init(void)
{   
    int ret;
    struct device *dev;
    
    /* Setup LED */
    ret = gpio_setup();
    if (ret)
        return ret;
   
    /* 1. Get major/minor numbers */
    ret = alloc_chrdev_region(&devno, 0, DEV_NUMS, DRV_NAME);
    if (ret)
        goto err_led;

    led_devno = MKDEV(MAJOR(devno), 0); /* Give minor numbers to each device */
    btn_devno = MKDEV(MAJOR(devno), 1);
    
    /* 2. Initialize the character device */
    cdev_init(&led_cdev, &led_fops);
    led_cdev.owner = THIS_MODULE;
    ret = cdev_add(&led_cdev, led_devno, 1);
    if (ret)
        goto err_region;

    cdev_init(&btn_cdev, &btn_fops);
    btn_cdev.owner = THIS_MODULE;
    ret = cdev_add(&btn_cdev, btn_devno, 1);
    if (ret)
        goto err_led_cdev;

    /* 3. Create device class */
    devclass = class_create(DRV_NAME);
    if (IS_ERR(devclass)) {
        ret = PTR_ERR(devclass);
        goto err_btn_cdev;
    }

    /* 4. Create device */
    dev = device_create(devclass, NULL, led_devno, NULL, LED_DEVNAME);
    if (IS_ERR(dev)) {
        ret = PTR_ERR(dev);
        goto err_class;
    }

    dev = device_create(devclass, NULL, btn_devno, NULL, BTN_DEVNAME);
    if (IS_ERR(dev)) {
        ret = PTR_ERR(dev);
        goto err_led_dev;
    }


    return 0;
    /* ----- Error handling ----- */
err_led_dev:   device_destroy(devclass, led_devno);
err_class:     class_destroy(devclass);
err_btn_cdev:  cdev_del(&btn_cdev);
err_led_cdev:  cdev_del(&led_cdev);
err_region:    unregister_chrdev_region(devno, DEV_NUMS);
err_led:       gpio_cleanup();

    return ret;
}

static void __exit my_exit(void)
{
    device_destroy(devclass, led_devno);
    device_destroy(devclass, btn_devno);
    class_destroy(devclass);
    cdev_del(&led_cdev);
    cdev_del(&btn_cdev);
    unregister_chrdev_region(devno, DEV_NUMS);
    gpio_cleanup();

    pr_info("%s: unloaded\n", DRV_NAME);
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("LED device driver");
MODULE_AUTHOR("DungVD");