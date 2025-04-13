#include "lexer.h"

int main(void)
{
    printf("TEst");
    const char *file_path = "D:\\Codes\\Zlang\\test\\main.zl";

    FILE *file = fopen(file_path, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    printf("Processing file: %s\n", file_path);

    lexer(file);

    fclose(file);
    printf("File processing completed.\n");

    return 0;
}