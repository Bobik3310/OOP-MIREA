#include <algorithm>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

// 2.4. Небалансированное бинарное дерево поиска.
template<class T, class Compare>
class SearchTree {
protected:
    struct Node {
        T value;
        Node* left{};
        Node* right{};
        Node* parent{};
        explicit Node(const T& v, Node* p=nullptr) : value(v), parent(p) {}
    };

    Node* root_{};
    Compare compare_{};

    // a и b имеют одинаковый ключ, если ни один не меньше другого.
    bool equivalent(const T& a, const T& b) const {
        return !compare_(a, b) && !compare_(b, a);
    }

    Node* find_node(const T& sample) const {
        Node* current = root_;
        while (current) {
            if (equivalent(sample, current->value))
                return current;
            current = compare_(sample, current->value)
                ? current->left
                : current->right;
        }
        return nullptr;
    }

    Node* insert_unique(const T& value) {
        if (!root_)
            return root_ = new Node(value);

        Node* current = root_;
        Node* parent = nullptr;

        while (current) {
            parent = current;
            if (equivalent(value, current->value))
                return current;
            current = compare_(value, current->value)
                ? current->left
                : current->right;
        }

        Node* node = new Node(value, parent);
        if (compare_(value, parent->value))
            parent->left = node;
        else
            parent->right = node;
        return node;
    }

    static Node* minimum(Node* node) {
        while (node && node->left) node = node->left;
        return node;
    }

    static Node* successor(Node* node) {
        if (!node) return nullptr;
        if (node->right) return minimum(node->right);
        Node* parent = node->parent;
        while (parent && node == parent->right) {
            node = parent;
            parent = parent->parent;
        }
        return parent;
    }

    void replace(Node* old_node, Node* new_node) {
        if (!old_node->parent) root_ = new_node;
        else if (old_node == old_node->parent->left)
            old_node->parent->left = new_node;
        else
            old_node->parent->right = new_node;
        if (new_node) new_node->parent = old_node->parent;
    }

    void destroy(Node* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    class iterator {
        Node* node_{};
        friend class SearchTree;
        explicit iterator(Node* node) : node_(node) {}
    public:
        iterator() = default;
        T& operator*() const { return node_->value; }
        T* operator->() const { return &node_->value; }
        bool operator==(const iterator& other) const { return node_ == other.node_; }
        bool operator!=(const iterator& other) const { return !(*this == other); }
        iterator& operator++() { node_ = successor(node_); return *this; }
    };

    ~SearchTree() { destroy(root_); }

    iterator begin() { return iterator(minimum(root_)); }
    iterator end() { return iterator(nullptr); }

    iterator add(const T& value) {
        return iterator(insert_unique(value));
    }

    iterator find(const T& sample) {
        return iterator(find_node(sample));
    }
    using FindFunction = bool (*)(const T&, const T&);

    // Поиск по ключу или значению через переданный указатель на функцию.
    iterator find(const T& wanted, FindFunction equal) {
        for (auto it = begin(); it != end(); ++it) {
            if (equal(*it, wanted))
                return it;
        }
        return end();
    }

    bool erase(const T& sample) {
        Node* node = find_node(sample);
        if (!node) return false;

        if (!node->left) {
            replace(node, node->right);
        } else if (!node->right) {
            replace(node, node->left);
        } else {
            Node* next = minimum(node->right);
            if (next->parent != node) {
                replace(next, next->right);
                next->right = node->right;
                next->right->parent = next;
            }
            replace(node, next);
            next->left = node->left;
            next->left->parent = next;
        }

        delete node;
        return true;
    }

    void print(std::ostream& os) {
        for (auto it = begin(); it != end(); ++it)
            os << *it << '\n';
    }
};

struct Airplane {
    std::string name, manufacturer;
    int capacity{};
    double range{}, max_speed{};
};

std::ostream& operator<<(std::ostream& os, const Airplane& a) {
    return os << a.manufacturer << ' ' << a.name
              << ", capacity=" << a.capacity
              << ", range=" << a.range
              << ", max_speed=" << a.max_speed;
}

// Компаратор задает ключ: производитель + название.
struct AirplaneCompare {
    bool operator()(const Airplane& a, const Airplane& b) const {
        if (a.manufacturer != b.manufacturer)
            return a.manufacturer < b.manufacturer;
        return a.name < b.name;
    }
};

bool compare_key(const Airplane& a, const Airplane& b) {
    return a.manufacturer == b.manufacturer && a.name == b.name;
}

bool compare_capacity(const Airplane& a, const Airplane& b) {
    return a.capacity == b.capacity;
}

int main() {
    using Tree = SearchTree<Airplane, AirplaneCompare>;
    Tree tree;

    tree.add({"A320", "Airbus", 180, 6100, 840});
    tree.add({"737-800", "Boeing", 189, 5765, 842});
    tree.add({"A321", "Airbus", 220, 5950, 840});
    tree.add({"A350", "Airbus", 325, 15000, 905});

    std::cout << "Дерево по возрастанию ключа:\n";
    tree.print(std::cout);

    Airplane key{"A321", "Airbus"};
    auto found_by_key = tree.find(key, compare_key);
    if (found_by_key != tree.end())
        std::cout << "\nНайдено по ключу: " << *found_by_key << '\n';

    Airplane sample{"", "", 220, 0, 0};
    auto found_by_value = tree.find(sample, compare_capacity);
    if (found_by_value != tree.end())
        std::cout << "Найдено по значению: " << *found_by_value << '\n';

    tree.erase(key);
    std::cout << "\nПосле удаления:\n";
    tree.print(std::cout);
}
