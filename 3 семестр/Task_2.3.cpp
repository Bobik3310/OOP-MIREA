#include <iostream>
#include <queue>
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

    friend ostream& operator<<(ostream& os, const Airplane& airplane) {
        os << "Название: " << airplane.name
           << ", производитель: " << airplane.manufacturer
           << ", вместимость: " << airplane.capacity
           << ", дальность полета: " << airplane.flightRange
           << ", максимальная скорость: " << airplane.maxSpeed;

        return os;
    }
};

template <typename T>
void print_queue(priority_queue<T> q) {
    while (!q.empty()) {
        cout << q.top() << endl;
        q.pop();
    }

    cout << endl;
}

int main() {
    priority_queue<Airplane> airplanes;

    airplanes.push(Airplane("737", "Boeing", 189, 5600, 850));
    airplanes.push(Airplane("A320", "Airbus", 180, 6100, 840));
    airplanes.push(Airplane("787", "Boeing", 330, 13600, 954));
    airplanes.push(Airplane("Tu-204", "Tupolev", 210, 4300, 850));
    airplanes.push(Airplane("A350", "Airbus", 350, 15000, 945));
    airplanes.push(Airplane("MS-21", "Irkut", 211, 6000, 870));

    airplanes.push(Airplane("777", "Boeing", 350, 15600, 950));
    airplanes.push(Airplane("A330", "Airbus", 350, 13450, 913));

    cout << "Очередь с приоритетами в порядке убывания приоритета:\n\n";

    print_queue(airplanes);

    return 0;
}