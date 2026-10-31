#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

// Универсальное декартово дерево:
// Key      — тип ключа;
// Value    — тип хранимого значения;
// Priority — тип приоритета.
template <
    class Key,
    class Value,
    class Priority,
    class CompareKey = std::less<Key>,
    class ComparePriority = std::less<Priority>>
class Treap {
public:
    class Node {
    private:
        Key key_;
        Value value_;
        Priority priority_;
        Node* left_ = nullptr;
        Node* right_ = nullptr;
        Node* parent_ = nullptr;

        Node(const Key& key, const Value& value, const Priority& priority)
            : key_(key), value_(value), priority_(priority) {}

        friend class Treap;

    public:
        const Key& getKey() const { return key_; }
        const Value& getValue() const { return value_; }
        const Priority& getPriority() const { return priority_; }
        Node* getLeft() const { return left_; }
        Node* getRight() const { return right_; }
        Node* getParent() const { return parent_; }
    };

    class Iterator {
    private:
        Node* current_ = nullptr;
        const Treap* tree_ = nullptr;

        Iterator(Node* node, const Treap* tree) : current_(node), tree_(tree) {}
        friend class Treap;

    public:
        Node& operator*() const {
            if (current_ == nullptr) {
                throw std::runtime_error("Iterator is not associated with any element");
            }
            return *current_;
        }

        Node* operator->() const {
            if (current_ == nullptr) {
                throw std::runtime_error("Iterator is not associated with any element");
            }
            return current_;
        }

        Iterator& operator++() {
            if (current_ != nullptr) {
                current_ = tree_->successor(current_);
            }
            return *this;
        }

        Iterator operator++(int) {
            Iterator old = *this;
            ++(*this);
            return old;
        }

        bool operator==(const Iterator& other) const {
            return current_ == other.current_;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
    };

private:
    Node* root_ = nullptr;
    CompareKey compareKey_{};
    ComparePriority comparePriority_{};

    static void clear(Node* node) {
        if (node == nullptr) return;
        clear(node->left_);
        clear(node->right_);
        delete node;
    }

    bool keyLess(const Key& left, const Key& right) const {
        return compareKey_(left, right);
    }

    bool priorityGreater(const Priority& left, const Priority& right) const {
        // std::less означает: «меньшее значение имеет меньший приоритет».
        return comparePriority_(right, left);
    }

    static void setLeft(Node* parent, Node* child) {
        parent->left_ = child;
        if (child != nullptr) child->parent_ = parent;
    }

    static void setRight(Node* parent, Node* child) {
        parent->right_ = child;
        if (child != nullptr) child->parent_ = parent;
    }

    static Node* mergeNodes(Node* left, Node* right,
                            const ComparePriority& comparePriority) {
        if (left == nullptr) {
            if (right != nullptr) right->parent_ = nullptr;
            return right;
        }
        if (right == nullptr) {
            left->parent_ = nullptr;
            return left;
        }

        const bool leftHasGreaterPriority = comparePriority(right->priority_, left->priority_);

        if (leftHasGreaterPriority) {
            Node* merged = mergeNodes(left->right_, right, comparePriority);
            setRight(left, merged);
            left->parent_ = nullptr;
            return left;
        }

        Node* merged = mergeNodes(left, right->left_, comparePriority);
        setLeft(right, merged);
        right->parent_ = nullptr;
        return right;
    }

    bool canMergeWith(const Treap& other) const {
        if (root_ == nullptr || other.root_ == nullptr) return true;
        return keyLess(max()->key_, other.min()->key_);
    }

    static void printInOrderNode(const Node* node) {
        if (node == nullptr) return;
        printInOrderNode(node->left_);
        std::cout << "key = " << node->key_
                  << ", value = " << node->value_
                  << ", priority = " << node->priority_;
        if (node->parent_ != nullptr) {
            std::cout << ", parent = " << node->parent_->key_;
        } else {
            std::cout << ", parent = NULL";
        }
        std::cout << '\n';
        printInOrderNode(node->right_);
    }

    static void printTreeNode(const Node* node, int level = 0) {
        if (node == nullptr) return;
        printTreeNode(node->right_, level + 1);
        for (int i = 0; i < level; ++i) std::cout << "    ";
        std::cout << '(' << node->key_ << ", value=" << node->value_
                  << ", p=" << node->priority_ << ")\n";
        printTreeNode(node->left_, level + 1);
    }

public:
    Treap() = default;

    Treap(CompareKey compareKey, ComparePriority comparePriority)
        : compareKey_(std::move(compareKey)),
          comparePriority_(std::move(comparePriority)) {}

    explicit Treap(Node* node) : root_(node) {
        if (root_ != nullptr) root_->parent_ = nullptr;
    }

    Treap(const Treap&) = delete;
    Treap& operator=(const Treap&) = delete;

    Treap(Treap&& other) noexcept : root_(other.root_) {
        other.root_ = nullptr;
    }

    Treap& operator=(Treap&& other) noexcept {
        if (this != &other) {
            clear(root_);
            root_ = other.root_;
            other.root_ = nullptr;
        }
        return *this;
    }

    ~Treap() { clear(root_); }

    Node* getRoot() const { return root_; }
    bool empty() const { return root_ == nullptr; }

    Iterator begin() const { return Iterator(min(), this); }
    Iterator end() const { return Iterator(nullptr, this); }

    Node* min() const { return min(root_); }

    Node* min(Node* node) const {
        if (node == nullptr) return nullptr;
        while (node->left_ != nullptr) node = node->left_;
        return node;
    }

    Node* max() const { return max(root_); }

    Node* max(Node* node) const {
        if (node == nullptr) return nullptr;
        while (node->right_ != nullptr) node = node->right_;
        return node;
    }

    Node* successor(Node* node) const {
        if (node == nullptr) return nullptr;
        if (node->right_ != nullptr) return min(node->right_);

        Node* parent = node->parent_;
        while (parent != nullptr && node == parent->right_) {
            node = parent;
            parent = parent->parent_;
        }
        return parent;
    }

    Node* find(const Key& key) const {
        Node* current = root_;
        while (current != nullptr) {
            if (!keyLess(current->key_, key) && !keyLess(key, current->key_)) {
                return current;
            }
            current = keyLess(key, current->key_) ? current->left_ : current->right_;
        }
        return nullptr;
    }

    void merge(Treap& other) {
        if (other.root_ == nullptr) return;
        if (!canMergeWith(other)) {
            throw std::invalid_argument("Merge condition violated: max(L) must be less than min(R)");
        }
        root_ = mergeNodes(root_, other.root_, comparePriority_);
        if (root_ != nullptr) root_->parent_ = nullptr;
        other.root_ = nullptr;
    }

    std::pair<Treap, Treap> split(const Key& splitKey) const {
        Treap left(compareKey_, comparePriority_);
        Treap right(compareKey_, comparePriority_);
        for (Iterator it = begin(); it != end(); ++it) {
            const Node& current = *it;
            Treap single(new Node(current.key_, current.value_, current.priority_));
            if (!keyLess(splitKey, current.key_)) left.merge(single);
            else right.merge(single);
        }
        return {std::move(left), std::move(right)};
    }

    bool insert(const Key& key, const Value& value, const Priority& priority) {
        if (find(key) != nullptr) return false;

        auto parts = split(key);
        Treap middle(new Node(key, value, priority));
        parts.first.merge(middle);
        parts.first.merge(parts.second);

        clear(root_);
        root_ = parts.first.root_;
        if (root_ != nullptr) root_->parent_ = nullptr;
        parts.first.root_ = nullptr;
        return true;
    }

    bool remove(const Key& key) {
        if (find(key) == nullptr) return false;

        Treap result;
        for (Iterator it = begin(); it != end(); ++it) {
            const Node& current = *it;
            if (!(!keyLess(current.key_, key) && !keyLess(key, current.key_))) {
                Treap single(new Node(current.key_, current.value_, current.priority_));
                result.merge(single);
            }
        }

        clear(root_);
        root_ = result.root_;
        if (root_ != nullptr) root_->parent_ = nullptr;
        result.root_ = nullptr;
        return true;
    }

    void printInOrder() const {
        if (root_ == nullptr) {
            std::cout << "Tree is empty\n";
            return;
        }
        printInOrderNode(root_);
    }

    void printAsTree() const {
        if (root_ == nullptr) {
            std::cout << "Tree is empty\n";
            return;
        }
        printTreeNode(root_);
    }
};

int main() {
    Treap<int, std::string, int> tree;

    tree.insert(5, "five", 60);
    tree.insert(3, "three", 80);
    tree.insert(7, "seven", 50);
    tree.insert(1, "one", 40);
    tree.insert(4, "four", 70);
    tree.insert(6, "six", 30);
    tree.insert(9, "nine", 90);

    std::cout << "Initial treap:\n";
    tree.printAsTree();

    std::cout << "\nIn-order traversal:\n";
    tree.printInOrder();

    std::cout << "\nIterator keys:\n";
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        std::cout << it->getKey() << ' ';
    }
    std::cout << '\n';

    auto parts = tree.split(4);
    std::cout << "\nKeys <= 4:\n";
    parts.first.printInOrder();
    std::cout << "\nKeys > 4:\n";
    parts.second.printInOrder();

    parts.first.merge(parts.second);
    tree.remove(5);

    return 0;
}
