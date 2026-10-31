#include <iostream>
#include <queue>
#include <string>
#include <vector>

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
          maxSpeed(s) {}

    int getCapacity() const {
        return capacity;
    }

    int getFlightRange() const {
        return flightRange;
    }

    const string& getManufacturer() const {
        return manufacturer;
    }

    const string& getName() const {
        return name;
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

// Внешний компаратор приоритета
struct AirplanePriority {
    bool operator()(const Airplane& first,
                    const Airplane& second) const {
        if (first.getCapacity() != second.getCapacity()) {
            return first.getCapacity() < second.getCapacity();
        }

        if (first.getFlightRange() != second.getFlightRange()) {
            return first.getFlightRange() < second.getFlightRange();
        }

        if (first.getManufacturer() != second.getManufacturer()) {
            return first.getManufacturer() > second.getManufacturer();
        }

        return first.getName() > second.getName();
    }
};

template <class T, class Container, class Compare>
void print_queue(
    priority_queue<T, Container, Compare> queue
) {
    while (!queue.empty()) {
        cout << queue.top() << endl;
        queue.pop();
    }
}

int main() {
    priority_queue<
        Airplane,
        vector<Airplane>,
        AirplanePriority
    > airplanes;

    airplanes.push(
        Airplane("A320", "Airbus", 180, 6100, 871)
    );

    airplanes.push(
        Airplane("A321", "Airbus", 220, 5950, 871)
    );

    airplanes.push(
        Airplane("737 MAX 8", "Boeing", 210, 6570, 839)
    );

    airplanes.push(
        Airplane("MS-21", "Irkut", 211, 6000, 870)
    );

    airplanes.push(
        Airplane("Tu-204", "Tupolev", 210, 4300, 850)
    );

    airplanes.push(
        Airplane("CRJ-900", "Bombardier", 90, 2950, 880)
    );

    cout << "Airplanes in descending priority order:"
         << endl;

    print_queue(airplanes);

    return 0;
}
