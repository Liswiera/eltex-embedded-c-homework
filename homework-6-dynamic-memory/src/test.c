#include "../../unity/unity.h"
#include "abonent_list.h"

static struct abonent_list *list;

void setUp(void) {
    list = abonent_list_create_empty();
}

void tearDown(void) {
    abonent_list_free(list);
    list = NULL;
}

void assert_list_values(size_t size, struct abonent_list_node *head, struct abonent_list_node *tail) {
    TEST_ASSERT_EQUAL_size_t(size, list->size);
    TEST_ASSERT_EQUAL_PTR(head, list->head);
    TEST_ASSERT_EQUAL_PTR(tail, list->tail);
}

void assert_list_empty(void) {
    assert_list_values(0, NULL, NULL);
}

void assert_node_order(struct abonent_list_node *prev, struct abonent_list_node *node, struct abonent_list_node *next) {
    TEST_ASSERT_EQUAL_PTR(prev, node->prev);
    TEST_ASSERT_EQUAL_PTR(next, node->next);
}


void test_list_node_build(void) {
    const char* name = "Vadim";
    const char* second_name = "Boyarkin";
    const char* tel = "123-45-67";

    struct abonent_list_node *node = abonent_list_node_build(name, second_name, tel);
    TEST_ASSERT_EQUAL_STRING(name, node->data.name);
    TEST_ASSERT_EQUAL_STRING(second_name, node->data.second_name);
    TEST_ASSERT_EQUAL_STRING(tel, node->data.tel);

    TEST_ASSERT_NULL(node->prev);
    TEST_ASSERT_NULL(node->next);
}

void test_list_empty(void) {
    assert_list_empty();
}


void test_list_add_one_node(void) {
    const char* name = "Vadim";
    const char* second_name = "Boyarkin";
    const char* tel = "123-45-67";

    struct abonent_list_node *node = abonent_list_add(list, name, second_name, tel);
    TEST_ASSERT_NOT_NULL(node);

    TEST_ASSERT_EQUAL_STRING(name, node->data.name);
    TEST_ASSERT_EQUAL_STRING(second_name, node->data.second_name);
    TEST_ASSERT_EQUAL_STRING(tel, node->data.tel);
    assert_node_order(NULL, node, NULL);

    assert_list_values(1, node, node);
}

void test_list_add_two_nodes(void) {
    struct abonent_list_node *node1 = abonent_list_add(list, "Abc", "Def", "123");
    struct abonent_list_node *node2 = abonent_list_add(list, "ZZZ", "ABC", "999");
    TEST_ASSERT_NOT_NULL(node2);
    TEST_ASSERT_TRUE(node1 != node2);

    assert_node_order(NULL, node1, node2);
    assert_node_order(node1, node2, NULL);

    assert_list_values(2, node1, node2);
}

void test_list_add_three_nodes(void) {
    struct abonent_list_node *node1 = abonent_list_add(list, "Abc", "Def", "123");
    struct abonent_list_node *node2 = abonent_list_add(list, "ZZZ", "ABC", "999");
    struct abonent_list_node *node3 = abonent_list_add(list, "a", "b", "c");
    TEST_ASSERT_NOT_NULL(node3);
    TEST_ASSERT_TRUE(node3 != node1);
    TEST_ASSERT_TRUE(node3 != node2);

    // Проверка корректности связей узлов списка
    assert_node_order(NULL, node1, node2);
    assert_node_order(node1, node2, node3);
    assert_node_order(node2, node3, NULL);

    // Проверка корректности объекта структуры abonent_list
    assert_list_values(3, node1, node3);
}

void test_list_get_element_empty_list(void) {
    TEST_ASSERT_NULL(abonent_list_get_node_at(list, 0));
    TEST_ASSERT_NULL(abonent_list_get_node_at(list, 1));
    TEST_ASSERT_NULL(abonent_list_get_node_at(list, SIZE_MAX));
    assert_list_empty();
}

void test_list_extract_element_empty_list(void) {
    TEST_ASSERT_NULL(abonent_list_extract_node_at(list, 0));
    TEST_ASSERT_NULL(abonent_list_extract_node_at(list, 1));
    TEST_ASSERT_NULL(abonent_list_extract_node_at(list, SIZE_MAX));
    assert_list_empty();
}

void test_list_remove_element_empty_list(void) {
    TEST_ASSERT_EQUAL_UINT32(0, abonent_list_remove_at(list, 0));
    TEST_ASSERT_EQUAL_UINT32(0, abonent_list_remove_at(list, 1));
    TEST_ASSERT_EQUAL_UINT32(0, abonent_list_remove_at(list, SIZE_MAX));
    assert_list_empty();
}

void test_list_extract_single_element(void) {
    const char* name = "Vadim";
    const char* second_name = "Boyarkin";
    const char* tel = "123-45-67";

    struct abonent_list_node *node = abonent_list_add(list, name, second_name, tel);

    TEST_ASSERT_NULL(abonent_list_extract_node_at(list, 1));
    TEST_ASSERT_NULL(abonent_list_extract_node_at(list, SIZE_MAX));

    struct abonent_list_node *extracted_node = abonent_list_extract_node_at(list, 0);
    TEST_ASSERT_EQUAL_PTR(node, extracted_node);


    // Проверка корректности извлеченного узла списка
    TEST_ASSERT_EQUAL_STRING(name, extracted_node->data.name);
    TEST_ASSERT_EQUAL_STRING(second_name, extracted_node->data.second_name);
    TEST_ASSERT_EQUAL_STRING(tel, extracted_node->data.tel);
    assert_node_order(NULL, extracted_node, NULL);

    assert_list_empty();

    abonent_list_node_free(extracted_node);
}

void test_list_extract_left_element(void) {
    struct abonent_list_node *node1 = abonent_list_add(list, "a", "b", "c");
    struct abonent_list_node *node2 = abonent_list_add(list, "a", "b", "c");
    struct abonent_list_node *node3 = abonent_list_add(list, "a", "b", "c");

    struct abonent_list_node *extracted_node = abonent_list_extract_node_at(list, 0);
    TEST_ASSERT_EQUAL_PTR(node1, extracted_node);

    // Проверка корректности извлеченного узла списка
    assert_node_order(NULL, extracted_node, node2);

    // Проверка корректности связей узлов списка
    assert_node_order(NULL, node2, node3);
    assert_node_order(node2, node3, NULL);

    // Проверка корректности объекта структуры abonent_list
    assert_list_values(2, node2, node3);

    abonent_list_node_free(extracted_node);
}

void test_list_extract_middle_element(void) {
    struct abonent_list_node *node1 = abonent_list_add(list, "a", "b", "c");
    struct abonent_list_node *node2 = abonent_list_add(list, "a", "b", "c");
    struct abonent_list_node *node3 = abonent_list_add(list, "a", "b", "c");

    struct abonent_list_node *extracted_node = abonent_list_extract_node_at(list, 1);
    TEST_ASSERT_EQUAL_PTR(node2, extracted_node);

    // Проверка корректности извлеченного узла списка
    assert_node_order(node1, extracted_node, node3);

    // Проверка корректности связей узлов списка
    assert_node_order(NULL, node1, node3);
    assert_node_order(node1, node3, NULL);

    // Проверка корректности объекта структуры abonent_list
    assert_list_values(2, node1, node3);

    abonent_list_node_free(extracted_node);
}

void test_list_extract_right_element(void) {
    struct abonent_list_node *node1 = abonent_list_add(list, "a", "b", "c");
    struct abonent_list_node *node2 = abonent_list_add(list, "a", "b", "c");
    struct abonent_list_node *node3 = abonent_list_add(list, "a", "b", "c");

    struct abonent_list_node *extracted_node = abonent_list_extract_node_at(list, 2);
    TEST_ASSERT_EQUAL_PTR(node3, extracted_node);

    // Проверка корректности извлеченного узла списка
    assert_node_order(node2, extracted_node, NULL);

    // Проверка корректности связей узлов списка
    assert_node_order(NULL, node1, node2);
    assert_node_order(node1, node2, NULL);

    // Проверка корректности объекта структуры abonent_list
    assert_list_values(2, node1, node2);

    abonent_list_node_free(extracted_node);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_list_node_build);

    RUN_TEST(test_list_empty);
    RUN_TEST(test_list_add_one_node);
    RUN_TEST(test_list_add_two_nodes);
    RUN_TEST(test_list_add_three_nodes);

    RUN_TEST(test_list_get_element_empty_list);
    RUN_TEST(test_list_extract_element_empty_list);
    RUN_TEST(test_list_remove_element_empty_list);

    RUN_TEST(test_list_extract_single_element);
    RUN_TEST(test_list_extract_left_element);
    RUN_TEST(test_list_extract_middle_element);
    RUN_TEST(test_list_extract_right_element);

    return UNITY_END();
}
