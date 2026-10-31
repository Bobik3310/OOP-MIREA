#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <exception>
#include <algorithm>

using namespace std;

class KeyExistsException : public exception {
private:
    string message;

public:
    explicit KeyExistsException(const string& msg)
        : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class NotFoundException : public exception {
private:
    string message;

public:
    explicit NotFoundException(const string& msg)
        : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class AirplaneKey {
private:
    string manufacturer;
    string name;

public:
    AirplaneKey(string m = "", string n = "")
        : manufacturer(m), name(n) {}

    string getManufacturer() const {
        return manufacturer;
    }

    string getName() const {
        return name;
    }
};

// Сравнение ключей вынесено из AirplaneKey
struct AirplaneKeyCompare {
    bool operator()(const AirplaneKey& a,
                    const AirplaneKey& b) const {
        if (a.getManufacturer() != b.getManufacturer()) {
            return a.getManufacturer() < b.getManufacturer();
        }

        return a.getName() < b.getName();
    }
};

class AirplaneValue {
private:
    int flightRange;
    int maxSpeed;

public:
    AirplaneValue(int range = 0, int speed = 0)
        : flightRange(range), maxSpeed(speed) {}

    int getFlightRange() const {
        return flightRange;
    }

    int getMaxSpeed() const {
        return maxSpeed;
    }
};

// Сравнение значений передается отдельно
struct AirplaneValueEqual {
    bool operator()(const AirplaneValue& a,
                    const AirplaneValue& b) const {
        return a.getFlightRange() == b.getFlightRange() &&
               a.getMaxSpeed() == b.getMaxSpeed();
    }
};

template <typename Map>
void addWithException(
    Map& tree,
    const typename Map::key_type& key,
    const typename Map::mapped_type& value
) {
    if (tree.find(key) != tree.end()) {
        throw KeyExistsException(
            "Ошибка: элемент с таким ключом уже существует"
        );
    }

    tree.emplace(key, value);
}

template <typename Map>
typename Map::mapped_type findByKey(
    const Map& tree,
    const typename Map::key_type& key
) {
    typename Map::const_iterator it = tree.find(key);

    if (it == tree.end()) {
        throw NotFoundException(
            "Ошибка: элемент с таким ключом не найден"
        );
    }

    return it->second;
}

template <typename Map, typename Equal>
vector<typename Map::key_type> findByValue(
    const Map& tree,
    const typename Map::mapped_type& value,
    Equal equal
) {
    vector<typename Map::key_type> result;

    for (typename Map::const_iterator it = tree.begin();
         it != tree.end(); ++it) {
        if (equal(it->second, value)) {
            result.push_back(it->first);
        }
    }

    if (result.empty()) {
        throw NotFoundException(
            "Ошибка: элемент с таким значением не найден"
        );
    }

    return result;
}

template <typename Map, typename KeyPrinter, typename ValuePrinter>
void printTree(
    const Map& tree,
    KeyPrinter printKey,
    ValuePrinter printValue
) {
    for (typename Map::const_iterator it = tree.begin();
         it != tree.end(); ++it) {
        cout << "Ключ: ";
        printKey(it->first);

        cout << "Значение: ";
        printValue(it->second);

        cout << "----------------------------------\n";
    }
}

template <typename Map, typename Predicate>
Map filter(const Map& tree, Predicate predicate) {
    Map result(tree.key_comp());

    for (typename Map::const_iterator it = tree.begin();
         it != tree.end(); ++it) {
        if (predicate(it->second)) {
            result.insert(*it);
        }
    }

    return result;
}

template <typename Map, typename Equal>
vector<typename Map::mapped_type> uniqueValues(
    const Map& tree,
    Equal equal
) {
    vector<typename Map::mapped_type> result;

    for (typename Map::const_iterator it = tree.begin();
         it != tree.end(); ++it) {
        bool alreadyExists = false;

        for (typename vector<typename Map::mapped_type>::const_iterator jt =
                 result.begin();
             jt != result.end(); ++jt) {
            if (equal(*jt, it->second)) {
                alreadyExists = true;
                break;
            }
        }

        if (!alreadyExists) {
            result.push_back(it->second);
        }
    }

    return result;
}

bool flightRangeMoreThan6000(const AirplaneValue& value) {
    return value.getFlightRange() > 6000;
}

void printKey(const AirplaneKey& key) {
    cout << key.getManufacturer() << " "
         << key.getName() << endl;
}

void printValue(const AirplaneValue& value) {
    cout << "дальность полета: "
         << value.getFlightRange()
         << " км, максимальная скорость: "
         << value.getMaxSpeed()
         << " км/ч" << endl;
}

int main() {
    typedef map<
        AirplaneKey,
        AirplaneValue,
        AirplaneKeyCompare
    > AirplaneMap;

    AirplaneMap airplanes;

    try {
        addWithException(
            airplanes,
            AirplaneKey("Boeing", "737"),
            AirplaneValue(5600, 850)
        );

        addWithException(
            airplanes,
            AirplaneKey("Airbus", "A320"),
            AirplaneValue(6100, 840)
        );

        addWithException(
            airplanes,
            AirplaneKey("Boeing", "787"),
            AirplaneValue(13600, 954)
        );

        addWithException(
            airplanes,
            AirplaneKey("Tupolev", "Tu-204"),
            AirplaneValue(4300, 850)
        );

        addWithException(
            airplanes,
            AirplaneKey("Airbus", "A350"),
            AirplaneValue(15000, 945)
        );

        addWithException(
            airplanes,
            AirplaneKey("Irkut", "MS-21"),
            AirplaneValue(6000, 870)
        );

        cout << "Исходное дерево map:\n";
        printTree(airplanes, printKey, printValue);

        cout << "\nПоиск по ключу Boeing 787:\n";
        AirplaneValue foundValue = findByKey(
            airplanes,
            AirplaneKey("Boeing", "787")
        );
        printValue(foundValue);

        cout << "\nПоиск по значению:\n";
        vector<AirplaneKey> foundKeys = findByValue(
            airplanes,
            AirplaneValue(5600, 850),
            AirplaneValueEqual()
        );

        for (size_t i = 0; i < foundKeys.size(); ++i) {
            printKey(foundKeys[i]);
        }

        cout << "\nСамолеты с дальностью больше 6000 км:\n";
        AirplaneMap filtered = filter(
            airplanes,
            flightRangeMoreThan6000
        );
        printTree(filtered, printKey, printValue);

        cout << "\nУникальные значения:\n";
        vector<AirplaneValue> values = uniqueValues(
            airplanes,
            AirplaneValueEqual()
        );

        for (size_t i = 0; i < values.size(); ++i) {
            printValue(values[i]);
        }

        cout << "\nПроверка повторяющегося ключа:\n";
        addWithException(
            airplanes,
            AirplaneKey("Boeing", "737"),
            AirplaneValue(7000, 900)
        );
    }
    catch (const exception& e) {
        cout << e.what() << endl;
    }

    return 0;
}
