#include <iostream>
#include <stdexcept>
#include <utility>
#include <ctime>
#include <cstdlib>
#include <limits>

using namespace std;

class Node {
private:
    double value;        // значение элемента массива
    int priority;        // приоритет, по нему дерево является max-heap
    int size;            // число узлов в поддереве
    double subtreeMin;   // минимум в поддереве, вариант 30
    double lazyAdd;      // отложенная команда: прибавить число ко всему поддереву

    Node* left;
    Node* right;
    Node* parent;

public:
    Node(double v = 0.0, int p = 0) {
        value = v;
        priority = p;
        size = 1;
        subtreeMin = v;
        lazyAdd = 0.0;
        left = nullptr;
        right = nullptr;
        parent = nullptr;
    }

    double getValue() const { return value; }
    int getPriority() const { return priority; }
    int getSize() const { return size; }
    double getSubtreeMin() const { return subtreeMin; }
    double getLazyAdd() const { return lazyAdd; }

    Node* getLeft() const { return left; }
    Node* getRight() const { return right; }
    Node* getParent() const { return parent; }

    void setParent(Node* p) {
        parent = p;
    }

    void setLeft(Node* node) {
        left = node;
        if (left != nullptr) {
            left->parent = this;
        }
        update();
    }

    void setRight(Node* node) {
        right = node;
        if (right != nullptr) {
            right->parent = this;
        }
        update();
    }

    void applyAdd(double x) {
        value += x;
        subtreeMin += x;
        lazyAdd += x;
    }

    void pushPromise() {
        if (lazyAdd != 0.0) {
            if (left != nullptr) {
                left->applyAdd(lazyAdd);
            }

            if (right != nullptr) {
                right->applyAdd(lazyAdd);
            }

            lazyAdd = 0.0;
        }
    }

    void update() {
        size = 1;
        subtreeMin = value;

        if (left != nullptr) {
            size += left->size;
            subtreeMin = std::min(subtreeMin, left->subtreeMin);
        }

        if (right != nullptr) {
            size += right->size;
            subtreeMin = std::min(subtreeMin, right->subtreeMin);
        }
    }

    friend class Treap;
    friend class ImplicitTreap;
};


class Treap {
protected:
    Node* root;

    static int nodeSize(Node* node) {
        return node == nullptr ? 0 : node->size;
    }

    static void clear(Node* node) {
        if (node == nullptr) return;

        node->pushPromise();

        clear(node->left);
        clear(node->right);

        delete node;
    }

    static Node* mergeNodes(Node* L, Node* R) {
        if (L == nullptr) return R;
        if (R == nullptr) return L;

        L->pushPromise();
        R->pushPromise();

        if (L->priority > R->priority) {
            Node* mergedRight = mergeNodes(L->right, R);

            L->right = mergedRight;
            if (mergedRight != nullptr) {
                mergedRight->parent = L;
            }

            L->update();
            return L;
        } else {
            Node* mergedLeft = mergeNodes(L, R->left);

            R->left = mergedLeft;
            if (mergedLeft != nullptr) {
                mergedLeft->parent = R;
            }

            R->update();
            return R;
        }
    }

    static Node* minNode(Node* node) {
        if (node == nullptr) return nullptr;

        node->pushPromise();

        while (node->left != nullptr) {
            node = node->left;
            node->pushPromise();
        }

        return node;
    }

    static Node* maxNode(Node* node) {
        if (node == nullptr) return nullptr;

        node->pushPromise();

        while (node->right != nullptr) {
            node = node->right;
            node->pushPromise();
        }

        return node;
    }

    bool canMergeWith(const Treap& other) const {
        if (root == nullptr || other.root == nullptr) return true;

        return maxNode(root)->value < minNode(other.root)->value;
    }

public:
    Treap() {
        root = nullptr;
    }

    explicit Treap(Node* node) {
        root = node;

        if (root != nullptr) {
            root->parent = nullptr;
        }
    }

    virtual ~Treap() {
        clear(root);
    }

    Treap(const Treap&) = delete;
    Treap& operator=(const Treap&) = delete;

    Node* getRoot() const {
        return root;
    }

    Node* detachRoot() {
        Node* oldRoot = root;
        root = nullptr;

        if (oldRoot != nullptr) {
            oldRoot->parent = nullptr;
        }

        return oldRoot;
    }

    void merge(Treap& other) {
        if (!canMergeWith(other)) {
            throw invalid_argument("For Treap::merge all keys of L must be less than all keys of R");
        }

        root = mergeNodes(root, other.detachRoot());

        if (root != nullptr) {
            root->parent = nullptr;
        }
    }
};

class ImplicitTreap : protected Treap {
private:
    Node* merge(Node* L, Node* R) {
        if (L == nullptr) return R;
        if (R == nullptr) return L;

        L->pushPromise();
        R->pushPromise();

        if (L->priority > R->priority) {
            Node* mergedRight = merge(L->right, R);

            L->right = mergedRight;
            if (mergedRight != nullptr) {
                mergedRight->parent = L;
            }

            L->update();
            return L;
        } else {
            Node* mergedLeft = merge(L, R->left);

            R->left = mergedLeft;
            if (mergedLeft != nullptr) {
                mergedLeft->parent = R;
            }

            R->update();
            return R;
        }
    }

    pair<Node*, Node*> split(Node* node, int leftCount) {
        if (node == nullptr) {
            return make_pair(nullptr, nullptr);
        }

        node->pushPromise();

        int leftSize = nodeSize(node->left);

        if (leftCount <= leftSize) {
            pair<Node*, Node*> result = split(node->left, leftCount);

            node->left = result.second;
            if (node->left != nullptr) {
                node->left->parent = node;
            }

            node->update();
            node->parent = nullptr;

            if (result.first != nullptr) {
                result.first->parent = nullptr;
            }

            return make_pair(result.first, node);
        } else {
            pair<Node*, Node*> result = split(node->right, leftCount - leftSize - 1);

            node->right = result.first;
            if (node->right != nullptr) {
                node->right->parent = node;
            }

            node->update();
            node->parent = nullptr;

            if (result.second != nullptr) {
                result.second->parent = nullptr;
            }

            return make_pair(node, result.second);
        }
    }

    void printInOrder(Node* node) {
        if (node == nullptr) return;

        node->pushPromise();

        printInOrder(node->left);
        cout << node->value << " ";
        printInOrder(node->right);

        node->update();
    }

    void printTree(Node* node, int level) {
        if (node == nullptr) return;

        node->pushPromise();

        printTree(node->right, level + 1);

        for (int i = 0; i < level; i++) {
            cout << "    ";
        }

        cout << "(" << node->value
             << ", p=" << node->priority
             << ", size=" << node->size
             << ", min=" << node->subtreeMin << ")" << endl;

        printTree(node->left, level + 1);

        node->update();
    }

    void checkPositionForInsert(int position) const {
        if (position < 0 || position > size()) {
            throw out_of_range("Incorrect position");
        }
    }

    void checkSegment(int left, int right) const {
        if (left < 0 || right < left || right >= size()) {
            throw out_of_range("Incorrect segment borders");
        }
    }

public:
    ImplicitTreap() : Treap() {}

    explicit ImplicitTreap(Node* node) : Treap(node) {}

    // Групповая функция size() для всего дерева.
    int size() const {
        return nodeSize(root);
    }

    bool empty() const {
        return root == nullptr;
    }

    void merge(ImplicitTreap& other) {
        root = merge(root, other.detachRoot());

        if (root != nullptr) {
            root->parent = nullptr;
        }
    }

    pair<ImplicitTreap*, ImplicitTreap*> split(int leftCount) {
        if (leftCount < 0 || leftCount > size()) {
            throw out_of_range("Incorrect split position");
        }

        pair<Node*, Node*> result = split(root, leftCount);
        root = nullptr;

        return make_pair(
            new ImplicitTreap(result.first),
            new ImplicitTreap(result.second)
        );
    }

    void insertAt(int position, double value, int priority) {
        checkPositionForInsert(position);

        pair<Node*, Node*> parts = split(root, position);

        Node* middle = new Node(value, priority);

        root = merge(merge(parts.first, middle), parts.second);

        if (root != nullptr) {
            root->parent = nullptr;
        }
    }

    void pushBack(double value, int priority) {
        insertAt(size(), value, priority);
    }

    // Удаление элемента по позиции.
    void removeAt(int position) {
        if (position < 0 || position >= size()) {
            throw out_of_range("Incorrect position");
        }

        pair<Node*, Node*> firstSplit = split(root, position);
        pair<Node*, Node*> secondSplit = split(firstSplit.second, 1);

        clear(secondSplit.first);

        root = merge(firstSplit.first, secondSplit.second);

        if (root != nullptr) {
            root->parent = nullptr;
        }
    }

    void addOnSegment(int left, int right, double delta) {
        checkSegment(left, right);

        pair<Node*, Node*> firstSplit = split(root, left);
        pair<Node*, Node*> secondSplit = split(firstSplit.second, right - left + 1);

        if (secondSplit.first != nullptr) {
            secondSplit.first->applyAdd(delta);
        }

        root = merge(
            merge(firstSplit.first, secondSplit.first),
            secondSplit.second
        );

        if (root != nullptr) {
            root->parent = nullptr;
        }
    }

    double getMinOnSegment(int left, int right) {
        checkSegment(left, right);

        pair<Node*, Node*> firstSplit = split(root, left);
        pair<Node*, Node*> secondSplit = split(firstSplit.second, right - left + 1);

        double answer = numeric_limits<double>::infinity();

        if (secondSplit.first != nullptr) {
            answer = secondSplit.first->subtreeMin;
        }

        root = merge(
            merge(firstSplit.first, secondSplit.first),
            secondSplit.second
        );

        if (root != nullptr) {
            root->parent = nullptr;
        }

        return answer;
    }

    double operator[](int position) {
        if (position < 0 || position >= size()) {
            throw out_of_range("Incorrect position");
        }

        pair<Node*, Node*> firstSplit = split(root, position);
        pair<Node*, Node*> secondSplit = split(firstSplit.second, 1);

        secondSplit.first->pushPromise();

        double answer = secondSplit.first->value;

        root = merge(
            merge(firstSplit.first, secondSplit.first),
            secondSplit.second
        );

        if (root != nullptr) {
            root->parent = nullptr;
        }

        return answer;
    }

    void printArray() {
        printInOrder(root);
        cout << endl;
    }

    void printAsTree() {
        printTree(root, 0);
    }
};

int main() {
    srand((unsigned int)time(nullptr));

    ImplicitTreap arr;

    arr.pushBack(7.5, 40);
    arr.pushBack(2.0, 90);
    arr.pushBack(9.1, 30);
    arr.pushBack(4.4, 70);
    arr.pushBack(6.0, 20);
    arr.pushBack(1.3, 60);

    cout << "Initial array:" << endl;
    arr.printArray();

    cout << "Size = " << arr.size() << endl;

    cout << "\nMinimum on segment [1, 4]: "
         << arr.getMinOnSegment(1, 4) << endl;

    cout << "\nAdd 10 to segment [1, 3] with lazy propagation" << endl;
    arr.addOnSegment(1, 3, 10.0);

    cout << "Array after adding:" << endl;
    arr.printArray();

    cout << "Minimum on segment [1, 4] after adding: "
         << arr.getMinOnSegment(1, 4) << endl;

    cout << "\nInsert 0.5 at position 2" << endl;
    arr.insertAt(2, 0.5, 95);
    arr.printArray();

    cout << "Minimum on full array: "
         << arr.getMinOnSegment(0, arr.size() - 1) << endl;

    cout << "\nRemove element at position 3" << endl;
    arr.removeAt(3);
    arr.printArray();

    cout << "Element at position 2 = " << arr[2] << endl;

    cout << "\nTree view:" << endl;
    arr.printAsTree();

    return 0;
}