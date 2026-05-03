#include <iostream>
#include <list>
#include <cmath>

using namespace std;

// Класс дроби
class Fraction {
private:
    int numerator;   // числитель
    int denominator; // знаменатель

    // НОД для сокращения дроби
    int gcd(int a, int b) {
        a = abs(a);
        b = abs(b);

        while (b != 0) {
            int r = a % b;
            a = b;
            b = r;
        }

        return a == 0 ? 1 : a;
    }

    // Нормализация дроби:
    // 1) знаменатель не должен быть отрицательным
    // 2) дробь сокращается
    void normalize() {
        if (denominator == 0) {
            denominator = 1;
        }

        if (denominator < 0) {
            numerator = -numerator;
            denominator = -denominator;
        }

        int d = gcd(numerator, denominator);
        numerator /= d;
        denominator /= d;
    }

public:
    Fraction(int num = 0, int den = 1) {
        numerator = num;
        denominator = den;
        normalize();
    }

    // Значение дроби как вещественного числа
    double value() const {
        return static_cast<double>(numerator) / denominator;
    }

    // Модуль дроби
    double absValue() const {
        return fabs(value());
    }

    // Сравнение дробей по величине
    bool operator<(const Fraction& other) const {
        return numerator * other.denominator < other.numerator * denominator;
    }

    bool operator>(const Fraction& other) const {
        return other < *this;
    }

    bool operator==(const Fraction& other) const {
        return numerator == other.numerator &&
               denominator == other.denominator;
    }

    // Вывод дроби
    friend ostream& operator<<(ostream& os, const Fraction& f) {
        os << f.numerator << "/" << f.denominator;
        return os;
    }
};

// Предикат для варианта 30: дроби, которые по модулю не превосходят некоторое значение a
bool predicate(const Fraction& f) {
    const double a = 1.5;
    return f.absValue() <= a;
}

// Добавление элемента с сохранением сортировки по возрастанию
template<class T>
void push(list<T>& lst, const T& element) {
    typename list<T>::iterator it = lst.begin();

    // Ищем первую позицию, где текущий элемент больше добавляемого
    while (it != lst.end() && *it < element) {
        it++;
    }

    lst.insert(it, element);
}

// Удаление элемента из начала списка
template<class T>
T pop(list<T>& lst) {
    if (lst.empty()) {
        throw runtime_error("Ошибка: нельзя удалить элемент из пустого списка");
    }

    T removed = lst.front();
    lst.pop_front();
    return removed;
}

// Дополнительно: удаление элемента с конца
template<class T>
T popBack(list<T>& lst) {
    if (lst.empty()) {
        throw runtime_error("Ошибка: нельзя удалить элемент из пустого списка");
    }

    T removed = lst.back();
    lst.pop_back();
    return removed;
}

// Дополнительно: удаление элемента из произвольной позиции
template<class T>
T popAt(list<T>& lst, int index) {
    if (lst.empty()) {
        throw runtime_error("Ошибка: нельзя удалить элемент из пустого списка");
    }

    if (index < 0 || index >= static_cast<int>(lst.size())) {
        throw runtime_error("Ошибка: индекс выходит за границы списка");
    }

    typename list<T>::iterator it = lst.begin();

    for (int i = 0; i < index; i++) {
        it++;
    }

    T removed = *it;
    lst.erase(it);
    return removed;
}

// Фильтрация списка по предикату
template<class T>
list<T> filter(const list<T>& lst, bool (*predicate)(const T&)) {
    list<T> result;

    typename list<T>::const_iterator it;

    for (it = lst.begin(); it != lst.end(); it++) {
        if (predicate(*it)) {
            result.push_back(*it);
        }
    }

    return result;
}

// Вывод списка с помощью итераторов
template<class T>
void print(const list<T>& lst) {
    typename list<T>::const_iterator it;

    for (it = lst.begin(); it != lst.end(); it++) {
        cout << *it << " ";
    }

    cout << endl;
}

int main() {
    // Связный список
    list<Fraction> fractions;

    // Добавляем дроби.
    // После каждого push список остается отсортированным.
    push(fractions, Fraction(3, 4));    // 0.75
    push(fractions, Fraction(5, 2));    // 2.5
    push(fractions, Fraction(-7, 3));   // -2.333...
    push(fractions, Fraction(1, 2));    // 0.5
    push(fractions, Fraction(-4, 5));   // -0.8
    push(fractions, Fraction(7, 4));    // 1.75
    push(fractions, Fraction(1, 3));    // 0.333...

    cout << "Исходный отсортированный список:" << endl;
    print(fractions);

    list<Fraction> filtered = filter(fractions, predicate);

    cout << "Список после filter(), условие |x| <= 1.5:" << endl;
    print(filtered);

    Fraction removedFront = pop(fractions);
    cout << "Удалили первый элемент: " << removedFront << endl;

    cout << "Список после pop():" << endl;
    print(fractions);

    Fraction removedBack = popBack(fractions);
    cout << "Удалили последний элемент: " << removedBack << endl;

    cout << "Список после popBack():" << endl;
    print(fractions);

    Fraction removedAt = popAt(fractions, 2);
    cout << "Удалили элемент с индексом 2: " << removedAt << endl;

    cout << "Список после popAt(2):" << endl;
    print(fractions);

    return 0;
}