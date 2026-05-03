#include <iostream>
#include <list>
#include <string>
#include <stdexcept>

using namespace std;

class Airplane {
private:
    string name;
    string manufacturer;
    int capacity;
    int flightRange;
    int maxSpeed;

public:
    Airplane(string n = "", string m = "", int c = 0, int r = 0, int s = 0) {
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

    friend ostream& operator<<(ostream& os, const Airplane& airplane) {
        os << "Название: " << airplane.name
           << ", производитель: " << airplane.manufacturer
           << ", вместимость: " << airplane.capacity
           << ", дальность полета: " << airplane.flightRange
           << ", максимальная скорость: " << airplane.maxSpeed;

        return os;
    }
};

template<class T>
void push(list<T>& lst, const T& element) {
    typename list<T>::iterator it = lst.begin();

    while (it != lst.end() && (*it > element)) {
        it++;
    }

    lst.insert(it, element);
}

template<class T>
T pop(list<T>& lst) {
    if (lst.empty()) {
        throw runtime_error("Ошибка: список пуст");
    }

    T result = lst.front();
    lst.pop_front();

    return result;
}

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

bool longRangeAirplane(const Airplane& airplane) {
    return airplane.getFlightRange() > 10000;
}

template<class T>
void print(const list<T>& lst) {
    typename list<T>::const_iterator it;

    for (it = lst.begin(); it != lst.end(); it++) {
        cout << *it << endl;
    }
}

int main() {
    list<Airplane> airplanes;

    push(airplanes, Airplane("A320", "Airbus", 180, 6100, 871));
    push(airplanes, Airplane("Boeing 737", "Boeing", 189, 5600, 876));
    push(airplanes, Airplane("A380", "Airbus", 853, 15200, 1020));
    push(airplanes, Airplane("Boeing 747", "Boeing", 660, 13450, 988));
    push(airplanes, Airplane("MC-21", "Irkut", 211, 6400, 870));
    push(airplanes, Airplane("CR929", "CRAIC", 280, 12000, 950));
    push(airplanes, Airplane("A350", "Airbus", 440, 15000, 945));

    cout << "Исходный список, отсортированный по убыванию приоритета:" << endl;
    print(airplanes);

    cout << endl;

    Airplane removed = pop(airplanes);

    cout << "Удален самолет с наибольшим приоритетом:" << endl;
    cout << removed << endl;

    cout << endl;

    cout << "Список после pop():" << endl;
    print(airplanes);

    cout << endl;

    list<Airplane> filtered = filter(airplanes, longRangeAirplane);

    cout << "Самолеты с дальностью полета больше 10000 км:" << endl;
    print(filtered);

    return 0;
}