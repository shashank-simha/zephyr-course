#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

static int hello_world_board_init(void)
{
	printk("Board Initialized\n");
	return 0;
}

SYS_INIT(hello_world_board_init, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
