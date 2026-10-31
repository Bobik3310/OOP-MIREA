#include <iostream>
#include <string>
#include <stdexcept>
#include <functional>

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
        : name(n),
          manufacturer(m),
          capacity(c),
          flightRange(r),
          maxSpeed(s) {
    }

    const string& getName() const {
        return name;
    }

    const string& getManufacturer() const {
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

    friend ostream& operator<<(ostream& os, const Airplane& plane) {
        os << "Name: " << plane.name
           << ", Manufacturer: " << plane.manufacturer
           << ", Capacity: " << plane.capacity
           << ", Range: " << plane.flightRange
           << ", Max speed: " << plane.maxSpeed;

        return os;
    }
};

// Отдельно описываем приоритет самолетов
struct AirplanePriority {
    bool operator()(const Airplane& a, const Airplane& b) const {
        if (a.getCapacity() != b.getCapacity()) {
            return a.getCapacity() > b.getCapacity();
        }

        if (a.getFlightRange() != b.getFlightRange()) {
            return a.getFlightRange() > b.getFlightRange();
        }

        if (a.getManufacturer() != b.getManufacturer()) {
            return a.getManufacturer() < b.getManufacturer();
        }

        return a.getName() < b.getName();
    }
};

// Отдельно описываем равенство объектов
struct AirplaneEqual {
    bool operator()(const Airplane& a, const Airplane& b) const {
        return a.getName() == b.getName()
            && a.getManufacturer() == b.getManufacturer()
            && a.getCapacity() == b.getCapacity()
            && a.getFlightRange() == b.getFlightRange()
            && a.getMaxSpeed() == b.getMaxSpeed();
    }
};

template<class T>
class Node {
private:
    T value;

public:
    Node() = default;

    explicit Node(const T& v)
        : value(v) {
    }

    const T& getValue() const {
        return value;
    }

    void setValue(const T& v) {
        value = v;
    }

    void print() const {
        cout << value;
    }
};

template<class T>
void printNode(const Node<T>* node) {
    cout << node->getValue() << endl;
}

template<
    class T,
    class HigherPriority,
    class Equal
>
class Heap {
private:
    Node<T>* arr;
    int len;
    int size;

    HigherPriority higherPriority;
    Equal equal;

public:
    Heap(
        int memorySize = 100,
        HigherPriority priority = HigherPriority(),
        Equal equality = Equal()
    )
        : arr(nullptr),
          len(0),
          size(memorySize),
          higherPriority(priority),
          equal(equality) {
        if (memorySize <= 0) {
            throw invalid_argument("Heap size must be positive");
        }

        arr = new Node<T>[size];
    }

    ~Heap() {
        delete[] arr;
    }

    Heap(const Heap&) = delete;
    Heap& operator=(const Heap&) = delete;

    int getCapacity() const {
        return size;
    }

    int getCount() const {
        return len;
    }

    Node<T>& operator[](int index) {
        if (index < 0 || index >= len) {
            throw out_of_range("Incorrect heap index");
        }

        return arr[index];
    }

    const Node<T>& operator[](int index) const {
        if (index < 0 || index >= len) {
            throw out_of_range("Incorrect heap index");
        }

        return arr[index];
    }

    void Swap(int index1, int index2) {
        if (
            index1 < 0 || index1 >= len ||
            index2 < 0 || index2 >= len
        ) {
            throw out_of_range("Incorrect index in Swap");
        }

        Node<T> temp = arr[index1];
        arr[index1] = arr[index2];
        arr[index2] = temp;
    }

    int GetLeftChildIndex(int index) const {
        if (index < 0 || index >= len) {
            return -1;
        }

        int left = 2 * index + 1;

        if (left >= len) {
            return -1;
        }

        return left;
    }

    int GetRightChildIndex(int index) const {
        if (index < 0 || index >= len) {
            return -1;
        }

        int right = 2 * index + 2;

        if (right >= len) {
            return -1;
        }

        return right;
    }

    int GetParentIndex(int index) const {
        if (index <= 0 || index >= len) {
            return -1;
        }

        return (index - 1) / 2;
    }

    void SiftUp(int index = -1) {
        if (index == -1) {
            index = len - 1;
        }

        if (index < 0 || index >= len) {
            return;
        }

        int parent = GetParentIndex(index);

        while (
            parent != -1 &&
            higherPriority(
                arr[index].getValue(),
                arr[parent].getValue()
            )
        ) {
            Swap(index, parent);

            index = parent;
            parent = GetParentIndex(index);
        }
    }

    void SiftDown(int index = 0) {
        if (index < 0 || index >= len) {
            return;
        }

        while (true) {
            int left = GetLeftChildIndex(index);
            int right = GetRightChildIndex(index);
            int best = index;

            if (
                left != -1 &&
                higherPriority(
                    arr[left].getValue(),
                    arr[best].getValue()
                )
            ) {
                best = left;
            }

            if (
                right != -1 &&
                higherPriority(
                    arr[right].getValue(),
                    arr[best].getValue()
                )
            ) {
                best = right;
            }

            if (best == index) {
                break;
            }

            Swap(index, best);
            index = best;
        }
    }

    void push(const T& value) {
        if (len >= size) {
            throw overflow_error("Heap is full");
        }

        arr[len].setValue(value);
        len++;

        SiftUp(len - 1);
    }

    T ExtractMax() {
        if (len == 0) {
            throw underflow_error("Heap is empty");
        }

        T maxValue = arr[0].getValue();

        arr[0] = arr[len - 1];
        len--;

        if (len > 0) {
            SiftDown(0);
        }

        return maxValue;
    }

    T RemoveAt(int index) {
        if (index < 0 || index >= len) {
            throw out_of_range("Incorrect index in RemoveAt");
        }

        T removedValue = arr[index].getValue();

        arr[index] = arr[len - 1];
        len--;

        if (index < len) {
            int parent = GetParentIndex(index);

            if (
                parent != -1 &&
                higherPriority(
                    arr[index].getValue(),
                    arr[parent].getValue()
                )
            ) {
                SiftUp(index);
            }
            else {
                SiftDown(index);
            }
        }

        return removedValue;
    }

    bool Remove(const T& value) {
        for (int i = 0; i < len; i++) {
            if (equal(arr[i].getValue(), value)) {
                RemoveAt(i);
                return true;
            }
        }

        return false;
    }

    void Straight(void (*f)(const Node<T>*)) const {
        for (int i = 0; i < len; i++) {
            f(&arr[i]);
        }
    }
};

int main() {
    Heap<
        Airplane,
        AirplanePriority,
        AirplaneEqual
    > airplanes;

    Airplane a1("A320", "Airbus", 180, 6100, 871);
    Airplane a2("A321", "Airbus", 220, 5950, 871);
    Airplane a3("737 MAX 8", "Boeing", 210, 6570, 839);
    Airplane a4("MS-21", "Irkut", 211, 6000, 870);
    Airplane a5("Tu-204", "Tupolev", 210, 4300, 850);
    Airplane a6("CRJ-900", "Bombardier", 90, 2950, 880);

    airplanes.push(a1);
    airplanes.push(a2);
    airplanes.push(a3);
    airplanes.push(a4);
    airplanes.push(a5);
    airplanes.push(a6);

    cout << "Heap as array:" << endl;
    airplanes.Straight(printNode<Airplane>);

    cout << "\nRemove arbitrary element:" << endl;
    cout << "Removed: " << a4 << endl;

    airplanes.Remove(a4);

    cout << "\nHeap after removing arbitrary element:" << endl;
    airplanes.Straight(printNode<Airplane>);

    cout << "\nExtractMax output, descending priority:" << endl;

    while (airplanes.getCount() > 0) {
        cout << airplanes.ExtractMax() << endl;
    }

    return 0;
}
