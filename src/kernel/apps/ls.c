#include <k/syscalls.h>
#include <stdio.h>

int lsMain(int argc, char **argv)
{
    const char *dir = argc == 1 ? "." : argv[1];

    // ls
    size_t n;
    char **children = ls(dir, &n);

    if (children == NULL)
    {
        fprintf(stderr, "\n Cannot access \"%s\": No such file or directory\n\n", dir);
        return 1;
    }

    for (size_t i = 0; i < n; i++)
    {
        printf("%s ", children[i]);
        free(children[i]);
    }

    free(children);

    puts("");

    return 0;
}
