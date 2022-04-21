#include "json.h"

using namespace std;

namespace json {

    namespace {

        Node LoadNode(istream& input);

        Node LoadArray(istream& input) {

            Array result;
            for (char c; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            if (!input)
                throw ParsingError("Array parsing error");
            return Node(move(result));
        }

        Node LoadString(istream& input) {
            static std::unordered_map<char, char> symbols = {
               {'\\', '\\'},
               {'"', '\"'},
               {'n', '\n'},
               {'r', '\r'},
               {'t', '\t'}
            };

            if (input.peek() == -1) {
                throw ParsingError("load string  error - empty stream"s);
            }

            bool is_end_str = false;

            string line;
            for (char c; input.get(c);) {
                bool is_escape = false;
                if (c == '\\' && symbols.find(input.peek()) != symbols.end()) {
                    is_escape = true;
                    input >> c;
                }
                else if (c == '"') {
                    is_end_str = true;
                    break;
                }

                if (is_escape)
                    line += symbols.find(c)->second;
                else
                    line += c;

            }

            if (!is_end_str) {
                throw ParsingError("load string  error - no close symbol"s);
            }

            return Node(move(line));
        }

        Node LoadBool(istream& input) {
            string res;
            char c;
            for (; input >> c && std::isalpha(c);)
                res += c;
            input.putback(c);
            if (res == "true")
                return Node(true);
            else if (res == "false")
                return Node(false);
            else
                throw ParsingError("Cant parse value to bool"s);
        }

        Node LoadNull(istream& input) {
            string res;
            for (char c, i = 0; input >> c && i < 4; ++i)
                res += c;
            if (res == "null")
                return Node();
            else
                throw ParsingError("Cant parse value to bool"s);
        }

        Node LoadDict(istream& input) {
            Dict result;
            if (input.peek() == -1)
                throw ParsingError("load Dict error - empty stream"s);
            for (char c; input >> c && c != '}';) {
                if (c == ',') {
                    input >> c;
                }

                string key = LoadString(input).AsString();
                input >> c;
                result.insert({ move(key), LoadNode(input) });
            }

            return Node(move(result));
        }

        Node LoadNumber(istream& input) {
            using namespace std::literals;

            std::string parsed_num;

            // Считывает в parsed_num очередной символ из input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // Считывает одну или более цифр в parsed_num из input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            // Парсим целую часть числа
            if (input.peek() == '0') {
                read_char();
                // После 0 в JSON не могут идти другие цифры
            }
            else {
                read_digits();
            }

            bool is_int = true;
            // Парсим дробную часть числа
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // Парсим экспоненциальную часть числа
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    // Сначала пробуем преобразовать строку в int
                    try {
                        return std::stoi(parsed_num);
                    }
                    catch (...) {
                        // В случае неудачи, например, при переполнении,
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return std::stod(parsed_num);
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        Node LoadNode(istream& input) {
            char c;
            input >> c;
            if (c == EOF)
                throw ParsingError("Nothing to parse"s);
            if (c == '[') {
                return LoadArray(input);
            }
            else if (c == '{') {
                return LoadDict(input);
            }
            else if (c == '"') {
                return LoadString(input);
            }
            else if (c == 't' || c == 'f') {
                input.putback(c);
                return LoadBool(input);
            }
            else if (c == 'n') {
                input.putback(c);
                return LoadNull(input);
            }
            else {
                input.putback(c);
                return LoadNumber(input);
            }
        }

    }  // namespace


    bool Node::operator==(const Node& other) const {
        return value_ == other.value_;
    }

    bool Node::operator!=(const Node& other) const {
        return !(*this == other);
    }

    bool Node::IsNull() const {
        return std::holds_alternative<std::nullptr_t>(value_);
    }

    bool Node::IsArray() const {
        return std::holds_alternative<Array>(value_);
    }

    bool Node::IsMap() const {
        return std::holds_alternative<Dict>(value_);
    }

    bool Node::IsBool() const {
        return std::holds_alternative<bool>(value_);
    }

    bool Node::IsInt() const {
        return std::holds_alternative<int>(value_);
    }

    bool Node::IsPureDouble() const {
        return std::holds_alternative<double>(value_);
    }

    const Node_JSON Node::GetValue() const
    {
        return value_;
    }

    bool Node::IsDouble() const {
        return IsInt() || IsPureDouble();
    }

    bool Node::IsString() const {
        return std::holds_alternative<std::string>(value_);
    }



    bool Node::AsBool() const {
        if (IsBool())
            return std::get<bool>(value_);
        else
            throw logic_error("Node is not a bool");
    }

    const Array& Node::AsArray() const {
        if (IsArray())
            return std::get<Array>(value_);
        else
            throw logic_error("Node is not an Array");
    }

    const Dict& Node::AsMap() const {
        if (IsMap())
            return std::get<Dict>(value_);
        else
            throw logic_error("Node is not a Map");
    }

    int Node::AsInt() const {
        if (IsInt())
            return std::get<int>(value_);
        else
            throw logic_error("Node is not an int");
    }

    const string& Node::AsString() const {
        if (IsString())
            return std::get<std::string>(value_);
        else
            throw logic_error("Node is not a string");
    }

    double Node::AsDouble() const {
        if (IsPureDouble())
            return std::get<double>(value_);
        else if (IsInt())
            return static_cast<double>(std::get<int>(value_));
        else
            throw logic_error("Node is not a double");
    }

    bool Document::operator==(const Document& other) const {
        return this->GetRoot() == other.GetRoot();
    }

    bool Document::operator!=(const Document& other) const {
        return !(*this == other);
    }

    Document::Document(Node root)
        : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }

    void Print(const Document& doc, std::ostream& output) {
        std::visit(DataPrinter{ output }, doc.GetRoot().GetValue());
    }

}  // namespace json