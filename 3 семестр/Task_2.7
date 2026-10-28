#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <list>
#include <string>
#include <vector>

struct AirplaneKey {
  std::string manufacturer;
  std::string name;
  bool operator==(const AirplaneKey &other) const {
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

std::ostream &operator<<(std::ostream &os, const Airplane &a) {
  return os << a.manufacturer << ' ' << a.name << ", capacity=" << a.capacity
            << ", range=" << a.range << ", max_speed=" << a.max_speed;
}

// Вариант 30: 4k+2 — метод умножения;
//             3k   — разрешение коллизий методом цепочек.
class HashTable30 {
  using BucketItem = std::pair<AirplaneKey, Airplane>;
  static constexpr double phi = 0.6180339887498948482;

  std::vector<std::list<BucketItem>> table_;

  static std::uint64_t ascii_sum(const std::string &text) {
    std::uint64_t result = 0;
    for (unsigned char ch : text)
      result += ch;
    return result;
  }

  // Для составного строкового ключа суммируем ASCII-коды
  // производителя и названия.
  static std::uint64_t numeric_key(const AirplaneKey &key) {
    return ascii_sum(key.manufacturer) + ascii_sum(key.name);
  }

  std::size_t hash(const AirplaneKey &key) const {
    const std::uint64_t K = numeric_key(key);
    const std::size_t raw_hash = static_cast<std::size_t>(std::floor(
        static_cast<double>(table_.size()) * static_cast<double>(K) * phi));
    return raw_hash % table_.size();
  }

public:
  explicit HashTable30(std::size_t size = 101) : table_(size == 0 ? 1 : size) {}

  void add(const Airplane &value) {
    const AirplaneKey key = value.key();
    table_[hash(key)].push_back({key, value});
  }

  // Возвращает все значения с заданным ключом.
  std::vector<Airplane> find(const AirplaneKey &key) const {
    std::vector<Airplane> result;

    for (const auto &item : table_[hash(key)]) {
      if (item.first == key)
        result.push_back(item.second);
    }

    return result;
  }

  std::vector<Airplane> operator[](const AirplaneKey &key) const {
    return find(key);
  }

  bool erase(const AirplaneKey &key) {
    auto &bucket = table_[hash(key)];

    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
      if (it->first == key) {
        bucket.erase(it);
        return true;
      }
    }

    return false;
  }

  void print(std::ostream &os) const {
    for (std::size_t i = 0; i < table_.size(); ++i) {
      for (const auto &item : table_[i])
        os << '[' << i << "] " << item.second << '\n';
    }
  }
};

int main() {
  HashTable30 table(11);

  table.add({"A320", "Airbus", 180, 6100, 840});
  table.add({"A321", "Airbus", 220, 5950, 840});
  table.add({"737-800", "Boeing", 189, 5765, 842});
  table.add({"A320", "Airbus", 186, 6100, 840});

  AirplaneKey key{"Airbus", "A320"};
  auto objects = table[key];

  std::cout << "Найдено объектов Airbus/A320: " << objects.size() << "\n";

  for (const auto &airplane : objects)
    std::cout << airplane << '\n';

  std::cout << "\nСодержимое хеш-таблицы:\n";
  table.print(std::cout);

  table.erase(key);
}
