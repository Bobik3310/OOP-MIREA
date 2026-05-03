#include <iostream>
#include <string>

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
        return other > *this;
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

template <class T>
class Node {
private:
    T value;

public:
    Node(T v = T()) {
        value = v;
    }

    T getValue() const {
        return value;
    }

    void setValue(T v) {
        value = v;
    }

    bool operator<(const Node<T>& other) const {
        return value < other.value;
    }

    bool operator>(const Node<T>& other) const {
        return value > other.value;
    }

    bool operator==(const Node<T>& other) const {
        return value == other.value;
    }

    void print() const {
        cout << value << endl;
    }
};

template <class T>
class Heap {
private:
    Node<T>* arr;
    int len;
    int size;

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
            throw out_of_range("Индекс выходит за границы кучи");
        }

        return arr[index];
    }

    int GetLeftChildIndex(int index) const {
        int left = 2 * index + 1;

        if (left >= len) {
            return -1;
        }

        return left;
    }

    int GetRightChildIndex(int index) const {
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

    void Swap(int index1, int index2) {
        if (index1 < 0 || index1 >= len || index2 < 0 || index2 >= len) {
            throw out_of_range("Ошибка обмена: индекс выходит за границы кучи");
        }

        Node<T> temp = arr[index1];
        arr[index1] = arr[index2];
        arr[index2] = temp;
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
        while (index >= 0 && index < len) {
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

    void push(T value) {
        if (len >= size) {
            throw overflow_error("Куча переполнена");
        }

        arr[len].setValue(value);
        len++;
        SiftUp();
    }

    T ExtractMax() {
        if (len == 0) {
            throw underflow_error("Нельзя извлечь максимум: куча пуста");
        }

        T maxValue = arr[0].getValue();

        arr[0] = arr[len - 1];
        len--;

        if (len > 0) {
            SiftDown(0);
        }

        return maxValue;
    }

    bool Remove(T value) {
        int index = -1;

        for (int i = 0; i < len; i++) {
            if (arr[i].getValue() == value) {
                index = i;
                break;
            }
        }

        if (index == -1) {
            return false;
        }

        arr[index] = arr[len - 1];
        len--;

        if (index < len) {
            int parent = GetParentIndex(index);

            if (parent != -1 && arr[index] > arr[parent]) {
                SiftUp(index);
            }
            else {
                SiftDown(index);
            }
        }

        return true;
    }

    void printAsArray() const {
        for (int i = 0; i < len; i++) {
            arr[i].print();
        }
    }
};

int main() {
    Heap<Airplane> heap;

    heap.push(Airplane("737", "Boeing", 189, 5600, 850));
    heap.push(Airplane("A320", "Airbus", 180, 6100, 840));
    heap.push(Airplane("787", "Boeing", 330, 13600, 954));
    heap.push(Airplane("Tu-204", "Tupolev", 210, 4300, 850));
    heap.push(Airplane("A350", "Airbus", 350, 15000, 945));
    heap.push(Airplane("MS-21", "Irkut", 211, 6000, 870));
    heap.push(Airplane("777", "Boeing", 350, 15600, 950));
    heap.push(Airplane("A330", "Airbus", 350, 13450, 913));

    cout << "Куча как массив после добавления элементов:\n\n";
    heap.printAsArray();

    cout << "\nУдаление произвольного элемента Airbus A330:\n\n";

    bool removed = heap.Remove(
        Airplane("A330", "Airbus", 350, 13450, 913)
    );

    if (removed) {
        cout << "Элемент удален.\n";
    }
    else {
        cout << "Элемент не найден.\n";
    }

    cout << "\nЭлементы Heap в порядке убывания приоритета:\n\n";

    while (heap.getCount() > 0) {
        Airplane maxAirplane = heap.ExtractMax();
        cout << maxAirplane << endl;
    }

    return 0;
}