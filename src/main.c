#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>

#define MAX_LINE_LEN 64

typedef enum {
    PARSE_OK,
    PARSE_INVALID_INPUT,
    PARSE_TRAILING_CHARACTERS,
    PARSE_OUT_OF_RANGE
} ParseStatus;

ParseStatus parse_integer(const char *line, int *out_value)
{
    if (line == NULL || out_value == NULL) {
        return PARSE_INVALID_INPUT;
    }

    const char *ptr = line;
    while (*ptr != '\0' && isspace((unsigned char)*ptr)) {
        ptr++;
    }

    if (*ptr == '\0') {
        return PARSE_INVALID_INPUT;
    }

    errno = 0;
    char *end = NULL;
    long parsed_value = strtol(ptr, &end, 10);

    if (end == ptr) {
        return PARSE_INVALID_INPUT;
    }

    if (errno == ERANGE || parsed_value < INT_MIN || parsed_value > INT_MAX) {
        return PARSE_OUT_OF_RANGE;
    }

    while (*end != '\0' && isspace((unsigned char)*end)) {
        end++;
    }

    if (*end != '\0') {
        return PARSE_TRAILING_CHARACTERS;
    }

    *out_value = (int)parsed_value;
    return PARSE_OK;
}

static void run_assertions(void)
{
    int val = 0;
    assert(parse_integer("42", &val) == PARSE_OK && val == 42);
    assert(parse_integer("  -17  \n", &val) == PARSE_OK && val == -17);
    assert(parse_integer("+8", &val) == PARSE_OK && val == 8);
    assert(parse_integer("0", &val) == PARSE_OK && val == 0);
    assert(parse_integer("abc", &val) == PARSE_INVALID_INPUT);
    assert(parse_integer("", &val) == PARSE_INVALID_INPUT);
    assert(parse_integer("   \n", &val) == PARSE_INVALID_INPUT);
    assert(parse_integer("42abc", &val) == PARSE_TRAILING_CHARACTERS);
    assert(parse_integer("42 17", &val) == PARSE_TRAILING_CHARACTERS);
    assert(parse_integer("9999999999999999999999999", &val) == PARSE_OUT_OF_RANGE);
}

int main(void)
{
    run_assertions();

    char line[MAX_LINE_LEN];

    if (fgets(line, sizeof(line), stdin) == NULL) {
        if (ferror(stdin)) {
            printf("error: read failure\n");
        } else {
            printf("error: no input\n");
        }
        return 0;
    }

    char *newline_pos = strchr(line, '\n');
    if (newline_pos == NULL) {
        int ch = getchar();
        if (ch != '\n' && ch != EOF) {
            printf("error: input too long\n");
            return 0;
        }
        if (ch == EOF && ferror(stdin)) {
            printf("error: read failure\n");
            return 0;
        }
    }

    int value = 0;
    ParseStatus status = parse_integer(line, &value);

    switch (status) {
        case PARSE_OK:
            printf("value: %d\n", value);
            break;
        case PARSE_INVALID_INPUT:
            printf("error: invalid input\n");
            break;
        case PARSE_TRAILING_CHARACTERS:
            printf("error: trailing characters\n");
            break;
        case PARSE_OUT_OF_RANGE:
            printf("error: out of range\n");
            break;
    }

    return 0;
}