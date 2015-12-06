Ring Buffer
===========

Ring Buffer (circular buffer) implemented in C using the POSIX API for virtual memory mapping see `MMAP(2)`. This creates a buffer in virtual memory which appears to, and can be used as, an actual ring buffer by mapping a virtual copy of the buffer at the end of it which in essence creates a memory "loop".

Usage
-----

Example using the Ring Buffer for storing data read from a socket:

```
ring_buffer_t* buffer = ring_buffer_create(12);
int fd = socket(...);

size_t amount = read(fd, ring_buffer_get_write_pointer(buffer),
                     ring_buffer_get_free(buffer));
ring_buffer_commit_write(buffer, amount);

size_t written = write(fd, ring_buffer_get_read_pointer(buffer),
                       ring_buffer_get_used(buffer));
ring_buffer_commit_read(buffer, written);
```

See `ring_buffer.h` and included unit tests for more information.

