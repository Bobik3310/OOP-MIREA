#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <exception>

using namespace std;

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
void add(multimap<K, V>& tree, const K& key, const V& value) {
    tree.insert(pair<K, V>(key, value));
}

template <typename K, typename V>
vector<V> findByKey(const multimap<K, V>& tree, const K& key) {
    vector<V> result;

    pair<
        typename multimap<K, V>::const_iterator,
        typename multimap<K, V>::const_iterator
    > range = tree.equal_range(key);

    typename multimap<K, V>::const_iterator it;

    for (it = range.first; it != range.second; it++) {
        result.push_back(it->second);
    }

    if (result.empty()) {
        throw NotFoundException("Ошибка: элементы с таким ключом не найдены");
    }

    return result;
}

template <typename K, typename V>
vector<K> findByValue(const multimap<K, V>& tree, const V& value) {
    vector<K> result;

    typename multimap<K, V>::const_iterator it;

    for (it = tree.begin(); it != tree.end(); it++) {
        if (it->second == value) {
            result.push_back(it->first);
        }
    }

    if (result.empty()) {
        throw NotFoundException("Ошибка: элементы с таким значением не найдены");
    }

    return result;
}

template <typename K, typename V>
void printTree(const multimap<K, V>& tree) {
    typename multimap<K, V>::const_iterator it;

    for (it = tree.begin(); it != tree.end(); it++) {
        cout << "Ключ: " << it->first << endl;
        cout << "Значение: " << it->second << endl;
        cout << "----------------------------------" << endl;
    }
}

template <typename K, typename V, typename Predicate>
multimap<K, V> filter(const multimap<K, V>& tree, Predicate predicate) {
    multimap<K, V> result;

    typename multimap<K, V>::const_iterator it;

    for (it = tree.begin(); it != tree.end(); it++) {
        if (predicate(it->second)) {
            result.insert(*it);
        }
    }

    return result;
}

template <typename K, typename V>
vector<V> uniqueValues(const multimap<K, V>& tree) {
    set<V> uniqueSet;

    typename multimap<K, V>::const_iterator it;

    for (it = tree.begin(); it != tree.end(); it++) {
        uniqueSet.insert(it->second);
    }

    vector<V> result(uniqueSet.begin(), uniqueSet.end());

    return result;
}

template <typename K, typename V>
vector<pair<K, V> > getElementsBySameKey(const multimap<K, V>& tree, const K& key) {
    vector<pair<K, V> > result;

    pair<
        typename multimap<K, V>::const_iterator,
        typename multimap<K, V>::const_iterator
    > range = tree.equal_range(key);

    typename multimap<K, V>::const_iterator it;

    for (it = range.first; it != range.second; it++) {
        result.push_back(*it);
    }

    if (result.empty()) {
        throw NotFoundException("Ошибка: элементы с таким ключом не найдены");
    }

    return result;
}

bool flightRangeMoreThan6000(const AirplaneValue& value) {
    const int threshold = 6000;
    return value.getFlightRange() > threshold;
}

int main() {
    multimap<AirplaneKey, AirplaneValue> airplanes;

    try {
        add(
            airplanes,
            AirplaneKey("Boeing", "737"),
            AirplaneValue(5600, 850)
        );

        add(
            airplanes,
            AirplaneKey("Airbus", "A320"),
            AirplaneValue(6100, 840)
        );

        add(
            airplanes,
            AirplaneKey("Boeing", "787"),
            AirplaneValue(13600, 954)
        );

        add(
            airplanes,
            AirplaneKey("Tupolev", "Tu-204"),
            AirplaneValue(4300, 850)
        );

        add(
            airplanes,
            AirplaneKey("Airbus", "A350"),
            AirplaneValue(15000, 945)
        );

        add(
            airplanes,
            AirplaneKey("Irkut", "MS-21"),
            AirplaneValue(6000, 870)
        );

        add(
            airplanes,
            AirplaneKey("Boeing", "737"),
            AirplaneValue(5800, 855)
        );

        add(
            airplanes,
            AirplaneKey("Boeing", "737"),
            AirplaneValue(5900, 860)
        );

        cout << "Исходное дерево multimap:\n";
        printTree(airplanes);

        cout << "\nПоиск по ключу Boeing 737:\n";
        vector<AirplaneValue> valuesByKey = findByKey(
            airplanes,
            AirplaneKey("Boeing", "737")
        );

        for (int i = 0; i < valuesByKey.size(); i++) {
            cout << valuesByKey[i] << endl;
        }

        cout << "\nПоиск по значению: дальность 5600, скорость 850:\n";
        vector<AirplaneKey> keysByValue = findByValue(
            airplanes,
            AirplaneValue(5600, 850)
        );

        for (int i = 0; i < keysByValue.size(); i++) {
            cout << keysByValue[i] << endl;
        }

        cout << "\nФильтр: самолеты с дальностью полета больше 6000 км:\n";
        multimap<AirplaneKey, AirplaneValue> filtered =
            filter(airplanes, flightRangeMoreThan6000);

        printTree(filtered);

        cout << "\nРазличные значения V:\n";
        vector<AirplaneValue> unique = uniqueValues(airplanes);

        for (int i = 0; i < unique.size(); i++) {
            cout << unique[i] << endl;
        }

        cout << "\nВсе элементы с одинаковым ключом Boeing 737:\n";
        vector<pair<AirplaneKey, AirplaneValue> > sameKeyElements =
            getElementsBySameKey(
                airplanes,
                AirplaneKey("Boeing", "737")
            );

        for (int i = 0; i < sameKeyElements.size(); i++) {
            cout << "Ключ: " << sameKeyElements[i].first << endl;
            cout << "Значение: " << sameKeyElements[i].second << endl;
            cout << "----------------------------------" << endl;
        }
    }
    catch (const NotFoundException& e) {
        cout << e.what() << endl;
    }
    catch (const exception& e) {
        cout << "Неизвестная ошибка: " << e.what() << endl;
    }

    return 0;
}