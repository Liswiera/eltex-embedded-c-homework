#include "../../unity/unity.h"
#include "lib.h"
#include <stdio.h>

size_t expected_id;

void setUp(void) {
    expected_id = 0x19194242;
}

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


void expect_null(const char* text, const char* pattern) {
    const char* pos = find_substr_naive(text, pattern);

    TEST_ASSERT_EQUAL_PTR(NULL, pos);
}

void expect(size_t e) {
    expected_id = e;
}

void given(const char* text, const char* pattern) {
    const char* pos = find_substr_naive(text, pattern);
    size_t id = (size_t)(pos - text);

    TEST_ASSERT_EQUAL_size_t(expected_id, id);
}

void test_find_substr_naive_both_empty(void) {
    expect(0);
    given("", "");
}

void test_find_substr_naive_text_empty(void) {
    expect_null("", "p");
}

void test_find_substr_naive_pattern_empty(void) {
    expect(0);
    given("Hello World", "");
}

void test_find_substr_naive_equal(void) {
    expect(0);
    given("Hello World", "Hello World");
}

void test_find_substr_naive_at_0(void) {
    expect(0);
    given("Hello World", "Hello");
}

void test_find_substr_naive_one_match(void) {
    expect(6);
    given("Hello World! This is a text.", "World");
}

void test_find_substr_naive_multiple_matches(void) {
    expect(6);
    given("Hello WorldWorldWorldWorld", "World");
}

void test_find_substr_naive_tricky(void) {
    expect(8);
    given("abacabadabacabadezzz", "abacabade");
}

void test_find_substr_naive_optimized_example(void) {
    expect_null("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "aaaaaaaaaaaaaaaz");
}


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_set_byte_zero_value);
    RUN_TEST(test_set_byte_zero_byte);
    RUN_TEST(test_set_no_change);
    RUN_TEST(test_set_rewrite_byte);

    RUN_TEST(test_find_substr_naive_both_empty);
    RUN_TEST(test_find_substr_naive_text_empty);
    RUN_TEST(test_find_substr_naive_pattern_empty);
    RUN_TEST(test_find_substr_naive_equal);
    RUN_TEST(test_find_substr_naive_at_0);
    RUN_TEST(test_find_substr_naive_one_match);
    RUN_TEST(test_find_substr_naive_tricky);
    RUN_TEST(test_find_substr_naive_optimized_example);

    return UNITY_END();
}
