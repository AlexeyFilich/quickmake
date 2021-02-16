#include <fstream>
#include <iostream>
#include <streambuf>
#include <sstream>
#include <string>
#include <vector>

class MakeString {
    public:
        template<class T>
        MakeString & operator << (const T & arg) {
            ss << arg;
            return *this;
        }

        operator std::string() const {
            return ss.str();
        }
    protected:
        std::stringstream ss;
};

void throwError(std::string filename, int line, int column, std::string type, std::string body) {
    if (filename != "")
        std::cout << filename << ":" << line + 1 << ":" << column + 1 << ": error: " << type << ": " << body << "\n";
    exit(-1);
}

void throwError(std::string type, std::string body) {
    throwError("", 0, 0, type, body);
}

class Tokenizer {
    public:
        Tokenizer(std::string, std::string, bool);
        ~Tokenizer();

        void next();
        bool in(std::string);

        void makeString();

        std::vector<std::string> makeTokens();
    private:
        char current = '\0';
        int index = -1, line = 0, column = -1;

        std::string filename, text, allowed_tokens;
        bool debug_newlines;

        std::vector<std::string> tokens;

        inline static const std::string WHITESPACE = " \t\r\n";
        inline static const std::string NUMBERS = "0123456789";
        inline static const std::string LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
};

Tokenizer::Tokenizer(std::string filename, std::string allowed_tokens, bool debug_newlines = false) : filename(filename), allowed_tokens(allowed_tokens), debug_newlines(debug_newlines) {
    std::fstream file(filename);

    if (!file)
        throwError("FileReadError", MakeString() << "Can't open file '" << filename << "'");

    this->text = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

    this->next();
}

Tokenizer::~Tokenizer() {
}

void Tokenizer::next() {
    this->index++;
    if (this->current == '\n') {
        this->column = 0;
        this->line++;
    } else {
        this->column++;
    }
    current = index < text.length() ? text[index] : '\0';
}

bool Tokenizer::in(std::string charset) {
    return charset.find(this->current) != std::string::npos;
}

void Tokenizer::makeString() {
    char quote_type = this->current;
    std::string token = "";
    bool escape = false;
    this->next();
    while (this->current != '\0' && this->current != '\n' && (this->current != quote_type || escape)) {
        if (this->current == '\\') {
            escape = true;
        } else {
            if (escape) {
                switch (this->current) {
                    case '\'': token += "\'"; break;
                    case '\"': token += "\""; break;
                    // case '?': token += "\?"; break;
                    case '\\': token += "\\"; break;
                    // case 'a': token += "\a"; break;
                    // case 'b': token += "\b"; break;
                    // case 'f': token += "\n"; break;
                    case 'n': token += "\n"; break;
                    case 'r': token += "\r"; break;
                    case 't': token += "\t"; break;
                    // case 'v': token += "\v"; break;
                    default:
                        throwError(this->filename, this->line, this->column, "UnexpectedCharError", MakeString() << "'\\" << this->current << "' is an illegal escape sequence");
                        break;
                }
                escape = false;
            } else {
                token += this->current;
            }
        }
        this->next();
    }
    if (this->current != quote_type) {
        throwError(this->filename, this->line, this->column, "UnexpectedCharError", MakeString() << "Expected '" << quote_type << "' but got '" << this->current << "'");
    }
    this->next();
    this->tokens.push_back(token);
}

std::vector<std::string> Tokenizer::makeTokens() {
    while (this->current != '\0') {
        if (in(Tokenizer::WHITESPACE)) {
            if (this->debug_newlines && this->current == '\n')
                this->tokens.push_back("<NL>");
            this->next();
        } else if (in(Tokenizer::LETTERS + "_")) {
            std::string token = "";
            while (this->current != '\0' && in(Tokenizer::LETTERS + Tokenizer::NUMBERS + "_")) {
                token += this->current;
                this->next();
            }
            this->tokens.push_back(token);
        } else if (in("\'\"")) {
            this->makeString();
        } else if (in(this->allowed_tokens)) {
            this->tokens.push_back(std::string(1, this->current));
            this->next();
        } else {
            throwError(this->filename, this->line, this->column, "UnexpectedCharError", MakeString() << "Charecter '" << this->current << "' is unexpected");
        }
    }
    return this->tokens;
}

int main(int argc, char * argv[]) {
    Tokenizer tokenizer("quickmake.txt", ":{}", false);
    auto tokens = tokenizer.makeTokens();
    for (auto & i : tokens) {
        std::cout << "[" << i << "] ";
        if (i == "<NL>") std::cout << "\n";
    }
    std::cout << "<END>\n";

    //

    return 0;
}
