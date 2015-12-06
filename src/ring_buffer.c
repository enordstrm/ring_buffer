#include "ring_buffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/mman.h>
#include <unistd.h>

#ifdef DEBUG
#   define fail() assert(0)
#else
#   define fail() return 0;
#endif

/*----------------------------------------------------------------------------*/

struct ring_buffer
{
    size_t size;
    size_t write_offset;
    size_t read_offset;
    byte_t* data;
};

/*----------------------------------------------------------------------------*/

size_t _create_buffer(ring_buffer_t* buffer, size_t order)
{
    char path[] = "/dev/shm/ring-buffer-XXXXXX";
    int file_descriptor;
    void *address;
    int status;

    file_descriptor = mkstemp (path);
    if (file_descriptor < 0)
        fail();

    status = unlink(path);
    if (status)
        fail();

    buffer->size = 1UL << order;
    buffer->write_offset = 0;
    buffer->read_offset = 0;

    status = ftruncate (file_descriptor, buffer->size);
    if (status)
        fail();

    buffer->data = mmap(NULL, buffer->size << 1, PROT_NONE,
                        MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (buffer->data == MAP_FAILED)
        fail();

    address = mmap(buffer->data, buffer->size, PROT_READ | PROT_WRITE,
                   MAP_FIXED | MAP_SHARED, file_descriptor, 0);

    if (address != buffer->data)
        fail();

    address = mmap(buffer->data + buffer->size,
                   buffer->size, PROT_READ | PROT_WRITE,
                   MAP_FIXED | MAP_SHARED, file_descriptor, 0);

    if (address != buffer->data + buffer->size)
        fail();

    status = close(file_descriptor);

    if (status)
        fail();

    return buffer->size;
}

/*----------------------------------------------------------------------------*/

ring_buffer_t* ring_buffer_new(const size_t order)
{
    ring_buffer_t* buffer = (ring_buffer_t*)malloc(sizeof(ring_buffer_t));
    buffer->size = 1UL << order;
    buffer->read_offset = 0;
    buffer->write_offset = 0;

    if (_create_buffer(buffer, order) == 0) {
        ring_buffer_free(buffer);
        return 0;
    }

    return buffer;
}

/*----------------------------------------------------------------------------*/

void ring_buffer_free(ring_buffer_t* buffer)
{
    munmap(buffer->data, buffer->size << 1);
    buffer->data = 0;
    buffer->size = 0;
    free(buffer);
}

/*----------------------------------------------------------------------------*/

byte_t* ring_buffer_get_write_pointer(ring_buffer_t* buffer)
{
    return buffer->data + buffer->write_offset;
}

/*----------------------------------------------------------------------------*/

void ring_buffer_commit_write(ring_buffer_t* buffer, size_t size)
{
    buffer->write_offset += size;
}

/*----------------------------------------------------------------------------*/

size_t ring_buffer_write(ring_buffer_t* buffer, const byte_t* data, const size_t size)
{
    if (ring_buffer_get_free(buffer) < size)
        return 0;

    memcpy(ring_buffer_get_write_pointer(buffer), data, size);
    ring_buffer_commit_write(buffer, size);

    return size;
}

/*----------------------------------------------------------------------------*/

void ring_buffer_commit_read(ring_buffer_t* buffer, size_t size)
{
    // advance read offset
    buffer->read_offset += size;

    // adjust offsets if wrapped around
    if (buffer->read_offset >= buffer->size) {
        buffer->read_offset -= buffer->size;
        buffer->write_offset -= buffer->size;
    }
}

/*----------------------------------------------------------------------------*/

const byte_t* ring_buffer_get_read_pointer(const ring_buffer_t* buffer)
{
    return buffer->data + buffer->read_offset;
}

/*----------------------------------------------------------------------------*/

const byte_t* ring_buffer_read(ring_buffer_t* buffer, const size_t size)
{
    if (ring_buffer_get_used(buffer) == 0)
        return 0;

    // save current
    const byte_t* current_read_position = ring_buffer_get_read_pointer(buffer);

    ring_buffer_commit_read(buffer, size);

    return current_read_position;
}

/*----------------------------------------------------------------------------*/

size_t ring_buffer_get_size(const ring_buffer_t* buffer)
{
    return buffer->size;
}

/*----------------------------------------------------------------------------*/

size_t ring_buffer_get_used(const ring_buffer_t* buffer)
{
    return buffer->write_offset - buffer->read_offset;
}

/*----------------------------------------------------------------------------*/

size_t ring_buffer_get_free(const ring_buffer_t* buffer)
{
    return buffer->size - ring_buffer_get_used(buffer);
}

/*----------------------------------------------------------------------------*/

void ring_buffer_clear(ring_buffer_t* buffer)
{
    buffer->write_offset = 0;
    buffer->read_offset = 0;
}

/*----------------------------------------------------------------------------*/

