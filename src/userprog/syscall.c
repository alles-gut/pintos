#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f) 
{
  printf ("system call!: %d\n", *(uint32_t *)(f->esp));
  hex_dump (f->esp, f->esp, 100, 1);
  switch (*(uint32_t *)(f->esp)){
    case SYS_HALT:
      break;
    case SYS_EXIT:
      exit(*(uint32_t *)(f->esp + 4));
      break;
    case SYS_EXEC:
      break;
    case SYS_WAIT:
      break;
    case SYS_CREATE:
      break;
    case SYS_REMOVE:
      break;
    case SYS_OPEN:
      break;
    case SYS_FILESIZE:
      break;
    case SYS_READ:
      break;
    case SYS_WRITE:
      f->eax = write((int)*(uint32_t *)(f->esp+4), (void *)(uint32_t *)(f->esp + 8), (unsigned)*((uint32_t *)(f->esp +12)));
      break;
    case SYS_SEEK:
      break;
    case SYS_TELL:
      break;
    case SYS_CLOSE:
      break;
  }
}

void
exit (int stat) {
  printf("%s: exit(%d)\n", thread_name (), stat);
  thread_current ()->exit_status = stat;
  thread_exit ();
}

int
write (int fd, const void *buf, unsigned size) {
  if (fd == 1){
    putbuf(buf, size);
    return size;
  }
  return -1;
}

void
halt (void) {
  shutdown_power_off ();
}

pid_t
exec (const char *cmd_line) {
  return process_execute(cmd_line);
}

int
wait (pid_t pid){
  return process_wait(pid);
}

int read (int fd, void *buf, unsigned size){
  int i;
  if (fd == 0){
    for(i = 0; i<size; i++){
      if(((char *)buf)[i] == '\0')
        break;
    }
  }
  return i;
}
