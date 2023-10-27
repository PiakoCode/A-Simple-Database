#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "inputBuffer.h"
#include "row.h"

// 执行结果
typedef enum
{
    EXECUTE_SUCCESS, // 执行成功
    EXECUTE_TABLE_FULL
} ExecuteResult;

// meta命令解析结果
typedef enum
{
    META_COMMAND_SUCCESS,             // 可识别
    META_COMMAND_UNRECOGNIZED_COMMAND // 无法识别
} MetaCommandResult;

// PREPARE 结果
typedef enum
{
    PREPARE_SUCCESS,                // 成功
    PREPARE_UNRECOGNIZED_STATEMENT, // 无法识别
    PREPARE_SYNTAX_ERROR,           // 语法错误
    PREPARE_STRING_TOO_LONG,        // 字符串过长
    PREPARE_NEGATIVE_ID,            // ID为负数
} PrepareResult;

// 语句类型
typedef enum
{
    STATEMENT_INSERT, // 插入
    STATEMENT_SELECT
} StatementType;

typedef struct
{
    StatementType type;
    Row row_to_insert;
} Statement;

PrepareResult prepare_insert(InputBuffer *input_buffer, Statement *statement)
{
    statement->type = STATEMENT_INSERT;

    char *keyword = strtok(input_buffer->buffer, " ");
    char *id_string = strtok(NULL, " ");
    char *username = strtok(NULL, " ");
    char *email = strtok(NULL, " ");

    if (id_string == NULL || username == NULL || email == NULL)
    {
        return PREPARE_SYNTAX_ERROR;
    }

    int id = atoi(id_string);
    if (id < 0)
    {
        return PREPARE_NEGATIVE_ID;
    }
    if (strlen(username) > COLUMN_USERNAME_SIZE)
    {
        return PREPARE_STRING_TOO_LONG;
    }
    if (strlen(email) > COLUMN_EMAIL_SIZE)
    {
        return PREPARE_STRING_TOO_LONG;
    }

    statement->row_to_insert.id = id;
    strcpy(statement->row_to_insert.username, username);
    strcpy(statement->row_to_insert.email, email);

    return PREPARE_SUCCESS;
}

// 解析meta命令
MetaCommandResult do_meta_command(InputBuffer *inputBuffer, Table *table)
{
    if (strcmp(inputBuffer->buffer, ".exit") == 0)
    {
        db_close(table);
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(inputBuffer->buffer, ".btree") == 0)
    {
        printf("Tree:\n");
        print_leaf_node(get_page(table->pager, 0));
        return META_COMMAND_SUCCESS;
    }
    else if (strcmp(inputBuffer->buffer, ".constants") == 0)
    {
        printf("Constants:\n");
        print_constants();
        return META_COMMAND_SUCCESS;
    }
    else
    {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    };
}

// 解析语句
PrepareResult prepare_statement(InputBuffer *input_buffer, Statement *statement)
{
    if (strncmp(input_buffer->buffer, "insert", 6) == 0)
    {
        return prepare_insert(input_buffer, statement);
    }
    if (strcmp(input_buffer->buffer, "select") == 0)
    {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult execute_insert(Statement *statement, Table *table)
{
    void *node = get_page(table->pager, table->root_page_num);
    if ((*leaf_node_num_cells(node) >= LEAF_NODE_MAX_CELLS))
    {
        return EXECUTE_TABLE_FULL;
    }

    Row *row_to_insert = &(statement->row_to_insert);
    Cursor *cursor = table_end(table);

    leaf_node_insert(cursor, row_to_insert->id, row_to_insert);

    free(cursor);

    return EXIT_SUCCESS;
}

ExecuteResult execute_select(Statement *statement, Table *table)
{
    Cursor *cursor = table_start(table);

    Row row;
    while (!(cursor->end_of_table)) // 遍历table
    {
        deserialize_row(cursor_value(cursor), &row); // 解析数据
        print_row(&row);
        cursor_advance(cursor);
    }

    free(cursor);

    return EXIT_SUCCESS;
}
// 执行语句
ExecuteResult execute_statement(Statement *statement, Table *table)
{
    switch (statement->type)
    {
    case (STATEMENT_INSERT):
        return execute_insert(statement, table);
    case (STATEMENT_SELECT):
        return execute_select(statement, table);
    }
}

// 打印 prompt
void print_prompt() { printf("db > "); }

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Must supply a database filename.\n");
        exit(EXIT_FAILURE);
    }

    char *filename = argv[1];
    Table *table = db_open(filename);

    InputBuffer *input_buffer = new_input_buffer();

    while (1)
    {
        print_prompt();
        read_input(input_buffer);

        if (input_buffer->buffer[0] == '.')
        {
            switch (do_meta_command(input_buffer, table))
            {
            case (META_COMMAND_SUCCESS):
                continue;
            case (META_COMMAND_UNRECOGNIZED_COMMAND):
                printf("Unrecognized command '%s' .\n", input_buffer->buffer);
                continue;
            }
        }
        Statement statement;
        switch (prepare_statement(input_buffer, &statement))
        {
        case (PREPARE_SUCCESS):
            break;
        case (PREPARE_NEGATIVE_ID):
            printf("ID must be positive.\n");
            continue;
        case (PREPARE_STRING_TOO_LONG):
            printf("String is too long.\n");
            continue;
        case (PREPARE_SYNTAX_ERROR):
            printf("Syntax error. Could not parse statement.\n");
            continue;
        case (PREPARE_UNRECOGNIZED_STATEMENT):
            printf("Unrecognized keyword at start of '%s' .\n", input_buffer->buffer);
            continue;
        }

        switch (execute_statement(&statement, table))
        {
        case (EXECUTE_SUCCESS):
            printf("Executed.\n");
            break;
        case (EXECUTE_TABLE_FULL):
            printf("Error: Table full.\n");
            break;
        }
    }

    return 0;
}

/*
command:

insert 1 user1 foo@bar.com

select

.exit

*/