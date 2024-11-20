#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x2ca90791, "module_layout" },
	{ 0xe180ce57, "usb_deregister" },
	{ 0x6d40fcba, "tty_driver_kref_put" },
	{ 0x2f3a5006, "tty_unregister_driver" },
	{ 0x942410fb, "usb_register_driver" },
	{ 0x8887ba02, "tty_register_driver" },
	{ 0x7ba0cb41, "__tty_alloc_driver" },
	{ 0xc365003e, "tty_port_register_device" },
	{ 0x1f00d418, "tty_port_init" },
	{ 0x437b7175, "usb_alloc_urb" },
	{ 0x1bd93c1b, "usb_alloc_coherent" },
	{ 0xa3859b3a, "kmem_cache_alloc_trace" },
	{ 0x3d7b9a95, "kmalloc_caches" },
	{ 0x69acdf38, "memcpy" },
	{ 0xb20c3797, "usb_submit_urb" },
	{ 0x507e74e3, "tty_flip_buffer_push" },
	{ 0xe4defd1f, "tty_insert_flip_string_fixed_flag" },
	{ 0x37a0cba, "kfree" },
	{ 0x14e15068, "tty_port_destroy" },
	{ 0x1da20e2b, "tty_unregister_device" },
	{ 0x77bed45b, "usb_free_coherent" },
	{ 0x2f886acb, "usb_free_urb" },
	{ 0x510508ea, "usb_kill_urb" },
	{ 0x92997ed8, "_printk" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("usb:v2341p0042d*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "6E3C7AC2E8E3644C32D7C90");
