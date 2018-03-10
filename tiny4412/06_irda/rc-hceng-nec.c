#include <media/rc-map.h>
#include <linux/module.h>

static struct rc_map_table hceng_nec[] = {
	{ 0x45, KEY_POWER},
	{ 0x46, KEY_UP},
	{ 0x47, KEY_MODE},
	{ 0x44, KEY_RIGHT},
	{ 0x40, KEY_OK},
	{ 0x37, KEY_LEFT},
	{ 0x07, KEY_VOLUMEDOWN},
	{ 0x15, KEY_DOWN},
	{ 0x09, KEY_VOLUMEUP},
	
	{ 0x16, KEY_1},
	{ 0x19, KEY_2},
	{ 0x0d, KEY_3},
	{ 0x0c, KEY_4},
	{ 0x18, KEY_5},
	{ 0x5e, KEY_6},
	{ 0x08, KEY_7},
	{ 0x1c, KEY_8},
	{ 0x5a, KEY_9},
	{ 0x42, KEY_0},
	
	{ 0x4a, KEY_BACK},
};

static struct rc_map_list hceng_nec_map = {
	.map = {
		.scan    = hceng_nec,
		.size    = ARRAY_SIZE(hceng_nec),
		.rc_type = RC_TYPE_NEC, //RC_TYPE_UNKNOWN //echo nec > /sys/class/rc/rc0/protocols
		.name    = "rc-hceng-nec",
	}
};

static int __init init_rc_map_hceng_nec(void)
{
	return rc_map_register(&hceng_nec_map);
}

static void __exit exit_rc_map_hceng_nec(void)
{
	rc_map_unregister(&hceng_nec_map);
}

module_init(init_rc_map_hceng_nec)
module_exit(exit_rc_map_hceng_nec)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("hceng");
