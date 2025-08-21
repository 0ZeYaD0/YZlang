#include "core/defines.h"
#include "YLogger/logger.h"
#include "tokenizer.hpp"
#include "parser.hpp"
#include "genration.hpp"

i32 main(int argc, char *argv[])
{
    if (argc != 2)
    {
        LLOG(RED_TEXT("Incorrect usage."), " Correct usage is...\n");
        LLOG("yz <filename.yz>\n");
        return EXIT_FAILURE;
    }

    std::string contents;
    {
        std::ifstream input(argv[1]);
        if (!input.is_open())
        {
            LLOG(RED_TEXT("Could not open file: "), argv[1], "\n");
            return EXIT_FAILURE;
        }
        std::stringstream contents_stream;

        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }

    Tokenizer tokenizer(contents);
    std::vector<Token> tokens = tokenizer.tokenize();

    Parser parser(std::move(tokens));
    std::optional<NodeProg> tree = parser.parse_prog();

    Generator genrator(tree.value());

    {
        std::ofstream file("out.s");
        file << genrator.genrate();
    }

    system("gcc -c out.s -o out.o");
    system("gcc out.o -o out.exe");

    int exit_code = system("out.exe && echo $?");
    LLOG(BLUE_TEXT("Exit Code: "), exit_code, "\n");

    return EXIT_SUCCESS;
}