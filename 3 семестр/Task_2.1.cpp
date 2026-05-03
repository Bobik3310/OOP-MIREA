#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <exception>

using namespace std;

class KeyExistsException : public exception {
private:
    string message;

public:
    KeyExistsException(const string& msg) {
        message = msg;
    }

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class NotFoundException : public exception {
private:
    string message;

public:
    NotFoundException(const string& msg) {
        message = msg;
    }

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class AirplaneKey {
private:
    string manufacturer;
    string name;

public:
    AirplaneKey(string m = "", string n = "") {
        manufacturer = m;
        name = n;
    }

    string getManufacturer() const {
        return manufacturer;
    }

    string getName() const {
        return name;
    }

    bool operator<(const AirplaneKey& other) const {
        if (manufacturer != other.manufacturer) {
            return manufacturer < other.manufacturer;
        }

        return name < other.name;
    }

    bool operator==(const AirplaneKey& other) const {
        return manufacturer == other.manufacturer && name == other.name;
    }

    friend ostream& operator<<(ostream& os, const AirplaneKey& key) {
        os << key.manufacturer << " " << key.name;
        return os;
    }
};

class AirplaneValue {
private:
    int flightRange;
    int maxSpeed;

public:
    AirplaneValue(int r = 0, int s = 0) {
        flightRange = r;
        maxSpeed = s;
    }

    int getFlightRange() const {
        return flightRange;
    }

    int getMaxSpeed() const {
        return maxSpeed;
    }

    bool operator==(const AirplaneValue& other) const {
        return flightRange == other.flightRange &&
               maxSpeed == other.maxSpeed;
    }

    bool operator<(const AirplaneValue& other) const {
        if (flightRange != other.flightRange) {
            return flightRange < other.flightRange;
        }

        return maxSpeed < other.maxSpeed;
    }

    friend ostream& operator<<(ostream& os, const AirplaneValue& value) {
        os << "дальность полета: " << value.flightRange
           << " км, максимальная скорость: " << value.maxSpeed << " км/ч";
        return os;
    }
};

template <typename K, typename V>
void addWithException(map<K, V>& tree, const K& key, const V& value) {
    typename map<K, V>::iterator it = tree.find(key);

    if (it != tree.end()) {
        throw KeyExistsException("Ошибка: элемент с таким ключом уже существует");
    }

    tree.insert(pair<K, V>(key, value));
}

template <typename K, typename V>
V findByKey(const map<K, V>& tree, const K& key) {
    typename map<K, V>::const_iterator it = tree.find(key);

    if (it == tree.end()) {
        throw NotFoundException("Ошибка: элемент с таким ключом не найден");
    }

    return it->second;
}

template <typename K, typename V>
vector<K> findByValue(const map<K, V>& tree, const V& value) {
    vector<K> result;

    typename map<K, V>::const_iterator it;

    for (it = tree.begin(); it != tree.end(); it++) {
        if (it->second == value) {
            result.push_back(it->first);
        }
    }

    if (result.empty()) {
        throw NotFoundException("Ошибка: элемент с таким значением не найден");
    }

    return result;
}

template <typename K, typename V>
void printTree(const map<K, V>& tree) {
    typename map<K, V>::const_iterator it;

    for (it = tree.begin(); it != tree.end(); it++) {
        cout << "Ключ: " << it->first << endl;
        cout << "Значение: " << it->second << endl;
        cout << "----------------------------------" << endl;
    }
}

template <typename K, typename V, typename Predicate>
map<K, V> filter(const map<K, V>& tree, Predicate predicate) {
    map<K, V> result;

    typename map<K, V>::const_iterator it;

    for (it = tree.begin(); it != tree.end(); it++) {
        if (predicate(it->second)) {
            result.insert(*it);
        }
    }

    return result;
}

template <typename K, typename V>
vector<V> uniqueValues(const map<K, V>& tree) {
    set<V> uniqueSet;

    typename map<K, V>::const_iterator it;

    for (it = tree.begin(); it != tree.end(); it++) {
        uniqueSet.insert(it->second);
    }

    vector<V> result(uniqueSet.begin(), uniqueSet.end());

    return result;
}

bool flightRangeMoreThan6000(const AirplaneValue& value) {
    const int threshold = 6000;
    return value.getFlightRange() > threshold;
}

int main() {
    map<AirplaneKey, AirplaneValue> airplanes;

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
        printTree(airplanes);

        cout << "\nПоиск по ключу Boeing 787:\n";
        AirplaneValue foundValue = findByKey(
            airplanes,
            AirplaneKey("Boeing", "787")
        );
        cout << foundValue << endl;

        cout << "\nПоиск по значению: дальность 5600, скорость 850:\n";
        vector<AirplaneKey> foundKeys = findByValue(
            airplanes,
            AirplaneValue(5600, 850)
        );

        for (int i = 0; i < foundKeys.size(); i++) {
            cout << foundKeys[i] << endl;
        }

        cout << "\nФильтр: самолеты с дальностью полета больше 6000 км:\n";
        map<AirplaneKey, AirplaneValue> filtered =
            filter(airplanes, flightRangeMoreThan6000);

        printTree(filtered);

        cout << "\nРазличные значения V:\n";
        vector<AirplaneValue> unique = uniqueValues(airplanes);

        for (int i = 0; i < unique.size(); i++) {
            cout << unique[i] << endl;
        }

        cout << "\nПроверка исключения при добавлении повторяющегося ключа:\n";
        addWithException(
            airplanes,
            AirplaneKey("Boeing", "737"),
            AirplaneValue(7000, 900)
        );
    }
    catch (const KeyExistsException& e) {
        cout << e.what() << endl;
    }
    catch (const NotFoundException& e) {
        cout << e.what() << endl;
    }
    catch (const exception& e) {
        cout << "Неизвестная ошибка: " << e.what() << endl;
    }

    return 0;
}