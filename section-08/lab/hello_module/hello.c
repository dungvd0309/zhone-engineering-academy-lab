#include <linux/module.h>
#include <linux/moduleparam.h>

static char *module_name = "hello";
static int count = 1;

module_param(count, int, 0644);
MODULE_PARM_DESC(count, "How many times to greet");

static int hello_init(void)
{
    for(int i = 0; i < count; i++)
        pr_info("%s: Hello World %d\n", module_name, i + 1);
    return 0;
}

static void hello_exit(void)
{
    pr_info("%s: Goodbye cruel world!\n", module_name);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DungVD");
MODULE_DESCRIPTION("A Hello, World Module");