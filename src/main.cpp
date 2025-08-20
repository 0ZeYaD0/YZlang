#include "core/defines.h"
#include "YLogger/logger.h"
#include "Tokenize.hpp"
#include "parser.hpp"
#include "genration.hpp"
#include "arena.hpp"

i32 main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Incorrect usage. Correct usage is..." << std::endl;
        std::cerr << "yz <filename.yz>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string contents;
    {
        std::ifstream input(argv[1]);
        if (!input.is_open())
        {
            std::cerr << "Could not open file: " << argv[1] << std::endl;
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

    int exit_code = system("out.exe");
    std::cout << "Exit code: " << exit_code << std::endl;

    return EXIT_SUCCESS;
}