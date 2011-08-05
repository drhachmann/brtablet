#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x4d5503c4, "module_layout" },
	{ 0x6c3bf04e, "platform_device_unregister" },
	{ 0xf8f75b6f, "sysfs_remove_group" },
	{ 0xe35a80aa, "input_unregister_device" },
	{ 0x1e47efc8, "input_register_device" },
	{ 0x72cb948f, "input_allocate_device" },
	{ 0xee811ee9, "sysfs_create_group" },
	{ 0xbdbf7c18, "platform_device_register_resndata" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0x42224298, "sscanf" },
	{ 0x50eedeb8, "printk" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "8B160D909BCFE55F1048F30");
