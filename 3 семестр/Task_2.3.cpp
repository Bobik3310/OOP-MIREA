#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

// Данные об одном самолёте.
struct Airplane {
    string name;          // Модель самолёта
    string manufacturer;  // Производитель
    int flightRange;      // Дальность полёта
    int maxSpeed;         // Максимальная скорость
};

// Позволяет выводить объект Airplane через cout.
ostream& operator<<(ostream& out, const Airplane& airplane) {
    out << airplane.manufacturer << ' ' << airplane.name
        << ", дальность: " << airplane.flightRange
        << ", скорость: " << airplane.maxSpeed;

    return out;
}

/*
    Построение хеш-таблицы.
    Ключом будет строка вида: "Производитель Модель"
    Значением будет объект Airplane.
*/
using AirplaneTable = unordered_map<string, Airplane>;

// Формирование ключа из производителя и названия самолёта.
string makeKey(const string& manufacturer, const string& name) {
    return manufacturer + ' ' + name;
}

/*
    Добавление элемента в хеш-таблицу.
    Если такой ключ уже существует, генерируется исключение.
*/
void addAirplane(AirplaneTable& table, const Airplane& airplane) {
    string key = makeKey(airplane.manufacturer, airplane.name);

    // emplace пытается добавить новую пару «ключ — значение».
    auto [iterator, inserted] = table.emplace(key, airplane);

    // Если inserted == false, такой ключ уже есть.
    if (!inserted) {
        throw invalid_argument("Ключ уже существует: " + key);
    }
}

/*
    Вывод содержимого таблицы с помощью итератора.
    У unordered_map порядок элементов не определён.
*/
void printTable(const AirplaneTable& table) {
    AirplaneTable::const_iterator iterator = table.cbegin();

    while (iterator != table.cend()) {
        cout << iterator->first << " -> " << iterator->second << '\n';
        ++iterator;
    }
}

/*
    Поиск элемента по ключу.
    Метод find() ищет элемент по ключу.
*/
const Airplane* findByKey(const AirplaneTable& table, const string& key) {
    auto iterator = table.find(key);

    if (iterator == table.end()) {
        return nullptr;
    }

    return &iterator->second;
}

/*
    Поиск элементов по значению.
    unordered_map быстро ищет только по ключу.
    Поэтому для поиска по полям значения
    приходится просмотреть всю таблицу.
*/
vector<string> findByValue(const AirplaneTable& table, int range, int speed) {
    vector<string> result;

    for (const auto& [key, airplane] : table) {
        if (airplane.flightRange == range && airplane.maxSpeed == speed) {
            result.push_back(key);
        }
    }

    return result;
}

/*
    Функция filter().
    Функция принимает предикат.
    Предикат получает ключ и значение и
    возвращает true или false.
    В результат попадают только элементы,
    для которых предикат вернул true.
*/
template <typename Predicate>
AirplaneTable filter(const AirplaneTable& table, Predicate predicate) {
    AirplaneTable result;

    for (const auto& element : table) {
        if (predicate(element.first, element.second)) {
            result.emplace(element.first, element.second);
        }
    }

    return result;
}

/*
    Версия filter(), если threshold присутствует среди ключей таблицы.
*/
AirplaneTable filterWithExistingThreshold(const AirplaneTable& table, const string& threshold) {
    // Проверяем наличие ключа threshold.
    table.find(threshold);

    return filter(table, [&threshold](const string& key, const Airplane&) {
            return key > threshold;
        }
    );
}

/*
    Версия filter(), если threshold может отсутствовать в таблице.
*/
AirplaneTable filterWithoutThreshold(const AirplaneTable& table, const string& threshold) {
    return filter(table, [&threshold](const string& key, const Airplane&) {
            return key > threshold;
        }
    );
}

/*
    Функция получения различных значений.
    В set помещаются пары: «дальность полёта — максимальная скорость».
    set автоматически удаляет повторы.
    Затем содержимое set преобразуется в vector.
*/
vector<pair<int, int>> getUniqueValues(const AirplaneTable& table) {
    set<pair<int, int>> uniqueValues;

    for (const auto& [key, airplane] : table) {
        uniqueValues.emplace(airplane.flightRange, airplane.maxSpeed);
    }

    return {uniqueValues.begin(), uniqueValues.end()};
}

int main() {
    AirplaneTable airplanes;
    addAirplane(airplanes, {"737", "Boeing", 5600, 850});
    addAirplane(airplanes, {"A320", "Airbus", 6100, 840});
    addAirplane(airplanes, {"787", "Boeing", 13600, 954});
    addAirplane(airplanes, {"A350", "Airbus", 15000, 945});
    addAirplane( airplanes, {"MS-21", "Irkut", 6000, 870});
    cout << "Исходная хеш-таблица:\n";
    printTable(airplanes);



    cout << "\nПоиск по ключу:\n";
    const Airplane* found = findByKey(airplanes, "Boeing 787");
    if (found != nullptr) {
        cout << *found << '\n';
    } else {
        cout << "Самолёт не найден\n";
    }



    cout << "\nПоиск по значению:\n";
    vector<string> foundKeys = findByValue(airplanes, 5600, 850);
    for (const string& key : foundKeys) {
        cout << key << '\n';
    }



    cout << "\nФильтрация:\n";
    AirplaneTable filtered = filterWithoutThreshold(airplanes, "Airbus");
    printTable(filtered);



    cout << "\nРазличные значения:\n";
    vector<pair<int, int>> values = getUniqueValues(airplanes);
    for (const auto& [range, speed] : values) {
        cout << "Дальность: " << range
             << ", скорость: " << speed << '\n';
    }



    cout << "\nПроверка дубликата:\n";
    try {
        addAirplane(airplanes, {"787", "Boeing", 13600, 954});
    } catch (const invalid_argument& error) {
        cout << error.what() << '\n';
    }

    return 0;
}
