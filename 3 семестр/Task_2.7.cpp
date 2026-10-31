#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <list>
#include <string>
#include <utility>
#include <vector>

template<
    class Key,
    class Value,
    class HashKey = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>
>
class HashTable {
    using Item = std::pair<Key, Value>;

    static constexpr double phi = 0.6180339887498948482;

    std::vector<std::list<Item>> buckets_;
    HashKey hash_key_{};
    KeyEqual equal_{};

    std::size_t index(const Key& key) const {
        const std::uint64_t hash =
            static_cast<std::uint64_t>(hash_key_(key));

        const auto raw = static_cast<std::size_t>(
            std::floor(
                static_cast<double>(buckets_.size()) *
                static_cast<double>(hash) *
                phi
            )
        );

        return raw % buckets_.size();
    }

public:
    explicit HashTable(std::size_t size = 101)
        : buckets_(size == 0 ? 1 : size) {}

    void add(const Key& key, const Value& value) {
        buckets_[index(key)].emplace_back(key, value);
    }

    std::vector<Value> find(const Key& key) const {
        std::vector<Value> result;

        for (const auto& item : buckets_[index(key)]) {
            if (equal_(item.first, key)) {
                result.push_back(item.second);
            }
        }

        return result;
    }

    std::vector<Value> operator[](const Key& key) const {
        return find(key);
    }

    bool erase(const Key& key) {
        auto& bucket = buckets_[index(key)];

        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (equal_(it->first, key)) {
                bucket.erase(it);
                return true;
            }
        }

        return false;
    }

    template<class PrintValue>
    void print(std::ostream& os, PrintValue print_value) const {
        for (std::size_t i = 0; i < buckets_.size(); ++i) {
            for (const auto& item : buckets_[i]) {
                os << '[' << i << "] ";
                print_value(os, item.second);
                os << '\n';
            }
        }
    }
};

struct AirplaneKey {
    std::string manufacturer;
    std::string name;

    bool operator==(const AirplaneKey& other) const {
        return manufacturer == other.manufacturer &&
               name == other.name;
    }
};

struct Airplane {
    std::string name;
    std::string manufacturer;
    int capacity{};
    double range{};
    double max_speed{};
};

namespace std {

template<>
struct hash<AirplaneKey> {
    static std::uint64_t ascii_sum(const std::string& text) {
        std::uint64_t result = 0;

        for (unsigned char ch : text) {
            result += ch;
        }

        return result;
    }

    std::size_t operator()(const AirplaneKey& key) const {
        // Вариант 30:
        // h(K) = сумма ASCII-кодов производителя и названия
        return static_cast<std::size_t>(
            ascii_sum(key.manufacturer) +
            ascii_sum(key.name)
        );
    }
};

} // namespace std

struct PrintAirplane {
    void operator()(std::ostream& os, const Airplane& airplane) const {
        os << airplane.manufacturer << ' '
           << airplane.name
           << ", capacity=" << airplane.capacity
           << ", range=" << airplane.range
           << ", max_speed=" << airplane.max_speed;
    }
};

int main() {
    using AirplaneTable =
        HashTable<AirplaneKey, Airplane>;

    AirplaneTable table(11);

    table.add(
        {"Airbus", "A320"},
        {"A320", "Airbus", 180, 6100, 840}
    );

    table.add(
        {"Airbus", "A321"},
        {"A321", "Airbus", 220, 5950, 840}
    );

    table.add(
        {"Boeing", "737-800"},
        {"737-800", "Boeing", 189, 5765, 842}
    );

    table.add(
        {"Airbus", "A320"},
        {"A320", "Airbus", 186, 6100, 840}
    );

    AirplaneKey key{"Airbus", "A320"};
    const auto objects = table[key];

    std::cout << "Найдено объектов: "
              << objects.size() << '\n';

    for (const auto& airplane : objects) {
        PrintAirplane{}(std::cout, airplane);
        std::cout << '\n';
    }

    std::cout << "\nХеш-таблица:\n";
    table.print(std::cout, PrintAirplane{});

    table.erase(key);

    return 0;
}
