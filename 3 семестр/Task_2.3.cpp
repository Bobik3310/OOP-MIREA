#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct Airplane {
    string name;
    string manufacturer;
    int flightRange;
    int maxSpeed;
};

ostream& operator<<(ostream& out, const Airplane& airplane) {
    out << airplane.manufacturer << ' ' << airplane.name
        << ", дальность: " << airplane.flightRange
        << ", скорость: " << airplane.maxSpeed;

    return out;
}

using AirplaneTable = unordered_map<int, Airplane>;

void addAirplane(
    AirplaneTable& table,
    int id,
    const Airplane& airplane
) {
    auto [iterator, inserted] = table.emplace(id, airplane);

    if (!inserted) {
        throw invalid_argument(
            "Самолёт с таким идентификатором уже существует: "
            + to_string(id)
        );
    }
}

void printTable(const AirplaneTable& table) {
    for (const auto& [id, airplane] : table) {
        cout << id << " -> " << airplane << '\n';
    }
}

const Airplane* findById(
    const AirplaneTable& table,
    int id
) {
    auto iterator = table.find(id);

    if (iterator == table.end()) {
        return nullptr;
    }

    return &iterator->second;
}

vector<int> findByValue(
    const AirplaneTable& table,
    int range,
    int speed
) {
    vector<int> result;

    for (const auto& [id, airplane] : table) {
        if (airplane.flightRange == range &&
            airplane.maxSpeed == speed) {
            result.push_back(id);
        }
    }

    return result;
}

template <typename Predicate>
AirplaneTable filter(
    const AirplaneTable& table,
    Predicate predicate
) {
    AirplaneTable result;

    for (const auto& [id, airplane] : table) {
        if (predicate(id, airplane)) {
            result.emplace(id, airplane);
        }
    }

    return result;
}

AirplaneTable filterById(
    const AirplaneTable& table,
    int threshold
) {
    return filter(
        table,
        [threshold](int id, const Airplane&) {
            return id > threshold;
        }
    );
}

vector<pair<int, int>> getUniqueValues(
    const AirplaneTable& table
) {
    set<pair<int, int>> uniqueValues;

    for (const auto& [id, airplane] : table) {
        uniqueValues.emplace(
            airplane.flightRange,
            airplane.maxSpeed
        );
    }

    return {
        uniqueValues.begin(),
        uniqueValues.end()
    };
}

int main() {
    AirplaneTable airplanes;

    addAirplane(airplanes, 1,
        {"737", "Boeing", 5600, 850});

    addAirplane(airplanes, 2,
        {"A320", "Airbus", 6100, 840});

    addAirplane(airplanes, 3,
        {"787", "Boeing", 13600, 954});

    addAirplane(airplanes, 4,
        {"A350", "Airbus", 15000, 945});

    addAirplane(airplanes, 5,
        {"MS-21", "Irkut", 6000, 870});

    cout << "Исходная таблица:\n";
    printTable(airplanes);

    cout << "\nПоиск по идентификатору:\n";

    const Airplane* found = findById(airplanes, 3);

    if (found != nullptr) {
        cout << *found << '\n';
    } else {
        cout << "Самолёт не найден\n";
    }

    cout << "\nПоиск по значению:\n";

    vector<int> foundIds = findByValue(
        airplanes,
        5600,
        850
    );

    for (int id : foundIds) {
        cout << "Идентификатор: " << id << '\n';
    }

    cout << "\nФильтрация:\n";

    AirplaneTable filtered = filterById(airplanes, 2);
    printTable(filtered);

    cout << "\nРазличные значения:\n";

    vector<pair<int, int>> values =
        getUniqueValues(airplanes);

    for (const auto& [range, speed] : values) {
        cout << "Дальность: " << range
             << ", скорость: " << speed << '\n';
    }

    cout << "\nПроверка дубликата:\n";

    try {
        addAirplane(
            airplanes,
            3,
            {"787", "Boeing", 13600, 954}
        );
    } catch (const invalid_argument& error) {
        cout << error.what() << '\n';
    }

    return 0;
}
