#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

template<
    class T,
    class Key,
    class KeyOf,
    class Compare = std::less<Key>
>
class AVLTree;

// Универсальное дерево поиска.
// T       — хранимое значение;
// Key     — ключ;
// KeyOf   — извлечение ключа из значения;
// Compare — сравнение ключей.
template<
    class T,
    class Key,
    class KeyOf,
    class Compare
>
class SearchTree {
protected:
    struct Node {
        T value;
        Node* left{};
        Node* right{};
        Node* parent{};
        int height{1};

        Node(const T& v, Node* p = nullptr)
            : value(v), parent(p) {}
    };

    Node* root_{};
    KeyOf key_of_{};
    Compare compare_{};

    Key key(const T& value) const {
        return key_of_(value);
    }

    bool less(const T& a, const T& b) const {
        return compare_(key(a), key(b));
    }

    bool equal_key(const T& value, const Key& k) const {
        Key value_key = key(value);

        return !compare_(value_key, k) &&
               !compare_(k, value_key);
    }

    Node* insert_node(const T& value) {
        if (!root_) {
            return root_ = new Node(value);
        }

        Node* current = root_;
        Node* parent = nullptr;

        while (current) {
            parent = current;

            if (less(value, current->value)) {
                current = current->left;
            } else {
                current = current->right;
            }
        }

        Node* node = new Node(value, parent);

        if (less(value, parent->value)) {
            parent->left = node;
        } else {
            parent->right = node;
        }

        return node;
    }

    void destroy(Node* node) {
        if (!node) {
            return;
        }

        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    void print_inorder(Node* node, std::ostream& os) const {
        if (!node) {
            return;
        }

        print_inorder(node->left, os);
        os << node->value << '\n';
        print_inorder(node->right, os);
    }

public:
    SearchTree() = default;

    // Запрещаем поверхностное копирование владеющего дерева.
    SearchTree(const SearchTree&) = delete;
    SearchTree& operator=(const SearchTree&) = delete;

    class iterator {
        Node* node_{};

        friend class SearchTree;
        friend class AVLTree<T, Key, KeyOf, Compare>;

    public:
        explicit iterator(Node* node = nullptr)
            : node_(node) {}

        T& operator*() const {
            return node_->value;
        }

        bool operator!=(const iterator& other) const {
            return node_ != other.node_;
        }
    };

    ~SearchTree() {
        destroy(root_);
    }

    void print(std::ostream& os) const {
        print_inorder(root_, os);
    }
};

template<
    class T,
    class Key,
    class KeyOf,
    class Compare
>
class AVLTree
    : public SearchTree<T, Key, KeyOf, Compare> {
    using Base = SearchTree<T, Key, KeyOf, Compare>;
    using Node = typename Base::Node;

    int height(Node* node) const {
        return node ? node->height : 0;
    }

    void update_height(Node* node) {
        if (node) {
            node->height =
                1 + std::max(
                    height(node->left),
                    height(node->right)
                );
        }
    }

    void rotate_left(Node* x) {
        Node* y = x->right;
        Node* parent = x->parent;

        x->right = y->left;

        if (y->left) {
            y->left->parent = x;
        }

        y->left = x;
        x->parent = y;
        y->parent = parent;

        if (!parent) {
            this->root_ = y;
        } else if (parent->left == x) {
            parent->left = y;
        } else {
            parent->right = y;
        }

        update_height(x);
        update_height(y);
    }

    void rotate_right(Node* y) {
        Node* x = y->left;
        Node* parent = y->parent;

        y->left = x->right;

        if (x->right) {
            x->right->parent = y;
        }

        x->right = y;
        y->parent = x;
        x->parent = parent;

        if (!parent) {
            this->root_ = x;
        } else if (parent->left == y) {
            parent->left = x;
        } else {
            parent->right = x;
        }

        update_height(y);
        update_height(x);
    }

    void rebalance(Node* node) {
        while (node) {
            update_height(node);

            int balance =
                height(node->left) -
                height(node->right);

            if (balance > 1) {
                if (height(node->left->left) <
                    height(node->left->right)) {
                    rotate_left(node->left);
                }

                rotate_right(node);
            } else if (balance < -1) {
                if (height(node->right->right) <
                    height(node->right->left)) {
                    rotate_right(node->right);
                }

                rotate_left(node);
            }

            node = node->parent;
        }
    }

    void collect(
        Node* node,
        const Key& search_key,
        std::vector<T>& result
    ) const {
        if (!node) {
            return;
        }

        collect(node->left, search_key, result);

        if (this->equal_key(node->value, search_key)) {
            result.push_back(node->value);
        }

        collect(node->right, search_key, result);
    }

public:
    using typename Base::iterator;

    void add(const T& value) {
        Node* node = this->insert_node(value);
        rebalance(node->parent);
    }

    std::vector<T> operator[](const Key& search_key) const {
        std::vector<T> result;
        collect(this->root_, search_key, result);
        return result;
    }
};

struct Airplane {
    std::string name;
    std::string manufacturer;
    int capacity{};
    double range{};
    double max_speed{};
};

std::ostream& operator<<(
    std::ostream& os,
    const Airplane& airplane
) {
    return os
        << airplane.manufacturer << ' '
        << airplane.name
        << " capacity=" << airplane.capacity;
}

struct AirplaneKey {
    std::string manufacturer;
    std::string name;

    bool operator<(const AirplaneKey& other) const {
        return std::tie(manufacturer, name) <
               std::tie(other.manufacturer, other.name);
    }
};

struct AirplaneKeyOf {
    AirplaneKey operator()(const Airplane& airplane) const {
        return {
            airplane.manufacturer,
            airplane.name
        };
    }
};

int main() {
    using Tree = AVLTree<
        Airplane,
        AirplaneKey,
        AirplaneKeyOf
    >;

    Tree tree;

    tree.add({
        "A320",
        "Airbus",
        180,
        6100,
        840
    });

    tree.add({
        "A320",
        "Airbus",
        186,
        6100,
        840
    });

    tree.add({
        "A321",
        "Airbus",
        220,
        5950,
        840
    });

    tree.print(std::cout);

    AirplaneKey key{
        "Airbus",
        "A320"
    };

    std::cout
        << "same key: "
        << tree[key].size()
        << '\n';
}
