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

class Position {
    public:
        Position(std::string, int, int, int);
        ~Position();
        void next(char ch);

        std::string filename;
        int index, line, column;
};

Position::Position(std::string filename, int index, int line, int column) : filename(filename), index(index), line(line), column(column) {
}

Position::~Position() {
}

void Position::next(char ch) {
    this->index++;
    if (ch == '\n') {
        this->column = 0;
        this->line++;
    } else {
        this->column++;
    }
}

void throwError(std::string filename, int line, int column, std::string type, std::string body) {
    if (filename != "")
        std::cout << filename << ":" << line + 1 << ":" << column + 1 << ": error: " << type << ": " << body << "\n";
    else
        std::cout << "error: " << type << ": " << body << "\n";
    exit(-1);
}

void throwError(Position pos, std::string type, std::string body) {
    throwError(pos.filename, pos.line, pos.column, type, body);
}

void throwError(std::string type, std::string body) {
    throwError("", 0, 0, type, body);
}
