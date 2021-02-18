enum TokenType {
    T_IDENTIFIER,
    T_STRING,
    T_SPECIAL
};

class Token {
    private:
        TokenType type;
        Position pos;
        std::string value;
    public:
        Token(TokenType, std::string, Position);
        ~Token();

        TokenType getType();
        std::string getValue();
        Position getPosition();
        std::string toString();
        bool compare(TokenType);
        bool compare(TokenType, std::string);
};

Token::Token(TokenType type, std::string value, Position pos) : type(type), value(value), pos(pos) {
}

Token::~Token() {
}

TokenType Token::getType() {
    return this->type;
}

std::string Token::getValue() {
    return this->value;
}

Position Token::getPosition() {
    return this->pos;
}

std::string Token::toString() {
    std::string string;
    switch (this->type) {
        case T_IDENTIFIER: string = "[Identifier: "; break;
        case T_STRING: string = "[String: "; break;
        case T_SPECIAL: string = "[Special: "; break;
        default: string = "[Error_type: "; break;
    }
    return string + "'" + this->value + "']";
}

bool Token::compare(TokenType type) {
    return this->type == type;
}

bool Token::compare(TokenType type, std::string value) {
    return this->type == type && this->value == value;
}

class Tokenizer {
    private:
        char current = '\0';
        Position pos;
        std::string text, special;
        std::vector<Token> tokens;

        inline static const std::string WHITESPACE = " \t\r\n";
        inline static const std::string NUMBERS = "0123456789";
        inline static const std::string LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    public:
        Tokenizer(std::string, std::string);
        ~Tokenizer();

        void next();
        bool in(std::string);

        void makeString();
        std::vector<Token> makeTokens();
};

Tokenizer::Tokenizer(std::string filename, std::string special) : special(special), pos(filename, -1, 0, -1) {
    std::fstream file(filename);
    if (!file)
        throwError("FileReadError", MakeString() << "Can't open file '" << filename << "'");
    this->text = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    this->next();
}

Tokenizer::~Tokenizer() {
}

void Tokenizer::next() {
    this->pos.next(this->current);
    this->current = this->pos.index < this->text.length() ? this->text[this->pos.index] : '\0';
}

bool Tokenizer::in(std::string charset) {
    return charset.find(this->current) != std::string::npos;
}

void Tokenizer::makeString() {
    char quote_type = this->current;
    std::string string = "";
    bool escape = false;
    this->next();
    while (this->current != '\0' && this->current != '\n' && (this->current != quote_type || escape)) {
        if (this->current == '\\') {
            escape = true;
        } else {
            if (escape) {
                switch (this->current) {
                    case '\'': string += "\'"; break;
                    case '\"': string += "\""; break;
                    // case '?': string += "\?"; break;
                    case '\\': string += "\\"; break;
                    // case 'a': string += "\a"; break;
                    // case 'b': string += "\b"; break;
                    // case 'f': string += "\n"; break;
                    case 'n': string += "\n"; break;
                    case 'r': string += "\r"; break;
                    case 't': string += "\t"; break;
                    // case 'v': string += "\v"; break;
                    default:
                        throwError(this->pos, "UnexpectedCharError", MakeString() << "'\\" << this->current << "' is an illegal escape sequence");
                        break;
                }
                escape = false;
            } else {
                string += this->current;
            }
        }
        this->next();
    }
    if (this->current != quote_type) {
        throwError(this->pos, "UnexpectedCharError", MakeString() << "Expected '" << quote_type << "' but got '" << this->current << "'");
    }
    this->next();
    this->tokens.emplace_back(T_STRING, string, this->pos);
}

std::vector<Token> Tokenizer::makeTokens() {
    while (this->current != '\0') {
        if (this->in(Tokenizer::WHITESPACE)) {
            this->next();
        } else if (this->in("#")) {
            while (this->current != '\n' && this->current != '\0')
                this->next();
        } else if (this->in(Tokenizer::LETTERS + "_")) {
            std::string string = "";
            while (this->current != '\0' && this->in(Tokenizer::LETTERS + Tokenizer::NUMBERS + "_")) {
                string += this->current;
                this->next();
            }
            this->tokens.emplace_back(T_IDENTIFIER, string, this->pos);
        } else if (this->in("\'\"")) {
            this->makeString();
        } else if (this->in(this->special)) {
            this->tokens.emplace_back(T_SPECIAL, std::string(1, this->current), this->pos);
            this->next();
        } else {
            throwError(this->pos, "UnexpectedCharError", MakeString() << "Charecter '" << this->current << "' is unexpected");
        }
    }
    this->tokens.emplace_back(T_SPECIAL, "<EOF>", this->pos);
    return this->tokens;
}
