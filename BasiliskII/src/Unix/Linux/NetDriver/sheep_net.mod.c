#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
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
__used
__attribute__((section("__versions"))) = {
	{ 0xedeb23cb, "module_layout" },
	{ 0xafdfd517, "misc_deregister" },
	{ 0xd6bb655d, "misc_register" },
	{ 0x92540fbf, "finish_wait" },
	{ 0x8c26d495, "prepare_to_wait_event" },
	{ 0x1000e51, "schedule" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0x4203ec67, "current_task" },
	{ 0x7c32d0f0, "printk" },
	{ 0xda85695c, "netif_rx" },
	{ 0x8a1f9aa3, "eth_type_trans" },
	{ 0x1601b892, "dev_queue_xmit" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x31e615e0, "skb_put" },
	{ 0x51663908, "__netdev_alloc_skb" },
	{ 0x3eeb2322, "__wake_up" },
	{ 0xe20531dc, "skb_queue_tail" },
	{ 0xea83090c, "skb_push" },
	{ 0x39db5265, "kfree_skb" },
	{ 0x10739949, "skb_copy" },
	{ 0xdb7305a1, "__stack_chk_fail" },
	{ 0x89dff613, "dev_mc_del" },
	{ 0xb2fd5ceb, "__put_user_4" },
	{ 0x3812050a, "_raw_spin_unlock_irqrestore" },
	{ 0x51760917, "_raw_spin_lock_irqsave" },
	{ 0xb44ad4b3, "_copy_to_user" },
	{ 0xd2a0db4e, "dev_add_pack" },
	{ 0x540541d0, "sk_alloc" },
	{ 0x2a20dad2, "dev_get_by_name" },
	{ 0x1d1fc3c4, "init_net" },
	{ 0xcb6974a6, "dev_mc_add" },
	{ 0x362ef408, "_copy_from_user" },
	{ 0x79aa04a2, "get_random_bytes" },
	{ 0xd9a5ea54, "__init_waitqueue_head" },
	{ 0x287665f6, "kmem_cache_alloc_trace" },
	{ 0xc52f874a, "kmalloc_caches" },
	{ 0x37a0cba, "kfree" },
	{ 0xccdbacf8, "skb_dequeue" },
	{ 0x2618236d, "consume_skb" },
	{ 0xc0d3c0e6, "sk_free" },
	{ 0x3e42fd34, "dev_remove_pack" },
	{ 0x2ea2c95c, "__x86_indirect_thunk_rax" },
	{ 0xbdfb6dbb, "__fentry__" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

