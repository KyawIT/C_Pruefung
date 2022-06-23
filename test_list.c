/*----------------------------------------------------------
 *				HTBLA-Leonding / Klasse: n/a
 * ---------------------------------------------------------
 * Title:			Implementation of UTs for lists
 * Author:			S. Schraml
 * ----------------------------------------------------------
 * Description:
 * Tests functions of a list.
 * ----------------------------------------------------------
 */

#include "test_list.h"

#include <stdio.h>
#include <string.h>

#include "shortcut.h"
#include "config.h"

#include "list.h"
#include "list_iterator.h"
#include "allocator.h"

#define ASSERT_NEW_VALID_LIST() _assert_new_valid_list(tc)
IntList _assert_new_valid_list(struct TestCase* tc) {
    int exp_alloc_cnt = mem_get_alloc_call_cnt() + 1;
    IntList list = list_obtain();
    ASSERT_TRUE(list != 0, MSG("Expected non-null list"));
    ASSERT_TRUE(list_is_valid(list), MSG("Expected a valid list"));
    ASSERT_TRUE(list_is_empty(list), MSG("Expected an empty list"));
    ASSERT_TRUE(mem_get_alloc_call_cnt() == exp_alloc_cnt, MSG("Expected mem_alloc being called exactly once, but was called %d time(s)", mem_get_alloc_call_cnt() - exp_alloc_cnt - 1));
    ASSERT_TRUE(mem_is_allocated(list), MSG("Expected obtained list1 is allocated using mem_alloc"));
    return list;
}

#define ASSERT_RESET_AND_NEW_VALID_LIST() _assert_reset_and_new_valid_list(tc)
IntList _assert_reset_and_new_valid_list(struct TestCase* tc) {
    mem_reset_stat();
    return _assert_new_valid_list(tc);
}


#define ASSERT_FREE_VALID_LIST(p_list) _assert_free_valid_list(tc, p_list)
void _assert_free_valid_list(struct TestCase* tc, IntList* p_list) {
    ASSERT_TRUE(p_list != 0 && list_is_valid(*p_list), MSG("Expected a valid list"));
    int additional_free_calls =  1 + list_get_size(*p_list);
    int exp_alloc_cnt = mem_get_alloc_call_cnt();
    int exp_free_cnt = mem_get_free_call_cnt() + additional_free_calls;
    ASSERT_TRUE(mem_is_allocated(*p_list), MSG("Expected that list to release was allocated using mem_alloc"));
    list_release(p_list);
    ASSERT_TRUE(p_list != 0 && *p_list == 0, MSG("Expected that list was set to 0"));
    ASSERT_TRUE(mem_get_alloc_call_cnt() == exp_alloc_cnt, MSG("Expected mem_alloc being NOT called, but was called %d time(s)", mem_get_alloc_call_cnt() - exp_alloc_cnt));
    ASSERT_TRUE(mem_get_free_call_cnt() == exp_free_cnt, MSG("Expected mem_free being called exactly %d time(s), but was called %d time(s)", exp_free_cnt, mem_get_free_call_cnt() - exp_free_cnt + additional_free_calls));
    ASSERT_FALSE(mem_is_allocated(*p_list), MSG("Expected that released list is not allocated anymore after release"));
}

#define ASSERT_MEM_STATUS_EXT(exp_alloc_call_cnt, exp_free_call_cnt, exp_block_cnt) _assert_mem_status_ext(tc, exp_alloc_call_cnt, exp_free_call_cnt, exp_block_cnt)
void _assert_mem_status_ext(struct TestCase* tc, int exp_alloc_call_cnt, int exp_free_call_cnt, int exp_block_cnt) {
    ASSERT_TRUE(mem_get_allocated_block_cnt() == exp_block_cnt, MSG("Expected %d allocated memory block(s), but is %d", exp_block_cnt, mem_get_allocated_block_cnt()));
    ASSERT_TRUE(mem_get_alloc_call_cnt() == exp_alloc_call_cnt, MSG("Expected %d mem_alloc() call(s), but is %d", exp_alloc_call_cnt, mem_get_alloc_call_cnt()));
    ASSERT_TRUE(mem_get_free_call_cnt() == exp_free_call_cnt, MSG("Expected %d mem_free() call(s), but is %d", exp_free_call_cnt, mem_get_free_call_cnt()));
}

#define ASSERT_MEM_STATUS(exp_alloc_call_cnt, exp_free_call_cnt) _assert_mem_status_ext(tc, exp_alloc_call_cnt, exp_free_call_cnt, (exp_alloc_call_cnt - exp_free_call_cnt))
/* ------------------------------------------------------------------- */

TEST(test_list_obtain__shall_allocate_a_list) {
    int exp_alloc_cnt = 1;
    mem_reset_stat();
    IntList list = list_obtain();
    ASSERT_TRUE(list != 0, MSG("Expected non-null list"));
    ASSERT_TRUE(mem_get_alloc_call_cnt() == exp_alloc_cnt, MSG("Expected mem_alloc being called exactly %d time(s)", exp_alloc_cnt));
    ASSERT_TRUE(mem_is_allocated(list), MSG("Expected obtained list is allocated using mem_alloc"));
    if (list != 0) {
        free(list);
    }
}

TEST(test_list_obtain__shall_allocate_a_fresh_list) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    if (list != 0) {
        free(list);
    }
}

TEST(test_list_obtain__shall_allocate_a_different_list) {
    IntList list1 = ASSERT_RESET_AND_NEW_VALID_LIST();
    IntList list2 = ASSERT_NEW_VALID_LIST();
    ASSERT_TRUE(list1 != list2, MSG("Expected different lists"));
    if (list1 != 0) {
        free(list1);
    }
    if (list2 != 0) {
        free(list2);
    }
}

TEST(test_list_obtain_failed__shall_not_allocate_a_list) {
    IntList list1 = ASSERT_RESET_AND_NEW_VALID_LIST();
    int exp_alloc_cnt = 1;
    mem_reset_calls();
    mem_block_allocs(true);
    IntList list = list_obtain();
    ASSERT_TRUE(list == 0, MSG("Expected null list"));
    ASSERT_TRUE(mem_get_alloc_call_cnt() == exp_alloc_cnt, MSG("Expected mem_alloc being called exactly %d time(s)", exp_alloc_cnt));
    if (list != 0) {
        free(list);
    }
    free(list1);
}



TEST(test_list_release__shall_release_a_valid_empty_list) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    ASSERT_FREE_VALID_LIST(&list);
    ASSERT_TRUE(list == 0, MSG("Expected that list is set to 0"));
}

TEST(test_list_release__shall_release_a_valid_filled_list) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 11);
    list_insert(list, 12);
    ASSERT_FREE_VALID_LIST(&list);
    ASSERT_TRUE(list == 0, MSG("Expected that list is set to 0"));
}

TEST(test_list_release__shall_ignore_release_of_an_invalid_list) {
    IntList tmp = ASSERT_RESET_AND_NEW_VALID_LIST();
    int block_cnt = mem_get_allocated_block_cnt();
    IntList* p_list = 0;
    list_release(p_list);
    ASSERT_TRUE(mem_get_allocated_block_cnt() == block_cnt, MSG("Expected unchanged memory allocation"));
    ASSERT_TRUE(mem_get_free_call_cnt() == 0, MSG("Expected NO mem_free() call, but was called %d time(s)", mem_get_free_call_cnt()));

    IntList list = 0;
    p_list = &list;
    list_release(p_list);
    ASSERT_TRUE(mem_get_allocated_block_cnt() == block_cnt, MSG("Expected unchanged memory allocation"));
    ASSERT_TRUE(mem_get_free_call_cnt() == 0, MSG("Expected NO mem_free() call, but was called %d time(s)", mem_get_free_call_cnt()));

    list_release(&tmp);
}



TEST(test_list_is_valid__shall_be_true_for_valid_list) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    ASSERT_TRUE(list_is_valid(list), MSG("Expected list to be valid"));
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_is_valid__shall_be_false_for_invalid_list) {
    ASSERT_FALSE(list_is_valid(0), MSG("Expected list to be invalid"));
}



TEST(test_list_insert__shall_add_one_value_to_empty_list) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    int exp_block_cnt = 2; // list and one node
    list_insert(list, 11);
    ASSERT_MEM_STATUS(exp_block_cnt, 0);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_insert__shall_add_multiple_values) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    int exp_block_cnt = 2; // list and one node
    list_insert(list, 11);
    ASSERT_MEM_STATUS(exp_block_cnt, 0);
    list_insert(list, 12);
    exp_block_cnt++;
    ASSERT_MEM_STATUS(exp_block_cnt, 0);
    list_insert(list, 13);
    exp_block_cnt++;
    ASSERT_MEM_STATUS(exp_block_cnt, 0);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_insert__shall_ignore_invalid_list) {
    IntList tmp = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(0, 11);
    ASSERT_FREE_VALID_LIST(&tmp);
}



TEST(test_list_remove__shall_not_remove_from_empty_list) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    int exp_block_cnt = 1; // list
    list_remove(list, 11);
    ASSERT_MEM_STATUS(exp_block_cnt, 0);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_remove__shall_remove_only_value) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 11);
    list_remove(list, 11);
    ASSERT_MEM_STATUS(2, 1); // list and node alloc, node free
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_remove__shall_remove_one_value) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 11);
    list_insert(list, 12);
    list_insert(list, 11);
    list_insert(list, 11);
    list_remove(list, 11);
    ASSERT_TRUE(list_get_at(list, 0) == 12, MSG("Expected that the fist occurrance was removed"));
    ASSERT_MEM_STATUS(5, 1); // list and node alloc, node free
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_remove__shall_remove_first_value) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 11);
    list_insert(list, 12);
    list_insert(list, 13);
    list_remove(list, 11);
    ASSERT_MEM_STATUS(4, 1); // list and node alloc, node free
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_remove__shall_remove_value_in_the_middle) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 11);
    list_insert(list, 12);
    list_insert(list, 13);
    list_remove(list, 12);
    ASSERT_MEM_STATUS(4, 1); // list and node alloc, node free
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_remove__shall_remove_value_at_the_end) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 11);
    list_insert(list, 12);
    list_insert(list, 13);
    list_remove(list, 13);
    ASSERT_MEM_STATUS(4, 1); // list and node alloc, node free
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_remove__shall_not_remove_missing_value) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 11);
    list_insert(list, 12);
    list_insert(list, 13);
    list_remove(list, 14);
    ASSERT_MEM_STATUS(4, 0); // list and node alloc, node free
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_remove__shall_ignore_invalid_list) {
    IntList tmp = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_remove(0, 11);
    ASSERT_MEM_STATUS(1, 0);
    ASSERT_FREE_VALID_LIST(&tmp);
}



TEST(test_list_remove_all__shall_not_remove_from_empty_list) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    int exp_block_cnt = 1; // list
    list_remove_all(list, 21);
    ASSERT_MEM_STATUS(exp_block_cnt, 0);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_remove_all__shall_remove_only_value) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 21);
    list_remove_all(list, 21);
    ASSERT_MEM_STATUS(2, 1); // list and node alloc, node free
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_remove_all__shall_remove_all_values) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 21);
    list_insert(list, 21);
    list_insert(list, 21);
    list_remove_all(list, 21);
    ASSERT_TRUE(list_is_empty(list), MSG("Expected list is empty after removal of all nodes"));
    ASSERT_MEM_STATUS(4, 3); // list and node alloc, node free
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_remove_all__shall_remove_one_value) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 21);
    list_insert(list, 22);
    list_insert(list, 21);
    list_remove_all(list, 22);
    ASSERT_MEM_STATUS(4, 1); // list and node alloc, node free
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_remove_all__shall_remove_all_values_in_a_row) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 21);
    list_insert(list, 22);
    list_insert(list, 22);
    list_insert(list, 22);
    list_insert(list, 23);
    list_remove_all(list, 22);
    ASSERT_MEM_STATUS(6, 3); // list and node alloc, node free
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_remove_all__shall_remove_all_distributed_values) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 22);
    list_insert(list, 23);
    list_insert(list, 22);
    list_insert(list, 24);
    list_insert(list, 22);
    list_remove_all(list, 22);
    ASSERT_MEM_STATUS(6, 3); // list and node alloc, node free
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_remove_all__shall_not_remove_missing_value) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 22);
    list_insert(list, 23);
    list_insert(list, 22);
    list_insert(list, 24);
    list_insert(list, 22);
    list_remove_all(list, 21);
    ASSERT_MEM_STATUS(6, 0); // list and node alloc, node free
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_remove_all__shall_ignore_invalid_list) {
    IntList tmp = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_remove_all(0, 21);
    ASSERT_MEM_STATUS(1, 0);
    ASSERT_FREE_VALID_LIST(&tmp);
}



TEST(test_list_insert_at__shall_insert_single_value_in_empty_list) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert_at(list, 0, 31);
    ASSERT_MEM_STATUS(2, 0);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_insert_at__shall_insert_value_at_the_beginning) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 11);
    list_insert(list, 12);
    list_insert(list, 13);
    list_insert_at(list, 0, 31);
    ASSERT_MEM_STATUS(5, 0);
    ASSERT_TRUE(list_get_at(list, 0) == 31, MSG("Expected value was inserted at the beginning of the list"));
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_insert_at__shall_insert_value_in_the_middle) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 11);
    list_insert(list, 12);
    list_insert(list, 13);
    list_insert_at(list, 1, 31);
    ASSERT_MEM_STATUS(5, 0);
    ASSERT_TRUE(list_get_at(list, 1) == 31, MSG("Expected value was inserted in the middle of the list"));
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_insert_at__shall_insert_value_at_the_end) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 11);
    list_insert(list, 12);
    list_insert(list, 13);
    list_insert_at(list, 3, 31);
    ASSERT_MEM_STATUS(5, 0);
    ASSERT_TRUE(list_get_at(list, 3) == 31, MSG("Expected value was inserted before at the end of the list"));
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_insert_at__shall_insert_value_after_the_end) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 11);
    list_insert(list, 12);
    list_insert(list, 13);
    list_insert_at(list, 99, 31);
    ASSERT_MEM_STATUS(5, 0);
    ASSERT_TRUE(list_get_at(list, 3) == 31, MSG("Expected value was inserted before at the end of the list"));
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_insert_at__shall_ignore_invalid_list) {
    IntList tmp = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert_at(0, 2, 31);
    ASSERT_MEM_STATUS(1, 0);
    ASSERT_FREE_VALID_LIST(&tmp);
}



TEST(test_list_remove_at__shall_not_remove_from_empty_list) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_remove_at(list, 0);
    ASSERT_MEM_STATUS(1, 0);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_remove_at__shall_remove_value_at_the_beginning) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 41);
    list_insert(list, 42);
    list_insert(list, 43);
    int val = list_remove_at(list, 0);
    ASSERT_MEM_STATUS(4, 1);
    ASSERT_TRUE(list_get_at(list, 0) == 42, MSG("Expected value was removed form the beginning of the list"));
    ASSERT_TRUE(val == 41, MSG("Expected value %d was removed, but got %d", 41, val));
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_remove_at__shall_remove_value_in_the_middle) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 41);
    list_insert(list, 42);
    list_insert(list, 43);
    int val = list_remove_at(list, 1);
    ASSERT_MEM_STATUS(4, 1);
    ASSERT_TRUE(list_get_at(list, 1) == 43, MSG("Expected value was removed form the middle of the list"));
    ASSERT_TRUE(val == 42, MSG("Expected value %d was removed, but got %d", 42, val));
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_remove_at__shall_remove_value_at_the_end) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 41);
    list_insert(list, 42);
    list_insert(list, 43);
    int val = list_remove_at(list, 2);
    ASSERT_MEM_STATUS(4, 1);
    ASSERT_TRUE(list_get_at(list, 1) == 42, MSG("Expected value was removed form the end of the list"));
    ASSERT_TRUE(val == 43, MSG("Expected value %d was removed, but got %d", 43, val));
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_remove_at__shall_not_remove_value_after_the_end) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 41);
    list_insert(list, 42);
    list_insert(list, 43);
    int val = list_remove_at(list, 3);
    ASSERT_MEM_STATUS(4, 0);
    ASSERT_TRUE(val == 0, MSG("Expected that no value (0) was removed, but got %d", val));
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_remove_at__shall_ignore_invalid_list) {
    IntList tmp = ASSERT_RESET_AND_NEW_VALID_LIST();
    int val = list_remove_at(0, 0);
    ASSERT_TRUE(val == 0, MSG("Expected that no value (0) was removed, but got %d", val));
    ASSERT_MEM_STATUS(1, 0);
    ASSERT_FREE_VALID_LIST(&tmp);
}



TEST(test_list_clear__shall_clear_empty_list) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_clear(list);
    ASSERT_MEM_STATUS(1, 0);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_clear__shall_clear_list_with_one_value) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 51);
    list_clear(list);
    ASSERT_MEM_STATUS(2, 1);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_clear__shall_clear_list_with_multiple_values) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 51);
    list_insert(list, 52);
    list_insert(list, 53);
    list_clear(list);
    ASSERT_MEM_STATUS(4, 3);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_clear__shall_ingore_invalid_list) {
    IntList tmp = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_clear(0);
    ASSERT_MEM_STATUS(1, 0);
    ASSERT_FREE_VALID_LIST(&tmp);
}



TEST(test_list_is_empty__shall_be_true__for_newly_obtained_list) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    ASSERT_TRUE(list_is_empty(0), MSG("Expected that new list IS empty"));
    ASSERT_MEM_STATUS(1, 0);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_is_empty__shall_be_false__after_adding_values) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 61);
    ASSERT_FALSE(list_is_empty(list), MSG("Expected that list with values IS NOT empty"));
    ASSERT_MEM_STATUS(2, 0);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_is_empty__shall_be_true__after_clear) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 51);
    list_insert(list, 52);
    list_insert(list, 53);
    list_clear(list);
    ASSERT_FALSE(list_is_empty(0), MSG("Expected that list IS empty after list_clear"));
    ASSERT_MEM_STATUS(4, 3);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_is_empty__shall_be_true_for_invalid_list) {
    IntList tmp = ASSERT_RESET_AND_NEW_VALID_LIST();
    ASSERT_TRUE(list_is_empty(0), MSG("Expected that invalid list IS empty"));
    ASSERT_MEM_STATUS(1, 0);
    ASSERT_FREE_VALID_LIST(&tmp);
}



TEST(test_list_get_size__shall_be_0_for_newly_obtained_list) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    ASSERT_TRUE(list_get_size(list) == 0, MSG("Expected that size of new list is 0"));
    ASSERT_MEM_STATUS(1, 0);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_get_size__shall_reflect_number_of_values_after_adding_values) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 71);
    ASSERT_TRUE(list_get_size(list) == 1, MSG("Expected that size of list is 1 after adding first value"));
    list_insert(list, 72);
    ASSERT_TRUE(list_get_size(list) == 2, MSG("Expected that size of list is 2 after adding second value"));
    list_insert(list, 73);
    ASSERT_TRUE(list_get_size(list) == 3, MSG("Expected that size of list is 3 after adding third value"));
    ASSERT_MEM_STATUS(4, 0);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_get_size__shall_reflect_number_of_values_after_removing_values) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 81);
    list_insert(list, 82);
    list_insert(list, 83);
    list_remove_at(list, 0);
    ASSERT_TRUE(list_get_size(list) == 2, MSG("Expected that size of list is 2 after removing first value"));
    list_remove_at(list, 1);
    ASSERT_TRUE(list_get_size(list) == 1, MSG("Expected that size of list is 1 after removing second value"));
    list_remove_at(list, 0);
    ASSERT_TRUE(list_get_size(list) == 0, MSG("Expected that size of list is 0 after removing third value"));
    ASSERT_MEM_STATUS(4, 3);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_get_size__shall_be_0_after_clear) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 81);
    list_insert(list, 82);
    list_insert(list, 83);
    list_clear(list);
    ASSERT_TRUE(list_get_size(list) == 0, MSG("Expected that size of list is 0 after clearing the list"));
    ASSERT_MEM_STATUS(4, 3);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_get_size__shall_be_0_for_invalid_list) {
    IntList tmp = ASSERT_RESET_AND_NEW_VALID_LIST();
    ASSERT_TRUE(list_get_size(0) == 0, MSG("Expected that size of invalid list is 0"));
    ASSERT_MEM_STATUS(1, 0);
    ASSERT_FREE_VALID_LIST(&tmp);
}



TEST(test_list_contains__shall_be_false_for_empty_list) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    ASSERT_FALSE(list_contains(list, 91), MSG("Expected that empty list does not contain any value"));
    ASSERT_MEM_STATUS(1, 0);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_contains__shall_be_true_for_one_value) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 91);
    ASSERT_TRUE(list_contains(list, 91), MSG("Expected that list does contain the value"));
    list_insert(list, 92);
    ASSERT_TRUE(list_contains(list, 91), MSG("Expected that list does contain the value"));
    ASSERT_TRUE(list_contains(list, 92), MSG("Expected that list does contain the value"));
    ASSERT_MEM_STATUS(3, 0);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_contains__shall_be_true_for_multiple_values) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 91);
    ASSERT_TRUE(list_contains(list, 91), MSG("Expected that list does contain the value"));
    list_insert(list, 91);
    ASSERT_TRUE(list_contains(list, 91), MSG("Expected that list does contain the value"));
    ASSERT_MEM_STATUS(3, 0);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_contains__shall_be_false_form_missing_value) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 91);
    ASSERT_FALSE(list_contains(list, 94), MSG("Expected that list does NOT contain the value"));
    list_insert(list, 92);
    ASSERT_FALSE(list_contains(list, 94), MSG("Expected that list does NOT contain the value"));
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_contains__shall_be_false_for_invalid_list) {
    IntList tmp = ASSERT_RESET_AND_NEW_VALID_LIST();
    ASSERT_FALSE(list_contains(0, 0), MSG("Expected that invalid list does not contain any value"));
    ASSERT_MEM_STATUS(1, 0);
    ASSERT_FREE_VALID_LIST(&tmp);
}



TEST(test_list_get_at__shall_return_0_for_empty_list) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    ASSERT_TRUE(list_get_at(list, 0) == 0, MSG("Expected that empty list provides always 0"));
    ASSERT_TRUE(list_get_at(list, 1) == 0, MSG("Expected that empty list provides always 0"));
    ASSERT_MEM_STATUS(1, 0);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_get_at__shall_return_single_value) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 101);
    ASSERT_TRUE(list_get_at(list, 0) == 101, MSG("Expected that list provides the value"));
    ASSERT_MEM_STATUS(2, 0);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_get_at__shall_return_first_value) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 101);
    list_insert(list, 102);
    list_insert(list, 103);
    ASSERT_TRUE(list_get_at(list, 0) == 101, MSG("Expected that list provides the value"));
    ASSERT_MEM_STATUS(4, 0);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_get_at__shall_return_values_in_added_order) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 101);
    list_insert(list, 102);
    list_insert(list, 103);
    ASSERT_TRUE(list_get_at(list, 0) == 101, MSG("Expected that list provides the first value"));
    ASSERT_TRUE(list_get_at(list, 1) == 102, MSG("Expected that list provides the second value"));
    ASSERT_TRUE(list_get_at(list, 2) == 103, MSG("Expected that list provides the third value"));
    list_insert_at(list, 1, 104);
    ASSERT_TRUE(list_get_at(list, 0) == 101, MSG("Expected that list provides the first value"));
    ASSERT_TRUE(list_get_at(list, 1) == 104, MSG("Expected that list provides the new second value"));
    ASSERT_TRUE(list_get_at(list, 2) == 102, MSG("Expected that list provides the new third value"));
    ASSERT_TRUE(list_get_at(list, 3) == 103, MSG("Expected that list provides the new forth value"));
    ASSERT_MEM_STATUS(5, 0);
    ASSERT_FREE_VALID_LIST(&list);
}

TEST(test_list_get_at__shall_return_0_for_invalid_list) {
    IntList tmp = ASSERT_RESET_AND_NEW_VALID_LIST();
    ASSERT_TRUE(list_get_at(0, 0) == 0, MSG("Expected that invalid list provides always 0"));
    ASSERT_MEM_STATUS(1, 0);
    ASSERT_FREE_VALID_LIST(&tmp);
}



TEST(test_list_append__shall_append_empty_list_to_empty_list) {
    IntList list1 = ASSERT_RESET_AND_NEW_VALID_LIST();
    IntList list2 = ASSERT_NEW_VALID_LIST();
    list_append(list1, list2);
    ASSERT_TRUE(list_is_empty(list1), MSG("Expected that empty list "));
    ASSERT_MEM_STATUS(2, 0);
    ASSERT_FREE_VALID_LIST(&list1);
    ASSERT_FREE_VALID_LIST(&list2);
}

TEST(test_list_append__shall_append_filled_list_to_empty_list) {
    IntList list1 = ASSERT_RESET_AND_NEW_VALID_LIST();
    IntList list2 = ASSERT_NEW_VALID_LIST();
    list_insert(list2, 111);
    list_insert(list2, 112);
    list_insert(list2, 113);
    list_append(list1, list2);
    ASSERT_TRUE(list_get_at(list1, 0) == 111, MSG("Expected that list provides the first value"));
    ASSERT_TRUE(list_get_at(list1, 1) == 112, MSG("Expected that list provides the second value"));
    ASSERT_TRUE(list_get_at(list1, 2) == 113, MSG("Expected that list provides the third value"));
    ASSERT_TRUE(list_get_size(list1) == 3, MSG("Expected that size of appended list as the size of the resulting list"));
    ASSERT_TRUE(list_get_size(list2) == 0, MSG("Expected that the appended list is empty"));
    ASSERT_MEM_STATUS(5, 0);
    ASSERT_FREE_VALID_LIST(&list1);
    ASSERT_FREE_VALID_LIST(&list2);
}

TEST(test_list_append__shall_append_empty_list_to_filled_list) {
    IntList list1 = ASSERT_RESET_AND_NEW_VALID_LIST();
    IntList list2 = ASSERT_NEW_VALID_LIST();
    list_insert(list1, 121);
    list_insert(list1, 122);
    list_insert(list1, 123);
    list_append(list1, list2);
    ASSERT_TRUE(list_get_at(list1, 0) == 121, MSG("Expected that list provides the first value"));
    ASSERT_TRUE(list_get_at(list1, 1) == 122, MSG("Expected that list provides the second value"));
    ASSERT_TRUE(list_get_at(list1, 2) == 123, MSG("Expected that list provides the third value"));
    ASSERT_TRUE(list_get_size(list1) == 3, MSG("Expected that size of original list as the size of the resulting list"));
    ASSERT_TRUE(list_get_size(list2) == 0, MSG("Expected that the appended list is empty"));
    ASSERT_MEM_STATUS(5, 0);
    ASSERT_FREE_VALID_LIST(&list1);
    ASSERT_FREE_VALID_LIST(&list2);
}

TEST(test_list_append__shall_append_filled_list_to_filled_list) {
    IntList list1 = ASSERT_RESET_AND_NEW_VALID_LIST();
    IntList list2 = ASSERT_NEW_VALID_LIST();
    list_insert(list1, 131);
    list_insert(list1, 132);
    list_insert(list1, 133);
    list_insert(list2, 141);
    list_insert(list2, 142);
    list_insert(list2, 143);
    list_append(list1, list2);
    ASSERT_TRUE(list_get_at(list1, 0) == 131, MSG("Expected that list provides the first value"));
    ASSERT_TRUE(list_get_at(list1, 1) == 132, MSG("Expected that list provides the second value"));
    ASSERT_TRUE(list_get_at(list1, 2) == 133, MSG("Expected that list provides the third value"));
    ASSERT_TRUE(list_get_at(list1, 3) == 141, MSG("Expected that list provides the first value of appended list"));
    ASSERT_TRUE(list_get_at(list1, 4) == 142, MSG("Expected that list provides the second value of appended list"));
    ASSERT_TRUE(list_get_at(list1, 5) == 143, MSG("Expected that list provides the third value of appended list"));
    ASSERT_TRUE(list_get_size(list1) == 6, MSG("Expected that size of list is the size of both lists"));
    ASSERT_TRUE(list_get_size(list2) == 0, MSG("Expected that the appended list is empty"));
    ASSERT_MEM_STATUS(8, 0);
    ASSERT_FREE_VALID_LIST(&list1);
    ASSERT_FREE_VALID_LIST(&list2);
}

TEST(test_list_append__shall_ignore_invalid_lists) {
    IntList tmp = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_append(0, tmp);
    ASSERT_MEM_STATUS(1, 0);
    list_append(tmp, 0);
    ASSERT_MEM_STATUS(1, 0);
    list_append(0, 0);
    ASSERT_MEM_STATUS(1, 0);
    ASSERT_FREE_VALID_LIST(&tmp);
}

/* ---------------------------------------- */

TEST(test_list_it_obtain__shall_allocate_and_release_a_list_iterator_for_a_list) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    mem_reset_calls();
    IntListIterator it = list_it_obtain(list);
    ASSERT_TRUE(it != 0, MSG("Expected non-null list iterator"));
    ASSERT_TRUE(mem_get_alloc_call_cnt() == 1, MSG("Expected mem_alloc being called exactly %d time(s) for the iterator", 1));
    list_it_release(&it);
    ASSERT_TRUE(mem_get_free_call_cnt() == 1, MSG("Expected mem_free being called exactly %d time(s) for the iterator", 1));
    list_release(&list);
}

TEST(test_list_it_obtain__shall_allocate_and_point_to_list_head) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 21);
    mem_reset_calls();
    IntListIterator it = list_it_obtain(list);
    ASSERT_TRUE(it != 0, MSG("Expected non-null list iterator"));
    ASSERT_TRUE(list_it_get(it) == 21, MSG("Expected list iterator pointing to list head (requires 'list_it_get()'"));
    ASSERT_TRUE(mem_get_alloc_call_cnt() == 1, MSG("Expected mem_alloc being called exactly %d time(s) for the iterator", 1));
    list_it_release(&it);
    ASSERT_TRUE(mem_get_free_call_cnt() == 1, MSG("Expected mem_free being called exactly %d time(s) for the iterator", 1));
    list_release(&list);
}

TEST(test_list_it_obtain__shall_not_allocate_a_list_iterator_for_an_invalid_list) {
    mem_reset_calls();
    IntListIterator it = list_it_obtain(0);
    ASSERT_TRUE(it == 0, MSG("Expected null list iterator"));
    ASSERT_TRUE(mem_get_alloc_call_cnt() == 0, MSG("Expected mem_alloc being called exactly %d time(s) for the iterator", 0));
}

TEST(test_list_it_is_valid) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 21);
    IntListIterator it = list_it_obtain(list);
    ASSERT_TRUE(list_it_is_valid(it), MSG("Expected valid list iterator"));
    list_it_release(&it);
    list_release(&list);
}

TEST(test_list_it_is_valid__shall_be_invalid_for_0_iterator) {
    ASSERT_FALSE(list_it_is_valid(0), MSG("Expected invalid list iterator"));
}

TEST(test_list_it_is_valid__shall_be_invalid_for_empty_list) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    IntListIterator it = list_it_obtain(list);
    ASSERT_FALSE(list_it_is_valid(it), MSG("Expected invalid list iterator"));
    list_it_release(&it);
    list_release(&list);
}

TEST(test_list_it_next__shall_move_to_next) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 21);
    list_insert(list, 22);
    IntListIterator it = list_it_obtain(list);
    ASSERT_TRUE(list_it_is_valid(it), MSG("Expected valid list iterator"));
    ASSERT_TRUE(list_it_get(it) == 21, MSG("Expected list iterator pointing to list head (requires 'list_it_get()'"));
    ASSERT_TRUE(list_it_next(it), MSG("Expected list iterator moved to next"));
    ASSERT_TRUE(list_it_get(it) == 22, MSG("Expected list iterator pointing to next node (requires 'list_it_get()'"));
    ASSERT_FALSE(list_it_next(it), MSG("Expected list iterator did not move to next"));
    ASSERT_TRUE(list_it_get(it) == 22, MSG("Expected list iterator pointing to last node (requires 'list_it_get()'"));
    list_it_release(&it);
    list_release(&list);
}

TEST(test_list_it_next__shall_return_false_for_invalid_it) {
    ASSERT_FALSE(list_it_next(0), MSG("Expected invalid list iterator has no next"));
}

TEST(test_list_it_next__shall_move_to_previous) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 21);
    list_insert(list, 22);
    IntListIterator it = list_it_obtain(list);
    ASSERT_TRUE(list_it_is_valid(it), MSG("Expected valid list iterator"));
    ASSERT_TRUE(list_it_get(it) == 21, MSG("Expected list iterator pointing to list head (requires 'list_it_get()'"));
    ASSERT_TRUE(list_it_next(it), MSG("Expected list iterator moved to next"));
    ASSERT_TRUE(list_it_get(it) == 22, MSG("Expected list iterator pointing to next node (requires 'list_it_get()'"));
    ASSERT_FALSE(list_it_next(it), MSG("Expected list iterator did not move to next"));
    ASSERT_TRUE(list_it_get(it) == 22, MSG("Expected list iterator pointing to last node (requires 'list_it_get()'"));
    ASSERT_TRUE(list_it_previous(it), MSG("Expected list iterator did move to previous"));
    ASSERT_TRUE(list_it_get(it) == 21, MSG("Expected list iterator pointing to last node (requires 'list_it_get()'"));
    ASSERT_FALSE(list_it_previous(it), MSG("Expected list iterator did not move to previous"));
    ASSERT_TRUE(list_it_get(it) == 21, MSG("Expected list iterator pointing to first node (requires 'list_it_get()'"));
    list_it_release(&it);
    list_release(&list);
}

TEST(test_list_it_previous_shall_return_false_for_invalid_it) {
    ASSERT_FALSE(list_it_previous(0), MSG("Expected invalid list iterator has no previous"));
}

TEST(test_list_it_set_get__shall_modify_value) {
    IntList list = ASSERT_RESET_AND_NEW_VALID_LIST();
    list_insert(list, 21);
    list_insert(list, 22);
    IntListIterator it = list_it_obtain(list);
    ASSERT_TRUE(list_it_is_valid(it), MSG("Expected valid list iterator"));
    ASSERT_TRUE(list_it_get(it) == 21, MSG("Expected list iterator pointing to list head"));
    ASSERT_TRUE(list_it_next(it), MSG("Expected list iterator moved to next"));
    ASSERT_TRUE(list_it_get(it) == 22, MSG("Expected list iterator pointing to next node"));
    list_it_set(it, 42);
    ASSERT_TRUE(list_it_get(it) == 42, MSG("Expected list iterator changed value"));

    ASSERT_TRUE(list_it_previous(it), MSG("Expected list iterator did move to previous"));
    ASSERT_TRUE(list_it_get(it) == 21, MSG("Expected list iterator pointing to last node"));
    ASSERT_TRUE(list_it_next(it), MSG("Expected list iterator moved to next"));
    ASSERT_TRUE(list_it_get(it) == 42, MSG("Expected list iterator pointing to next node"));
    list_it_release(&it);
    list_release(&list);
}

TEST(test_list_it_get__shall_return_0_for_invalid_iterator) {
    ASSERT_TRUE(list_it_get(0) == 0, MSG("Expected list iterator return 0 for invalid iterator"));
}

TEST(test_list_it_set__shall_ignore_call_for_invalid_iterator) {
    list_it_set(0, 53);
}