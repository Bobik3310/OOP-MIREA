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
    explicit NotFoundException(const string& msg)
        : message(msg) {
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
    AirplaneKey(const string& manufacturer = "",
                const string& name = "")
        : manufacturer(manufacturer),
          name(name) {
    }

    string getManufacturer() const {
        return manufacturer;
    }

    string getName() const {
        return name;
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
    AirplaneValue(int flightRange = 0,
                  int maxSpeed = 0)
        : flightRange(flightRange),
          maxSpeed(maxSpeed) {
    }

    int getFlightRange() const {
        return flightRange;
    }

    int getMaxSpeed() const {
        return maxSpeed;
    }

    friend ostream& operator<<(ostream& os,
                               const AirplaneValue& value) {
        os << "дальность полета: "
           << value.flightRange
           << " км, максимальная скорость: "
           << value.maxSpeed
           << " км/ч";

        return os;
    }
};

// Компаратор ключей
struct AirplaneKeyCompare {
    bool operator()(const AirplaneKey& left,
                    const AirplaneKey& right) const {
        if (left.getManufacturer() != right.getManufacturer()) {
            return left.getManufacturer() <
                   right.getManufacturer();
        }

        return left.getName() < right.getName();
    }
};

// Компаратор значений
struct AirplaneValueCompare {
    bool operator()(const AirplaneValue& left,
                    const AirplaneValue& right) const {
        if (left.getFlightRange() != right.getFlightRange()) {
            return left.getFlightRange() <
                   right.getFlightRange();
        }

        return left.getMaxSpeed() <
               right.getMaxSpeed();
    }
};

// Проверка равенства значений
struct AirplaneValueEqual {
    bool operator()(const AirplaneValue& left,
                    const AirplaneValue& right) const {
        return left.getFlightRange() ==
                   right.getFlightRange()
            && left.getMaxSpeed() ==
                   right.getMaxSpeed();
    }
};

template <typename K, typename V, typename Compare>
void add(
    multimap<K, V, Compare>& tree,
    const K& key,
    const V& value
) {
    tree.insert(make_pair(key, value));
}

template <typename K, typename V, typename Compare>
vector<V> findByKey(
    const multimap<K, V, Compare>& tree,
    const K& key
) {
    vector<V> result;

    pair<
        typename multimap<K, V, Compare>::const_iterator,
        typename multimap<K, V, Compare>::const_iterator
    > range = tree.equal_range(key);

    for (typename multimap<K, V, Compare>::const_iterator it =
             range.first;
         it != range.second;
         ++it) {
        result.push_back(it->second);
    }

    if (result.empty()) {
        throw NotFoundException(
            "Ошибка: элементы с таким ключом не найдены"
        );
    }

    return result;
}

template <typename K, typename V, typename Compare, typename Equal>
vector<K> findByValue(
    const multimap<K, V, Compare>& tree,
    const V& value,
    Equal equal
) {
    vector<K> result;

    for (typename multimap<K, V, Compare>::const_iterator it =
             tree.begin();
         it != tree.end();
         ++it) {
        if (equal(it->second, value)) {
            result.push_back(it->first);
        }
    }

    if (result.empty()) {
        throw NotFoundException(
            "Ошибка: элементы с таким значением не найдены"
        );
    }

    return result;
}

template <typename K, typename V, typename Compare>
void printTree(
    const multimap<K, V, Compare>& tree
) {
    for (typename multimap<K, V, Compare>::const_iterator it =
             tree.begin();
         it != tree.end();
         ++it) {
        cout << "Ключ: " << it->first << endl;
        cout << "Значение: " << it->second << endl;
        cout << "----------------------------------" << endl;
    }
}

template <typename K, typename V, typename Compare, typename Predicate>
multimap<K, V, Compare> filter(
    const multimap<K, V, Compare>& tree,
    Predicate predicate
) {
    multimap<K, V, Compare> result;

    for (typename multimap<K, V, Compare>::const_iterator it =
             tree.begin();
         it != tree.end();
         ++it) {
        if (predicate(it->second)) {
            result.insert(*it);
        }
    }

    return result;
}

template <
    typename K,
    typename V,
    typename KeyCompare,
    typename ValueCompare
>
vector<V> uniqueValues(
    const multimap<K, V, KeyCompare>& tree,
    ValueCompare valueCompare
) {
    set<V, ValueCompare> uniqueSet(valueCompare);

    for (typename multimap<K, V, KeyCompare>::const_iterator it =
             tree.begin();
         it != tree.end();
         ++it) {
        uniqueSet.insert(it->second);
    }

    return vector<V>(
        uniqueSet.begin(),
        uniqueSet.end()
    );
}

template <typename K, typename V, typename Compare>
vector<pair<K, V> > getElementsBySameKey(
    const multimap<K, V, Compare>& tree,
    const K& key
) {
    vector<pair<K, V> > result;

    pair<
        typename multimap<K, V, Compare>::const_iterator,
        typename multimap<K, V, Compare>::const_iterator
    > range = tree.equal_range(key);

    for (typename multimap<K, V, Compare>::const_iterator it =
             range.first;
         it != range.second;
         ++it) {
        result.push_back(*it);
    }

    if (result.empty()) {
        throw NotFoundException(
            "Ошибка: элементы с таким ключом не найдены"
        );
    }

    return result;
}

bool flightRangeMoreThan6000(
    const AirplaneValue& value
) {
    return value.getFlightRange() > 6000;
}

int main() {
    typedef multimap<
        AirplaneKey,
        AirplaneValue,
        AirplaneKeyCompare
    > AirplaneTree;

    AirplaneTree airplanes;

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

        vector<AirplaneValue> valuesByKey =
            findByKey(
                airplanes,
                AirplaneKey("Boeing", "737")
            );

        for (size_t i = 0; i < valuesByKey.size(); ++i) {
            cout << valuesByKey[i] << endl;
        }

        cout << "\nПоиск по значению:\n";

        vector<AirplaneKey> keysByValue =
            findByValue(
                airplanes,
                AirplaneValue(5600, 850),
                AirplaneValueEqual()
            );

        for (size_t i = 0; i < keysByValue.size(); ++i) {
            cout << keysByValue[i] << endl;
        }

        cout << "\nФильтр: дальность больше 6000 км:\n";

        AirplaneTree filtered =
            filter(
                airplanes,
                flightRangeMoreThan6000
            );

        printTree(filtered);

        cout << "\nРазличные значения V:\n";

        vector<AirplaneValue> unique =
            uniqueValues(
                airplanes,
                AirplaneValueCompare()
            );

        for (size_t i = 0; i < unique.size(); ++i) {
            cout << unique[i] << endl;
        }

        cout << "\nВсе элементы с ключом Boeing 737:\n";

        vector<pair<AirplaneKey, AirplaneValue> >
            sameKeyElements =
            getElementsBySameKey(
                airplanes,
                AirplaneKey("Boeing", "737")
            );

        for (size_t i = 0;
             i < sameKeyElements.size();
             ++i) {
            cout << "Ключ: "
                 << sameKeyElements[i].first
                 << endl;

            cout << "Значение: "
                 << sameKeyElements[i].second
                 << endl;

            cout << "----------------------------------"
                 << endl;
        }
    }
    catch (const NotFoundException& e) {
        cout << e.what() << endl;
    }
    catch (const exception& e) {
        cout << "Неизвестная ошибка: "
             << e.what()
             << endl;
    }

    return 0;
}
