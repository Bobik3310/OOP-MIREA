#include <algorithm>
#include <iostream>
#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

template<
    class Key,
    class Value,
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

        Node(const Value& v, Node* p = nullptr)
            : value(v), parent(p) {}
    };

    Node* root_{};
    Compare compare_{};
    KeyOfValue key_of_{};

    const Key& key(const Value& value) const {
        return key_of_(value);
    }

    bool less(const Value& value, const Key& k) const {
        return compare_(key(value), k);
    }

    bool equivalent(const Value& value, const Key& k) const {
        return !compare_(key(value), k) &&
               !compare_(k, key(value));
    }

    Node* insert_right_node(const Value& value) {
        if (!root_)
            return root_ = new Node(value);

        Node* current = root_;
        Node* parent = nullptr;

        while (current) {
            parent = current;

            if (compare_(key(value), key(current->value)))
                current = current->left;
            else
                current = current->right;
        }

        Node* node = new Node(value, parent);

        if (compare_(key(value), key(parent->value)))
            parent->left = node;
        else
            parent->right = node;

        return node;
    }

    static Node* next(Node* node) {
        if (!node)
            return nullptr;

        if (node->right) {
            node = node->right;

            while (node->left)
                node = node->left;

            return node;
        }

        Node* parent = node->parent;

        while (parent && node == parent->right) {
            node = parent;
            parent = parent->parent;
        }

        return parent;
    }

    void destroy(Node* node) {
        if (!node)
            return;

        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    void print_inorder(Node* node, std::ostream& os) const {
        if (!node)
            return;

        print_inorder(node->left, os);
        os << node->value << '\n';
        print_inorder(node->right, os);
    }

public:
    class iterator {
        Node* node_{};

        friend class SearchTree;

    public:
        explicit iterator(Node* node = nullptr)
            : node_(node) {}

        Value& operator*() const {
            return node_->value;
        }

        Value* operator->() const {
            return &node_->value;
        }

        Node* node() const {
            return node_;
        }

        iterator& operator++() {
            node_ = SearchTree::next(node_);
            return *this;
        }

        bool operator==(const iterator& other) const {
            return node_ == other.node_;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }
    };

    virtual ~SearchTree() {
        destroy(root_);
    }

    iterator begin() {
        Node* node = root_;

        while (node && node->left)
            node = node->left;

        return iterator(node);
    }

    iterator end() {
        return iterator(nullptr);
    }

    void print(std::ostream& os) const {
        print_inorder(root_, os);
    }
};


template<
    class Key,
    class Value,
    class KeyOfValue,
    class Compare = std::less<Key>
>
class AVLTree
    : public SearchTree<Key, Value, KeyOfValue, Compare> {
protected:
    using Base = SearchTree<Key, Value, KeyOfValue, Compare>;
    using Node = typename Base::Node;

    std::map<Node*, int> heights_;

    int height(Node* node) const {
        return node ? heights_.at(node) : 0;
    }

    void update_height(Node* node) {
        if (node) {
            heights_[node] =
                1 + std::max(height(node->left), height(node->right));
        }
    }

    void rotate_left(Node* x) {
        Node* y = x->right;
        Node* parent = x->parent;

        x->right = y->left;

        if (y->left)
            y->left->parent = x;

        y->left = x;
        x->parent = y;
        y->parent = parent;

        if (!parent)
            this->root_ = y;
        else if (parent->left == x)
            parent->left = y;
        else
            parent->right = y;

        update_height(x);
        update_height(y);
    }

    void rotate_right(Node* y) {
        Node* x = y->left;
        Node* parent = y->parent;

        y->left = x->right;

        if (x->right)
            x->right->parent = y;

        x->right = y;
        y->parent = x;
        x->parent = parent;

        if (!parent)
            this->root_ = x;
        else if (parent->left == y)
            parent->left = x;
        else
            parent->right = x;

        update_height(y);
        update_height(x);
    }

    void rebalance(Node* node) {
        while (node) {
            update_height(node);

            int balance =
                height(node->left) - height(node->right);

            if (balance > 1) {
                if (height(node->left->left) <
                    height(node->left->right)) {
                    rotate_left(node->left);
                }

                rotate_right(node);
            }
            else if (balance < -1) {
                if (height(node->right->right) <
                    height(node->right->left)) {
                    rotate_right(node->right);
                }

                rotate_left(node);
            }

            node = node->parent;
        }
    }

    void collect(Node* node,
                 const Key& search_key,
                 std::vector<Value>& result) const {
        if (!node)
            return;

        const Key& current_key =
            this->key(node->value);

        if (this->compare_(search_key, current_key)) {
            collect(node->left, search_key, result);
        }
        else if (this->compare_(current_key, search_key)) {
            collect(node->right, search_key, result);
        }
        else {
            collect(node->left, search_key, result);
            result.push_back(node->value);
            collect(node->right, search_key, result);
        }
    }

public:
    using typename Base::iterator;

    iterator add(const Value& value) {
        Node* node = this->insert_right_node(value);

        heights_[node] = 1;
        rebalance(node->parent);

        return iterator(node);
    }

    std::vector<Value> operator[](const Key& search_key) const {
        std::vector<Value> result;
        collect(this->root_, search_key, result);
        return result;
    }
};


template<
    class Key,
    class Value,
    class KeyOfValue,
    class Compare = std::less<Key>
>
class SplayTree
    : public AVLTree<Key, Value, KeyOfValue, Compare> {
    using Base = AVLTree<Key, Value, KeyOfValue, Compare>;
    using Node = typename Base::Node;

    void rotate_left(Node* x) {
        Node* y = x->right;

        if (!y)
            return;

        x->right = y->left;

        if (y->left)
            y->left->parent = x;

        y->parent = x->parent;

        if (!x->parent)
            this->root_ = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;

        y->left = x;
        x->parent = y;
    }

    void rotate_right(Node* x) {
        Node* y = x->left;

        if (!y)
            return;

        x->left = y->right;

        if (y->right)
            y->right->parent = x;

        y->parent = x->parent;

        if (!x->parent)
            this->root_ = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;

        y->right = x;
        x->parent = y;
    }

    void splay(Node* node) {
        while (node && node->parent) {
            Node* parent = node->parent;
            Node* grandparent = parent->parent;

            if (!grandparent) {
                if (node == parent->left)
                    rotate_right(parent);
                else
                    rotate_left(parent);
            }
            else if (node == parent->left &&
                     parent == grandparent->left) {
                rotate_right(grandparent);
                rotate_right(parent);
            }
            else if (node == parent->right &&
                     parent == grandparent->right) {
                rotate_left(grandparent);
                rotate_left(parent);
            }
            else if (node == parent->right) {
                rotate_left(parent);
                rotate_right(grandparent);
            }
            else {
                rotate_right(parent);
                rotate_left(grandparent);
            }
        }
    }

    void collect_equal(Node* node,
                      const Key& search_key,
                      std::vector<Value>& result) const {
        if (!node)
            return;

        if (this->equivalent(node->value, search_key))
            result.push_back(node->value);

        collect_equal(node->left, search_key, result);
        collect_equal(node->right, search_key, result);
    }

public:
    using typename Base::iterator;

    iterator add(const Value& value) {
        Node* node = this->insert_right_node(value);
        splay(node);
        return iterator(node);
    }

    iterator find(const Key& search_key) {
        Node* current = this->root_;
        Node* last = nullptr;

        while (current) {
            last = current;

            if (this->equivalent(current->value, search_key)) {
                splay(current);
                return iterator(current);
            }

            if (this->less(current->value, search_key))
                current = current->right;
            else
                current = current->left;
        }

        if (last)
            splay(last);

        return this->end();
    }

    std::vector<Value> operator[](const Key& search_key) {
        std::vector<Value> result;

        find(search_key);
        collect_equal(this->root_, search_key, result);

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
    const Airplane& airplane) {
    return os
        << airplane.manufacturer << ' '
        << airplane.name
        << " capacity=" << airplane.capacity;
}


struct AirplaneKey {
    std::pair<std::string, std::string>
    operator()(const Airplane& airplane) const {
        return {
            airplane.manufacturer,
            airplane.name
        };
    }
};


using AirplaneTree = SplayTree<
    std::pair<std::string, std::string>,
    Airplane,
    AirplaneKey
>;


int main() {
    AirplaneTree tree;

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

    auto airplanes =
        tree[{"Airbus", "A320"}];

    std::cout << airplanes.size() << '\n';

    for (const auto& airplane : airplanes)
        std::cout << airplane << '\n';
}
