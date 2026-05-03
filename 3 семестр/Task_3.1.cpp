#include <iostream>
#include <queue>
#include <string>

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


    bool operator<(const Airplane& other) const {
        if (capacity != other.capacity) {
            return capacity < other.capacity;
        }

        if (flightRange != other.flightRange) {
            return flightRange < other.flightRange;
        }

        // По возрастанию: меньшая строка имеет больший приоритет.
        if (manufacturer != other.manufacturer) {
            return manufacturer > other.manufacturer;
        }

        // По возрастанию: меньшее название имеет больший приоритет.
        return name > other.name;
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

// Вывод очереди в порядке убывания приоритета
template <class T>
void print_queue(priority_queue<T> q) {
    while (!q.empty()) {
        cout << q.top() << endl;
        q.pop();
    }
}

int main() {
    priority_queue<Airplane> airplanes;

    airplanes.push(Airplane("A320", "Airbus", 180, 6100, 871));
    airplanes.push(Airplane("A321", "Airbus", 220, 5950, 871));
    airplanes.push(Airplane("737 MAX 8", "Boeing", 210, 6570, 839));
    airplanes.push(Airplane("MS-21", "Irkut", 211, 6000, 870));
    airplanes.push(Airplane("Tu-204", "Tupolev", 210, 4300, 850));
    airplanes.push(Airplane("CRJ-900", "Bombardier", 90, 2950, 880));

    cout << "Airplanes in descending priority order:" << endl;
    print_queue(airplanes);

    return 0;
}