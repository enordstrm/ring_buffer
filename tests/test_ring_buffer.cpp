#include "catch.hpp"
#include "ring_buffer.h"
#include <string.h>

TEST_CASE("ring buffer init and write", "[ring_buffer]")
{
    const byte_t test_data[] = "test data";

    // init
    ring_buffer_t* cut = ring_buffer_new(16);
    REQUIRE(cut);
    REQUIRE(1UL << 16 == ring_buffer_get_size(cut));
    REQUIRE(ring_buffer_get_used(cut) == 0);

    // write
    REQUIRE(ring_buffer_write(cut, test_data, sizeof(test_data)) == sizeof(test_data));
    REQUIRE(ring_buffer_get_used(cut) == sizeof(test_data));

    // peek twice, should still be there
    REQUIRE(memcmp(ring_buffer_get_read_pointer(cut), test_data, sizeof(test_data)) == 0);
    REQUIRE(memcmp(ring_buffer_get_read_pointer(cut), test_data, sizeof(test_data)) == 0);
    REQUIRE(ring_buffer_get_used(cut) == sizeof(test_data));

    // read
    REQUIRE(memcmp(ring_buffer_read(cut, sizeof(test_data)), test_data, sizeof(test_data)) == 0);
    REQUIRE(ring_buffer_get_used(cut) == 0);

    // read again with empty buffer returns null ptr
    REQUIRE(ring_buffer_read(cut, sizeof(test_data)) == 0); 
    REQUIRE(ring_buffer_get_used(cut) == 0);

    // free
    ring_buffer_free(cut);
}

TEST_CASE("ring buffer multiple write/read", "[ring buffer]")
{
    const int loops = 200;
    const byte_t test_data[] = "test data is right here";

    // init
    ring_buffer_t* cut = ring_buffer_new(16);
    REQUIRE(cut);
    REQUIRE(1UL << 16 == ring_buffer_get_size(cut));
    REQUIRE(ring_buffer_get_used(cut) == 0);

    for (int i = 0; i < 200; i++)
    {
        // write
        REQUIRE(ring_buffer_write(cut, test_data, sizeof(test_data)) == sizeof(test_data));
        REQUIRE(ring_buffer_get_used(cut) == sizeof(test_data));

        // read everything
        REQUIRE(ring_buffer_get_used(cut) == sizeof(test_data));
        const byte_t* read = ring_buffer_read(cut, ring_buffer_get_used(cut));
        REQUIRE(memcmp(read, test_data, sizeof(test_data)) == 0);
        REQUIRE(ring_buffer_get_used(cut) == 0);
    }

    // write some
    REQUIRE(ring_buffer_write(cut, test_data, sizeof(test_data)) == sizeof(test_data));
    REQUIRE(ring_buffer_get_used(cut) == sizeof(test_data));

    // peek
    REQUIRE(ring_buffer_get_used(cut) == sizeof(test_data));
    const byte_t* read = ring_buffer_get_read_pointer(cut);
    REQUIRE(memcmp(read, test_data, ring_buffer_get_used(cut)) == 0);
    REQUIRE(ring_buffer_get_used(cut) == sizeof(test_data));

    // clear
    ring_buffer_clear(cut);
    REQUIRE(ring_buffer_get_used(cut) == 0);

    // free
    ring_buffer_free(cut);
}

TEST_CASE("multiple writes followed by one read", "[ring buffer]")
{
    // init
    ring_buffer_t* cut = ring_buffer_new(12);
    REQUIRE(cut);
    REQUIRE(1UL << 12 == ring_buffer_get_size(cut));
    REQUIRE(ring_buffer_get_used(cut) == 0);

    // set up some test data
    const size_t test_data_size = ring_buffer_get_free(cut);
    const byte_t* test_data = (const byte_t*)malloc(test_data_size);

    // write
    REQUIRE(ring_buffer_write(cut, test_data, test_data_size) == test_data_size);
    REQUIRE(ring_buffer_get_used(cut) == test_data_size);
    REQUIRE(ring_buffer_get_free(cut) == 0);

    // try to write again with full buffer which should fail
    REQUIRE(ring_buffer_write(cut, test_data, test_data_size) == 0);

    // read some
    const byte_t* read = ring_buffer_read(cut, 1000);
    REQUIRE(memcmp(read, test_data, 1000) == 0);
    REQUIRE(ring_buffer_get_used(cut) == test_data_size - 1000);

    // write some
    REQUIRE(ring_buffer_write(cut, test_data, 900) == 900);
    REQUIRE(ring_buffer_get_used(cut) == test_data_size - 100);
    REQUIRE(ring_buffer_get_free(cut) == 100);

    // read everything
    const byte_t* read2 = ring_buffer_read(cut, ring_buffer_get_used(cut));
    REQUIRE(memcmp(read, test_data, test_data_size - 100) == 0);
    REQUIRE(ring_buffer_get_used(cut) == 0);

    // free
    ring_buffer_free(cut);
    free((void*)test_data);
}

TEST_CASE("failed initialization", "[ring buffer]")
{
    // order too small
    ring_buffer_t* cut = ring_buffer_new(2);
    REQUIRE(!cut);
}

