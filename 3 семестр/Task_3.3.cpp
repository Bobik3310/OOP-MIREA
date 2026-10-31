#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>
#include <functional>
#include <utility>

using namespace std;

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
    )
        : name(move(n)),
          manufacturer(move(m)),
          capacity(c),
          flightRange(r),
          maxSpeed(s) {}

    friend ostream& operator<<(ostream& os, const Airplane& plane) {
        os << "Name: " << plane.name
           << ", Manufacturer: " << plane.manufacturer
           << ", Capacity: " << plane.capacity
           << ", Range: " << plane.flightRange
           << ", Max speed: " << plane.maxSpeed;

        return os;
    }
};

// Отдельный тип приоритета
class AirplanePriority {
private:
    int capacity;
    int flightRange;
    string manufacturer;
    string name;

public:
    AirplanePriority(
        int c = 0,
        int r = 0,
        string m = "",
        string n = ""
    )
        : capacity(c),
          flightRange(r),
          manufacturer(move(m)),
          name(move(n)) {}

    // true, если текущий приоритет выше другого
    bool operator>(const AirplanePriority& other) const {
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

    friend ostream& operator<<(
        ostream& os,
        const AirplanePriority& priority
    ) {
        os << "(capacity = " << priority.capacity
           << ", range = " << priority.flightRange
           << ", manufacturer = " << priority.manufacturer
           << ", name = " << priority.name << ")";

        return os;
    }
};

template<class Value, class Priority>
class FibonacciNode {
private:
    Value value;
    Priority priority;

    FibonacciNode<Value, Priority>* parent;
    list<FibonacciNode<Value, Priority>*> children;

public:
    FibonacciNode(
        const Value& v,
        const Priority& p
    )
        : value(v),
          priority(p),
          parent(nullptr) {}

    const Value& getValue() const {
        return value;
    }

    const Priority& getPriority() const {
        return priority;
    }

    FibonacciNode<Value, Priority>* getParent() const {
        return parent;
    }

    void setParent(FibonacciNode<Value, Priority>* p) {
        parent = p;
    }

    list<FibonacciNode<Value, Priority>*>& getChildren() {
        return children;
    }

    const list<FibonacciNode<Value, Priority>*>& getChildren() const {
        return children;
    }

    int degree() const {
        return static_cast<int>(children.size());
    }
};

template<
    class Value,
    class Priority,
    class Compare = greater<Priority>
>
class FibonacciHeap {
private:
    using Node = FibonacciNode<Value, Priority>;

    list<Node*> roots;
    Node* maxNode;
    int count;
    Compare compare;

    bool hasHigherPriority(
        const Node* first,
        const Node* second
    ) const {
        return compare(
            first->getPriority(),
            second->getPriority()
        );
    }

    void destroySubtree(Node* node) {
        if (node == nullptr) {
            return;
        }

        for (Node* child : node->getChildren()) {
            destroySubtree(child);
        }

        delete node;
    }

    void link(Node* child, Node* parent) {
        child->setParent(parent);
        parent->getChildren().push_back(child);
    }

    void consolidate() {
        if (roots.empty()) {
            maxNode = nullptr;
            return;
        }

        vector<Node*> table(count + 2, nullptr);

        list<Node*> oldRoots = roots;
        roots.clear();

        for (Node* current : oldRoots) {
            current->setParent(nullptr);

            int degree = current->degree();

            while (true) {
                if (degree >= static_cast<int>(table.size())) {
                    table.resize(degree + 2, nullptr);
                }

                if (table[degree] == nullptr) {
                    table[degree] = current;
                    break;
                }

                Node* other = table[degree];
                table[degree] = nullptr;

                if (hasHigherPriority(other, current)) {
                    swap(current, other);
                }

                link(other, current);
                degree = current->degree();
            }
        }

        maxNode = nullptr;

        for (Node* node : table) {
            if (node != nullptr) {
                roots.push_back(node);

                if (
                    maxNode == nullptr ||
                    hasHigherPriority(node, maxNode)
                ) {
                    maxNode = node;
                }
            }
        }
    }

public:
    FibonacciHeap()
        : maxNode(nullptr),
          count(0),
          compare(Compare()) {}

    explicit FibonacciHeap(const Compare& comparator)
        : maxNode(nullptr),
          count(0),
          compare(comparator) {}

    FibonacciHeap(const FibonacciHeap&) = delete;
    FibonacciHeap& operator=(const FibonacciHeap&) = delete;

    ~FibonacciHeap() {
        for (Node* root : roots) {
            destroySubtree(root);
        }
    }

    int getCount() const {
        return count;
    }

    bool empty() const {
        return count == 0;
    }

    void push(
        const Value& value,
        const Priority& priority
    ) {
        Node* newNode = new Node(value, priority);

        roots.push_back(newNode);

        if (
            maxNode == nullptr ||
            hasHigherPriority(newNode, maxNode)
        ) {
            maxNode = newNode;
        }

        ++count;
    }

    const Value& getMax() const {
        if (maxNode == nullptr) {
            throw underflow_error("FibonacciHeap is empty");
        }

        return maxNode->getValue();
    }

    const Priority& getMaxPriority() const {
        if (maxNode == nullptr) {
            throw underflow_error("FibonacciHeap is empty");
        }

        return maxNode->getPriority();
    }

    Value ExtractMax() {
        if (maxNode == nullptr) {
            throw underflow_error("FibonacciHeap is empty");
        }

        Node* oldMax = maxNode;
        Value result = oldMax->getValue();

        roots.remove(oldMax);

        for (Node* child : oldMax->getChildren()) {
            child->setParent(nullptr);
            roots.push_back(child);
        }

        oldMax->getChildren().clear();

        delete oldMax;
        --count;

        if (roots.empty()) {
            maxNode = nullptr;
        } else {
            consolidate();
        }

        return result;
    }

    void printRootLevel() const {
        cout << "Root level:\n";

        for (Node* node : roots) {
            cout << node->getValue()
                 << " | priority = "
                 << node->getPriority()
                 << " | degree = "
                 << node->degree()
                 << '\n';
        }
    }
};

int main() {
    FibonacciHeap<Airplane, AirplanePriority> heap;

    heap.push(
        Airplane("A320", "Airbus", 180, 6100, 871),
        AirplanePriority(180, 6100, "Airbus", "A320")
    );

    heap.push(
        Airplane("A321", "Airbus", 220, 5950, 871),
        AirplanePriority(220, 5950, "Airbus", "A321")
    );

    heap.push(
        Airplane("737 MAX 8", "Boeing", 210, 6570, 839),
        AirplanePriority(210, 6570, "Boeing", "737 MAX 8")
    );

    heap.push(
        Airplane("MS-21", "Irkut", 211, 6000, 870),
        AirplanePriority(211, 6000, "Irkut", "MS-21")
    );

    heap.push(
        Airplane("Tu-204", "Tupolev", 210, 4300, 850),
        AirplanePriority(210, 4300, "Tupolev", "Tu-204")
    );

    heap.push(
        Airplane("CRJ-900", "Bombardier", 90, 2950, 880),
        AirplanePriority(90, 2950, "Bombardier", "CRJ-900")
    );

    heap.push(
        Airplane("A330", "Airbus", 250, 11750, 913),
        AirplanePriority(250, 11750, "Airbus", "A330")
    );

    heap.push(
        Airplane("787", "Boeing", 250, 13620, 954),
        AirplanePriority(250, 13620, "Boeing", "787")
    );

    cout << "Initial Fibonacci heap root level:\n";
    heap.printRootLevel();

    cout << "\nExtractMax output:\n";

    while (!heap.empty()) {
        cout << heap.ExtractMax() << '\n';
    }

    return 0;
}
