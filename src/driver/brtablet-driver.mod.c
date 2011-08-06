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
	{ 0x60d90935, "module_layout" },
	{ 0x1bf3efef, "serio_unregister_driver" },
	{ 0xb00d681b, "__serio_register_driver" },
	{ 0x2714cf38, "input_event" },
	{ 0x8235805b, "memmove" },
	{ 0x91a3ab7b, "input_register_device" },
	{ 0xc369e86a, "serio_open" },
	{ 0xee811ee9, "sysfs_create_group" },
	{ 0xbdbf7c18, "platform_device_register_resndata" },
	{ 0x7b048d57, "input_set_abs_params" },
	{ 0x701d0ebd, "snprintf" },
	{ 0x275bf9dc, "input_free_device" },
	{ 0xbeee71a9, "input_allocate_device" },
	{ 0xba236513, "kmem_cache_alloc_trace" },
	{ 0xc6177942, "kmalloc_caches" },
	{ 0x37a0cba, "kfree" },
	{ 0x6c3bf04e, "platform_device_unregister" },
	{ 0xf8f75b6f, "sysfs_remove_group" },
	{ 0x79e41425, "input_unregister_device" },
	{ 0x97f67b83, "dev_set_drvdata" },
	{ 0xfeef09ed, "serio_close" },
	{ 0x50eedeb8, "printk" },
	{ 0x42224298, "sscanf" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0x89ae6e69, "dev_get_drvdata" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("serio:ty02pr03id*ex*");

MODULE_INFO(srcversion, "76BA53B5FF4DA925911B79C");
