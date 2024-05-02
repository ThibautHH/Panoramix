/*
** EPITECH PROJECT, 2024
** Panoramix
** File description:
** Program entrypoint
*/

#include <ctype.h>

#include "panoramix.h"

static const char USAGE[] = "USAGE: ./panoramix <nb_villagers> <pot_size>"
" <nb_fights> <nb_refills>";

static _Bool is_whitespace(const char *str)
{
    for (; *str; str++)
        if (!isspace(*str))
            return 0;
    return 1;
}

static signed char parse_args(long *numbers, const char *const *argv)
{
    unsigned char i = 0;
    char *endptr;

    for (; i < 4 && argv[i]; i++) {
        numbers[i] = strtol(argv[i], &endptr, 0);
        if (endptr == argv[i] || !is_whitespace(endptr))
            return 0;
        if (numbers[i] <= 0)
            return -1;
        if (errno == ERANGE) {
            perror("strtol");
            exit(84);
        }
    }
    return (i == 4) ? 1 : 0;
}

int main(int argc, const char *const *argv)
{
    long numbers[4];
    char result;

    if (argc == 1) {
        fprintf(stderr, "%s\n", USAGE);
        return 84;
    }
    result = parse_args(numbers, argv + 1);
    if (result < 0)
        fprintf(stderr, "%s\n%s\n", USAGE, "Values must be >0.");
    else if (!result)
        fprintf(stderr, "%s\n", USAGE);
    else
        return panoramix(numbers[0], numbers[1], numbers[2], numbers[3]);
    return 84;
}
