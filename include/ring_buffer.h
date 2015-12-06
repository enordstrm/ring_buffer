#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_
/**
 * Ring Buffer
 * ===========
 *
 * Description:
 *  Ring Buffer implementated through mapping a virtual memory into a
 *  contigious memory region.
 *
 *  Benefits of this method alllows the caller to unambigously treat the
 *  ring buffer as an  actual circular buffer without having to take into
 *  account any borders of the memory region in use.
 *
 * For usage example see included unit tests.
 *
 * Author: Eric Nordström <eric@nrdstrm.org>
 * 
 * Copyright 2015.
 *
 */
#include "stdint.h"
#include "stddef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declaration */
struct ring_buffer;

/* Types */
typedef struct ring_buffer ring_buffer_t;
typedef uint8_t byte_t;

/**
 * Create a new Ring Buffer.
 *
 * @param order to the power of two equals size 
 *
 * @return ring_buffer_t* The new Ring Buffer
 */
ring_buffer_t *ring_buffer_new(const size_t order);

/**
 * Free existing Ring Buffer.
 *
 * @param buffer Ring Buffer to operate on
 */
void ring_buffer_free(ring_buffer_t* buffer);

/**
 * Commit a read from the buffer, moves read pointer forward
 *
 * @param buffer Ring Buffer to operate on
 */
void ring_buffer_commit_read(ring_buffer_t* buffer, size_t size);

/**
 * Commit a write to the buffer, moves write pointer forward
 *
 * @param buffer Ring Buffer to operate on
 */
void ring_buffer_commit_write(ring_buffer_t* buffer, size_t size);

/**
 * Write data to the Ring Buffer, moving the write pointer forward.
 *
 * Convenience function for copying(!) data into the write pointer
 * and commiting the write.
 *
 * @param buffer Ring Buffer to operate on
 * @param data Data to write to the buffer
 * @param size Size of the data to write
 *
 *  @return size_t Amount of data written
 */
size_t ring_buffer_write(ring_buffer_t* buffer, const byte_t* data, const size_t size);

/**
 * Get pointer to data to read. Moves read pointer forward.
 *
 * Convenience for getting the read pointer and moving the read
 * pointer ahead. Verifies that the amount of data to read is
 * available.
 *
 * @param buffer Ring Buffer to operate on
 * @param size Amount to read (tip: use ring_buffer_get_used())
 * 
 * @return byte_t pointer to data, null on error 
 */
const byte_t* ring_buffer_read(ring_buffer_t* buffer, const size_t size);

/**
 * Get const pointer to the start of data to read.
 *
 * @param buffer Ring Buffer to operate on
 */
const byte_t* ring_buffer_get_read_pointer(const ring_buffer_t* buffer);

/**
 * Get pointer to the start of the write area of the buffer.
 *
 * @param buffer Ring Buffer to operate on
 */
byte_t* ring_buffer_get_write_pointer(ring_buffer_t* buffer);

/**
 * Get the amount of free space in the Ring Buffer.
 *
 * @param buffer Ring Buffer to operate on
 *
 * @return size_t The available space in the buffer.
 */
size_t ring_buffer_get_free(const ring_buffer_t* buffer);

/**
 * Get the size of the actual/internal Ring Buffer.
 *
 * @param buffer Ring Buffer to operate on.
 *
 * @return size_t The total size of the internal buffer.
 *
 *  Note: total size of the entire ring buffer = 
 *      ring_buffer_get_size(..) + sizeof(ring_buffer_t)
 */
size_t ring_buffer_get_size(const ring_buffer_t* buffer);

/**
 * Get amount of used space in the Ring Buffer.
 *
 * @param buffer Ring Buffer to operate on
 *
 * @return size_t Amount of free space in the ring buffer
 */
size_t ring_buffer_get_used(const ring_buffer_t* buffer);

/**
 * Clear the Ring Buffer.
 *
 * @param buffer Ring Buffer to operate on
 */
void ring_buffer_clear(ring_buffer_t* buffer);

#ifdef __cplusplus
}
#endif

#endif /* RING_BUFFER_H_ */
