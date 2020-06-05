#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "filesys/off_t.h"

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
      bad_vaddr(f->esp + 4);
      bad_vaddr(f->esp + 8);
      f->eax = create((const char *)*(uint32_t *)(f->esp +4), (unsigned)*(uint32_t *)(f->esp +8));
      break;
    case SYS_REMOVE:
      bad_vaddr(f->esp +4);
      f->eax = remove((const char*)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_OPEN:
      bad_vaddr(f->esp +4);
      f->eax = open((const char*)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_FILESIZE:
      bad_vaddr(f->esp +4);
      f->eax = filesize((int)*(uint32_t *)(f->esp + 4));
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
      bad_vaddr(f->esp +16);
      bad_vaddr(f->esp +20);
      seek((int)*(uint32_t *)(f->esp + 16), (unsigned)*(uint32_t *)(f->esp + 20));
      break;
    case SYS_TELL:
      bad_vaddr(f->esp +4);
      f->eax = tell((int)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_CLOSE:
      bad_vaddr(f->esp +4);
      close((int)*(uint32_t *)(f->esp + 4));
      break;
  }
}

// copy from filesys/file.c
struct file{
  struct inode *inode;
  off_t pos;
  bool deny_write
};

void
bad_vaddr(const void *vaddr){
  if (!is_user_vaddr(vaddr)){
    exit(-1);
  }
}

void
halt (void) {
  shutdown_power_off ();
}

void
exit (int status) {
  printf("%s: exit(%d)\n", thread_name (), status);
  thread_current ()->exit_status = status;
  int i;
  for(i=0; i<128; i++){
    if(thread_current()->fd[i] != NULL) close(i);
  }
  thread_exit ();
}

pid_t
exec (const char *file) {
  return process_execute(file);
}

int
wait (pid_t pid){
  return process_wait(pid);
}


bool create (const char *file, unsigned initial_size) {
  if(file == NULL) {exit(-1);}
  bad_vaddr(file);
  return filesys_create(file, initial_size);
}

bool remove (const char *file) {
  if(file == NULL) exit(-1);
  bad_vaddr(file);
  return filesys_remove(file);
}

int open (const char *file) {
  int i;
  struct file* fp;
  if(file == NULL) exit(-1);
  bad_vaddr(file);

  fp = filesys_open(file);
  if (fp == NULL) {
    return -1;
  } else{
    for(i=3; i<128; i++) {
      if(thread_current()->fd[i] == NULL) {
        thread_current()->fd[i] = fp;
        return i;
      }
    }
  }
  return -1;
}

int filesize (int fd) {
  if(thread_current()->fd[fd] == NULL) exit(-1);
  return file_length(thread_current()->fd[fd]);
}

int read (int fd, void* buffer, unsigned size) {
  int i;
  bad_vaddr(buffer);
  if (fd == 0) {
    for (i=0; i<size; i++){
      if (((char *)buffer)[i] == '\0') {
        break;
      }
    }
  } else if(fd > 2){
    if(thread_current()->fd[fd] == NULL) exit(-1);
    return file_read(thread_current()->fd[fd], buffer, size);
  }
  return i;
}

int write (int fd, const void *buffer, unsigned size){
  bad_vaddr(buffer);
  if (fd == 1){
    putbuf(buffer, size);
    return size;
  } else if(fd > 2){
    if(thread_current()->fd[fd] == NULL) exit(-1);
    return file_write(thread_current()->fd[fd], buffer, size);
  }
  return -1;
}

void seek (int fd, unsigned position){
  if(thread_current()->fd[fd] == NULL) exit(-1);
  file_seek(thread_current()->fd[fd], position);
}

unsigned tell(int fd){
  return file_tell(thread_current()->fd[fd]);
}

void close(int fd){
  if(thread_current()->fd[fd] == NULL) exit(-1);
  struct file* fp = thread_current()->fd[fd];
  thread_current()->fd[fd] = NULL;
  return file_close(fp);
}
