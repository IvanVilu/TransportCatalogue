#include "json_builder.h"

namespace json
{

    Builder::Builder() {
        nodes_stack_.push_back(&root_);
    }

    DictItemContext Builder::StartDict() {
        if (nodes_stack_.back()->IsArray()) {
            const_cast<Array&>(nodes_stack_.back()->AsArray()).push_back(Dict());
            Node* node = &const_cast<Array&>(nodes_stack_.back()->AsArray()).back();
            nodes_stack_.push_back(node);
        } else {
            *nodes_stack_.back() = Dict();
        }
        return std::move(*this);
    }

    ArrayItemContext Builder::StartArray() {
        if (nodes_stack_.back()->IsArray()) {
            const_cast<Array&>(nodes_stack_.back()->AsArray()).push_back(Array());
            Node* node = &const_cast<Array&>(nodes_stack_.back()->AsArray()).back();
            nodes_stack_.push_back(node);
        } else {
            *nodes_stack_.back() = Array();
        }
        return std::move(*this);
    }

    Builder& Builder::EndDict() {
        if(nodes_stack_.empty() || !nodes_stack_.back()->IsDict())
            throw std::logic_error("Error. There is no Dict to end");
        nodes_stack_.erase(nodes_stack_.end() - 1);
        return *this;
    }

    Builder& Builder::EndArray() {
        if(nodes_stack_.empty() || !nodes_stack_.back()->IsArray())
            throw std::logic_error("Error. There is no Array to end");
        nodes_stack_.erase(nodes_stack_.end() - 1);
        return *this;
    }

    KeyContext Builder::Key(std::string key) {
        nodes_stack_.emplace_back(&const_cast<Dict&>(nodes_stack_.back()->AsDict())[key]);
        return std::move(*this);
    }

    Builder& Builder::Value(Node value) {
        if (nodes_stack_.back()->IsArray()) {
            const_cast<Array&>(nodes_stack_.back()->AsArray()).push_back(value);
        } else {
            *nodes_stack_.back() = value;
            nodes_stack_.erase(nodes_stack_.end() - 1);
        }
        return *this;
    }

    Node Builder::Build() {
        return std::move(root_);
    }

    KeyContext::KeyContext(Builder&& builder)
        : builder_(builder) {}

    DictItemContext KeyContext::Value(Node value) {
        return std::move(builder_.Value(std::move(value)));
    }

    ArrayItemContext KeyContext::StartArray() {
        return builder_.StartArray();
    }

    DictItemContext KeyContext::StartDict() {
        return builder_.StartDict();
    }

    //------------------DictItemContext------------------

    DictItemContext::DictItemContext(Builder&& builder)
        : builder_(builder) {}

    KeyContext DictItemContext::Key(std::string key) {
        return builder_.Key(std::move(key));
    }

    Builder& DictItemContext::EndDict() {
        return builder_.EndDict();
    }


    ArrayItemContext::ArrayItemContext(Builder&& builder)
        : builder_(builder) {}

    ArrayItemContext& ArrayItemContext::Value(Node value) {
        builder_.Value(std::move(value));
        return *this;
    }

    DictItemContext ArrayItemContext::StartDict() {
        return builder_.StartDict();
    }

    ArrayItemContext ArrayItemContext::StartArray() {
        return builder_.StartArray();
    }

    Builder& ArrayItemContext::EndArray() {
        return builder_.EndArray();
    }

} // namespace json
