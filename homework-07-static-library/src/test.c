#include "../../unity/unity.h"
#include "libcalc/libcalc.h"

void setUp(void) {}

void tearDown(void) {}


void test_add(void) {
    int64_t (*func)(int64_t, int64_t) = calc_add;

    TEST_ASSERT_EQUAL_INT64(0, func(0, 0));
    TEST_ASSERT_EQUAL_INT64(1337, func(1337, 0));
    TEST_ASSERT_EQUAL_INT64(1337, func(0, 1337));
    TEST_ASSERT_EQUAL_INT64(8, func(5, 3));
    TEST_ASSERT_EQUAL_INT64(20, func(10, 10));
    TEST_ASSERT_EQUAL_INT64(4, func(10, -6));
    TEST_ASSERT_EQUAL_INT64(0, func(99, -99));
}

void test_sub(void) {
    int64_t (*func)(int64_t, int64_t) = calc_sub;

    TEST_ASSERT_EQUAL_INT64(0, func(0, 0));
    TEST_ASSERT_EQUAL_INT64(1337, func(1337, 0));
    TEST_ASSERT_EQUAL_INT64(-1337, func(0, 1337));
    TEST_ASSERT_EQUAL_INT64(2, func(5, 3));
    TEST_ASSERT_EQUAL_INT64(0, func(10, 10));
    TEST_ASSERT_EQUAL_INT64(16, func(10, -6));
    TEST_ASSERT_EQUAL_INT64(198, func(99, -99));
}

void test_mul(void) {
    int64_t (*func)(int64_t, int64_t) = calc_mul;

    TEST_ASSERT_EQUAL_INT64(0, func(0, 0));
    TEST_ASSERT_EQUAL_INT64(0, func(1337, 0));
    TEST_ASSERT_EQUAL_INT64(0, func(0, 1337));
    TEST_ASSERT_EQUAL_INT64(15, func(5, 3));
    TEST_ASSERT_EQUAL_INT64(100, func(10, 10));
    TEST_ASSERT_EQUAL_INT64(-60, func(10, -6));
    TEST_ASSERT_EQUAL_INT64(555, func(555, 1));
    TEST_ASSERT_EQUAL_INT64(-555, func(-1, 555));
    TEST_ASSERT_EQUAL_INT64(555, func(-1, -555));
}

void test_div(void) {
    int64_t (*func)(int64_t, int64_t) = calc_div;

    TEST_ASSERT_EQUAL_INT64(0, func(0, 10));
    TEST_ASSERT_EQUAL_INT64(0, func(0, -15));
    TEST_ASSERT_EQUAL_INT64(999, func(999, 1));
    TEST_ASSERT_EQUAL_INT64(5, func(15, 3));
    TEST_ASSERT_EQUAL_INT64(1, func(10, 10));
    TEST_ASSERT_EQUAL_INT64(-2, func(10, -5));
    TEST_ASSERT_EQUAL_INT64(-5, func(-30, 6));
    TEST_ASSERT_EQUAL_INT64(5, func(-30, -6));
    TEST_ASSERT_EQUAL_INT64(4, func(25, 6));
    TEST_ASSERT_EQUAL_INT64(3, func(23, 6));
    TEST_ASSERT_EQUAL_INT64(-4, func(25, -6));
    TEST_ASSERT_EQUAL_INT64(-3, func(23, -6));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_add);
    RUN_TEST(test_sub);
    RUN_TEST(test_mul);
    RUN_TEST(test_div);

    return UNITY_END();
}
