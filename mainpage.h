/**
* @mainpage
* @section intro Introduction
*
* The implementation of a "Doubly Linked List with List Iterator".
* 
* @section objective Assignment Objective
* In this assignment an Abstract Data Type of a "Doubly Linked List" shall be implemented against 
* the defined common interface of a list. This interface is declared within file 'list.h'. In addition
* shall support a list iterator with the interface declared within file 'list_iterator.h'. It is intended
* to be used by applications or other libraries and aims to abstract the 'list' functionality as far as possible.
*
* The 'Doubly Linked List' shall have the following members of the list data:
* - the pointer to the list head node 
* - the pointer to the list tail node 
* - the size of the list
*
* The list iterator shall be implemented in the same .c file as the list.
*
* **Tip:** Feel free to reuse **YOUR** Simple Singly Linked List implementation:
*          Copy the functions from simple_singly_linked_list.c into doubly_linked_list.c 
           and modify the data and functions accordingly. Don't miss the iterator functions.
* 
* **Configuration**
*
* There is no configuration required.
*
* @section assignment Assignment
* The 'Doubly Linked List' shall be implemented in file 'doubly_linked_list_with_iterator.c'. The corresponding header file
* includes the common interface 'list.h' and 'list_iterator.h' but does not add functions by its own.
*
* The library 'allocator' is provided for memory allocation functionality that integrates with unit testing. Behind the facade,
* allocats memory dynamically similar to `malloc`. Allocated memory must be freed when it is not used anymore. The 'allocator'
* library must be used for memory alloction operations, otherwise unit tests will fail.
* 
*
* -# Implement 'doubly_linked_list_with_iterator.c' against interface indirectly declared in 'list.h' AND 'list.iterator.h':
*    Make the program and tests compile: Implement all functions in all relevant files declared in the headers 
*    EMTPY (return nothing, 0, false, ... as required).
*   - All unit tests shall run but FAIL after this step
*   - **--COMMIT--**
* -# Implement the empty functions one by one to make the unit tests pass one by one.
*   - The purpose of a function is specified as API documentation within the header files.
*   - Obey comments in source files. Run the unit tests frequently and fix failures.
*   - **--COMMIT-- after each implemented function.**
*
* Main driver is not required in this assignment.
*
* @section notes Notes
* -# make cleantest: This new make target for clearing the console, building, and running unit test is available.
* -# Sometimes changes are not properly detected by incremental builds. If something very strange 
*    happens during compilation, try to run `make clean` followed by `make` to start a clean build.
*    This approach is also recommended after everthing is done, because some compiler warning appears
*    only in clean builds. 
*/