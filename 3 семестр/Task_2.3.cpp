#include <functional>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
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
};

using AirplaneTable = unordered_map<int, Airplane>;

template<class Table, class Key, class Value>
void addEntry(Table& table, Key&& key, Value&& value) {
    auto [iterator, inserted] = table.emplace(
        forward<Key>(key),
        forward<Value>(value)
    );

    if (!inserted) {
        ostringstream message;
        message << "Запись с таким ключом уже существует: "
                << key;

        throw invalid_argument(message.str());
    }
}

template<class Table>
void printTable(const Table& table) {
    for (const auto& [key, value] : table) {
        cout << key << " -> " << value << '\n';
    }
}

template<class Table, class Key>
const typename Table::mapped_type*
findByKey(const Table& table, const Key& key) {
    auto iterator = table.find(key);

    if (iterator == table.end()) {
        return nullptr;
    }

    return &iterator->second;
}

template<class Table, class Predicate>
vector<typename Table::key_type>
findBy(const Table& table, Predicate predicate) {
    vector<typename Table::key_type> result;

    for (const auto& [key, value] : table) {
        if (predicate(key, value)) {
            result.push_back(key);
        }
    }

    return result;
}

template<class Table, class Predicate>
Table filter(const Table& table, Predicate predicate) {
    Table result;

    for (const auto& [key, value] : table) {
        if (predicate(key, value)) {
            result.emplace(key, value);
        }
    }

    return result;
}

template<class Table, class Threshold>
Table filterByKeyGreaterThan(
    const Table& table,
    const Threshold& threshold
) {
    return filter(
        table,
        [&threshold](const auto& key, const auto&) {
            return key > threshold;
        }
    );
}

template<class Table, class Projection>
auto getUniqueValues(
    const Table& table,
    Projection projection
) {
    using Mapped = typename Table::mapped_type;

    using ProjectedValue = decay_t<
        invoke_result_t<Projection, const Mapped&>
    >;

    set<ProjectedValue> uniqueValues;

    for (const auto& [key, value] : table) {
        uniqueValues.insert(projection(value));
    }

    return vector<ProjectedValue>(
        uniqueValues.begin(),
        uniqueValues.end()
    );
}

int main() {
    AirplaneTable airplanes;

    addEntry(airplanes, 1,
        Airplane{"737", "Boeing", 5600, 850});

    addEntry(airplanes, 2,
        Airplane{"A320", "Airbus", 6100, 840});

    addEntry(airplanes, 3,
        Airplane{"787", "Boeing", 13600, 954});

    addEntry(airplanes, 4,
        Airplane{"A350", "Airbus", 15000, 945});

    addEntry(airplanes, 5,
        Airplane{"MS-21", "Irkut", 6000, 870});

    cout << "Исходная таблица:\n";
    printTable(airplanes);

    cout << "\nПоиск по ключу:\n";

    const Airplane* found = findByKey(airplanes, 3);

    if (found != nullptr) {
        cout << *found << '\n';
    } else {
        cout << "Самолёт не найден\n";
    }

    cout << "\nПоиск по значению:\n";

    auto foundIds = findBy(
        airplanes,
        [](int, const Airplane& airplane) {
            return airplane.flightRange == 5600 &&
                   airplane.maxSpeed == 850;
        }
    );

    for (int id : foundIds) {
        cout << "Идентификатор: " << id << '\n';
    }

    cout << "\nФильтрация:\n";

    auto filtered = filterByKeyGreaterThan(airplanes, 2);
    printTable(filtered);

    cout << "\nРазличные значения:\n";

    auto values = getUniqueValues(
        airplanes,
        [](const Airplane& airplane) {
            return pair{
                airplane.flightRange,
                airplane.maxSpeed
            };
        }
    );

    for (const auto& [range, speed] : values) {
        cout << "Дальность: " << range
             << ", скорость: " << speed << '\n';
    }

    cout << "\nПроверка дубликата:\n";

    try {
        addEntry(
            airplanes,
            3,
            Airplane{"787", "Boeing", 13600, 954}
        );
    } catch (const invalid_argument& error) {
        cout << error.what() << '\n';
    }

    return 0;
}
