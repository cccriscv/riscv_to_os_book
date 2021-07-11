#include "kernel/syscall.h"

.macro entry name
  .global \name
  \name:
  li a7, SYS_\name
  ecall
  ret
.endm

# entry fork
.global fork
fork:
li a7,SYS_fork
ecall
ret

entry exit
entry wait
entry pipe
entry read
entry write
entry close
entry kill
entry exec
entry open
entry mknod
entry unlink
entry fstat
entry link
entry mkdir
entry chdir
entry dup
entry getpid
entry sbrk
entry sleep
entry uptime
