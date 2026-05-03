#include <iostream>
#include <string>
#include <stdexcept>

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

    bool operator==(const Airplane& other) const {
        return name == other.name
            && manufacturer == other.manufacturer
            && capacity == other.capacity
            && flightRange == other.flightRange
            && maxSpeed == other.maxSpeed;
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

template <class T>
class Node {
private:
    T value;

public:
    Node(T v = T()) {
        value = v;
    }

    const T& getValue() const {
        return value;
    }

    void setValue(const T& v) {
        value = v;
    }

    bool operator<(const Node<T>& other) const {
        return value < other.value;
    }

    bool operator>(const Node<T>& other) const {
        return value > other.value;
    }

    void print() const {
        cout << value;
    }
};

template <class T>
void printNode(const Node<T>* node) {
    cout << node->getValue() << endl;
}

template <class T>
class Heap {
private:
    Node<T>* arr; // массив узлов
    int len;      // сколько элементов добавлено
    int size;     // сколько памяти выделено

public:
    Heap(int memorySize = 100) {
        arr = new Node<T>[memorySize];
        len = 0;
        size = memorySize;
    }

    ~Heap() {
        delete[] arr;
    }

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

    void Swap(int index1, int index2) {
        if (index1 < 0 || index1 >= len || index2 < 0 || index2 >= len) {
            throw out_of_range("Incorrect index in Swap");
        }

        Node<T> temp = arr[index1];
        arr[index1] = arr[index2];
        arr[index2] = temp;
    }

    int GetLeftChildIndex(int index) const {
        int left = 2 * index + 1;

        if (index < 0 || left >= len) {
            return -1;
        }

        return left;
    }

    int GetRightChildIndex(int index) const {
        int right = 2 * index + 2;

        if (index < 0 || right >= len) {
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

    // Просеивание вверх
    void SiftUp(int index = -1) {
        if (index == -1) {
            index = len - 1;
        }

        int parent = GetParentIndex(index);

        while (parent != -1 && arr[index] > arr[parent]) {
            Swap(index, parent);
            index = parent;
            parent = GetParentIndex(index);
        }
    }

    // Просеивание вниз
    void SiftDown(int index = 0) {
        while (true) {
            int left = GetLeftChildIndex(index);
            int right = GetRightChildIndex(index);
            int maxIndex = index;

            if (left != -1 && arr[left] > arr[maxIndex]) {
                maxIndex = left;
            }

            if (right != -1 && arr[right] > arr[maxIndex]) {
                maxIndex = right;
            }

            if (maxIndex == index) {
                break;
            }

            Swap(index, maxIndex);
            index = maxIndex;
        }
    }

    // Добавление элемента: кладем в конец и просеиваем вверх
    void push(const T& value) {
        if (len >= size) {
            throw overflow_error("Heap is full");
        }

        arr[len].setValue(value);
        len++;

        SiftUp(len - 1);
    }

    // Удаление корня, то есть максимального элемента
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

    // Удаление произвольного элемента по индексу
    T RemoveAt(int index) {
        if (index < 0 || index >= len) {
            throw out_of_range("Incorrect index in RemoveAt");
        }

        T removedValue = arr[index].getValue();

        arr[index] = arr[len - 1];
        len--;

        if (index < len) {
            int parent = GetParentIndex(index);

            if (parent != -1 && arr[index] > arr[parent]) {
                SiftUp(index);
            } else {
                SiftDown(index);
            }
        }

        return removedValue;
    }

    // Удаление произвольного элемента по значению
    bool Remove(const T& value) {
        for (int i = 0; i < len; i++) {
            if (arr[i].getValue() == value) {
                RemoveAt(i);
                return true;
            }
        }

        return false;
    }

    // Прямой просмотр массива кучи
    void Straight(void (*f)(const Node<T>*)) const {
        for (int i = 0; i < len; i++) {
            f(&arr[i]);
        }
    }
};

int main() {
    Heap<Airplane> airplanes;

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