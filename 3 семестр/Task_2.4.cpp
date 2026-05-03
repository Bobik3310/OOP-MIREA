#include <iostream>
#include <string>
#include <vector>
#include <exception>

using namespace std;

class KeyExistsException : public exception {
private:
    string message;

public:
    KeyExistsException(const string& msg) {
        message = msg;
    }

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class NotFoundException : public exception {
private:
    string message;

public:
    NotFoundException(const string& msg) {
        message = msg;
    }

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class IteratorException : public exception {
private:
    string message;

public:
    IteratorException(const string& msg) {
        message = msg;
    }

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class AirplaneKey {
private:
    string manufacturer;
    string name;

public:
    AirplaneKey(string m = "", string n = "") {
        manufacturer = m;
        name = n;
    }

    string getManufacturer() const {
        return manufacturer;
    }

    string getName() const {
        return name;
    }

    bool operator<(const AirplaneKey& other) const {
        if (manufacturer != other.manufacturer) {
            return manufacturer < other.manufacturer;
        }

        return name < other.name;
    }

    bool operator>(const AirplaneKey& other) const {
        return other < *this;
    }

    bool operator==(const AirplaneKey& other) const {
        return manufacturer == other.manufacturer &&
               name == other.name;
    }

    friend ostream& operator<<(ostream& os, const AirplaneKey& key) {
        os << key.manufacturer << " " << key.name;
        return os;
    }
};

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

    string getName() const {
        return name;
    }

    string getManufacturer() const {
        return manufacturer;
    }

    int getCapacity() const {
        return capacity;
    }

    int getFlightRange() const {
        return flightRange;
    }

    int getMaxSpeed() const {
        return maxSpeed;
    }

    bool operator==(const Airplane& other) const {
        return name == other.name &&
               manufacturer == other.manufacturer &&
               capacity == other.capacity &&
               flightRange == other.flightRange &&
               maxSpeed == other.maxSpeed;
    }

    friend ostream& operator<<(ostream& os, const Airplane& airplane) {
        os << "Название: " << airplane.name
           << ", производитель: " << airplane.manufacturer
           << ", вместимость: " << airplane.capacity
           << ", дальность полета: " << airplane.flightRange
           << ", максимальная скорость: " << airplane.maxSpeed;

        return os;
    }
};

template <typename K, typename V>
class AVLTree {
private:
    struct Node {
        K key;
        V value;
        Node* left;
        Node* right;
        Node* parent;
        int height;

        Node(const K& k, const V& v) {
            key = k;
            value = v;
            left = NULL;
            right = NULL;
            parent = NULL;
            height = 1;
        }
    };

    Node* root;

    int height(Node* node) const {
        if (node == NULL) {
            return 0;
        }

        return node->height;
    }

    int maxValue(int a, int b) const {
        if (a > b) {
            return a;
        }

        return b;
    }

    void updateHeight(Node* node) {
        if (node != NULL) {
            node->height = 1 + maxValue(height(node->left), height(node->right));
        }
    }

    int getBalance(Node* node) const {
        if (node == NULL) {
            return 0;
        }

        return height(node->left) - height(node->right);
    }

    Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* t2 = x->right;

        x->right = y;
        y->left = t2;

        x->parent = y->parent;
        y->parent = x;

        if (t2 != NULL) {
            t2->parent = y;
        }

        updateHeight(y);
        updateHeight(x);

        return x;
    }

    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* t2 = y->left;

        y->left = x;
        x->right = t2;

        y->parent = x->parent;
        x->parent = y;

        if (t2 != NULL) {
            t2->parent = x;
        }

        updateHeight(x);
        updateHeight(y);

        return y;
    }

    Node* rebalance(Node* node) {
        updateHeight(node);

        int balance = getBalance(node);

        if (balance > 1) {
            if (getBalance(node->left) < 0) {
                node->left = rotateLeft(node->left);

                if (node->left != NULL) {
                    node->left->parent = node;
                }
            }

            return rotateRight(node);
        }

        if (balance < -1) {
            if (getBalance(node->right) > 0) {
                node->right = rotateRight(node->right);

                if (node->right != NULL) {
                    node->right->parent = node;
                }
            }

            return rotateLeft(node);
        }

        return node;
    }

    Node* insert(Node* node, Node* parent, const K& key, const V& value) {
        if (node == NULL) {
            Node* newNode = new Node(key, value);
            newNode->parent = parent;
            return newNode;
        }

        if (key < node->key) {
            node->left = insert(node->left, node, key, value);

            if (node->left != NULL) {
                node->left->parent = node;
            }
        }
        else if (key > node->key) {
            node->right = insert(node->right, node, key, value);

            if (node->right != NULL) {
                node->right->parent = node;
            }
        }
        else {
            throw KeyExistsException("Ошибка: ключ уже существует");
        }

        return rebalance(node);
    }

    Node* findNodeByKey(Node* node, const K& key) const {
        if (node == NULL) {
            return NULL;
        }

        if (key == node->key) {
            return node;
        }

        if (key < node->key) {
            return findNodeByKey(node->left, key);
        }

        return findNodeByKey(node->right, key);
    }

    Node* minNode(Node* node) const {
        if (node == NULL) {
            return NULL;
        }

        while (node->left != NULL) {
            node = node->left;
        }

        return node;
    }

    Node* maxNode(Node* node) const {
        if (node == NULL) {
            return NULL;
        }

        while (node->right != NULL) {
            node = node->right;
        }

        return node;
    }

    Node* successor(Node* node) const {
        if (node == NULL) {
            return NULL;
        }

        if (node->right != NULL) {
            return minNode(node->right);
        }

        Node* parent = node->parent;

        while (parent != NULL && node == parent->right) {
            node = parent;
            parent = parent->parent;
        }

        return parent;
    }

    Node* predecessor(Node* node) const {
        if (node == NULL) {
            return NULL;
        }

        if (node->left != NULL) {
            return maxNode(node->left);
        }

        Node* parent = node->parent;

        while (parent != NULL && node == parent->left) {
            node = parent;
            parent = parent->parent;
        }

        return parent;
    }

    void clear(Node* node) {
        if (node == NULL) {
            return;
        }

        clear(node->left);
        clear(node->right);
        delete node;
    }

public:
    class Iterator {
    private:
        Node* current;
        const AVLTree<K, V>* tree;

    public:
        Iterator(Node* node = NULL, const AVLTree<K, V>* t = NULL) {
            current = node;
            tree = t;
        }

        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }

        bool operator==(const Iterator& other) const {
            return current == other.current;
        }

        Iterator& operator++() {
            if (current != NULL) {
                current = tree->successor(current);
            }

            return *this;
        }

        Iterator& operator--() {
            if (current != NULL) {
                current = tree->predecessor(current);
            }

            return *this;
        }

        const K& getKey() const {
            if (current == NULL) {
                throw IteratorException("Ошибка: итератор не связан с элементом");
            }

            return current->key;
        }

        const V& getValue() const {
            if (current == NULL) {
                throw IteratorException("Ошибка: итератор не связан с элементом");
            }

            return current->value;
        }
    };

    AVLTree() {
        root = NULL;
    }

    ~AVLTree() {
        clear(root);
    }

    void add(const K& key, const V& value) {
        root = insert(root, NULL, key, value);

        if (root != NULL) {
            root->parent = NULL;
        }
    }

    V findByKey(const K& key) const {
        Node* node = findNodeByKey(root, key);

        if (node == NULL) {
            throw NotFoundException("Ошибка: элемент с таким ключом не найден");
        }

        return node->value;
    }

    vector<K> findByValue(const V& value) const {
        vector<K> result;

        for (Iterator it = begin(); it != end(); ++it) {
            if (it.getValue() == value) {
                result.push_back(it.getKey());
            }
        }

        if (result.empty()) {
            throw NotFoundException("Ошибка: элемент с таким значением не найден");
        }

        return result;
    }

    Iterator begin() const {
        return Iterator(minNode(root), this);
    }

    Iterator end() const {
        return Iterator(NULL, this);
    }

    Iterator rbegin() const {
        return Iterator(maxNode(root), this);
    }

    Iterator rend() const {
        return Iterator(NULL, this);
    }

    void printAscending() const {
        for (Iterator it = begin(); it != end(); ++it) {
            cout << "Ключ: " << it.getKey() << endl;
            cout << "Значение: " << it.getValue() << endl;
            cout << "----------------------------------" << endl;
        }
    }

    void printDescending() const {
        for (Iterator it = rbegin(); it != rend(); --it) {
            cout << "Ключ: " << it.getKey() << endl;
            cout << "Значение: " << it.getValue() << endl;
            cout << "----------------------------------" << endl;
        }
    }
};

int main() {
    AVLTree<AirplaneKey, Airplane> airplanes;

    try {
        airplanes.add(
            AirplaneKey("Boeing", "737"),
            Airplane("737", "Boeing", 189, 5600, 850)
        );

        airplanes.add(
            AirplaneKey("Airbus", "A320"),
            Airplane("A320", "Airbus", 180, 6100, 840)
        );

        airplanes.add(
            AirplaneKey("Boeing", "787"),
            Airplane("787", "Boeing", 330, 13600, 954)
        );

        airplanes.add(
            AirplaneKey("Tupolev", "Tu-204"),
            Airplane("Tu-204", "Tupolev", 210, 4300, 850)
        );

        airplanes.add(
            AirplaneKey("Airbus", "A350"),
            Airplane("A350", "Airbus", 350, 15000, 945)
        );

        airplanes.add(
            AirplaneKey("Irkut", "MS-21"),
            Airplane("MS-21", "Irkut", 211, 6000, 870)
        );

        cout << "Вывод дерева в порядке возрастания ключей:\n\n";
        airplanes.printAscending();

        cout << "\nВывод дерева в порядке убывания ключей:\n\n";
        airplanes.printDescending();

        cout << "\nПоиск по ключу Boeing 787:\n";
        Airplane foundByKey = airplanes.findByKey(
            AirplaneKey("Boeing", "787")
        );
        cout << foundByKey << endl;

        cout << "\nПоиск по значению:\n";
        Airplane target("A350", "Airbus", 350, 15000, 945);

        vector<AirplaneKey> keys = airplanes.findByValue(target);

        for (int i = 0; i < keys.size(); i++) {
            cout << keys[i] << endl;
        }

        cout << "\nПроверка исключения при повторном ключе:\n";
        airplanes.add(
            AirplaneKey("Boeing", "737"),
            Airplane("737-duplicate", "Boeing", 200, 7000, 900)
        );
    }
    catch (const KeyExistsException& e) {
        cout << e.what() << endl;
    }
    catch (const NotFoundException& e) {
        cout << e.what() << endl;
    }
    catch (const IteratorException& e) {
        cout << e.what() << endl;
    }
    catch (const exception& e) {
        cout << "Неизвестная ошибка: " << e.what() << endl;
    }

    return 0;
}