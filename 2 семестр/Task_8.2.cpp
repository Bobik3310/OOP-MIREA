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
        if (capacity != other.capacity) {
            return capacity < other.capacity;
        }

        if (flightRange != other.flightRange) {
            return flightRange < other.flightRange;
        }

        if (manufacturer != other.manufacturer) {
            return manufacturer > other.manufacturer;
        }

        return name > other.name;
    }

    bool operator==(const Airplane& other) const {
        return name == other.name &&
               manufacturer == other.manufacturer &&
               capacity == other.capacity &&
               flightRange == other.flightRange &&
               maxSpeed == other.maxSpeed;
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

// узел дерева
template<class T>
class Node {
private:
    T value;

public:
    Node(T v = T()) {
        value = v;
    }

    // установить данные в узле
    T getValue() const {
        return value;
    }

    void setValue(T v) {
        value = v;
    }

    //сравнение узлов 
    bool operator<(const Node<T>& other) const {
        return value < other.value;
    }

    bool operator>(const Node<T>& other) const {
        return value > other.value;
    }

    bool operator==(const Node<T>& other) const {
        return value == other.value;
    }

    // вывод содержимого одного узла
    void print() const {
        cout << value;
    }
};

template<class T>
void print(Node<T>* node) {
    if (node != NULL) {
        cout << node->getValue() << "\n";
    }
}

//куча (heap)
template<class T>
class Heap {
private:
    Node<T>* arr; // массив узлов
    int len;      // сколько элементов добавлено
    int size;     // сколько памяти выделено

public:
    Heap(int MemorySize = 100) {
        if (MemorySize <= 0) {
            throw invalid_argument("Heap size must be positive");
        }

        arr = new Node<T>[MemorySize];
        len = 0;
        size = MemorySize;
    }

    // Деструктор
    ~Heap() {
        delete[] arr;
        arr = NULL;
        len = 0;
        size = 0;
    }

    //доступ к вспомогательным полям кучи и оператор индекса
    int getCapacity() const {
        return size;
    }

    int getCount() const {
        return len;
    }

    Node<T>& operator[](int index) {
        if (index < 0 || index >= len) {
            throw out_of_range("Index out of range");
        }

        return arr[index];
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

    void Swap(int index1, int index2) {
        if (index1 < 0 || index1 >= len || index2 < 0 || index2 >= len) {
            throw out_of_range("Swap index out of range");
        }

        Node<T> temp = arr[index1];
        arr[index1] = arr[index2];
        arr[index2] = temp;
    }

    void Copy(Node<T>* dest, Node<T>* source) {
        if (dest == NULL || source == NULL) {
            return;
        }

        dest->setValue(source->getValue());
    }

    void SiftUp(int index = -1) {
        if (len == 0) {
            return;
        }

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

    void SiftDown(int index = 0) {
        if (index < 0 || index >= len) {
            return;
        }

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

    void Heapify() {
        for (int i = len / 2 - 1; i >= 0; i--) {
            SiftDown(i);
        }
    }

    void push(Node<T>* node) {
        if (node == NULL) {
            return;
        }

        if (len >= size) {
            throw overflow_error("Heap is full");
        }

        Copy(&arr[len], node);
        len++;

        SiftUp();
    }

    void push(T value) {
        Node<T> node(value);

        if (len >= size) {
            throw overflow_error("Heap is full");
        }

        arr[len] = node;
        len++;

        SiftUp();
    }

    Node<T> ExtractMax() {
        if (len == 0) {
            throw underflow_error("Heap is empty");
        }

        Node<T> maxNode = arr[0];

        arr[0] = arr[len - 1];
        len--;

        if (len > 0) {
            SiftDown(0);
        }

        return maxNode;
    }

    int IndexOf(const T& value) const {
        for (int i = 0; i < len; i++) {
            if (arr[i].getValue() == value) {
                return i;
            }
        }

        return -1;
    }

    bool Remove(const T& value) {
        int index = IndexOf(value);

        if (index == -1) {
            return false;
        }

        arr[index] = arr[len - 1];
        len--;

        if (index < len) {
            SiftDown(index);
            SiftUp(index);
        }

        return true;
    }

    void Straight(void (*f)(Node<T>*)) {
        for (int i = 0; i < len; i++) {
            f(&arr[i]);
        }
    }

    void PrintDescending() {
        Heap<T> copy(size);

        for (int i = 0; i < len; i++) {
            copy.push(arr[i].getValue());
        }

        while (copy.getCount() > 0) {
            Node<T> current = copy.ExtractMax();
            current.print();
            cout << "\n";
        }
    }
};

int main() {
    try {
        Heap<Airplane> heap(20);

        Airplane a1("A320", "Airbus", 180, 6100, 890);
        Airplane a2("737", "Boeing", 189, 5600, 850);
        Airplane a3("A350", "Airbus", 350, 15000, 945);
        Airplane a4("Superjet", "Sukhoi", 98, 4500, 870);
        Airplane a5("777", "Boeing", 396, 15600, 905);
        Airplane a6("MC-21", "Irkut", 211, 6400, 870);
        Airplane a7("Tu-204", "Tupolev", 210, 6500, 850);

        heap.push(a1);
        heap.push(a2);
        heap.push(a3);
        heap.push(a4);
        heap.push(a5);
        heap.push(a6);
        heap.push(a7);

        cout << "========== Internal heap array ==========\n";
        void (*fptr)(Node<Airplane>*) = print;
        heap.Straight(fptr);




        cout << "\n========== Elements by descending priority ==========\n";
        heap.PrintDescending();




        cout << "\n========== ExtractMax ==========\n";
        Node<Airplane> maxNode = heap.ExtractMax();
        cout << "Extracted: ";
        maxNode.print();
        cout << "\n";




        cout << "\n========== Heap after ExtractMax ==========\n";
        heap.Straight(fptr);




        cout << "\n========== Remove arbitrary element ==========\n";
        bool removed = heap.Remove(a3);




        if (removed) {
            cout << "Element removed\n";
        }
        else {
            cout << "Element not found\n";
        }

        cout << "\n========== Heap after Remove ==========\n";
        heap.Straight(fptr);



        
        cout << "\n========== Final descending output ==========\n";
        heap.PrintDescending();
    }
    catch (const exception& e) {
        cout << "Error: " << e.what() << "\n";
    }

    return 0;
}