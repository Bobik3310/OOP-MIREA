#include <iostream>
#include <string>
#include <stdexcept>
#include <cmath>

using namespace std;

class IteratorException : public exception {
private:
    string message;
public:
    IteratorException(const string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

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

template<typename ValueType>
class ListIterator {
private:
    Element<ValueType>* ptr;

public:
    ListIterator() : ptr(NULL) {}
    ListIterator(Element<ValueType>* p) : ptr(p) {}
    ListIterator(const ListIterator& it) : ptr(it.ptr) {}

    bool operator!=(const ListIterator& other) const { return ptr != other.ptr; }
    bool operator==(const ListIterator& other) const { return ptr == other.ptr; }

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

    ListIterator& operator++() {
        if (ptr != NULL) ptr = ptr->getNext();
        return *this;
    }

    ListIterator operator++(int) {
        ListIterator old(*this);
        ++(*this);
        return old;
    }

    ListIterator& operator--() {
        if (ptr != NULL) ptr = ptr->getPrevious();
        return *this;
    }

    ListIterator operator--(int) {
        ListIterator old(*this);
        --(*this);
        return old;
    }
};

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

    virtual int Number() const { return num; }
    virtual Element<T>* getBegin() const { return head; }
    virtual Element<T>* getEnd() const { return tail; }

    virtual ListIterator<T> push(const T& value) = 0;
    virtual T pop() = 0;

    virtual Element<T>* operator[](int i) const {
        if (i < 0 || i >= num) return NULL;
        Element<T>* cur = head;
        for (int k = 0; k < i; k++) {
            cur = cur->getNext();
        }
        return cur;
    }

    template<class U>
    friend ostream& operator<<(ostream& stream, const LinkedListParent<U>& obj);
};

template<class T>
ostream& operator<<(ostream& stream, const LinkedListParent<T>& obj) {
    stream << "Length: " << obj.Number() << "\n";
    int i = 0;
    for (Element<T>* current = obj.getBegin(); current != NULL; current = current->getNext(), i++) {
        stream << "arr[" << i << "] = " << current->getValue() << "\n";
    }
    return stream;
}

template <class T>
class IteratedLinkedList : public LinkedListParent<T> {
public:
    using value_type = T;

    IteratedLinkedList() : LinkedListParent<T>() {}
    virtual ~IteratedLinkedList() {}

    ListIterator<T> begin() const {
        return ListIterator<T>(this->head);
    }

    ListIterator<T> end() const {
        return ListIterator<T>(this->tail);
    }

    void printWithIterators() const {
        if (this->head == NULL) {
            cout << "empty\n";
            return;
        }

        for (ListIterator<T> it = begin(); ; ++it) {
            cout << (*it).getValue() << "\n";
            if (it == end()) break;
        }
    }
};

template <class T>
class Stack : public IteratedLinkedList<T> {
public:
    using value_type = T;

    Stack() : IteratedLinkedList<T>() {}

    Stack(const Stack& other) : IteratedLinkedList<T>() {
        for (Element<T>* current = other.getBegin(); current != NULL; current = current->getNext()) {
            push(current->getValue());
        }
    }

    Stack& operator=(const Stack& other) {
        if (this != &other) {
            this->clear();
            for (Element<T>* current = other.getBegin(); current != NULL; current = current->getNext()) {
                push(current->getValue());
            }
        }
        return *this;
    }

    virtual ~Stack() {}


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


    virtual T pop() override {
        if (this->num == 0) {
            throw IteratorException("Stack is empty");
        }

        Element<T>* last = this->tail;
        T value = last->getValue();

        if (this->num == 1) {
            this->head = this->tail = NULL;
        } else {
            this->tail = last->getPrevious();
            this->tail->setNext(NULL);
        }

        this->num--;
        delete last;
        return value;
    }
};


template <class T>
class SortedStack : public Stack<T> {
public:
    using value_type = T;

    SortedStack() : Stack<T>() {}

    SortedStack(const SortedStack& other) : Stack<T>() {
        for (Element<T>* current = other.getBegin(); current != NULL; current = current->getNext()) {
            push(current->getValue());
        }
    }

    SortedStack& operator=(const SortedStack& other) {
        if (this != &other) {
            this->clear();
            for (Element<T>* current = other.getBegin(); current != NULL; current = current->getNext()) {
                push(current->getValue());
            }
        }
        return *this;
    }

    virtual ListIterator<T> push(const T& value) override {
        Element<T>* newElement = new Element<T>(value);

        if (this->num == 0) {
            this->head = this->tail = newElement;
        } else {
            Element<T>* current = this->head;
            Element<T>* previous = NULL;

            while (current != NULL && current->getValue() < value) {
                previous = current;
                current = current->getNext();
            }

            if (previous == NULL) {
                newElement->setNext(this->head);
                this->head->setPrevious(newElement);
                this->head = newElement;
            } else if (current == NULL) {
                newElement->setPrevious(this->tail);
                this->tail->setNext(newElement);
                this->tail = newElement;
            } else {
                newElement->setPrevious(previous);
                newElement->setNext(current);
                previous->setNext(newElement);
                current->setPrevious(newElement);
            }
        }

        this->num++;
        return ListIterator<T>(newElement);
    }
};


template<class T, class Predicate>
Stack<T> filter(const LinkedListParent<T>& source, Predicate predicate) {
    Stack<T> result;

    for (Element<T>* current = source.getBegin(); current != NULL; current = current->getNext()) {
        const T& value = current->getValue();
        if (predicate(value)) {
            result.push(value);
        }
    }

    return result;
}


template<class ResultList, class SourceList, class Predicate, class Transform>
ResultList universalFilter(const SourceList& source, Predicate predicate, Transform transform) {
    ResultList result;

    for (Element<typename SourceList::value_type>* current = source.getBegin();
         current != NULL;
         current = current->getNext()) {
        const typename SourceList::value_type& value = current->getValue();
        if (predicate(value)) {
            result.push(transform(value));
        }
    }

    return result;
}

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

        cout << "===== Task 1.3: Fraction + Stack =====\n";
        SortedStack<Fraction> fractions;
        fractions.push(Fraction(3, 4));
        fractions.push(Fraction(5, 2));
        fractions.push(Fraction(-7, 4));
        fractions.push(Fraction(1, 3));
        fractions.push(Fraction(4, 3));

        cout << "Sorted fractions:\n" << fractions << endl;

        Stack<Fraction> filteredFractions = filter(fractions, [a](const Fraction& f) {
            return f.absValue() <= a;
        });

        cout << "Filtered fractions, |x| <= " << a << ":\n" << filteredFractions << endl;

        Fraction removedFraction = fractions.pop();
        cout << "Popped from sorted stack: " << removedFraction << "\n";
        cout << "Fractions after pop:\n" << fractions << endl;

        cout << "===== Airplane + SortedStack =====\n";
        SortedStack<Airplane> airplanes;
        airplanes.push(Airplane("A320", "Airbus", 180, 6100, 840));
        airplanes.push(Airplane("737-800", "Boeing", 189, 5765, 850));
        airplanes.push(Airplane("MS-21", "Irkut", 211, 6000, 870));
        airplanes.push(Airplane("A321XLR", "Airbus", 220, 8700, 840));
        airplanes.push(Airplane("737 MAX 9", "Boeing", 220, 6570, 839));

        cout << "Sorted airplanes, low priority -> high priority:\n" << airplanes << endl;

        Airplane best = airplanes.pop();
        cout << "Popped airplane with highest priority:\n" << best << "\n\n";
        cout << "Airplanes after pop:\n" << airplanes << endl;

        Stack<string> longRangeNames = universalFilter<Stack<string>>(
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