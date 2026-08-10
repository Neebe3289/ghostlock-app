# AGENTS.md

本文件记录仓库当前状态、两条利用路线及其支持情况，以及如何为新的内核/设备添加支持。

## 仓库状态

- 项目：ghostlock-app —— 面向 Android 内核的 ghostlock（CVE-2026-43499）提权利用实现。
- 触发链：futex `FUTEX_WAIT_REQUEUE_PI` 使内核在栈上放置 `rt_mutex_waiter`；运行时伪造 waiter 布局并配合内核页（fake FOPS 等）完成提权。
- 当前工作树有大量未提交改动（含 mcast 路线），既有改动一律保留，默认不提交。
- 构建：
  - 原生库：`make ghostlock`（NDK clang，`src/core/*.c`）。
  - APK：`.\gradlew.bat :app:assembleDebug`（`buildGhostlockNative` → `prepareGhostlockJniLibs` 拷到 `app/src/main/jniLibs/arm64-v8a/libghostlock.so`）。
  - 校验：`llvm-nm -C ghostlock` 查符号；APK 内 `lib/arm64-v8a/libghostlock.so` 应与剥离后二进制一致。
- 实时日志：`init_file_log()` 在 `init_runtime_paths()` 后按可用性依次尝试 `GHOSTLOCK_LOG`（显式）→ `/sdcard/Download/ghostlock.log`（root/shell 直跑可用）→ `GHOSTLOCK_LOG_DIR/ghostlock.log`（App 传外部私有目录，作用域存储下可写）→ `$GHOSTLOCK_HOME/ghostlock.log`（兜底），用 `open(O_WRONLY|O_CREAT|O_APPEND)` 打开日志 fd。所有 `pr_*` 宏经 `log_line()` 用 `write()` 同步输出：stdout 写彩色行（与旧 printf 字节一致），日志 fd 写剥色行。落盘：`log_line()` 只做 `write()` 追加，**不再逐行 `fdatasync()`**（避免 punch/route 热路径每行增加 ~1-5ms 同步 I/O 干扰时序）；由 `log_flush_file()` 在 `spawn_child()`/`clone_child()` fork 前和每次 write attempt 前显式刷盘，保证卡死/panic 重启后脏页与文件长度不丢失（`ghostlock.log` 不会为 0 字节）。全程无后台线程、无 stdio 缓冲，保证 spawn/exploit 每次 fork 都是单线程——上一版 tee 线程在 `fwrite/fflush` 持 stdio/malloc 锁时被 fork，锁会继承进 W3 子进程，导致其 `fopen/fread("/proc/self/comm")` 偶发挂死（表现为“不是次次失败”）。App 跑完后还会通过 MediaStore 把 `ghostlock.log` 发布到 `/sdcard/Download/`，无需 root 即可用文件管理器/ADB 拉取。

## 两条路线

### 1. pselect 路线（默认）

- `do_pselect_fake_lock_route()`（`src/core/fops.c`）：构造 fd_set，使 `pselect()` 的内核栈拷贝覆盖 futex waiter，fake waiter 放入用户可控的 fd_set 字区。
- 参数 `pselect_waiter_shift`：waiter 相对 fd_set 起点的 qword 偏移；0 用 `target.h` 默认值。
- 适用：大部分 6.6 / 6.12 内核（shift = -2 或 0）。

### 2. mcast 路线（setsockopt 栈拷贝）

- `do_mcast_fake_lock_route()`（`src/core/fops.c`）：复用全局 `mcast_sock`（waiter 线程在 UAF 建立前打开、跨轮复用，路线内不再创建也不 close），调用 `setsockopt(AF_INET6, IPPROTO_IPV6, 46, optval, 264)`；`do_ipv6_setsockopt` 会把 264 字节整段拷到内核栈，若该拷贝区覆盖 futex waiter，就把 fake waiter 放进 `optval[mcast_payload_off]`。
- socket 获取：`mcast_open_socket()` 由 `waiter_thread` 在 `FUTEX_WAIT_REQUEUE_PI`（UAF 建立）**之前**调用，按口味列表依次尝试（DGRAM/UDP/CLOEXEC/STREAM/RAW 等）并记录每个 errno；App 必须声明 `android.permission.INTERNET`，否则 HyperOS 会在 `security_socket_create`（SELinux）处对 `socket(AF_INET6, SOCK_DGRAM, 0)` 返回 `EPERM`（真机实测 errno=1）。socket 失败时跳过 UAF、直接结束本轮，避免 UAF 后首个深 syscall 覆盖悬挂 waiter 栈区。
- 参数 `mcast_payload_off`：waiter 在 264 字节拷贝区内的偏移；0 = 走 pselect 路线。
- 两个已支持内核的 optname 46 均落在 switch case 45（264 字节拷贝块）。
- 拷贝先于校验：readmik70 反汇编显示 case 45 先 `memset(sp+0x40, 0, 0x108)` 再 `copy_from_user(sp+0x40, optval, 0x108)`，之后才校验组播参数，所以 `setsockopt` 返回 `-1/errno=99`（EADDRNOTAVAIL）时 264 字节仍然落栈，属预期行为。
- 窗口维持（`MCAST_REFRESH_WINDOW`，默认 1）：首次 `setsockopt` 落栈后，waiter 循环重发同一 `setsockopt` 刷新 264B 拷贝，直到 consumer 完成 punch 清零 `punch_consume_go` 或 3s 兜底超时（对齐 8550-43499 参考实现的刷新循环；每次返回 `-1/errno=99` 属预期）。`MCAST_REFRESH_WINDOW=0` 保留旧模型（单次拷贝 + 纯用户态忙等）供回归对比。
- 全链单次 route（完整 mcast 逻辑，`run_exploit` 的 `use_mcast` 分支）：**整个 exploit 只做一次 route**。waiter 的 `FUTEX_WAIT_REQUEUE_PI`（1s 超时）先超时出树——vendor `futex_wait_requeue_pi` 直接调 `rt_mutex_slowlock_block` + `remove_waiter` 把栈 waiter 从真实 `pi_state->pi_mutex.waiters` 树移除（镜像反汇编 `0x29a6bc/0x29a7cc`，`remove_waiter` 在 `0x8b0` 清 `pi_blocked_on`，但先前的悬挂窗口仍构成 CVE 触发链）。随后 setsockopt 覆盖栈内容 + consumer 单次 `sched_setattr` punch：`rt_mutex_adjust_prio_chain(orig_waiter=NULL)` 在 `rt_mutex_dequeue()` 时因 `waiter->lock=fake_lock` 把 `rb_erase` 重定向到空 fake_lock 树，`tree_left!=0 && tree_right==0` 走 `else if (!child)` 分支完成写原语（写1 `*(ASHMEM_MISC_FOPS)=fake_fops`；写2 破坏 fake_fops 的 llseek 槽 +8，由 `repair_fake_fops_llseek()` 修复，write_iter/read_iter 槽位不受影响）。**真实树自始至终健康，这是与旧实现（覆盖时节点还在真实树上 → 树损坏 → 卡死）的本质区别。**
- 永久阻塞路由线程：route 落地后 waiter 永久 yield（栈不释放）、owner 永久持锁（不 `FUTEX_UNLOCK_PI(f_pi_target)`）、consumer 永久挂起（不再 punch）；`run_main_route_threads()` 在 mcast 下**不 join** 这三个线程，主线程直接返回。任何 join/线程退出/解锁都会让内核再次触碰悬挂引用而卡死。进程退出/被杀时树清理的不确定性可接受（exploit 已完成）。
- configfs 全链写：route 后主线程 `mcast_install_fake_fops()` 打开 `/dev/ashmem`（此时 misc_fops 已被替换为 fake_fops，fd->f_op 绑到 fake_fops），写验证 + `repair_fake_fops_llseek()` + 读回 misc_fops==fake_fops + `leak_kernel_base()` 解 KASLR 后，W1（`SELINUX_ENFORCING=0`）、W2（`child_task+TASK_CRED_OFF ← data_addr(init_cred)`）、W3（`thread_info.flags` 读-改-写清 `TIF_SECCOMP` 位 + `seccomp.mode=0`）全部走 `kernel_write_data/kernel_read_data`（configfs 原语，不碰 futex 树），最后 `mcast_restore_misc_fops()` 恢复真实 fops。**pselect 路线的 `do_one_write`/`retry_write_stage` 多轮 route 模式在 mcast 下禁用**：写原语只能装 fake_fops（位置固定），W1/W2/W3 的目标写必须由 configfs 完成。
- 卡死防护：UAF 后路线内不再做 close/usleep 等深 syscall，punch 后等待 owner 收尾改用 yield spin，避免调度器重走悬挂 PI 链；`FUTEX_UNLOCK_PI(f_pi_chain)` 为握手必需，其 pi_state 与 UAF 链无关。

### punch 触发（`src/core/main.c` 的 `consumer_thread`）

- 触发链要求内核在 `FUTEX_WAIT_REQUEUE_PI` 超时后仍保留指向栈上 waiter 的引用（vendor bug 的 pi_state / pi_blocked_on 悬挂），punch 让内核再次遍历该链并对伪造 waiter 的 `pi_tree` 节点做 `rb_erase`，从而写出值。
- 三种 punch 模式（`punch_mode`，默认 0）：
  - 0 = `sched`：对 waiter tid 做 `sched_setattr(SCHED_BATCH, nice)`；失败时回退 `FUTEX_LOCK_PI`。
  - 1 = `futex`：只对 `f_pi_target` 做 `FUTEX_LOCK_PI`（50ms 超时）——直接在 pi_state 链上入队并遍历，是最直接的触发；`ETIMEDOUT/EAGAIN` 视为已入队走过链（owner 不会释放锁）。
  - 2 = `both`：先 sched 再 futex（早期 mcast 默认；同一轮两次遍历已被 rb_erase 摘除的链，是“mcast route done 后卡死”的挂起源，现不再作为默认）。
- 每轮 punch 记录 `sched=%d/%d futex=%d/%d locked=%d entered=%d`；`mcast window done` / `mcast route done` 行均带这些计数。

### 运行时选择（`src/core/main.c` 的 `waiter_thread`）

- `active_offsets->mcast_payload_off != 0` → `do_mcast_fake_lock_route()`
- 否则 → `do_pselect_fake_lock_route()`

## 支持情况

| 内核（uname -r） | 设备 | pselect | mcast |
| --- | --- | --- | --- |
| `5.15.194-android13-8-00019-gf4321180a397-ab15212794` | Redmi K70 | 不可行（waiter 与 fd_set 负重叠 -232） | 可行，`mcast_payload_off=0xa8` |
| `6.6.77-android15-8-gf9a1d4bd8353-abogki440974771-4k` | Xiaomi 15 | 不可行（waiter 在 fd_set 上方 12 qword，shift 最多 3） | 可行，`mcast_payload_off=0x90` |
| 其余表内 6.6.77 / 6.6.89 / 6.6.118 / 6.12.23 内核 | Redmi K90 / Civi 5 Pro / Xiaomi 15 与 15 Pro / OPPO Pad 4 Pro / Find N5 / K90 Ultra / Pad 5 / Find X8 系 / K90 Pro Max / Xiaomi 17 系 / OnePlus 15 | 可行（shift = -2 或 0） | 未启用 |

- 真机实测进度（Xiaomi 15，`6.6.77-gf9a1d4bd8353`）：
  - 第 1 轮：heap spray 偶发成功，但 `socket(AF_INET6, SOCK_DGRAM, 0)` 返回 `errno=1` → 补 `android.permission.INTERNET` + socket 口味回退。
  - 第 2 轮：socket 成功，mcast 路线报告 `calls=1 success=1` 但 W1 8 次全失败；曾将默认 punch 改为 `both` 并加每轮计数日志，真机随后出现“mcast socket ok / mcast route done 后卡死”高概率挂起。
  - 第 3 轮（8550-43499 参考实现逆向）：socket 提前到 UAF 前创建并跨轮复用；窗口改为循环 `setsockopt` 刷新；punch 恢复单次 `sched`（参考实现证明刷新窗口下单次 punch 足够，mode 2 二次遍历为挂起源）；路线收尾改 yield spin。待真机复测 5.15（readmik70）与 6.6/6.12 mcast 设备。
  - 第 4 轮（完整 mcast 逻辑）：整链改为**单次 route + configfs 全链写**——route 只落地写原语（misc_fops ← fake_fops），落地后 waiter/owner/consumer 三线程永久阻塞（`run_main_route_threads` 不 join），W1（`SELINUX_ENFORCING=0`）/W2（`child_task+cred ← init_cred`）/W3（清 `TIF_SECCOMP` + `seccomp.mode`）全部由主线程经 `kernel_write_data/kernel_read_data`（configfs 原语）完成，不再触碰 futex 树；失败路径统一 `mcast_restore_misc_fops()` 恢复。构建与 APK 打包通过，待真机验证 readmik70 是否消除“route done 后卡死”。
- 调试旋钮（env 或 `$GHOSTLOCK_HOME/ghostlock.conf` 的 `KEY=VALUE`，env 优先；App 启动只传 `GHOSTLOCK_HOME/TMPDIR/HOME`，App 内跑需用 conf 文件）：
  - `PUNCH` = `sched` / `futex` / `both`（默认 `sched`）
  - `VARY_NICE` = 1（sched 的 nice 按 1..19 轮换，替代固定 19）
  - `CALLS` = 每轮 punch 次数（默认 1）、`BURST` = 每次连打（默认 1）、`DELAY_USEC` = punch 相对拷贝的延时（默认跟随 `route_delay_usec`）
- 结构体布局按内核宏区分：`src/kernels/offsets.h` 的 `STRUCT_OFFSETS_5_15 / STRUCT_OFFSETS_6_6 / STRUCT_OFFSETS_6_12`，配合 `src/core/runtime_struct_offsets.h` 的 `FAKE_WAITER_* / FOPS_* / CRED_* / STRUCT_PAGE_*` 宏（表内为 0 时回退 `target.h` 默认值）。
- 参考链几何（提取器 `--check-route` 实测）：
  - `5.15.194`：`__arm64_sys_futex(0xa0) → do_futex(0x140) → futex_wait_requeue_pi(0x1b0)`，waiter 栈 `sp+0x98`（abs -0x2f8）；mcast 链 `__arm64_sys_setsockopt(0x10) → __sys_setsockopt(0x80) → sock_common_setsockopt(0x40) → udpv6_setsockopt(0x50) → do_ipv6_setsockopt(0x2c0)`，copy=0x40。
  - `6.6.77-gf9a1d4bd8353`：`__arm64_sys_futex(0x70) → do_futex(0x60) → futex_wait_requeue_pi(0x1c0)`，waiter 栈 `sp+0x90`（abs -0x200）；mcast 链 `__arm64_sys_setsockopt(0x10) → __sys_setsockopt(0x50) → do_sock_setsockopt(0x50) → sock_common_setsockopt(0x10) → udpv6_setsockopt(0x10) → ipv6_setsockopt(0x40) → do_ipv6_setsockopt(0x2c0)`，copy=0x140。

## 如何支持新内核

1. 放镜像：`devices/<厂商>/<版本>/` 下放 `boot.img` 与 `xbl_config.img`（XBL 用于解析内核物理加载地址）。
2. 推导路线：

   ```powershell
   python tools/extract_target.py devices/<厂商>/<版本>/boot.img `
     --xbl-config devices/<厂商>/<版本>/xbl_config.img --check-route
   ```

   输出 futex 链、waiter 栈偏移、pselect 可行性、mcast 可行性（payload/copy/case）。
   - pselect 可行 → 记录 `pselect_waiter_shift`
   - mcast 可行 → 记录 `mcast_payload_off`
3. 注册偏移表：`python tools/extract_target.py <boot> --xbl-config <xbl> --register`。表已存在会跳过，需先删 `src/kernels/<release>/` 再注册；缺符号可用 `--allow-missing` 排查。
4. 核对 per-kernel 结构体布局（waiter / cred / fops / struct page），必要时补 `STRUCT_OFFSETS_*` 宏。
5. 构建验证：`make ghostlock`；`.\gradlew.bat :app:assembleDebug`；解包 APK 确认 `lib/arm64-v8a/libghostlock.so` 含新逻辑。
6. 更新 `README.md` / `README_ZH.md` 支持表与路线说明；真机实测后更新“已验证”结论。

## 注意事项

- Windows 下文件可能 CRLF/LF 混杂，`apply_patch` 失败时先统一转 LF 再打补丁。
- 只改与任务相关的文件，保留工作树既有未提交改动，不要提交。
