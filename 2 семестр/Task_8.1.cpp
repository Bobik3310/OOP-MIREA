#include <iostream>
#include <string>
#include <stdexcept>
#include <algorithm>

using namespace std;

class AirplaneKey {
private:
    string manufacturer;
    string name;

public:
    AirplaneKey(string m = "", string n = "") {
        manufacturer = m;
        name = n;
    }

    string getManufacturer() const { return manufacturer; }
    string getName() const { return name; }

    bool operator<(const AirplaneKey& other) const {
        if (manufacturer != other.manufacturer)
            return manufacturer < other.manufacturer;

        return name < other.name;
    }

    bool operator>(const AirplaneKey& other) const {
        return other < *this;
    }

    bool operator==(const AirplaneKey& other) const {
        return manufacturer == other.manufacturer && name == other.name;
    }

    friend ostream& operator<<(ostream& os, const AirplaneKey& key);
};

ostream& operator<<(ostream& os, const AirplaneKey& key) {
    os << key.manufacturer << " " << key.name;
    return os;
}

class Airplane {
private:
    string name;
    string manufacturer;
    int capacity;
    int flightRange;
    int maxSpeed;

public:
    Airplane(
        string n = "",
        string m = "",
        int c = 0,
        int r = 0,
        int s = 0
    ) {
        name = n;
        manufacturer = m;
        capacity = c;
        flightRange = r;
        maxSpeed = s;
    }

    string getName() const { return name; }
    string getManufacturer() const { return manufacturer; }
    int getCapacity() const { return capacity; }
    int getFlightRange() const { return flightRange; }
    int getMaxSpeed() const { return maxSpeed; }

    AirplaneKey getKey() const {
        return AirplaneKey(manufacturer, name);
    }

    friend ostream& operator<<(ostream& os, const Airplane& plane);
};

ostream& operator<<(ostream& os, const Airplane& plane) {
    os << "Name: " << plane.name
       << ", Manufacturer: " << plane.manufacturer
       << ", Capacity: " << plane.capacity
       << ", Range: " << plane.flightRange
       << ", Max speed: " << plane.maxSpeed;

    return os;
}

template<class K, class V>
class Node {
protected:
    K key;
    V data;

    // не можем хранить Node, но имеем право хранить указатель
    Node* left;
    Node* right;
    Node* parent;

     // переменная, необходимая для поддержания баланса дерева 
    int height;

public:
    Node(const K& k = K(), const V& v = V()) {
        key = k;
        data = v;

        left = NULL;
        right = NULL;
        parent = NULL;

        height = 1;
    }

    // доступные извне переменные и функции
    K getKey() const { return key; }
    V& getData() { return data; }

    Node* getLeft() const { return left; }
    Node* getRight() const { return right; }
    Node* getParent() const { return parent; }

    int getHeight() const { return height; }

    void setKey(const K& k) { key = k; }
    void setData(const V& v) { data = v; }

    void setLeft(Node* n) { left = n; }
    void setRight(Node* n) { right = n; }
    void setParent(Node* n) { parent = n; }

    void setHeight(int h) { height = h; }

    void print() const {
        cout << "\nKey: " << key << "\nData: " << data << "\nHeight: " << height << "\n";
    }
};

// 1. Шаблон дерева поиска
template<class K, class V>
class SearchTree {
protected:
    Node<K, V>* root;

    Node<K, V>* Add_R(Node<K, V>* current, Node<K, V>* newNode) {
        if (current == NULL) {
            return newNode;
        }

        if (newNode->getKey() < current->getKey()) {
            Node<K, V>* leftChild = Add_R(current->getLeft(), newNode);
            current->setLeft(leftChild);
            leftChild->setParent(current);
        }
        else if (newNode->getKey() > current->getKey()) {
            Node<K, V>* rightChild = Add_R(current->getRight(), newNode);
            current->setRight(rightChild);
            rightChild->setParent(current);
        }
        else {
            throw runtime_error("Key already exists");
        }

        return current;
    }

    Node<K, V>* Find_R(Node<K, V>* current, const K& key) const {
        if (current == NULL)
            return NULL;

        if (key == current->getKey())
            return current;

        if (key < current->getKey())
            return Find_R(current->getLeft(), key);

        return Find_R(current->getRight(), key);
    }

    Node<K, V>* Min(Node<K, V>* current) const {
        if (current == NULL)
            return NULL;

        while (current->getLeft() != NULL) {
            current = current->getLeft();
        }

        return current;
    }

    Node<K, V>* Remove_R(Node<K, V>* current, const K& key) {
        if (current == NULL)
            return NULL;

        if (key < current->getKey()) {
            Node<K, V>* leftChild = Remove_R(current->getLeft(), key);
            current->setLeft(leftChild);

            if (leftChild != NULL)
                leftChild->setParent(current);
        }
        else if (key > current->getKey()) {
            Node<K, V>* rightChild = Remove_R(current->getRight(), key);
            current->setRight(rightChild);

            if (rightChild != NULL)
                rightChild->setParent(current);
        }
        else {
            // Случай 1: нет левого потомка
            if (current->getLeft() == NULL) {
                Node<K, V>* temp = current->getRight();

                if (temp != NULL)
                    temp->setParent(current->getParent());

                delete current;
                return temp;
            }

            // Случай 2: нет правого потомка
            if (current->getRight() == NULL) {
                Node<K, V>* temp = current->getLeft();

                if (temp != NULL)
                    temp->setParent(current->getParent());

                delete current;
                return temp;
            }

            // Случай 3: есть оба потомка.
            // Берем минимальный узел из правого поддерева.
            Node<K, V>* successor = Min(current->getRight());

            current->setKey(successor->getKey());
            current->setData(successor->getData());

            Node<K, V>* rightChild = Remove_R(current->getRight(), successor->getKey());
            current->setRight(rightChild);

            if (rightChild != NULL)
                rightChild->setParent(current);
        }

        return current;
    }

    void InOrder_R(Node<K, V>* current) const {
        if (current == NULL)
            return;

        InOrder_R(current->getLeft());
        current->print();
        InOrder_R(current->getRight());
    }

    void Clear(Node<K, V>* current) {
        if (current == NULL)
            return;

        Clear(current->getLeft());
        Clear(current->getRight());

        delete current;
    }

public:
    SearchTree() {
        root = NULL;
    }

    virtual ~SearchTree() {
        Clear(root);
        root = NULL;
    }

    Node<K, V>* getRoot() const {
        return root;
    }

    // Добавление
    virtual void Add(const K& key, const V& value) {
        Node<K, V>* newNode = new Node<K, V>(key, value);
        root = Add_R(root, newNode);

        if (root != NULL)
            root->setParent(NULL);
    }

    virtual Node<K, V>* Find(const K& key) const {
        return Find_R(root, key);
    }

    // Перегрузка [] как интерфейс поиска
    V& operator[](const K& key) const {
        Node<K, V>* found = Find(key);

        if (found == NULL)
            throw runtime_error("Key not found");

        return found->getData();
    }

    // Удаление узла
    virtual void Remove(Node<K, V>* node) {
        if (node == NULL)
            return;

        K key = node->getKey();

        root = Remove_R(root, key);

        if (root != NULL)
            root->setParent(NULL);
    }

    void InOrder() const {
        InOrder_R(root);
    }
};

template<class K, class V>
class AVLTree : public SearchTree<K, V> {
protected:
    using SearchTree<K, V>::root;

    int height(Node<K, V>* node) const {
        if (node == NULL)
            return 0;

        return node->getHeight();
    }

    int balanceFactor(Node<K, V>* node) const {
        if (node == NULL)
            return 0;

        return height(node->getLeft()) - height(node->getRight());
    }

    void updateHeight(Node<K, V>* node) {
        if (node == NULL)
            return;

        int leftHeight = height(node->getLeft());
        int rightHeight = height(node->getRight());

        node->setHeight(max(leftHeight, rightHeight) + 1);
    }

    Node<K, V>* rotateRight(Node<K, V>* y) {
        Node<K, V>* x = y->getLeft();
        Node<K, V>* t2 = x->getRight();

        x->setRight(y);
        y->setLeft(t2);

        x->setParent(y->getParent());
        y->setParent(x);

        if (t2 != NULL)
            t2->setParent(y);

        updateHeight(y);
        updateHeight(x);

        return x;
    }

    Node<K, V>* rotateLeft(Node<K, V>* x) {
        Node<K, V>* y = x->getRight();
        Node<K, V>* t2 = y->getLeft();

        y->setLeft(x);
        x->setRight(t2);

        y->setParent(x->getParent());
        x->setParent(y);

        if (t2 != NULL)
            t2->setParent(x);

        updateHeight(x);
        updateHeight(y);

        return y;
    }

    Node<K, V>* balance(Node<K, V>* node) {
        if (node == NULL)
            return NULL;

        updateHeight(node);

        int bf = balanceFactor(node);

        // Левый перегруз
        if (bf > 1) {
            if (balanceFactor(node->getLeft()) < 0) {
                Node<K, V>* newLeft = rotateLeft(node->getLeft());
                node->setLeft(newLeft);

                if (newLeft != NULL)
                    newLeft->setParent(node);
            }

            return rotateRight(node);
        }

        // Правый перегруз
        if (bf < -1) {
            if (balanceFactor(node->getRight()) > 0) {
                Node<K, V>* newRight = rotateRight(node->getRight());
                node->setRight(newRight);

                if (newRight != NULL)
                    newRight->setParent(node);
            }

            return rotateLeft(node);
        }

        return node;
    }

    Node<K, V>* Add_AVL(Node<K, V>* current, Node<K, V>* newNode) {
        if (current == NULL)
            return newNode;

        if (newNode->getKey() < current->getKey()) {
            Node<K, V>* leftChild = Add_AVL(current->getLeft(), newNode);
            current->setLeft(leftChild);
            leftChild->setParent(current);
        }
        else if (newNode->getKey() > current->getKey()) {
            Node<K, V>* rightChild = Add_AVL(current->getRight(), newNode);
            current->setRight(rightChild);
            rightChild->setParent(current);
        }
        else {
            throw runtime_error("Key already exists");
        }

        return balance(current);
    }

    Node<K, V>* Min(Node<K, V>* current) const {
        if (current == NULL)
            return NULL;

        while (current->getLeft() != NULL) {
            current = current->getLeft();
        }

        return current;
    }

    Node<K, V>* Remove_AVL(Node<K, V>* current, const K& key) {
        if (current == NULL)
            return NULL;

        if (key < current->getKey()) {
            Node<K, V>* leftChild = Remove_AVL(current->getLeft(), key);
            current->setLeft(leftChild);

            if (leftChild != NULL)
                leftChild->setParent(current);
        }
        else if (key > current->getKey()) {
            Node<K, V>* rightChild = Remove_AVL(current->getRight(), key);
            current->setRight(rightChild);

            if (rightChild != NULL)
                rightChild->setParent(current);
        }
        else {
            if (current->getLeft() == NULL) {
                Node<K, V>* temp = current->getRight();

                if (temp != NULL)
                    temp->setParent(current->getParent());

                delete current;
                return temp;
            }

            if (current->getRight() == NULL) {
                Node<K, V>* temp = current->getLeft();

                if (temp != NULL)
                    temp->setParent(current->getParent());

                delete current;
                return temp;
            }

            Node<K, V>* successor = Min(current->getRight());

            current->setKey(successor->getKey());
            current->setData(successor->getData());

            Node<K, V>* rightChild = Remove_AVL(current->getRight(), successor->getKey());
            current->setRight(rightChild);

            if (rightChild != NULL)
                rightChild->setParent(current);
        }

        return balance(current);
    }

public:
    AVLTree() : SearchTree<K, V>() {} // Унаследуем от дерева поиска

    // Переопределение добавления
    void Add(const K& key, const V& value) override {
        Node<K, V>* newNode = new Node<K, V>(key, value);

        root = Add_AVL(root, newNode);

        if (root != NULL)
            root->setParent(NULL);
    }

    // Переопределение удаления
    void Remove(Node<K, V>* node) override {
        if (node == NULL)
            return;

        K key = node->getKey();

        root = Remove_AVL(root, key);

        if (root != NULL)
            root->setParent(NULL);
    }
};

int main() {
    try {
        AVLTree<AirplaneKey, Airplane> tree;

        Airplane a1("A320", "Airbus", 180, 6100, 890);
        Airplane a2("737", "Boeing", 189, 5600, 850);
        Airplane a3("A350", "Airbus", 350, 15000, 945);
        Airplane a4("Superjet", "Sukhoi", 98, 4500, 870);
        Airplane a5("777", "Boeing", 396, 15600, 905);

        tree.Add(a1.getKey(), a1);
        tree.Add(a2.getKey(), a2);
        tree.Add(a3.getKey(), a3);
        tree.Add(a4.getKey(), a4);
        tree.Add(a5.getKey(), a5);

        cout << "========== InOrder after adding ==========\n";
        tree.InOrder();




        cout << "\n========== Search through operator[] ==========\n";
        AirplaneKey searchKey("Boeing", "777");
        cout << tree[searchKey] << "\n";




        cout << "\n========== Remove node ==========\n";
        Node<AirplaneKey, Airplane>* nodeToRemove = tree.Find(AirplaneKey("Airbus", "A320"));
        tree.Remove(nodeToRemove);



        
        cout << "\n========== InOrder after removing ==========\n";
        tree.InOrder();
    }
    catch (const exception& e) {
        cout << "Error: " << e.what() << "\n";
    }

    return 0;
}