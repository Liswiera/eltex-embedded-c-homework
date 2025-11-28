#include "../../unity/unity.h"
#include "lib.h"
#include <string.h>
#include <stdio.h>

#define BUF_SIZE 128
#define MEMSET_INIT_U32 0xAAAAAAAA


static uint32_t output[BUF_SIZE];
static const uint32_t *expected;

void setUp(void) { 
    memset(output, MEMSET_INIT_U32, sizeof(output));
    expected = NULL;
}

void tearDown(void) { }


void expect(const uint32_t *arr) {
    expected = arr;
}

void given(size_t length) {
    if (length > 0) {
        TEST_ASSERT_EQUAL_UINT32_ARRAY(expected, output, length);
    }

    TEST_ASSERT_EQUAL_UINT32(MEMSET_INIT_U32, output[length]);
}


void test_fill_array(const uint32_t *expected_arr, size_t length) {
    fill_array(output, length);

    expect(expected_arr);
    given(length);
}

void test_fill_array_0(void) {
    uint32_t expected_arr[] = {};
    test_fill_array(expected_arr, 0);
}

void test_fill_array_1(void) {
    uint32_t expected_arr[] = { 1 };
    test_fill_array(expected_arr, 1);
}

void test_fill_array_5(void) {
    uint32_t expected_arr[] = { 1, 2, 3, 4, 5 };
    test_fill_array(expected_arr, 5);
}


void test_reverse_array_0(void) {
    uint32_t expected_arr[] = { };
    reverse_array(output, 0);

    expect(expected_arr);
    given(0);
}

void test_reverse_array_1(void) {
    uint32_t expected_arr[] = { 42 };

    static const uint32_t init_arr[] = { 42 };
    memcpy(output, init_arr, sizeof(init_arr));
    reverse_array(output, 1);

    expect(expected_arr);
    given(1);
}

void test_reverse_array_3(void) {
    uint32_t expected_arr[] = { 3, 2, 1 };

    static const uint32_t init_arr[] = { 1, 2, 3 };
    memcpy(output, init_arr, sizeof(init_arr));
    reverse_array(output, 3);

    expect(expected_arr);
    given(3);
}

void test_reverse_array_6(void) {
    uint32_t expected_arr[] = { 9, 7, 4, 1, 2, 8 };

    static const uint32_t init_arr[] = { 8, 2, 1, 4, 7, 9 };
    memcpy(output, init_arr, sizeof(init_arr));
    reverse_array(output, 6);

    expect(expected_arr);
    given(6);
}


void test_fill_triangle_matrix(const uint32_t *expected_mat, size_t n) {
    fill_triangle_matrix(output, n);

    expect(expected_mat);
    given(n * n);
}

void test_fill_triangle_matrix_0(void) {
    uint32_t expected_mat[] = { };
    test_fill_triangle_matrix(expected_mat, 0);
}

void test_fill_triangle_matrix_1(void) {
    uint32_t expected_mat[] = {
        1
    };
    test_fill_triangle_matrix(expected_mat, 1);
}

void test_fill_triangle_matrix_3(void) {
    uint32_t expected_mat[] = {
        0, 0, 1,
        0, 1, 1,
        1, 1, 1,
    };
    test_fill_triangle_matrix(expected_mat, 3);
}

void test_fill_triangle_matrix_4(void) {
    uint32_t expected_mat[] = {
        0, 0, 0, 1,
        0, 0, 1, 1,
        0, 1, 1, 1,
        1, 1, 1, 1,
    };
    test_fill_triangle_matrix(expected_mat, 4);
}


void test_fill_spiral_matrix(const uint32_t *expected_mat, size_t n) {
    fill_spiral_matrix(output, n);

    expect(expected_mat);
    given(n * n);
}

void test_fill_spiral_matrix_0(void) {
    uint32_t expected_mat[] = { };
    test_fill_spiral_matrix(expected_mat, 0);
}

void test_fill_spiral_matrix_1(void) {
    uint32_t expected_mat[] = {
        1
    };
    test_fill_spiral_matrix(expected_mat, 1);
}

void test_fill_spiral_matrix_2(void) {
    uint32_t expected_mat[] = {
        1, 2,
        4, 3,
    };
    test_fill_spiral_matrix(expected_mat, 2);
}

void test_fill_spiral_matrix_3(void) {
    uint32_t expected_mat[] = {
        1, 2, 3,
        8, 9, 4,
        7, 6, 5,
    };
    test_fill_spiral_matrix(expected_mat, 3);
}

void test_fill_spiral_matrix_4(void) {
    uint32_t expected_mat[] = {
         1,  2,  3, 4,
        12, 13, 14, 5,
        11, 16, 15, 6,
        10,  9,  8, 7,
    };
    test_fill_spiral_matrix(expected_mat, 4);
}


void test_fill_spiral_matrix_5(void) {
    uint32_t expected_mat[] = {
         1,  2,  3,  4, 5,
        16, 17, 18, 19, 6,
        15, 24, 25, 20, 7,
        14, 23, 22, 21, 8,
        13, 12, 11, 10, 9,
    };
    test_fill_spiral_matrix(expected_mat, 5);
}


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_fill_array_0);
    RUN_TEST(test_fill_array_1);
    RUN_TEST(test_fill_array_5);

    RUN_TEST(test_reverse_array_0);
    RUN_TEST(test_reverse_array_1);
    RUN_TEST(test_reverse_array_3);
    RUN_TEST(test_reverse_array_6);

    RUN_TEST(test_fill_triangle_matrix_0);
    RUN_TEST(test_fill_triangle_matrix_1);
    RUN_TEST(test_fill_triangle_matrix_3);
    RUN_TEST(test_fill_triangle_matrix_4);

    RUN_TEST(test_fill_spiral_matrix_0);
    RUN_TEST(test_fill_spiral_matrix_1);
    RUN_TEST(test_fill_spiral_matrix_2);
    RUN_TEST(test_fill_spiral_matrix_3);
    RUN_TEST(test_fill_spiral_matrix_4);
    RUN_TEST(test_fill_spiral_matrix_5);

    return UNITY_END();
}
