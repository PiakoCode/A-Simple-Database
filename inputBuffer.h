#include <stdlib.h>

typedef struct
{
    char *buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

InputBuffer *new_input_buffer();

// 读取输入
void read_input(InputBuffer *);

void close_input_buffer();