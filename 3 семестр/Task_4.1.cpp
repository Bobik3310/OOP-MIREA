#include <iostream>
#include <stdexcept>
#include <utility>
#include <cstdlib>
#include <ctime>

using namespace std;

// Узел декартова дерева.
class Node {
private:
    int key;
    int priority;

    Node* left;
    Node* right;
    Node* parent;

public:
    Node(int k = 0, int p = 0) {
        key = k;
        priority = p;
        left = nullptr;
        right = nullptr;
        parent = nullptr;
    }

    int getKey() const {
        return key;
    }

    int getPriority() const {
        return priority;
    }

    Node* getLeft() const {
        return left;
    }

    Node* getRight() const {
        return right;
    }

    Node* getParent() const {
        return parent;
    }

    void setLeft(Node* node) {
        left = node;
        if (left != nullptr) {
            left->parent = this;
        }
    }

    void setRight(Node* node) {
        right = node;
        if (right != nullptr) {
            right->parent = this;
        }
    }

    void setParent(Node* node) {
        parent = node;
    }

    friend class Treap;
};

// Декартово дерево
class Treap {
private:
    Node* root;

    // Рекурсивная очистка памяти.
    static void clear(Node* node) {
        if (node == nullptr) return;

        clear(node->left);
        clear(node->right);

        delete node;
    }

    static Node* mergeNodes(Node* L, Node* R) {
        if (L == nullptr) return R;
        if (R == nullptr) return L;

        if (L->priority > R->priority) {
            Node* mergedRight = mergeNodes(L->right, R);
            L->setRight(mergedRight);
            return L;
        } else {
            Node* mergedLeft = mergeNodes(L, R->left);
            R->setLeft(mergedLeft);
            return R;
        }
    }

    static void printInOrder(Node* node) {
        if (node == nullptr) return;

        printInOrder(node->left);

        cout << "key = " << node->key
             << ", priority = " << node->priority;

        if (node->parent != nullptr) {
            cout << ", parent = " << node->parent->key;
        } else {
            cout << ", parent = NULL";
        }

        cout << endl;

        printInOrder(node->right);
    }

    static void printTree(Node* node, int level = 0) {
        if (node == nullptr) return;

        printTree(node->right, level + 1);

        for (int i = 0; i < level; i++) {
            cout << "    ";
        }

        cout << "(" << node->key << ", p=" << node->priority << ")" << endl;

        printTree(node->left, level + 1);
    }

    bool canMergeWith(const Treap& other) const {
        if (root == nullptr || other.root == nullptr) return true;

        Node* leftMax = max();
        Node* rightMin = other.min();

        return leftMax->getKey() < rightMin->getKey();
    }

public:
    class Iterator {
    private:
        Node* current;
        const Treap* tree;

    public:
        Iterator(Node* node = nullptr, const Treap* owner = nullptr) {
            current = node;
            tree = owner;
        }

        Node& operator*() const {
            if (current == nullptr) {
                throw runtime_error("Iterator is not associated with any element");
            }

            return *current;
        }

        Node* operator->() const {
            if (current == nullptr) {
                throw runtime_error("Iterator is not associated with any element");
            }

            return current;
        }

        Iterator& operator++() {
            if (current != nullptr) {
                current = tree->successor(current);
            }

            return *this;
        }

        Iterator operator++(int) {
            Iterator old = *this;
            ++(*this);
            return old;
        }

        bool operator==(const Iterator& other) const {
            return current == other.current;
        }

        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }

        Node* getNode() const {
            return current;
        }
    };

    Treap() {
        root = nullptr;
    }

    explicit Treap(Node* node) {
        root = node;
        if (root != nullptr) {
            root->parent = nullptr;
        }
    }

    Treap(const Treap&) = delete;
    Treap& operator=(const Treap&) = delete;

    Treap(Treap&& other) noexcept {
        root = other.root;
        other.root = nullptr;
    }

    Treap& operator=(Treap&& other) noexcept {
        if (this != &other) {
            clear(root);

            root = other.root;
            other.root = nullptr;
        }

        return *this;
    }

    ~Treap() {
        clear(root);
    }

    Node* getRoot() const {
        return root;
    }

    bool empty() const {
        return root == nullptr;
    }

    Iterator begin() const {
        return Iterator(min(), this);
    }

    Iterator end() const {
        return Iterator(nullptr, this);
    }

    Node* min() const {
        return min(root);
    }

    Node* min(Node* node) const {
        if (node == nullptr) return nullptr;

        while (node->left != nullptr) {
            node = node->left;
        }

        return node;
    }

    Node* max() const {
        return max(root);
    }

    Node* max(Node* node) const {
        if (node == nullptr) return nullptr;

        while (node->right != nullptr) {
            node = node->right;
        }

        return node;
    }

    // Следующий элемент по возрастанию ключа.
    Node* successor(Node* node) const {
        if (node == nullptr) return nullptr;

        if (node->right != nullptr) {
            return min(node->right);
        }

        // Иначе поднимаемся вверх, пока текущий узел является правым сыном.
        Node* parent = node->parent;

        while (parent != nullptr && node == parent->right) {
            node = parent;
            parent = parent->parent;
        }

        return parent;
    }

    Node* find(int key) const {
        Node* current = root;

        while (current != nullptr) {
            if (key == current->key) {
                return current;
            }

            if (key < current->key) {
                current = current->left;
            } else {
                current = current->right;
            }
        }

        return nullptr;
    }

    void merge(Treap& other) {
        if (other.root == nullptr) return;

        if (root == nullptr) {
            root = other.root;
            root->parent = nullptr;
            other.root = nullptr;
            return;
        }

        if (!canMergeWith(other)) {
            throw invalid_argument("Merge condition violated: max(L) must be less than min(R)");
        }

        root = mergeNodes(root, other.root);

        if (root != nullptr) {
            root->parent = nullptr;
        }

        // Узлы были переданы текущему дереву.
        other.root = nullptr;
    }

    pair<Treap, Treap> split(int splitKey) {
        Treap L;
        Treap R;

        for (Iterator it = begin(); it != end(); ++it) {
            Node* current = it.getNode();

            Node* copy = new Node(current->key, current->priority);
            Treap single(copy);

            if (current->key <= splitKey) {
                L.merge(single);
            } else {
                R.merge(single);
            }
        }

        return make_pair(std::move(L), std::move(R));
    }

    void insert(int key, int priority) {
        if (find(key) != nullptr) {
            cout << "Key " << key << " already exists. Insertion skipped." << endl;
            return;
        }

        pair<Treap, Treap> parts = split(key);

        Treap middle(new Node(key, priority));

        parts.first.merge(middle);
        parts.first.merge(parts.second);

        clear(root);

        root = parts.first.root;
        if (root != nullptr) {
            root->parent = nullptr;
        }

        parts.first.root = nullptr;
    }

    void insert(int key) {
        insert(key, rand() % 100);
    }

    bool remove(int key) {
        if (find(key) == nullptr) {
            return false;
        }

        Treap result;

        for (Iterator it = begin(); it != end(); ++it) {
            Node* current = it.getNode();

            if (current->key != key) {
                Node* copy = new Node(current->key, current->priority);
                Treap single(copy);
                result.merge(single);
            }
        }

        clear(root);

        root = result.root;
        if (root != nullptr) {
            root->parent = nullptr;
        }

        result.root = nullptr;

        return true;
    }

    void printInOrder() const {
        if (root == nullptr) {
            cout << "Tree is empty" << endl;
            return;
        }

        printInOrder(root);
    }

    void printAsTree() const {
        if (root == nullptr) {
            cout << "Tree is empty" << endl;
            return;
        }

        printTree(root);
    }
};

int main() {
    srand(time(nullptr));

    Treap tree;

    tree.insert(5, 60);
    tree.insert(3, 80);
    tree.insert(7, 50);
    tree.insert(1, 40);
    tree.insert(4, 70);
    tree.insert(6, 30);
    tree.insert(9, 90);

    cout << "Initial treap, printed as tree:" << endl;
    tree.printAsTree();

    cout << "\nInitial treap, in-order traversal:" << endl;
    tree.printInOrder();

    cout << "\nIterator traversal by increasing key:" << endl;
    for (Treap::Iterator it = tree.begin(); it != tree.end(); ++it) {
        cout << (*it).getKey() << " ";
    }
    cout << endl;

    Node* minNode = tree.min();
    Node* maxNode = tree.max();

    if (minNode != nullptr) {
        cout << "\nMin key: " << minNode->getKey() << endl;
    }

    if (maxNode != nullptr) {
        cout << "Max key: " << maxNode->getKey() << endl;
    }

    Node* node4 = tree.find(4);
    if (node4 != nullptr) {
        Node* next = tree.successor(node4);

        if (next != nullptr) {
            cout << "Successor of key 4: " << next->getKey() << endl;
        } else {
            cout << "Key 4 has no successor" << endl;
        }
    }

    cout << "\nSplit by key 4:" << endl;

    pair<Treap, Treap> parts = tree.split(4);

    cout << "\nLeft tree, keys <= 4:" << endl;
    parts.first.printInOrder();

    cout << "\nRight tree, keys > 4:" << endl;
    parts.second.printInOrder();

    cout << "\nMerge left and right again:" << endl;
    parts.first.merge(parts.second);
    parts.first.printInOrder();

    cout << "\nRemove key 5 from original tree:" << endl;
    tree.remove(5);
    tree.printInOrder();

    return 0;
}