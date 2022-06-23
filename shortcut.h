/*----------------------------------------------------------
 *				HTBLA-Leonding
 * ---------------------------------------------------------
 * Title:			shortcut
 * Author:			P. Bauer
 * Date:			November 03, 2010
 * ----------------------------------------------------------
 * Description:
 * A simple unit testing frame work for C.
 * ----------------------------------------------------------
 */
#ifndef ___SHORTCUT_H
#define ___SHORTCUT_H

#include <stdbool.h>

/** TestCase is the struct to define one test case. A test case can
*** be added to a test. If the test is run all added test cases are
*** run and the result of the run of each test case is checked automatically.
*/
struct TestCase {
	const char *name;
	/** true if the test passed, false otherwise. */
	bool success;

	/** The test function which is executed by the test framework. */
	void (*test_function)(struct TestCase *tc);
};

/**
*** @return Version of shortcut as string
***/
const char* version();

/**
*** @return The fromated string as generated using sprintf(format, ...)
***/
char* format_msg(char* format, ...);

/** assert_true checks, whether a boolean expression passed is true or false.
*** in case it is false the test case stating the assertion is marked
*** as failed and msg is printed.
*** @param bool_expr Expression which is evaluated.
*** @param tc Pointer to the test case which states this assertion.
*** @param msg Message to be printed if assertion evaluates to false.
*** @param file File in which the assert is given.
*** @param line Line in which the assert is given.
*/
void assert_true(bool bool_expr, struct TestCase *tc, const char *msg,
									const char* file, int line);

/** assert_false does the same as assert() but the boolean expression
*** has to evaluate to false. If it evaluates to true the assertion
*** fails.
*** @see assert
*/
void assert_false(bool bool_expr, struct TestCase* tc, const char* msg,
									const char* file, int line);

/** assert_equals checks whether two values are equal. Currently the following
*** data formats are supported:
*** - strings
*** - integer
*** @param expected The expected string value
*** @param actual The actual string value
*** @param tc Pointer to the test case which states this assertion.
*** @param msg Message to be printed if assertion evaluates to false.
*** @param file File in which the assert is given.
*** @param line Line in which the assert is given.
*** @see assert
*/
void assert_equals(int expected, int actual, struct TestCase* tc,
										const char* msg, const char* file, int line);
void assert_equals_str(const char* expected, char* actual, struct TestCase* tc,
										const char* msg, const char* file, int line);
void assert_equals_f(double expected, double actual, double tolerance, struct TestCase* tc,
										const char* msg, const char* file, int line);

/** @return The total number of test cases added to the test.
*/
int get_test_count();

/** add_test creates a new test case and adds the a test function to
*** this test case.
*** @param test_function Pointer to the test function to be added
*** to the newly created test case.
*** @param test_case_name Name which should be assigned to the newly
*** created test case.
*/
bool add_test(void (*test_function)(struct TestCase *tc), const char *test_case_name);

void run_tests();

#define TEST(testname) void testname(struct TestCase *tc)

#define MSG(format, ...) format_msg(format, ##__VA_ARGS__)

#define ASSERT_TRUE(condition, msg) assert_true(condition, tc, msg, __FILE__, __LINE__)

#define ASSERT_FALSE(condition, msg) assert_false(condition, tc, msg, __FILE__, __LINE__)

#define ASSERT_EQUALS(expected, actual) assert_equals(expected, actual, tc, "", __FILE__, __LINE__)

#define ASSERT_EQUALS_STR(expected, actual) assert_equals_str(expected, actual, tc, "", __FILE__, __LINE__)

#define ASSERT_EQUALS_TOLERANCE(expected, actual, tolerance) assert_equals_f(expected, actual, tolerance, tc, "", __FILE__, __LINE__)

#define ADD_TEST(testfunction) add_test(testfunction, #testfunction)

#endif
