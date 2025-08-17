#include "core/defines.h"
#include "YLogger/logger.h"
#include <optional>

enum class TokenType
{
    _return,
    _int_lit,
    semi
};

struct Token
{
    TokenType type;
    std::optional<std::string> value;
};

std::vector<Token> tokenize(const std::string &str)
{
    std::vector<Token> tokens;
    std::string buff;

    for (size_t i = 0; i < str.length(); i++)
    {
        if (std::isdigit(str[i]))
        {
            // Collect all consecutive digits
            buff += str[i];

            // Continue collecting digits
            while (i + 1 < str.length() && std::isdigit(str[i + 1]))
            {
                i++;
                buff += str[i];
            }

            // Now buff contains the complete number
            tokens.push_back({.type = TokenType::_int_lit, .value = buff});
            buff.clear();
        }
        else if (std::isalpha(str[i]))
        {
            // Handle keywords similarly...
            buff += str[i];
            while (i + 1 < str.length() && std::isalpha(str[i + 1]))
            {
                i++;
                buff += str[i];
            }

            if (buff == "return")
            {
                tokens.push_back({.type = TokenType::_return});
            }
            buff.clear();
        }
        else if (str[i] == ';')
        {
            tokens.push_back({.type = TokenType::semi});
        }
        // Skip whitespace
    }

    return tokens;
}

std::string tokens_to_asm(const std::vector<Token> &tokens)
{
    std::stringstream output;

    // GCC-compatible assembly syntax
    output << ".section .text\n";
    output << ".global main\n";
    output << "\n";
    output << "main:\n";

    for (size_t i = 0; i < tokens.size(); i++)
    {
        const Token &token = tokens.at(i);
        if (token.type == TokenType::_return)
        {
            if (i + 1 < tokens.size() && tokens.at(i + 1).type == TokenType::_int_lit)
            {
                if (i + 2 < tokens.size() && tokens.at(i + 2).type == TokenType::semi)
                {
                    // Return the exit code from main
                    output << "    movl $" << tokens.at(i + 1).value.value() << ", %eax\n";
                    output << "    ret\n";
                    i += 2;
                }
            }
        }
    }

    return output.str();
}

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
        std::ifstream input(argv[1], std::ios::in);

        if (!input.is_open())
        {
            std::cerr << "Error: Could not open file " << argv[1] << std::endl;
            return EXIT_FAILURE;
        }

        std::stringstream contents_stream;
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
        input.close();
    }

    std::cout << "File contents:\n"
              << contents << std::endl;

    std::vector<Token> tokens = tokenize(contents);

    std::cout << "Tokenized " << tokens.size() << " tokens" << std::endl;

    {
        std::ofstream file("./out.s");
        if (!file.is_open())
        {
            std::cerr << "Error: Could not create output file out.s" << std::endl;
            return EXIT_FAILURE;
        }

        std::string asm_code = tokens_to_asm(tokens);
        file << asm_code;
        file.close();

        std::cout << "Assembly written to out.s" << std::endl;

        // Step 1: Assemble to object file
        std::cout << "Assembling to object file..." << std::endl;
        int result = system("gcc -c out.s -o out.o");

        if (result == 0)
        {
            std::cout << "Object file created: out.o" << std::endl;

            // Step 2: Link to executable
            std::cout << "Linking to executable..." << std::endl;
            result = system("gcc out.o -o compiled.exe");

            if (result == 0)
            {
                std::cout << "Executable created: compiled.exe" << std::endl;

                // Step 3: Run the executable
                std::cout << "Running compiled program..." << std::endl;
                result = system("compiled.exe");
                std::cout << "Program exited with code: " << result << std::endl;
            }
            else
            {
                std::cerr << "Linking failed!" << std::endl;
            }
        }
        else
        {
            std::cerr << "Assembly failed!" << std::endl;
        }
    }

    return EXIT_SUCCESS;
}