#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define check(expr)                             check_func( (expr), \
                                                    __FILE__, __LINE__, __func__, \
                                                    #expr, NULL)
#define check_msg(expr, ...)                    check_func( (expr), \
                                                    __FILE__, __LINE__, __func__, \
                                                    #expr, __VA_ARGS__)
#define check_str(actual, expected)             check_func( strcmp((actual), (expected)) == 0, \
                                                    __FILE__, __LINE__, __func__, \
                                                    #actual " == " #expected, "got \"%s\", expected \"%s\"", actual, expected)
#define check_int(actual, expected)             check_func( (actual) == (expected), \
                                                    __FILE__, __LINE__, __func__, \
                                                    #actual " == " #expected, "got %d, expected %d", actual, expected)
#define check_float(actual, expected, epsilon)  check_func( fabs((actual) - (expected)) < (epsilon), \
                                                    __FILE__, __LINE__, __func__, \
                                                    #actual " == " #expected, "got %f, expected %f", actual, expected)
#define check_not_null(actual)                  check_func( (actual) != NULL, \
                                                    __FILE__, __LINE__, __func__, \
                                                    #actual " != NULL", "got NULL")
#define check_null(actual)                      check_func( (actual) == NULL, \
                                                    __FILE__, __LINE__, __func__, \
                                                    #actual " == NULL", "got %p", actual)

typedef void (*test_func_t)();

bool   run(test_func_t test);
bool   check_func(bool expr, const char *file, const int line, const char *func_name, const char *code, const char *message, ...);
size_t show_report();