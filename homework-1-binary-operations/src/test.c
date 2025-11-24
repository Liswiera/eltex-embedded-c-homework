#include "../../unity/unity.h"
#include "lib.h"
#include <string.h>
#include <stdio.h>

#define MEMSET_INIT_BYTE 0xAA

static char output[128];
static const char *expected;

void setUp(void) {
    memset(output, MEMSET_INIT_BYTE, sizeof(output));
    expected = "";
}

void tearDown(void) { }


void expect(const char *s) {
    expected = s;
}

void given(size_t bytes_written) {
    size_t bytes_expected = strlen(expected);

    TEST_ASSERT_EQUAL_size_t(bytes_expected, bytes_written);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, output, bytes_expected);
    TEST_ASSERT_EQUAL_UINT8(MEMSET_INIT_BYTE, output[bytes_written]);
}


void test_binary_u32_5(void) {
    expect("101");
    given(uint32_to_binary(5, output));
}

void test_binary_u32_max_value(void) {
    expect("11111111""11111111""11111111""11111111");
    given(uint32_to_binary(UINT32_MAX, output));
}

void test_binary_u32_0(void) {
    expect("0");
    given(uint32_to_binary(0, output));
}


void test_binary_i32_neg_5(void) {
    expect("-101");
    given(int32_to_binary(-5, output));
}
void test_binary_i32_neg_1(void) {
    expect("-1");
    given(int32_to_binary(-1, output));
}
void test_binary_i32_min_value(void) {
    expect("-10000000""00000000""00000000""00000000");
    given(int32_to_binary(INT32_MIN, output));
}
void test_binary_i32_0(void) {
    expect("0");
    given(int32_to_binary(0, output));
}


void test_bit_count_0(void) {
    TEST_ASSERT_EQUAL_UINT32(0, uint32_bit_count(0));
}

void test_bit_count_1024(void) {
    TEST_ASSERT_EQUAL_UINT32(1, uint32_bit_count(1024));
}

void test_bit_count_15(void) {
    TEST_ASSERT_EQUAL_UINT32(4, uint32_bit_count(15));
}

void test_bit_count_u32_max(void) {
    TEST_ASSERT_EQUAL_UINT32(32, uint32_bit_count(UINT32_MAX));
}


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

    RUN_TEST(test_binary_u32_5);
    RUN_TEST(test_binary_u32_max_value);
    RUN_TEST(test_binary_u32_0);

    RUN_TEST(test_binary_i32_neg_5);
    RUN_TEST(test_binary_i32_neg_1);
    RUN_TEST(test_binary_i32_min_value);
    RUN_TEST(test_binary_i32_0);

    RUN_TEST(test_bit_count_0);
    RUN_TEST(test_bit_count_1024);
    RUN_TEST(test_bit_count_15);
    RUN_TEST(test_bit_count_u32_max);

    RUN_TEST(test_set_byte_zero_value);
    RUN_TEST(test_set_byte_zero_byte);
    RUN_TEST(test_set_no_change);
    RUN_TEST(test_set_rewrite_byte);

    return UNITY_END();
}