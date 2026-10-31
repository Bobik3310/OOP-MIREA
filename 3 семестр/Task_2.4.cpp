#include <functional>
#include <iostream>
#include <string>

// Дерево хранит Value, но сортирует объекты по отдельному Key.
template<
    class Value,
    class Key,
    class KeyOfValue,
    class Compare = std::less<Key>
>
class SearchTree {
protected:
    struct Node {
        Value value;
        Node* left{};
        Node* right{};
        Node* parent{};

        explicit Node(const Value& v, Node* p = nullptr)
            : value(v), parent(p) {}
    };

    Node* root_{};
    KeyOfValue key_of_{};
    Compare compare_{};

    bool equivalent(const Key& a, const Key& b) const {
        return !compare_(a, b) && !compare_(b, a);
    }

    Node* find_node(const Key& key) const {
        Node* current = root_;

        while (current) {
            const Key current_key = key_of_(current->value);

            if (equivalent(key, current_key))
                return current;

            current = compare_(key, current_key)
                ? current->left
                : current->right;
        }

        return nullptr;
    }

    Node* insert_unique(const Value& value) {
        const Key value_key = key_of_(value);

        if (!root_)
            return root_ = new Node(value);

        Node* current = root_;
        Node* parent = nullptr;

        while (current) {
            parent = current;

            const Key current_key = key_of_(current->value);

            if (equivalent(value_key, current_key))
                return current;

            current = compare_(value_key, current_key)
                ? current->left
                : current->right;
        }

        Node* node = new Node(value, parent);

        if (compare_(value_key, key_of_(parent->value)))
            parent->left = node;
        else
            parent->right = node;

        return node;
    }

    static Node* minimum(Node* node) {
        while (node && node->left)
            node = node->left;

        return node;
    }

    static Node* successor(Node* node) {
        if (!node)
            return nullptr;

        if (node->right)
            return minimum(node->right);

        Node* parent = node->parent;

        while (parent && node == parent->right) {
            node = parent;
            parent = parent->parent;
        }

        return parent;
    }

    void replace(Node* old_node, Node* new_node) {
        if (!old_node->parent) {
            root_ = new_node;
        } else if (old_node == old_node->parent->left) {
            old_node->parent->left = new_node;
        } else {
            old_node->parent->right = new_node;
        }

        if (new_node)
            new_node->parent = old_node->parent;
    }

    void destroy(Node* node) {
        if (!node)
            return;

        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    class iterator {
        Node* node_{};

        friend class SearchTree;

        explicit iterator(Node* node)
            : node_(node) {}

    public:
        iterator() = default;

        Value& operator*() const {
            return node_->value;
        }

        Value* operator->() const {
            return &node_->value;
        }

        bool operator==(const iterator& other) const {
            return node_ == other.node_;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

        iterator& operator++() {
            node_ = successor(node_);
            return *this;
        }
    };

    ~SearchTree() {
        destroy(root_);
    }

    iterator begin() {
        return iterator(minimum(root_));
    }

    iterator end() {
        return iterator(nullptr);
    }

    iterator add(const Value& value) {
        return iterator(insert_unique(value));
    }

    iterator find(const Key& key) {
        return iterator(find_node(key));
    }

    template<class Predicate>
    iterator find_if(Predicate predicate) {
        for (auto it = begin(); it != end(); ++it) {
            if (predicate(*it))
                return it;
        }

        return end();
    }

    bool erase(const Key& key) {
        Node* node = find_node(key);

        if (!node)
            return false;

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
    std::string name;
    std::string manufacturer;
    int capacity{};
    double range{};
    double max_speed{};
};

std::ostream& operator<<(std::ostream& os, const Airplane& airplane) {
    return os
        << airplane.manufacturer << ' '
        << airplane.name
        << ", capacity=" << airplane.capacity
        << ", range=" << airplane.range
        << ", max_speed=" << airplane.max_speed;
}


// Отдельный тип ключа.
struct AirplaneKey {
    std::string manufacturer;
    std::string name;
};


// Извлечение ключа из значения.
struct AirplaneKeyOfValue {
    AirplaneKey operator()(const Airplane& airplane) const {
        return {
            airplane.manufacturer,
            airplane.name
        };
    }
};


// Сравнение ключей.
struct AirplaneKeyCompare {
    bool operator()(const AirplaneKey& a,
                    const AirplaneKey& b) const {
        if (a.manufacturer != b.manufacturer)
            return a.manufacturer < b.manufacturer;

        return a.name < b.name;
    }
};


int main() {
    using Tree = SearchTree<
        Airplane,
        AirplaneKey,
        AirplaneKeyOfValue,
        AirplaneKeyCompare
    >;

    Tree tree;

    tree.add({"A320",   "Airbus", 180, 6100,  840});
    tree.add({"737-800","Boeing",189, 5765,  842});
    tree.add({"A321",   "Airbus",220, 5950,  840});
    tree.add({"A350",   "Airbus",325, 15000, 905});

    std::cout << "Дерево по возрастанию ключа:\n";
    tree.print(std::cout);

    AirplaneKey key{"Airbus", "A321"};

    auto found_by_key = tree.find(key);

    if (found_by_key != tree.end())
        std::cout << "\nНайдено по ключу: "
                  << *found_by_key << '\n';

    auto found_by_capacity = tree.find_if(
        [](const Airplane& airplane) {
            return airplane.capacity == 220;
        }
    );

    if (found_by_capacity != tree.end())
        std::cout << "Найдено по вместимости: "
                  << *found_by_capacity << '\n';

    tree.erase(key);

    std::cout << "\nПосле удаления:\n";
    tree.print(std::cout);
}
