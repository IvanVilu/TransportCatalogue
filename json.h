#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <unordered_map>

namespace json {
    using namespace std::literals;
    class Node;
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;
    using Node_JSON = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
    inline const std::string NoneData{ "null" };

    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:
        explicit Node() = default;

        template<typename T>
        Node(T value)
            :value_(std::move(value)) {
        }

        bool operator==(const Node& other) const;
        bool operator!=(const Node& other) const;

        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;
        bool IsBool() const;
        bool IsInt() const;
        bool IsPureDouble() const;
        bool IsDouble() const;
        bool IsString() const;


        bool AsBool() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;
        int AsInt() const;
        const std::string& AsString() const;
        double AsDouble() const;

        const Node_JSON GetValue() const;
    private:
        Node_JSON value_;
    };

    class Document {
    public:
        Document() = default;
        explicit Document(Node root);

        bool operator==(const Document& other) const;
        bool operator!=(const Document& other) const;

        const Node& GetRoot() const;

    private:
        Node root_;
    };

    Document Load(std::istream& input);

    struct DataPrinter {
        std::ostream& out;

        void operator()(const std::nullptr_t) const {
            out << NoneData;
        }

        void operator()(Array array_of_nodes) const {
            out << '[' << std::endl;
            bool is_first = true;
            for (const auto& node : array_of_nodes) {
                if (!is_first)
                    out << ", " << std::endl;
                std::visit(DataPrinter{ out }, node.GetValue());
                is_first = false;

            }
            out << std::endl << ']';
        }

        void operator()(Dict dict) const {
            out << '{' << std::endl;
            bool is_first = true;
            for (const auto& [key, value] : dict) {
                if (!is_first)
                    out << "," << std::endl;

                out << "\"" << key << "\"";
                out << ": ";
                std::visit(DataPrinter{ out }, value.GetValue());
                is_first = false;
            }
            out << std::endl << '}';
        }

        void operator()(bool val) const {
            if (val == true)
                out << "true";
            else
                out << "false";
        }

        void operator()(int val) const {
            out << val;
        }

        void operator()(double val) const {
            out << val;
        }

        void operator()(std::string string) const {
            static std::unordered_map<char, char> symbols = {
               {'\\', '\\'},
               {'\"', '"'},
               {'\n', 'n'},
               {'\r', 'r'},
               {'\t', 't'}
            };
            out << "\"";
            for (const char c : string) {
                if (c == '\t') {
                    out << '\t';
                    continue;
                }
                if (symbols.find(c) != symbols.end()) {
                    out << '\\';
                    out << symbols.at(c);
                }
                else {
                    out << c;
                }


            }
            out << "\"";
        }
    };

    void Print(const Document& doc, std::ostream& output);
}  // namespace json