#include "../../unity/unity.h"
#include "lib.h"
#include <string.h>
#include <stdio.h>


void setUp(void) { }

void tearDown(void) { }


void test_set_byte_zero_value(void) {
    TEST_ASSERT_EQUAL_UINT32(0x00A70000u, uint32_set_byte_2(0x00000000u, 0xA7u));
}

void test_set_byte_zero_byte(void) {
    TEST_ASSERT_EQUAL_UINT32(0x12005678u, uint32_set_byte_2(0x12345678u, 0x00u));
}

void test_set_no_change(void) {
    TEST_ASSERT_EQUAL_UINT32(0x12345678u, uint32_set_byte_2(0x12345678u, 0x34u));
}

void test_set_rewrite_byte(void) {
    TEST_ASSERT_EQUAL_UINT32(0x87AB4321u, uint32_set_byte_2(0x87654321u, 0xABu));
}


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_set_byte_zero_value);
    RUN_TEST(test_set_byte_zero_byte);
    RUN_TEST(test_set_no_change);
    RUN_TEST(test_set_rewrite_byte);

    return UNITY_END();
}
