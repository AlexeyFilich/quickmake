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
//     if (tokens[i].getType() == T_SPECIAL && tokens[i].getValue() == "<EOF>") {
//         return;
//     } else if (tokens[i].getType() == T_IDENTIFIER) {
//         root->nodes.emplace_back(tokens[i].getValue(), root);
//         i++;
//         if (tokens[i].getType() == T_SPECIAL && tokens[i].getValue() == ":") {
//             i++;
//             if (tokens[i].getType() == T_STRING) {
//                 root->nodes[root->nodes.size() - 1].value = tokens[i].getValue();
//                 i++;
//             } else {
//                 throwError("Unexpected", "Expected String, got " + tokens[i].toString());
//             }
//         } else {
//             throwError("Unexpected", "Expected ':', got " + tokens[i].toString());
//         }
//     } else {
//         throwError("Unexpected", "Expected Identifier or <EOF>, got " + tokens[i].toString());
//     }
//     parseBlock(tokens, i, root);
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

        void print(int);
};

Block::Block(BlockType type, Block * parent) : type(type), parent(parent) {
}

Block::Block(BlockType type, std::string name, Block * parent) : type(type), name(name), parent(parent) {
}

Block::Block(BlockType type, Block * parent, std::string value) : type(type), parent(parent), value(value) {
}

Block::Block(BlockType type, std::string name, Block * parent, std::string value) : type(type), name(name), parent(parent), value(value) {
}

void Block::print(int x) {
    for (int i = 0; i < x; i++) std::cout << " ";
    switch (this->type) {
    case B_BLOCK: std::cout << "[block:"; break;
    case B_BUILDRULE: std::cout << "[buildrule:"; break;
    case B_COMMAND: std::cout << "[command:"; break;
    case B_ROOT: std::cout << "[root:"; break;
    case B_SET: std::cout << "[set:"; break;
    case B_VALUE: std::cout << "[value:"; break;
    default: std::cout << "[error-type:"; break;
    }
    std::cout << this->name << ":'" << this->value << "':\n";
    x++;
    for (int i = 0; i < x; i++) std::cout << " ";
    std::cout << "{\n";
    x++;
    for (int i = 0; i < this->blocks.size(); i++) {
        this->blocks[i].print(x);
        if (i < this->blocks.size() - 1)
            std::cout << ",\n";
    }
    x--;
    for (int i = 0; i < x; i++) std::cout << " ";
    std::cout << "}";
    std::cout << "]\n";
}

//////////
//////////
//////////

void parse(std::vector<Token> tokens, int i, Block * root) {
    if (tokens.size() == 0) return;

    int idx = -1;
    Token current = tokens[0];
    Block * scope = root;

    auto next = [&]() {
        idx++;
        if (idx >= 0 && idx < tokens.size())
            current = tokens[idx];
    };

    next();

    while (!current.compare(T_SPECIAL, "<EOF>")) {
        if (current.compare(T_SPECIAL, "@")) {
            next();
            if (current.compare(T_IDENTIFIER)) {
                root->blocks.emplace_back(B_BLOCK, current.getValue(), root);
                scope = &root->blocks[root->blocks.size() - 1];
                next();
            } else {
                throwError(current.getPosition(), "UnexpectedToken", MakeString() << "Expected identifier, but got '" << current.getValue() << "'");
            }
        } else if (current.compare(T_IDENTIFIER)) {
            std::string name = current.getValue();
            next();
            if (current.compare(T_IDENTIFIER) || current.compare(T_SPECIAL, "}")) {
                scope->blocks.emplace_back(B_BUILDRULE, name, scope);
            } else if (current.compare(T_SPECIAL, ":")) {
                next();
                if (current.compare(T_STRING)) {
                    scope->blocks.emplace_back(B_VALUE, name, scope, current.getValue());
                    next();
                } else {
                    throwError(current.getPosition(), "UnexpectedToken", MakeString() << "Expected identifier, but got '" << current.getValue() << "'");
                }
            } else if (current.compare(T_SPECIAL, "{")) {
                next();
                scope->blocks.emplace_back(B_SET, name, scope);
                scope = &scope->blocks[scope->blocks.size() - 1];
            } else if (current.compare(T_SPECIAL, "(")) {
                next();
                scope->blocks.emplace_back(B_COMMAND, name, scope);
                scope = &scope->blocks[scope->blocks.size() - 1];
            } else {
                throwError(current.getPosition(), "UnexpectedToken", MakeString() << "Expected identifier, ':', '(', '{', but got '" << current.getValue() << "'");
            }
        } else if (current.compare(T_STRING)) {
            if (scope->type == B_SET || scope->type == B_COMMAND) {
                scope->blocks.emplace_back(B_VALUE, scope, current.getValue());
                next();
            } else {
                throwError(current.getPosition(), "UnexpectedToken", MakeString() << "Standalone string only can exist in set or command");
            }
        } else if (current.compare(T_SPECIAL, "}")) {
            scope = scope->parent;
            next();
        } else if (current.compare(T_SPECIAL, ")")) {
            scope = scope->parent;
            next();
        } else {
            throwError(current.getPosition(), "UnexpectedToken", MakeString() << "Token '" << current.getValue() << "' is unexpected");
        }
    }
}

int main(int argc, char * argv[]) {
    Tokenizer tokenizer("quickmake.txt", "{}()@:");
    auto tokens = tokenizer.makeTokens();

    for (auto & i : tokens)
        std::cout << i.toString() << " ";
    std::cout << "\n\n";

    Block root(B_ROOT, nullptr);
    parse(tokens, 0, &root);
    root.print(0);

    return 0;
}
