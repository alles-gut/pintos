#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);
void bad_vaddr(const void *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f) 
{
  //printf ("system call!: %d\n", *(uint32_t *)(f->esp));
  //hex_dump (f->esp, f->esp+20, 100, 1);
  switch (*(uint32_t *)(f->esp)){
    case SYS_HALT:
      halt ();
      break;
    case SYS_EXIT:
      bad_vaddr(f->esp + 4);
      exit(*(uint32_t *)(f->esp + 4));
      break;
    case SYS_EXEC:
      bad_vaddr(f->esp + 4);
      f->eax = exec((const char *)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_WAIT:
      bad_vaddr(f->esp + 4);
      f->eax = wait((pid_t)*(uint32_t *)(f->esp + 4));
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
      bad_vaddr(f->esp + 4);
      bad_vaddr(f->esp + 8);
      bad_vaddr(f->esp + 12);
      f->eax = read((int)*(uint32_t *)(f->esp+4), (void *)*(uint32_t *)(f->esp + 8), (unsigned)*((uint32_t *)(f->esp +12)));
      break;
    case SYS_WRITE:
      bad_vaddr(f->esp + 4);
      bad_vaddr(f->esp + 8);
      bad_vaddr(f->esp + 12);
      f->eax = write((int)*(uint32_t *)(f->esp+4), (void *)*(uint32_t *)(f->esp + 8), (unsigned)*((uint32_t *)(f->esp +12)));
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
exit (int status) {
  printf("%s: exit(%d)\n", thread_name (), status);
  thread_current ()->exit_status = status;
  thread_exit ();
}

int
write (int fd, const void *buffer, unsigned size) {
  if (fd == 1){
    putbuf(buffer, size);
    return size;
  }
  return -1;
}

void
halt (void) {
  shutdown_power_off ();
}

pid_t
exec (const char *file) {
  return process_execute(file);
}

int
wait (pid_t pid){
  return process_wait(pid);
}

int
read (int fd, void *buffer, unsigned size){
  int i;
  if (fd == 0){
    for(i = 0; i<size; i++){
      if(((char *)buffer)[i] == '\0')
        break;
    }
  }
  return i;
}

void
bad_vaddr(const void *vaddr){
  if (!is_user_vaddr(vaddr)){
    exit(-1);
  }
}
