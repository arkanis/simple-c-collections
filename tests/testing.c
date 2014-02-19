#define _GNU_SOURCE

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <setjmp.h>
#include <execinfo.h>

#include "testing.h"


typedef struct report_item report_item_t, *report_item_p;
struct report_item {
	char *msg;
	report_item_p next;
};


jmp_buf runner_context;
size_t tests_passed = 0, tests_failed = 0;
size_t checks_passed = 0, checks_failed = 0;
report_item_p report_items = NULL;


/**
 * Runs a test function. This test function has to be a void function with no arguments:
 * 
 * 	
 * 	void foo_test(){
 * 		…
 * 	}
 * 	
 * 	int main(){
 * 		run(foo_test);
 * 		return show_report();
 * 	}
 * 
 * This function uses setjmp and longjmp to abort the execution in such a test function as soon as the first
 * check fails.
 */
bool run(test_func_t test){
	if ( setjmp(runner_context) == 0 ) {
		// Jump target saved, run the test.
		test();
		tests_passed++;
		fprintf(stderr, ".");
		fflush(stderr);
		return true;
	} else {
		// Returned from a jump via longjmp. That means the test failed. The
		// `message` variable contains the failure description.
		tests_failed++;
		fprintf(stderr, "F");
		fflush(stderr);
		return false;
	}
}


/**
 * Please use the `check` macro instead of this function. The macro automatically inserts some
 * reporting stuff into the argument list (file, line, function name, etc.). `assert` is the established
 * name for this but is already taken by the `assert()` function of C. Therefore `check()` is used
 * instead.
 * 
 * 	check(1 == 0, "it exploded");
 * 
 * The second argument can be used like the `format` argument of `printf()`. You can also specify
 * `NULL` if you don't want to output a fail message.
 */
bool check_func(bool expr, const char *file, const int line, const char *func_name, const char *code, const char *message, ...){
	if (expr) {
		checks_passed++;
		return true;
	} else {
		checks_failed++;
		
		// Allocate a new report item for the message and insert it at the end of the linked list
		report_item_p item = malloc(sizeof(report_item_t));
		memset(item, 0, sizeof(report_item_t));
		if (report_items == NULL) {
			report_items = item;
		} else {
			report_item_p curr = report_items;
			while(curr->next != NULL)
				curr = curr->next;
			curr->next = item;
		}
		
		// Create the message
		size_t msg_buffer_size = 1024, msg_buffer_filled = 0;
		item->msg = malloc(msg_buffer_size);
		msg_buffer_filled += snprintf(item->msg + msg_buffer_filled, msg_buffer_size - msg_buffer_filled, "- %s", code);
		
		if (message != NULL){
			msg_buffer_filled += snprintf(item->msg + msg_buffer_filled, msg_buffer_size - msg_buffer_filled, "\n  ");
			va_list args;
			va_start(args, message);
			msg_buffer_filled += vsnprintf(item->msg + msg_buffer_filled, msg_buffer_size - msg_buffer_filled, message, args);
			va_end(args);
		}
		
		// Create a backtrace, our arguments are used for the first entry. This is more accurate
		// than the runtime stuff. The first two entries of the runtime backtrace are skipped because
		// the first is the check_func itself and the second the place of the check macro we got via
		// out own arguments.
		msg_buffer_filled += snprintf(item->msg + msg_buffer_filled, msg_buffer_size - msg_buffer_filled, "\n  %s in %s:%d\n", func_name, file, line);
		
		char buffer[1024];
		snprintf(buffer, 1024, "/proc/%d/exe", getpid());
		
		char exe_name[1024];
		ssize_t exe_name_length = readlink(buffer, exe_name, 1024);
		
		void* return_addresses[4];
		size_t return_address_count = backtrace(return_addresses, 4);
		
		for(size_t i = 2; i < return_address_count; i++){
			snprintf(buffer, 1024, "addr2line --exe %.*s %p", (int)exe_name_length, exe_name, return_addresses[i]);
			FILE* proc = popen(buffer, "r");
			msg_buffer_filled += snprintf(item->msg + msg_buffer_filled, msg_buffer_size - msg_buffer_filled, "  %p: ", return_addresses[i]);
			msg_buffer_filled += fread(item->msg + msg_buffer_filled, 1, msg_buffer_size - msg_buffer_filled, proc);
			pclose(proc);
		}
		
		longjmp(runner_context, 1);
		return false;
	}
}

/*
bool check_str_func(const char *actual, const char *expected, const char *file, const int line, const char *func_name, const char *code){
	return check_func(strcmp(actual, expected) == 0, file, line, func_name, code, "string check failed, got \"%s\", expected \"%s\"", actual, expected);
}

bool check_int_func(int actual, int expected, const char *file, const int line, const char *func_name, const char *code){
	return check_func(actual == expected, file, line, func_name, code, "integer check failed, got %d, expected %d", actual, expected);
}
*/


/**
 * Shows all the report messages of the tests and prints statistics. Returns the number of failed tests.
 */
size_t show_report(){
	fprintf(stderr, "\n");
	
	for(report_item_p item = report_items; item != NULL; item = item->next)
		fprintf(stderr, "%s\n", item->msg);
	
	fprintf(stderr, "\x1b[%sm", (tests_failed > 0) ? "31" : "32");
	fprintf(stderr, "%zu tests failed, %zu tests passed, %zu checks passed\n", tests_failed, tests_passed, checks_passed);
	fprintf(stderr, "\x1b[0m");
	return tests_failed;
}


/*
bool check_atom(atom_t actual_atom, atom_t expected_atom){
	check(actual_atom->type == expected_atom->type, );
}
*/