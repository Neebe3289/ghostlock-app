#ifndef OFFSETS_H
#define OFFSETS_H

#include <stdint.h>

struct kernel_offsets {
  const char *uname_r;
  /* Bootloader-selected physical load address; 0 uses target.h. */
  uint64_t kernel_phys_load;
  /* pselect fd_set waiter word shift; 0 uses target.h default. */
  int pselect_waiter_shift;
  /* IPv6 mcast route: offset of the futex waiter inside the 264-byte
   * setsockopt(AF_INET6, IPPROTO_IPV6, 46) copy; 0 uses the pselect route. */
  uint32_t mcast_payload_off;
  uint64_t off_init_task, off_init_cred;
  uint64_t off_root_task_group, off_selinux_enforcing;
  uint64_t off_selinux_blob_sizes, off_security_hook_heads, off_kmalloc_caches;
  uint64_t off_anon_pipe_buf_ops, off_ashmem_misc_fops, off_ashmem_fops;
  uint64_t off_ashmem_ioctl, off_ashmem_compat_ioctl, off_ashmem_mmap;
  uint64_t off_ashmem_open, off_ashmem_release, off_ashmem_show_fdinfo;
  uint64_t off_configfs_read_iter, off_configfs_bin_write_iter;
  uint64_t off_copy_splice_read, off_noop_llseek;
  uint64_t off_slide_nfulnl_logger, off_slide_loggers_0_1, off_slide_boot_id;

  /* Per-kernel struct offsets; 0 uses target.h defaults. */
  uint32_t task_usage;
  uint32_t task_prio, task_normal_prio, task_sched_task_group;
  uint32_t task_pi_lock, task_pi_waiters, task_pi_top_task, task_pi_blocked_on;
  uint32_t task_pid, task_tgid, task_atomic_flags;
  uint32_t task_real_cred, task_cred, task_comm, task_tasks, task_seccomp;

  /* rt_mutex_waiter (fake waiter payload); 0 uses target.h defaults. */
  uint32_t waiter_tree, waiter_pi_tree, waiter_task, waiter_lock;
  uint32_t waiter_wake_state, waiter_ww_ctx;
  uint32_t waiter_tree_prio, waiter_tree_deadline;
  uint32_t waiter_pi_tree_prio, waiter_pi_tree_deadline;

  /* cred; 0 uses target.h defaults. */
  uint32_t cred_uid, cred_securebits, cred_caps, cred_security;

  /* file_operations; 0 uses target.h defaults. */
  uint32_t fops_llseek, fops_read, fops_write, fops_read_iter, fops_write_iter;
  uint32_t fops_ioctl, fops_compat_ioctl, fops_mmap, fops_open, fops_release;
  uint32_t fops_splice_read, fops_show_fdinfo;

  /* struct page / mm_struct; 0 uses target.h defaults. */
  uint32_t struct_page_size, struct_page_compound_head, struct_page_type;
  uint32_t struct_slab_cache, struct_mm_struct;
};

#define OFFSETS_ENTRY(uname, ...) { .uname_r = uname, __VA_ARGS__ }

#define STRUCT_OFFSETS_6_12                                                    \
  .task_usage = 0x40,                                                          \
  .task_prio = 0x94, .task_normal_prio = 0x9C, .task_sched_task_group = 0x420, \
  .task_pi_lock = 0x9EC, .task_pi_waiters = 0xA00,                             \
  .task_pi_top_task = 0xA10, .task_pi_blocked_on = 0xA18,                      \
  .task_pid = 0x708, .task_tgid = 0x70C,                                       \
  .task_atomic_flags = 0x6C8, .task_real_cred = 0x8F8, .task_cred = 0x900,     \
  .task_comm = 0x910, .task_tasks = 0x638, .task_seccomp = 0x9C8,              \
  .waiter_tree = 0x00, .waiter_pi_tree = 0x28, .waiter_task = 0x50,            \
  .waiter_lock = 0x58, .waiter_wake_state = 0x60, .waiter_ww_ctx = 0x68,       \
  .waiter_tree_prio = 0x18, .waiter_tree_deadline = 0x20,                      \
  .waiter_pi_tree_prio = 0x40, .waiter_pi_tree_deadline = 0x48,                \
  .cred_uid = 0x08, .cred_securebits = 0x28, .cred_caps = 0x30,                \
  .cred_security = 0x80,                                                       \
  .fops_llseek = 0x10, .fops_read = 0x18, .fops_write = 0x20,                  \
  .fops_read_iter = 0x28, .fops_write_iter = 0x30, .fops_ioctl = 0x50,         \
  .fops_compat_ioctl = 0x58, .fops_mmap = 0x60, .fops_open = 0x68,             \
  .fops_release = 0x78, .fops_splice_read = 0xb8, .fops_show_fdinfo = 0xd8,    \
  .struct_page_size = 0x40, .struct_page_compound_head = 0x08,                 \
  .struct_page_type = 0x30, .struct_slab_cache = 0x08, .struct_mm_struct = 0x500

#define STRUCT_OFFSETS_6_6                                                     \
  .task_usage = 0x40,                                                          \
  .task_prio = 0x84, .task_normal_prio = 0x8C, .task_sched_task_group = 0x348, \
  .task_pi_lock = 0x90C, .task_pi_waiters = 0x920,                             \
  .task_pi_top_task = 0x930, .task_pi_blocked_on = 0x938,                      \
  .task_pid = 0x618, .task_tgid = 0x61C,                                       \
  .task_atomic_flags = 0x5D8, .task_real_cred = 0x818, .task_cred = 0x820,     \
  .task_comm = 0x830, .task_tasks = 0x550, .task_seccomp = 0x8E8,              \
  .waiter_tree = 0x00, .waiter_pi_tree = 0x28, .waiter_task = 0x50,            \
  .waiter_lock = 0x58, .waiter_wake_state = 0x60, .waiter_ww_ctx = 0x68,       \
  .waiter_tree_prio = 0x18, .waiter_tree_deadline = 0x20,                      \
  .waiter_pi_tree_prio = 0x40, .waiter_pi_tree_deadline = 0x48,                \
  .cred_uid = 0x08, .cred_securebits = 0x28, .cred_caps = 0x30,                \
  .cred_security = 0x80,                                                       \
  .fops_llseek = 0x08, .fops_read = 0x10, .fops_write = 0x18,                  \
  .fops_read_iter = 0x20, .fops_write_iter = 0x28, .fops_ioctl = 0x48,         \
  .fops_compat_ioctl = 0x50, .fops_mmap = 0x58, .fops_open = 0x68,             \
  .fops_release = 0x78, .fops_splice_read = 0xb8, .fops_show_fdinfo = 0xd8,    \
  .struct_page_size = 0x40, .struct_page_compound_head = 0x08,                 \
  .struct_page_type = 0x30, .struct_slab_cache = 0x08, .struct_mm_struct = 0x500

#define STRUCT_OFFSETS_5_15                                                    \
  .task_usage = 0x38,                                                          \
  .task_prio = 0x7C, .task_normal_prio = 0x84, .task_sched_task_group = 0x400, \
  .task_pi_lock = 0x884, .task_pi_waiters = 0x898,                             \
  .task_pi_top_task = 0x8A8, .task_pi_blocked_on = 0x8B0,                      \
  .task_pid = 0x5D8, .task_tgid = 0x5DC,                                       \
  .task_atomic_flags = 0x598, .task_real_cred = 0x790, .task_cred = 0x798,     \
  .task_comm = 0x7A8, .task_tasks = 0x4D0, .task_seccomp = 0x860,              \
  .waiter_tree = 0x00, .waiter_pi_tree = 0x18, .waiter_task = 0x30,            \
  .waiter_lock = 0x38, .waiter_wake_state = 0x40, .waiter_ww_ctx = 0x50,       \
  .waiter_tree_prio = 0x44, .waiter_tree_deadline = 0x48,                      \
  .waiter_pi_tree_prio = 0x44, .waiter_pi_tree_deadline = 0x48,                \
  .cred_uid = 0x04, .cred_securebits = 0x24, .cred_caps = 0x28,                \
  .cred_security = 0x78,                                                       \
  .fops_llseek = 0x08, .fops_read = 0x10, .fops_write = 0x18,                  \
  .fops_read_iter = 0x20, .fops_write_iter = 0x28, .fops_ioctl = 0x50,         \
  .fops_compat_ioctl = 0x58, .fops_mmap = 0x60, .fops_open = 0x70,             \
  .fops_release = 0x80, .fops_splice_read = 0xc8, .fops_show_fdinfo = 0xe0,    \
  .struct_page_size = 0x40, .struct_page_compound_head = 0x08,                 \
  .struct_page_type = 0x30, .struct_slab_cache = 0x18, .struct_mm_struct = 0x400

static const struct kernel_offsets known_offsets[] = {
/* Add new kernels by creating src/kernels/<uname-release>/offsets.h */
#include "5.15.194-android13-8-00019-gf4321180a397-ab15212794/offsets.h"
#include "6.6.77-android15-8-g4a507830d890-ab13636293-4k/offsets.h"
#include "6.6.77-android15-8-g63ce7556864c-ab13994517-4k/offsets.h"
#include "6.6.77-android15-8-gca30f3b4bef6-abogki440974771-4k/offsets.h"
#include "6.6.77-android15-8-gf9a1d4bd8353-abogki440974771-4k/offsets.h"
#include "6.6.89-android15-8-g096cdb6ecefc-ab14358676-4k/offsets.h"
#include "6.6.118-android15-8-g2e6b9c3812c5-ab15114928-4k/offsets.h"
#include "6.6.118-android15-8-g93e223c276e7-abogki500782043-4k/offsets.h"
#include "6.6.118-android15-8-g608a629fedf7-ab15154340-4k/offsets.h"
#include "6.6.118-android15-8-ge56cf6b09cca-ab15511674-4k/offsets.h"
#include "6.6.118-android15-8-ge58033dc8ea6-abogki498046332-4k/offsets.h"
#include "6.6.118-android15-8-gebdfad32d749-ab15099304-4k/offsets.h"
#include "6.12.23-android16-5-g16e473de48a3-abogki462654244-4k/offsets.h"
#include "6.12.23-android16-5-g75e9b1c7ae7c-abogki463945075-4k/offsets.h"
#include "6.12.23-android16-5-g82efd98459a2-ab14457512-4k/offsets.h"
#include "6.12.23-android16-5-gb2a876903b49-ab14541642-4k/offsets.h"
#include "6.12.38-android16-5-g844001fb8721-ab14552068-4k/offsets.h"
  { .uname_r = NULL }
};

#endif
