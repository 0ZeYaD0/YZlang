# YZLang Programming Language

![YZLang Logo](https://img.shields.io/badge/YZLang-v1.0-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows-lightgrey.svg)
![Language](https://img.shields.io/badge/language-C++-00599C.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

A modern, minimalistic programming language designed for learning compiler construction and systems programming.

## Quick Start

```bash
# Clone the repository
git clone https://github.com/0ZeYaD0/Zlang.git
cd Zlang

# Build the compiler
build.bat

# Run your first program
bin\yzlang.exe test\main.yz
```

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Installation](#installation)
- [Language Guide](#language-guide)
- [Architecture](#architecture)
- [Examples](#examples)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [License](#license)

## Overview

YZLang is a **statically-typed, compiled programming language** that prioritizes simplicity and educational value. It compiles directly to **native x86-64 assembly** through a clean, multi-stage pipeline:

```
Source Code (.yz) → Tokenizer → Parser → AST → Code Generator → Assembly → Native Executable
```

### Why YZLang?
- **Educational**: Perfect for learning compiler design and low-level programming
- **Fast**: Compiles directly to native machine code
- **Simple**: Clean syntax that's easy to understand and extend
- **Modern Architecture**: Built with modern C++ and clean design patterns

## Features

### Core Language Features
- **Variables & Assignment** - `val` declarations and variable reassignment
- **Arithmetic Operations** - Full expression support with operator precedence (`+`, `-`, `*`, `/`)
- **Integer Literals** - Numeric values and identifiers
- **Lexical Scoping** - Block statements with proper variable shadowing
- **Control Flow** - Complete conditional statements (`if`, `elif`, `else`)
- **I/O Operations** - Multi-value output statements (`out(x, y, z)`)
- **Program Control** - Exit statements with custom exit codes

### Comments & Documentation
- **Single-line comments** - `// This is a comment`
- **Multi-line comments** - `/* This spans multiple lines */`

### Compiler Features
- **Lexical Analysis** - Advanced tokenizer with keyword and operator recognition
- **AST Generation** - Recursive descent parser with precedence climbing
- **Code Generation** - Direct x86-64 assembly generation
- **Native Compilation** - Produces optimized Windows executables
- **Memory Management** - Modern arena allocator with proper alignment
- **Error Handling** - Comprehensive error reporting and validation

## Installation

### Prerequisites
- **Windows 10/11** (Current target platform)
- **GCC/MinGW-w64** (For C++ compilation and assembly)
- **Git** (For cloning the repository)

### Setup
```bash
# 1. Clone the repository
git clone https://github.com/0ZeYaD0/Zlang.git
cd Zlang

# 2. Build the compiler (Windows)
build.bat

# 3. Test your installation
bin\yzlang.exe test\main.yz
```

### Manual Build (Alternative)
```bash
# Compile the C++ source files
g++ -std=c++17 src/main.cpp src/YLogger/logger.cpp -o bin/yzlang.exe -O2

# Verify the build
bin\yzlang.exe --version
```

## Language Guide

### Basic Syntax

#### Variables
Variables are declared with `val` and can be reassigned:
```yzlang
val x = 42;           // Integer declaration
val y = x + 10;       // Expression
x = 100;              // Reassignment
```

#### Comments
YZLang supports both single-line and multi-line comments:
```yzlang
// This is a single-line comment

/* This is a multi-line comment
   that can span multiple lines
   and is very useful for documentation */

val x = 5;  // Inline comments are also supported
```

#### Arithmetic Operations
YZLang supports all basic arithmetic operations with proper precedence:
```yzlang
val a = 2 + 3 * 4;      // a = 14 (multiplication first)
val b = (2 + 3) * 4;    // b = 20 (parentheses override precedence)
val c = 10 - 4 / 2;     // c = 8  (division first)
val d = (10 - 4) / 2;   // d = 3  (parentheses first)
```

#### Block Statements & Scoping
YZLang implements lexical scoping with block statements:
```yzlang
val x = 10;
val y = 20;

{
    val x = 5;        // Shadows outer x
    val z = x + y;    // z = 25 (inner x=5, outer y=20)
    out(z);           // Prints: 25
    
    {
        val y = 1;    // Shadows outer y
        val w = x + y; // w = 6 (inner x=5, inner y=1)
        out(w);       // Prints: 6
    }
    // Inner y is now out of scope
    out(y);           // Prints: 20 (outer y)
}
// Inner x and z are now out of scope
out(x);               // Prints: 10 (outer x)
```

#### Conditional Statements
Complete conditional logic with `if`, `elif`, and `else`:
```yzlang
val score = 85;

if (score - 90) {           // if score >= 90
    out(1);                 // Grade A
} elif (score - 80) {       // elif score >= 80
    out(2);                 // Grade B
} elif (score - 70) {       // elif score >= 70
    out(3);                 // Grade C
} else {
    out(4);                 // Grade F
}
```

#### Input/Output Operations
Print multiple values to the console using the `out` statement:
```yzlang
val x = 42;
val y = 10;
out(x);                    // Prints: 42
out(x, y);                 // Prints: 42 10
out(x + y, x - y);         // Prints: 52 32
```

#### Program Termination
Control program exit with custom exit codes:
```yzlang
exit(0);        // Normal termination
exit(42);       // Exit with error code 42

val error_code = 1;
exit(error_code);          // Exit with variable value
```

## Architecture

YZLang follows a **clean, modular architecture** with distinct phases:

```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   Source    │───▶│ Tokenizer   │───▶│   Parser    │───▶│ Generator   │
│   (.yz)     │    │   (Lexer)   │    │   (AST)     │    │ (Assembly)  │
└─────────────┘    └─────────────┘    └─────────────┘    └─────────────┘
                                                                   │
                                                                   ▼
┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│ Executable  │◀───│   Linker    │◀───│ Assembler   │◀───│ Assembly    │
│   (.exe)    │    │   (GCC)     │    │   (GCC)     │    │   (.s)      │
└─────────────┘    └─────────────┘    └─────────────┘    └─────────────┘
```

### Core Components

#### 1. **Tokenizer** (`src/tokenizer.hpp`)
**Purpose**: Lexical analysis - converts source text into structured tokens.

**Features**:
- Keyword recognition (`val`, `if`, `elif`, `else`, `out`, `exit`)
- Operator tokenization (`+`, `-`, `*`, `/`, `=`, `,`)
- Delimiter handling (`(`, `)`, `{`, `}`, `;`)
- Comment processing (single-line `//` and multi-line `/* */`)
- Number and identifier parsing

#### 2. **Parser** (`src/parser.hpp`)
**Purpose**: Syntax analysis - builds an Abstract Syntax Tree (AST) from tokens.

**Features**:
- **Recursive descent parsing** with precedence climbing
- **Expression parsing** with proper operator precedence
- **Statement parsing** for variables, conditionals, and control flow
- **Scope management** for block statements
- **Error reporting** with descriptive messages

#### 3. **Code Generator** (`src/genration.hpp`)
**Purpose**: Code generation - traverses the AST to produce x86-64 assembly.

**Features**:
- **Visitor pattern** for clean AST traversal
- **Stack-based variable management** with proper scope handling
- **Register allocation** and calling conventions
- **Label management** for control flow (jumps, conditionals)
- **Windows ABI compliance** for function calls

#### 4. **Arena Allocator** (`src/core/arena.hpp`)
**Purpose**: Memory management for AST nodes.

**Features**:
- **Fast allocation** with minimal overhead
- **Memory alignment** for optimal performance
- **RAII compliance** with proper cleanup
- **Template-based** with type safety

## Examples

### Getting Started

#### Hello World
```yzlang
/* My first YZLang program */
out(42);    // Prints: 42
exit(0);    // Exit successfully
```

#### Basic Arithmetic
```yzlang
val x = (10 + 2) * 2;  // x = 24
val y = x / 3;         // y = 8
out(x, y);             // Prints: 24 8
exit(0);
```

### Control Flow Examples

#### Conditional Logic
```yzlang
val grade = 85;

if (grade - 90) {       // A grade (90+)
    out(4);
} elif (grade - 80) {   // B grade (80-89)
    out(3);             // This will execute
} elif (grade - 70) {   // C grade (70-79)
    out(2);
} else {                // F grade (below 70)
    out(1);
}

exit(0);
```
**Output**: `3`

#### Variable Assignment
```yzlang
val x = 10;
val y = 20;

if (x - 5) {
    x = 100;
    y = x + 50;
} else {
    x = 0;
    y = 0;
}

out(x, y);  // Prints: 100 150
exit(0);
```

### Scoping Examples

#### Variable Shadowing
```yzlang
val x = 10;             // Outer scope
val y = 20;

{
    val x = 5;          // Shadows outer x
    out(x, y);          // Prints: 5 20
    
    {
        val y = 1;      // Shadows outer y
        out(x, y);      // Prints: 5 1
    }
    
    out(x, y);          // Prints: 5 20 (outer y restored)
}

out(x, y);              // Prints: 10 20 (outer x restored)
exit(0);
```

### Advanced Arithmetic

#### Calculator Example
```yzlang
/* Simple calculator demonstration */
val num1 = 15;
val num2 = 3;

val addition = num1 + num2;        // 18
val subtraction = num1 - num2;     // 12
val multiplication = num1 * num2;   // 45
val division = num1 / num2;        // 5

out(addition, subtraction, multiplication, division);
// Prints: 18 12 45 5

exit(0);
```

## Roadmap

### Phase 1-3: Foundation (Completed)
- [x] **Lexical Analysis** - Complete tokenizer with keyword and operator support
- [x] **Syntax Analysis** - Recursive descent parser with AST generation
- [x] **Code Generation** - x86-64 assembly generation with GCC compatibility
- [x] **Variables** - `val` declarations with lexical scoping and assignment
- [x] **Arithmetic** - Full expression support with operator precedence
- [x] **Control Flow** - Complete conditional statements (`if`/`elif`/`else`)
- [x] **I/O Operations** - Multi-value output statements and program termination
- [x] **Comments** - Single-line and multi-line comment support
- [x] **Memory Management** - Modern arena allocator with alignment

### Phase 4: Language Enhancement (In Progress)
- [x] **Variable Assignment** - Reassignment of existing variables
- [x] **Enhanced I/O** - Multiple argument output statements
- [x] **Arithmetic Fixes** - Correct operator evaluation order
- [ ] **Boolean Operations** - Logical operators (`&&`, `||`, `!`)
- [ ] **Comparison Operators** - Relational operators (`==`, `!=`, `<`, `>`)
- [ ] **Error Handling** - Improved error messages and recovery

### Phase 5: Advanced Features (Planned)
- [ ] **Loop Constructs** - `while` loops with condition checking
- [ ] **Functions** - Function definitions with parameters and return values
- [ ] **Boolean Type** - `true`/`false` literals and boolean expressions
- [ ] **String Literals** - Basic string support and operations

### Future Enhancements
- [ ] **Cross-Platform Support** - Linux and macOS targets
- [ ] **Standard Library** - Built-in functions and utilities
- [ ] **Developer Tools** - Syntax highlighting and debugging support

## Usage

### Compilation & Execution
```bash
# Compile and run a YZLang program
bin\yzlang.exe your_program.yz

# The compiler automatically:
# 1. Parses your source code
# 2. Generates assembly (out.s)
# 3. Assembles to object code (out.o)
# 4. Links to executable (out.exe)
# 5. Runs the program and reports exit code
```

### File Structure
```
your_project/
├── main.yz          # Your main program file
├── out.s            # Generated assembly (temporary)
├── out.o            # Object file (temporary)
└── out.exe          # Final executable
```

## Contributing

We welcome contributions! Here's how you can help:

### Bug Reports
- Use the [GitHub Issues](https://github.com/0ZeYaD0/Zlang/issues) to report bugs
- Include sample code that reproduces the issue
- Specify your Windows version and GCC version

### Feature Requests
- Check the [roadmap](#roadmap) for planned features
- Open an issue with the `enhancement` label
- Describe the use case and proposed syntax

### Development Setup
```bash
# Fork and clone the repository
git clone https://github.com/yourusername/Zlang.git
cd Zlang

# Build in debug mode
g++ -std=c++17 -g -DDEBUG src/main.cpp src/YLogger/logger.cpp -o bin/yzlang-debug.exe

# Run tests
bin\yzlang-debug.exe test\main.yz
```

### Code Style
- Follow existing C++17 conventions
- Use meaningful variable names
- Add comments for complex algorithms
- Write tests for new features

## License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

## Authors & Acknowledgments

- **[Zeyad Emad](https://github.com/0ZeYaD0)** - *Creator & Lead Developer*
- **[Yassin Ibrahim](https://github.com/DeltaY0)** - *Contributor & Architecture*

### Special Thanks
- The C++ community for excellent resources on compiler design
- GCC/MinGW teams for the excellent toolchain
- Everyone who tests YZLang and provides feedback

---

<div align="center">

**YZLang** - *Simple. Fast. Educational.*

[![GitHub Stars](https://img.shields.io/github/stars/0ZeYaD0/Zlang?style=social)](https://github.com/0ZeYaD0/Zlang/stargazers)
[![GitHub Forks](https://img.shields.io/github/forks/0ZeYaD0/Zlang?style=social)](https://github.com/0ZeYaD0/Zlang/network/members)

Made with care for the programming language community

</div>
