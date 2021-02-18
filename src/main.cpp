#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>

#include <util.hpp>
#include <token.hpp>

// void parseBlock(std::vector<Token> tokens, int i, Blk * root) {
    // if (tokens[i].getType() == T_SPECIAL && tokens[i].getValue() == "<EOF>") {
    //     return;
    // } else if (tokens[i].getType() == T_IDENTIFIER) {
    //     root->nodes.emplace_back(tokens[i].getValue(), root);
    //     i++;
    //     if (tokens[i].getType() == T_SPECIAL && tokens[i].getValue() == ":") {
    //         i++;
    //         if (tokens[i].getType() == T_STRING) {
    //             root->nodes[root->nodes.size() - 1].value = tokens[i].getValue();
    //             i++;
    //         } else {
    //             throwError("Unexpected", "Expected String, got " + tokens[i].toString());
    //         }
    //     } else {
    //         throwError("Unexpected", "Expected ':', got " + tokens[i].toString());
    //     }
    // } else {
    //     throwError("Unexpected", "Expected Identifier or <EOF>, got " + tokens[i].toString());
    // }
    // parseBlock(tokens, i, root);
// }

// Block
// Build rule
// Command
// Set
// Value

// class Set {
//     public:
//         std::string name;
//         std::vector<std::string> values;
// };
//
// class Value {
//     public:
//         std::string name;
//         std::string value;
// };
//
// class BuildRule {
//     public:
//         std::string name;
//         std::string compiler, input, output;
//         std::vector<BuildRule> build_rules;
//         std::vector<Set> sets;
//         std::vector<Value> Values;
// };
//
// class Command {
//     public:
//         std::string name;
//         Set arguments;
// };
//
// class Block {
//     public:
//         std::string name;
//         std::vector<BuildRule> build_rules;
//         std::vector<Command> commands;
// };

enum BlockType {
    B_BLOCK,
    B_BUILDRULE,
    B_COMMAND,
    B_ROOT,
    B_SET,
    B_VALUE
};

class Block {
    public:
        std::string name;
        std::string value;
        BlockType type;
        Block * parent;
        std::vector<Block> blocks;

        Block(BlockType, Block *);
        Block(BlockType, std::string, Block *);
        Block(BlockType, Block *, std::string);
        Block(BlockType, std::string, Block *, std::string);
};

Block::Block(BlockType type, Block * parent) : type(type), parent(parent) {
}

Block::Block(BlockType type, std::string name, Block * parent) : type(type), name(name), parent(parent) {
}

Block::Block(BlockType type, Block * parent, std::string value) : type(type), parent(parent), value(value) {
}

Block::Block(BlockType type, std::string name, Block * parent, std::string value) : type(type), name(name), parent(parent), value(value) {
}

int main(int argc, char * argv[]) {
    Tokenizer tokenizer("quickmake.txt", "{}()@:");
    auto tokens = tokenizer.makeTokens();

    for (auto & i : tokens)
        std::cout << i.toString() << " ";
    std::cout << "\n";

    Block root(B_ROOT, nullptr);

    return 0;
}
