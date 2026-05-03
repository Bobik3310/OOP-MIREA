#include <iostream>
#include <string>
#include <stdexcept>
#include <cmath>

using namespace std;

// Исключение для ошибки работы с итератором.
class IteratorException : public exception {
private:
    string message;

public:
    IteratorException(const string& msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

// Элемент двусвязного списка.
template <class T>
class Element {
private:
    Element<T>* next;
    Element<T>* prev;
    T field;

public:
    Element(const T& value = T(), Element<T>* next_ptr = NULL, Element<T>* prev_ptr = NULL)
        : next(next_ptr), prev(prev_ptr), field(value) {}

    Element<T>* getNext() const { return next; }
    void setNext(Element<T>* value) { next = value; }

    Element<T>* getPrevious() const { return prev; }
    void setPrevious(Element<T>* value) { prev = value; }

    T& getValue() { return field; }
    const T& getValue() const { return field; }
    void setValue(const T& value) { field = value; }

    template<class U>
    friend ostream& operator<<(ostream& stream, const Element<U>& obj);
};

template<class T>
ostream& operator<<(ostream& stream, const Element<T>& obj) {
    stream << obj.field;
    return stream;
}

// Итератор двусвязного списка.
// Итератор не является полем контейнера: он хранит только указатель на отдельный элемент.
template <class ValueType>
class ListIterator {
private:
    Element<ValueType>* ptr;

public:
    ListIterator() : ptr(NULL) {}
    ListIterator(Element<ValueType>* p) : ptr(p) {}
    ListIterator(const ListIterator& it) : ptr(it.ptr) {}

    Element<ValueType>* getPointer() const {
        return ptr;
    }

    bool operator!=(const ListIterator& other) const {
        return ptr != other.ptr;
    }

    bool operator==(const ListIterator& other) const {
        return ptr == other.ptr;
    }

    Element<ValueType>& operator*() {
        if (ptr == NULL) {
            throw IteratorException("Iterator is not associated with any element");
        }
        return *ptr;
    }

    const Element<ValueType>& operator*() const {
        if (ptr == NULL) {
            throw IteratorException("Iterator is not associated with any element");
        }
        return *ptr;
    }

    // Переход к следующему элементу.
    ListIterator& operator++() {
        if (ptr != NULL) {
            ptr = ptr->getNext();
        }
        return *this;
    }

    ListIterator operator++(int) {
        ListIterator old(*this);
        ++(*this);
        return old;
    }

    // Переход к предыдущему элементу.
    ListIterator& operator--() {
        if (ptr != NULL) {
            ptr = ptr->getPrevious();
        }
        return *this;
    }

    ListIterator operator--(int) {
        ListIterator old(*this);
        --(*this);
        return old;
    }

    ListIterator& operator=(const ListIterator& other) {
        ptr = other.ptr;
        return *this;
    }
};

// Абстрактный базовый класс списка.
template <class T>
class LinkedListParent {
protected:
    Element<T>* head;
    Element<T>* tail;
    int num;

public:
    using value_type = T;

    LinkedListParent() : head(NULL), tail(NULL), num(0) {}

    LinkedListParent(const LinkedListParent& other) = delete;
    LinkedListParent& operator=(const LinkedListParent& other) = delete;

    virtual ~LinkedListParent() {
        clear();
    }

    // Очистка всей памяти, выделенной под элементы списка.
    void clear() {
        Element<T>* current = head;

        while (current != NULL) {
            Element<T>* next = current->getNext();
            delete current;
            current = next;
        }

        head = NULL;
        tail = NULL;
        num = 0;
    }

    virtual int Number() const {
        return num;
    }

    virtual Element<T>* getBegin() const {
        return head;
    }

    virtual Element<T>* getEnd() const {
        return tail;
    }

    virtual ListIterator<T> push(const T& value) = 0;
    virtual T pop() = 0;

    virtual Element<T>* operator[](int index) const {
        if (index < 0 || index >= num) {
            return NULL;
        }

        Element<T>* current = head;
        for (int i = 0; i < index; i++) {
            current = current->getNext();
        }

        return current;
    }
};

// Список с итераторами.
template <class T>
class IteratedLinkedList : public LinkedListParent<T> {
public:
    using value_type = T;

    IteratedLinkedList() : LinkedListParent<T>() {}
    virtual ~IteratedLinkedList() {}

    // begin указывает на первый элемент.
    ListIterator<T> begin() const {
        return ListIterator<T>(this->head);
    }

    // end указывает на позицию после последнего элемента.
    ListIterator<T> end() const {
        return ListIterator<T>(NULL);
    }

    // last нужен для обратного прохода и удаления с конца.
    ListIterator<T> last() const {
        return ListIterator<T>(this->tail);
    }
};

// Переопределенный вывод списка через итераторы.
template<class T>
ostream& operator<<(ostream& stream, const IteratedLinkedList<T>& obj) {
    stream << "Length: " << obj.Number() << "\n";

    int index = 0;
    for (ListIterator<T> it = obj.begin(); it != obj.end(); ++it) {
        stream << "arr[" << index << "] = " << (*it).getValue() << "\n";
        index++;
    }

    return stream;
}

// D для варианта 30.
// 30 - четный вариант, поэтому D является стеком.
// Стек: push добавляет в конец, pop извлекает с конца.
template <class T>
class D : public IteratedLinkedList<T> {
protected:
    // Удаление элемента по итератору. Используется в pop().
    T removeByIterator(ListIterator<T> it) {
        Element<T>* element = it.getPointer();

        if (element == NULL) {
            throw IteratorException("Cannot remove element by empty iterator");
        }

        Element<T>* previous = element->getPrevious();
        Element<T>* next = element->getNext();
        T value = element->getValue();

        if (previous != NULL) {
            previous->setNext(next);
        } else {
            this->head = next;
        }

        if (next != NULL) {
            next->setPrevious(previous);
        } else {
            this->tail = previous;
        }

        delete element;
        this->num--;

        return value;
    }

public:
    using value_type = T;

    D() : IteratedLinkedList<T>() {}

    D(const D& other) : IteratedLinkedList<T>() {
        for (ListIterator<T> it = other.begin(); it != other.end(); ++it) {
            push((*it).getValue());
        }
    }

    D& operator=(const D& other) {
        if (this != &other) {
            this->clear();

            for (ListIterator<T> it = other.begin(); it != other.end(); ++it) {
                push((*it).getValue());
            }
        }

        return *this;
    }

    virtual ~D() {}

    // Для стека добавляем элемент в конец списка.
    virtual ListIterator<T> push(const T& value) override {
        Element<T>* newElement = new Element<T>(value);

        if (this->num == 0) {
            this->head = this->tail = newElement;
        } else {
            newElement->setPrevious(this->tail);
            this->tail->setNext(newElement);
            this->tail = newElement;
        }

        this->num++;
        return ListIterator<T>(newElement);
    }

    // Для стека извлекаем элемент с конца списка.
    // Удаление сделано через итератор на последний элемент.
    virtual T pop() override {
        if (this->num == 0) {
            throw IteratorException("Stack is empty");
        }

        ListIterator<T> it = this->last();
        return removeByIterator(it);
    }
};

// Наследник класса D.
// Переопределяет push так, чтобы список оставался упорядоченным.
// Переопределяет pop с использованием итератора.
template <class T>
class SortedD : public D<T> {
public:
    using value_type = T;

    SortedD() : D<T>() {}

    SortedD(const SortedD& other) : D<T>() {
        for (ListIterator<T> it = other.begin(); it != other.end(); ++it) {
            push((*it).getValue());
        }
    }

    SortedD& operator=(const SortedD& other) {
        if (this != &other) {
            this->clear();

            for (ListIterator<T> it = other.begin(); it != other.end(); ++it) {
                push((*it).getValue());
            }
        }

        return *this;
    }

    virtual ~SortedD() {}

    // Вставка с сохранением упорядоченности.
    // Список хранится от меньшего приоритета к большему.
    // Поэтому максимальный по приоритету элемент находится в конце,
    // и pop() стека достает именно его.
    virtual ListIterator<T> push(const T& value) override {
        if (this->num == 0) {
            return D<T>::push(value);
        }

        // Ищем через итератор первый элемент, перед которым надо вставить value.
        for (ListIterator<T> it = this->begin(); it != this->end(); ++it) {
            Element<T>& current = *it;

            if (value < current.getValue()) {
                Element<T>* currentPtr = it.getPointer();
                Element<T>* previousPtr = currentPtr->getPrevious();
                Element<T>* newElement = new Element<T>(value);

                newElement->setNext(currentPtr);
                newElement->setPrevious(previousPtr);
                currentPtr->setPrevious(newElement);

                if (previousPtr != NULL) {
                    previousPtr->setNext(newElement);
                } else {
                    this->head = newElement;
                }

                this->num++;
                return ListIterator<T>(newElement);
            }
        }

        // Если место не найдено, значит новый элемент самый большой.
        // Для стека это конец списка.
        return D<T>::push(value);
    }

    // Удаление максимального элемента по приоритету.
    // Так как список отсортирован по возрастанию приоритета,
    // максимум находится в конце.
    virtual T pop() override {
        if (this->num == 0) {
            throw IteratorException("Sorted stack is empty");
        }

        ListIterator<T> it = this->last();
        return this->removeByIterator(it);
    }
};

// filter из пункта 1.
// Результат - объект типа D<T>.
template<class T, class Predicate>
D<T> filter(const IteratedLinkedList<T>& source, Predicate predicate) {
    D<T> result;

    for (ListIterator<T> it = source.begin(); it != source.end(); ++it) {
        T value = (*it).getValue();

        if (predicate(value)) {
            result.push(value);
        }
    }

    return result;
}

// Универсальный фильтр.
// SourceList - любой список из нашей иерархии.
// ResultList - любой список-результат, у которого есть push().
// Тип элементов исходного и результирующего списков может отличаться.
template<class ResultList, class SourceList, class Predicate, class Transform>
ResultList universalFilter(const SourceList& source, Predicate predicate, Transform transform) {
    ResultList result;

    for (ListIterator<typename SourceList::value_type> it = source.begin(); it != source.end(); ++it) {
        typename SourceList::value_type value = (*it).getValue();

        if (predicate(value)) {
            result.push(transform(value));
        }
    }

    return result;
}

// Тип T из задания 1.1 для варианта 30: Fraction.
// Предикат: дроби, модуль которых не превосходит некоторое a.
class Fraction {
private:
    int numerator;
    int denominator;

public:
    Fraction(int num = 0, int den = 1) : numerator(num), denominator(den) {
        if (denominator == 0) {
            denominator = 1;
        }
    }

    double value() const {
        return static_cast<double>(numerator) / denominator;
    }

    double absValue() const {
        return fabs(value());
    }

    bool operator<(const Fraction& other) const {
        return value() < other.value();
    }

    friend ostream& operator<<(ostream& stream, const Fraction& f) {
        stream << f.numerator << "/" << f.denominator;
        return stream;
    }
};

// Класс C из задания 1.2 для варианта 30: Самолет.
class Airplane {
private:
    string name;
    string manufacturer;
    int capacity;
    int flightRange;
    int maxSpeed;

public:
    Airplane(string n = "", string m = "", int c = 0, int r = 0, int s = 0)
        : name(n), manufacturer(m), capacity(c), flightRange(r), maxSpeed(s) {}

    string getName() const { return name; }
    string getManufacturer() const { return manufacturer; }
    int getCapacity() const { return capacity; }
    int getFlightRange() const { return flightRange; }
    int getMaxSpeed() const { return maxSpeed; }

    // operator< означает: текущий самолет имеет меньший приоритет.
    // Приоритет варианта 30:
    // 1) вместимость - больше значит выше приоритет;
    // 2) дальность полета - больше значит выше приоритет;
    // 3) производитель - по возрастанию;
    // 4) название - по возрастанию.
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

    friend ostream& operator<<(ostream& stream, const Airplane& airplane) {
        stream << airplane.name
               << ", manufacturer: " << airplane.manufacturer
               << ", capacity: " << airplane.capacity
               << ", range: " << airplane.flightRange
               << ", max speed: " << airplane.maxSpeed;
        return stream;
    }
};

int main() {
    try {
        const double a = 1.5;

        cout << "===== Task 1.3 and 1.4: Fraction + D stack + iterators =====\n";

        SortedD<Fraction> fractions;
        fractions.push(Fraction(3, 4));
        fractions.push(Fraction(5, 2));
        fractions.push(Fraction(-7, 4));
        fractions.push(Fraction(1, 3));
        fractions.push(Fraction(4, 3));

        cout << "Sorted fractions:\n";
        cout << fractions << endl;

        D<Fraction> filteredFractions = filter(fractions, [a](const Fraction& f) {
            return f.absValue() <= a;
        });

        cout << "Filtered fractions, |x| <= " << a << ":\n";
        cout << filteredFractions << endl;

        cout << "Reverse iterator movement from last element:\n";
        ListIterator<Fraction> reverseIt = fractions.last();
        cout << (*reverseIt).getValue() << "\n";
        --reverseIt;
        cout << (*reverseIt).getValue() << "\n\n";

        cout << "Iterator exception demonstration:\n";
        try {
            ListIterator<Fraction> badIterator;
            cout << (*badIterator).getValue() << endl;
        }
        catch (const IteratorException& e) {
            cout << "Caught exception: " << e.what() << "\n\n";
        }

        cout << "===== Airplane + SortedD =====\n";

        SortedD<Airplane> airplanes;
        airplanes.push(Airplane("A320", "Airbus", 180, 6100, 840));
        airplanes.push(Airplane("737-800", "Boeing", 189, 5765, 850));
        airplanes.push(Airplane("MS-21", "Irkut", 211, 6000, 870));
        airplanes.push(Airplane("A321XLR", "Airbus", 220, 8700, 840));
        airplanes.push(Airplane("737 MAX 9", "Boeing", 220, 6570, 839));

        cout << "Sorted airplanes, low priority -> high priority:\n";
        cout << airplanes << endl;

        Airplane best = airplanes.pop();
        cout << "Popped airplane with highest priority:\n";
        cout << best << "\n\n";

        cout << "Airplanes after pop:\n";
        cout << airplanes << endl;

        D<string> longRangeNames = universalFilter<D<string>>(
            airplanes,
            [](const Airplane& airplane) {
                return airplane.getFlightRange() >= 6000;
            },
            [](const Airplane& airplane) {
                return airplane.getManufacturer() + " " + airplane.getName();
            }
        );

        cout << "Universal filter result, names of airplanes with range >= 6000:\n";
        cout << longRangeNames << endl;
    }
    catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}
