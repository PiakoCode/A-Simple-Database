//
// Created by Piako on 23-10-19.
//

#ifndef BUILD_MY_OWN_SQLITE_INPUTBUFFER_H
#define BUILD_MY_OWN_SQLITE_INPUTBUFFER_H

#endif //BUILD_MY_OWN_SQLITE_INPUTBUFFER_H
#include <stdlib.h>

typedef struct {
    char *buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;


InputBuffer *new_input_buffer();

void read_input(InputBuffer *);

void close_input_buffer();