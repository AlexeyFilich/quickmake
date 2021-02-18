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
    else
        std::cout << "error: " << type << ": " << body << "\n";
    exit(-1);
}

void throwError(std::string type, std::string body) {
    throwError("", 0, 0, type, body);
}
