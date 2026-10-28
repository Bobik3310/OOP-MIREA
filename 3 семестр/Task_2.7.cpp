#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <list>
#include <string>
#include <utility>
#include <vector>

// Универсальная хеш-таблица. Вариант 30:
// h(K) = floor(size * K * phi), коллизии — методом цепочек.
template<class Key, class Value, class KeyOf, class HashKey,
         class KeyEqual = std::equal_to<Key>>
class HashTable {
    using Item = std::pair<Key, Value>;
    static constexpr double phi = 0.6180339887498948482;

    std::vector<std::list<Item>> buckets_;
    KeyOf key_of_{};
    HashKey hash_key_{};
    KeyEqual equal_{};

    std::size_t index(const Key& key) const {
        const std::uint64_t K = hash_key_(key);
        const auto raw = static_cast<std::size_t>(
            std::floor(static_cast<double>(buckets_.size()) *
                       static_cast<double>(K) * phi));
        return raw % buckets_.size();
    }

public:
    explicit HashTable(std::size_t size = 101)
        : buckets_(size == 0 ? 1 : size) {}

    void add(const Value& value) {
        const Key key = key_of_(value);
        buckets_[index(key)].push_back({key, value});
    }

    std::vector<Value> find(const Key& key) const {
        std::vector<Value> result;
        for (const auto& item : buckets_[index(key)]) {
            if (equal_(item.first, key))
                result.push_back(item.second);
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

// Конкретный пример для варианта 30.
struct AirplaneKey {
    std::string manufacturer;
    std::string name;
    bool operator==(const AirplaneKey& other) const {
        return manufacturer == other.manufacturer && name == other.name;
    }
};

struct Airplane {
    std::string name;
    std::string manufacturer;
    int capacity{};
    double range{};
    double max_speed{};
    AirplaneKey key() const { return {manufacturer, name}; }
};

struct AirplaneKeyOf {
    AirplaneKey operator()(const Airplane& airplane) const {
        return airplane.key();
    }
};

struct AirplaneHash30 {
    static std::uint64_t ascii_sum(const std::string& text) {
        std::uint64_t result = 0;
        for (unsigned char ch : text)
            result += ch;
        return result;
    }

    std::uint64_t operator()(const AirplaneKey& key) const {
        return ascii_sum(key.manufacturer) + ascii_sum(key.name);
    }
};

struct PrintAirplane {
    void operator()(std::ostream& os, const Airplane& airplane) const {
        os << airplane.manufacturer << ' ' << airplane.name
           << ", capacity=" << airplane.capacity
           << ", range=" << airplane.range
           << ", max_speed=" << airplane.max_speed;
    }
};

int main() {
    using AirplaneTable = HashTable<
        AirplaneKey,
        Airplane,
        AirplaneKeyOf,
        AirplaneHash30
    >;

    AirplaneTable table(11);

    table.add({"A320", "Airbus", 180, 6100, 840});
    table.add({"A321", "Airbus", 220, 5950, 840});
    table.add({"737-800", "Boeing", 189, 5765, 842});
    table.add({"A320", "Airbus", 186, 6100, 840});

    AirplaneKey key{"Airbus", "A320"};
    const auto objects = table[key];

    std::cout << "Найдено объектов: " << objects.size() << '\n';
    for (const auto& airplane : objects)
        PrintAirplane{}(std::cout, airplane), std::cout << '\n';

    std::cout << "\nХеш-таблица:\n";
    table.print(std::cout, PrintAirplane{});

    table.erase(key);
}
