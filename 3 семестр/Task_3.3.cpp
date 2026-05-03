#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>
#include <algorithm>

using namespace std;

class Airplane {
private:
    string name;          // название самолета
    string manufacturer;  // производитель
    int capacity;         // вместимость
    int flightRange;      // дальность полета
    int maxSpeed;         // максимальная скорость

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

    bool operator>(const Airplane& other) const {
        if (capacity != other.capacity) {
            return capacity > other.capacity;
        }

        if (flightRange != other.flightRange) {
            return flightRange > other.flightRange;
        }

        if (manufacturer != other.manufacturer) {
            return manufacturer < other.manufacturer;
        }

        return name < other.name;
    }

    bool operator<(const Airplane& other) const {
        return other > *this;
    }

    friend ostream& operator<<(ostream& os, const Airplane& plane) {
        os << "Name: " << plane.name
           << ", Manufacturer: " << plane.manufacturer
           << ", Capacity: " << plane.capacity
           << ", Range: " << plane.flightRange
           << ", Max speed: " << plane.maxSpeed;

        return os;
    }
};

// Узел фибоначчиевой пирамиды
template <class T>
class FibonacciNode {
private:
    T value;
    FibonacciNode<T>* parent;
    list<FibonacciNode<T>*> children;

public:
    FibonacciNode(const T& v = T()) {
        value = v;
        parent = nullptr;
    }

    T getValue() const {
        return value;
    }

    void setValue(const T& v) {
        value = v;
    }

    FibonacciNode<T>* getParent() const {
        return parent;
    }

    void setParent(FibonacciNode<T>* p) {
        parent = p;
    }

    list<FibonacciNode<T>*>& getChildren() {
        return children;
    }

    const list<FibonacciNode<T>*>& getChildren() const {
        return children;
    }

    int degree() const {
        return static_cast<int>(children.size());
    }

    bool operator>(const FibonacciNode<T>& other) const {
        return value > other.value;
    }

    bool operator<(const FibonacciNode<T>& other) const {
        return value < other.value;
    }
};

// Фибоначчиева пирамида, максимум хранится на верхнем уровне
template <class T>
class FibonacciHeap {
private:

    list<FibonacciNode<T>*> roots;

    // Указатель на максимальный корень
    FibonacciNode<T>* maxNode;

    // Количество элементов
    int count;

    // Рекурсивное удаление дерева
    void destroySubtree(FibonacciNode<T>* node) {
        if (node == nullptr) {
            return;
        }

        typename list<FibonacciNode<T>*>::iterator it;

        for (it = node->getChildren().begin(); it != node->getChildren().end(); ++it) {
            destroySubtree(*it);
        }

        delete node;
    }

    // Сделать nodeChild ребенком nodeParent
    void link(FibonacciNode<T>* nodeChild, FibonacciNode<T>* nodeParent) {
        nodeChild->setParent(nodeParent);
        nodeParent->getChildren().push_back(nodeChild);
    }

    // Пересчитать maxNode по списку корней
    void updateMaxNode() {
        maxNode = nullptr;

        typename list<FibonacciNode<T>*>::iterator it;

        for (it = roots.begin(); it != roots.end(); ++it) {
            if (maxNode == nullptr || **it > *maxNode) {
                maxNode = *it;
            }
        }
    }

    void consolidate() {
        if (roots.empty()) {
            maxNode = nullptr;
            return;
        }

        vector<FibonacciNode<T>*> table(count + 2, nullptr);

        list<FibonacciNode<T>*> oldRoots = roots;
        roots.clear();

        typename list<FibonacciNode<T>*>::iterator it;

        for (it = oldRoots.begin(); it != oldRoots.end(); ++it) {
            FibonacciNode<T>* x = *it;
            x->setParent(nullptr);

            int d = x->degree();

            while (true) {
                if (d >= static_cast<int>(table.size())) {
                    table.resize(d + 2, nullptr);
                }

                if (table[d] == nullptr) {
                    table[d] = x;
                    break;
                }

                FibonacciNode<T>* y = table[d];
                table[d] = nullptr;

                if (*y > *x) {
                    swap(x, y);
                }

                link(y, x);
                d = x->degree();
            }
        }

        maxNode = nullptr;

        for (int i = 0; i < static_cast<int>(table.size()); i++) {
            if (table[i] != nullptr) {
                roots.push_back(table[i]);

                if (maxNode == nullptr || *table[i] > *maxNode) {
                    maxNode = table[i];
                }
            }
        }
    }

public:
    FibonacciHeap() {
        maxNode = nullptr;
        count = 0;
    }

    FibonacciHeap(const FibonacciHeap<T>& other) = delete;
    FibonacciHeap<T>& operator=(const FibonacciHeap<T>& other) = delete;

    ~FibonacciHeap() {
        typename list<FibonacciNode<T>*>::iterator it;

        for (it = roots.begin(); it != roots.end(); ++it) {
            destroySubtree(*it);
        }

        roots.clear();
        maxNode = nullptr;
        count = 0;
    }

    int getCount() const {
        return count;
    }

    bool empty() const {
        return count == 0;
    }

    // Добавление нового элемента на верхний уровень
    void push(const T& value) {
        FibonacciNode<T>* newNode = new FibonacciNode<T>(value);

        roots.push_back(newNode);

        if (maxNode == nullptr || *newNode > *maxNode) {
            maxNode = newNode;
        }

        count++;
    }

    // Посмотреть максимум без удаления
    T getMax() const {
        if (maxNode == nullptr) {
            throw underflow_error("FibonacciHeap is empty");
        }

        return maxNode->getValue();
    }

    T ExtractMax() {
        if (maxNode == nullptr) {
            throw underflow_error("FibonacciHeap is empty");
        }

        FibonacciNode<T>* oldMax = maxNode;
        T result = oldMax->getValue();

        // Удаляем максимальный корень с верхнего уровня
        roots.remove(oldMax);

        // Его дети становятся корнями верхнего уровня
        typename list<FibonacciNode<T>*>::iterator it;

        for (it = oldMax->getChildren().begin(); it != oldMax->getChildren().end(); ++it) {
            (*it)->setParent(nullptr);
            roots.push_back(*it);
        }

        oldMax->getChildren().clear();

        delete oldMax;
        count--;

        if (roots.empty()) {
            maxNode = nullptr;
        } else {
            consolidate();
        }

        return result;
    }

    // Вывод только верхнего уровня
    void printRootLevel() const {
        cout << "Root level:" << endl;

        typename list<FibonacciNode<T>*>::const_iterator it;

        for (it = roots.begin(); it != roots.end(); ++it) {
            cout << (*it)->getValue()
                 << " | degree = " << (*it)->degree()
                 << endl;
        }
    }
};

int main() {
    FibonacciHeap<Airplane> heap;

    heap.push(Airplane("A320", "Airbus", 180, 6100, 871));
    heap.push(Airplane("A321", "Airbus", 220, 5950, 871));
    heap.push(Airplane("737 MAX 8", "Boeing", 210, 6570, 839));
    heap.push(Airplane("MS-21", "Irkut", 211, 6000, 870));
    heap.push(Airplane("Tu-204", "Tupolev", 210, 4300, 850));
    heap.push(Airplane("CRJ-900", "Bombardier", 90, 2950, 880));
    heap.push(Airplane("A330", "Airbus", 250, 11750, 913));
    heap.push(Airplane("787", "Boeing", 250, 13620, 954));

    cout << "Initial Fibonacci heap root level:" << endl;
    heap.printRootLevel();

    cout << "\nExtractMax output, descending priority:" << endl;

    while (!heap.empty()) {
        cout << heap.ExtractMax() << endl;
    }

    return 0;
}