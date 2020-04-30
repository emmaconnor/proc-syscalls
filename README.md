# proc-syscalls
A Linux kernel module for listing the exact state of the syscall table in a running system.

The module creates a file at `/proc/syscalls` that lists syscall number, function address, and function name. Depending on the kptr_restrict sysctl, the address may be hidden from unprivileged users (just like /proc/kallsyms).

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

## Why?

When debugging and analyzing kernel code, I found it frustrating trying to sift through several header files and kernel sources to find all of the syscall number-to-function mappings, or using online sources that are not always up-to-date or accurate for every configuration. This kernel module dumps the syscall table to userspace to give a very simple, complete, and accurate picture of the syscall mappings in a live system.
