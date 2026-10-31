#include <algorithm>
#include <cstddef>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <utility>

// Политика по умолчанию: числовое значение, прибавление на отрезке,
// минимум на отрезке.
template <class T>
struct AddMinOperations {
    using Lazy = T;

    static Lazy identityLazy() { return Lazy{}; }

    static bool isIdentity(const Lazy& value) {
        return value == identityLazy();
    }

    static void apply(T& value, T& subtreeMin, Lazy& lazy, const Lazy& delta) {
        value += delta;
        subtreeMin += delta;
        lazy += delta;
    }

    static T aggregate(const T& value, const T* left, const T* right) {
        T result = value;
        if (left != nullptr) {
            result = std::min(result, *left);
        }
        if (right != nullptr) {
            result = std::min(result, *right);
        }
        return result;
    }
};

template <
    class Value,
    class Priority,
    class Operations = AddMinOperations<Value>>
class ImplicitTreap {
private:
    struct Node {
        Value value;
        Priority priority;
        std::size_t size = 1;
        Value subtreeMin;
        typename Operations::Lazy lazy = Operations::identityLazy();
        Node* left = nullptr;
        Node* right = nullptr;
        Node* parent = nullptr;

        Node(const Value& value_, const Priority& priority_)
            : value(value_), priority(priority_), subtreeMin(value_) {}
    };

    Node* root_ = nullptr;

    static std::size_t nodeSize(const Node* node) {
        return node == nullptr ? 0 : node->size;
    }

    static void clear(Node* node) {
        if (node == nullptr) {
            return;
        }
        push(node);
        clear(node->left);
        clear(node->right);
        delete node;
    }

    static void push(Node* node) {
        if (node == nullptr || Operations::isIdentity(node->lazy)) {
            return;
        }

        if (node->left != nullptr) {
            Operations::apply(
                node->left->value,
                node->left->subtreeMin,
                node->left->lazy,
                node->lazy);
        }
        if (node->right != nullptr) {
            Operations::apply(
                node->right->value,
                node->right->subtreeMin,
                node->right->lazy,
                node->lazy);
        }

        node->lazy = Operations::identityLazy();
    }

    static void update(Node* node) {
        if (node == nullptr) {
            return;
        }

        node->size = 1 + nodeSize(node->left) + nodeSize(node->right);
        const Value* leftMin = node->left == nullptr
            ? nullptr
            : &node->left->subtreeMin;
        const Value* rightMin = node->right == nullptr
            ? nullptr
            : &node->right->subtreeMin;
        node->subtreeMin = Operations::aggregate(node->value, leftMin, rightMin);
    }

    static Node* mergeNodes(Node* left, Node* right) {
        if (left == nullptr) {
            return right;
        }
        if (right == nullptr) {
            return left;
        }

        push(left);
        push(right);

        if (left->priority > right->priority) {
            left->right = mergeNodes(left->right, right);
            if (left->right != nullptr) {
                left->right->parent = left;
            }
            update(left);
            return left;
        }

        right->left = mergeNodes(left, right->left);
        if (right->left != nullptr) {
            right->left->parent = right;
        }
        update(right);
        return right;
    }

    static std::pair<Node*, Node*> splitNodes(Node* node, std::size_t leftCount) {
        if (node == nullptr) {
            return {nullptr, nullptr};
        }

        push(node);
        const std::size_t leftSize = nodeSize(node->left);

        if (leftCount <= leftSize) {
            auto parts = splitNodes(node->left, leftCount);
            node->left = parts.second;
            if (node->left != nullptr) {
                node->left->parent = node;
            }
            update(node);
            node->parent = nullptr;
            if (parts.first != nullptr) {
                parts.first->parent = nullptr;
            }
            return {parts.first, node};
        }

        auto parts = splitNodes(node->right, leftCount - leftSize - 1);
        node->right = parts.first;
        if (node->right != nullptr) {
            node->right->parent = node;
        }
        update(node);
        node->parent = nullptr;
        if (parts.second != nullptr) {
            parts.second->parent = nullptr;
        }
        return {node, parts.second};
    }

    void checkPosition(std::size_t position) const {
        if (position > size()) {
            throw std::out_of_range("Incorrect position");
        }
    }

    void checkSegment(std::size_t left, std::size_t right) const {
        if (left > right || right >= size()) {
            throw std::out_of_range("Incorrect segment borders");
        }
    }

public:
    ImplicitTreap() = default;

    ~ImplicitTreap() {
        clear(root_);
    }

    ImplicitTreap(const ImplicitTreap&) = delete;
    ImplicitTreap& operator=(const ImplicitTreap&) = delete;

    ImplicitTreap(ImplicitTreap&& other) noexcept : root_(other.root_) {
        other.root_ = nullptr;
    }

    ImplicitTreap& operator=(ImplicitTreap&& other) noexcept {
        if (this != &other) {
            clear(root_);
            root_ = other.root_;
            other.root_ = nullptr;
        }
        return *this;
    }

    std::size_t size() const { return nodeSize(root_); }
    bool empty() const { return root_ == nullptr; }

    void insertAt(
        std::size_t position,
        const Value& value,
        const Priority& priority) {
        checkPosition(position);
        auto parts = splitNodes(root_, position);
        Node* middle = new Node(value, priority);
        root_ = mergeNodes(mergeNodes(parts.first, middle), parts.second);
        if (root_ != nullptr) {
            root_->parent = nullptr;
        }
    }

    void pushBack(const Value& value, const Priority& priority) {
        insertAt(size(), value, priority);
    }

    void merge(ImplicitTreap& other) {
        root_ = mergeNodes(root_, other.root_);
        other.root_ = nullptr;
        if (root_ != nullptr) {
            root_->parent = nullptr;
        }
    }

    std::pair<ImplicitTreap*, ImplicitTreap*> split(std::size_t leftCount) {
        if (leftCount > size()) {
            throw std::out_of_range("Incorrect split position");
        }

        auto parts = splitNodes(root_, leftCount);
        root_ = nullptr;

        auto* left = new ImplicitTreap();
        auto* right = new ImplicitTreap();
        left->root_ = parts.first;
        right->root_ = parts.second;
        return {left, right};
    }

    void removeAt(std::size_t position) {
        if (position >= size()) {
            throw std::out_of_range("Incorrect position");
        }

        auto first = splitNodes(root_, position);
        auto second = splitNodes(first.second, 1);
        clear(second.first);
        root_ = mergeNodes(first.first, second.second);
        if (root_ != nullptr) {
            root_->parent = nullptr;
        }
    }

    void applyOnSegment(std::size_t left, std::size_t right,
                        const typename Operations::Lazy& tag) {
        checkSegment(left, right);

        auto first = splitNodes(root_, left);
        auto second = splitNodes(first.second, right - left + 1);

        if (second.first != nullptr) {
            Operations::apply(
                second.first->value,
                second.first->subtreeMin,
                second.first->lazy,
                tag);
        }

        root_ = mergeNodes(mergeNodes(first.first, second.first), second.second);
        if (root_ != nullptr) {
            root_->parent = nullptr;
        }
    }

    const Value& minOnSegment(std::size_t left, std::size_t right) {
        checkSegment(left, right);

        auto first = splitNodes(root_, left);
        auto second = splitNodes(first.second, right - left + 1);
        const Value* result = &second.first->subtreeMin;

        root_ = mergeNodes(mergeNodes(first.first, second.first), second.second);
        if (root_ != nullptr) {
            root_->parent = nullptr;
        }
        return *result;
    }

    Value operator[](std::size_t position) {
        if (position >= size()) {
            throw std::out_of_range("Incorrect position");
        }

        auto first = splitNodes(root_, position);
        auto second = splitNodes(first.second, 1);
        push(second.first);
        Value result = second.first->value;

        root_ = mergeNodes(mergeNodes(first.first, second.first), second.second);
        if (root_ != nullptr) {
            root_->parent = nullptr;
        }
        return result;
    }

    void printArray(std::ostream& out = std::cout) {
        printInOrder(root_, out);
        out << '\n';
    }

private:
    static void printInOrder(Node* node, std::ostream& out) {
        if (node == nullptr) {
            return;
        }
        push(node);
        printInOrder(node->left, out);
        out << node->value << ' ';
        printInOrder(node->right, out);
        update(node);
    }
};

int main() {
    using Treap = ImplicitTreap<double, int>;
    Treap array;

    array.pushBack(7.5, 40);
    array.pushBack(2.0, 90);
    array.pushBack(9.1, 30);
    array.pushBack(4.4, 70);
    array.pushBack(6.0, 20);
    array.pushBack(1.3, 60);

    std::cout << "Initial array: ";
    array.printArray();
    std::cout << "Minimum [1, 4]: " << array.minOnSegment(1, 4) << '\n';

    array.applyOnSegment(1, 3, 10.0);
    std::cout << "After adding: ";
    array.printArray();
    std::cout << "Minimum [1, 4]: " << array.minOnSegment(1, 4) << '\n';

    array.insertAt(2, 0.5, 95);
    array.removeAt(3);
    std::cout << "Element [2]: " << array[2] << '\n';
}
