# proc-syscalls
A Linux kernel module for listing the exact state of the syscall table in a running system.

Lists syscall number, functin address, and function name. Depending on the kptr_restrict sysctl, the address may be hidden from unprivileged users (just like /proc/kallsyms).

Example:

```
$ make
$ sudo insmod syscalls.ko
$ cat /proc/syscalls
  0 0000000000000000 __x64_sys_read
  ...
$ sudo cat /proc/syscalls
  ...
  0 fffffffface65390 __x64_sys_read
```
