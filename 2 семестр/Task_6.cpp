#include <iostream>
#include <stdexcept>
#include <functional>
#include <fstream>
#include <iomanip>
#include <string>
using namespace std;

ostream& laptopHeader(ostream& os) {
    os << left 
       << setw(15) << "Brand" 
       << setw(20) << "Model" 
       << setw(8) << "Screen"
       << setw(12) << "CPU"
       << setw(8) << "Cores"
       << setw(8) << "RAM(GB)"
       << setw(10) << "Disk(GB)"
       << setw(8) << "DiskType"
       << setw(10) << "Price" << "\n";
    os << string(100, '-') << "\n";
    return os;
}

template<class T>
class Element {
protected:
    Element* next;
    T info;

public:
    Element(T data) : next(nullptr), info(data) {}
    
    virtual ~Element() {}
    
    Element* getNext() { return next; }
    void setNext(Element* n) { next = n; }
    T getInfo() const { return info; }
    T& getInfoRef() { return info; }
};

// Связный список
template<class T>
class LinkedList {
protected:
    // Поля защищены
    Element<T>* head;
    Element<T>* tail;

public:
    LinkedList() : head(nullptr), tail(nullptr) {}
    
    virtual ~LinkedList() {
        cout << "\nLinkedList destructor called\n";
        Element<T>* current = head;
        while (current != nullptr) {
            Element<T>* next = current->getNext();
            delete current;
            current = next;
        }
    }
    
    T& operator[](int index) {
        Element<T>* current = head;
        int i = 0;
        while (i < index && current != nullptr) {
            current = current->getNext();
            i++;
        }
        if (current == nullptr) throw out_of_range("Index out of range");
        return current->getInfoRef();
    }

    Element<T>* getHead() const { return head; }
    Element<T>* getTail() const { return tail; }
    void setHead(Element<T>* h) { head = h; }
    void setTail(Element<T>* t) { tail = t; }
        
    virtual void push(T value) = 0;
    virtual T pop() = 0;
};

// Унаследуем связный список
template<class T>
class ImprovedList : public LinkedList<T> {
protected:
    using LinkedList<T>::head;
    using LinkedList<T>::tail;

public:
    // Открытый интерфейс

    ~ImprovedList() override {
        cout << "\nImprovedList destructor called\n";
    }

    // Переопределяем push()
    void push(T value) override {
        Element<T>* newElem = new Element<T>(value);
        if (head == nullptr) {
            head = tail = newElem;
        } 
        else {
            newElem->setNext(head);
            head = newElem;
        }
    }
    
    // Переопределяем pop()
    T pop() override {
        if (head == nullptr) throw runtime_error("List is empty");
        if (head == tail) {
            T value = head->getInfo();
            delete head;
            head = tail = nullptr;
            return value;
        }
        T value = tail->getInfo();
        Element<T>* current = head;
        while (current->getNext() != tail) {
            current = current->getNext();
        }
        delete tail;
        tail = current;
        tail->setNext(nullptr);
        return value;
    }

    // Вставка в произвольное место
    void insert(int position, T value) {
        if (position < 0) return;
        
        if (position == 0) {
            push(value);
            return;
        }
        
        Element<T>* current = head;
        int index = 0;
        
        while (current != nullptr && index < position - 1) {
            current = current->getNext();
            index++;
        }
        
        if (current == nullptr) return;
        
        Element<T>* newElem = new Element<T>(value);
        newElem->setNext(current->getNext());
        current->setNext(newElem);
        
        if (newElem->getNext() == nullptr) {
            tail = newElem;
        }
    }

    // Удаление элемента по указателю
    void remove(Element<T>* element) {
        if (!element) return;
        
        if (element == head) {
            head = head->getNext();
            delete element;
            if (head == nullptr) {
                tail = nullptr;
            }
            return;
        }
        
        Element<T>* current = head;
        while (current != nullptr && current->getNext() != element) {
            current = current->getNext();
        }
        
        if (current == nullptr) return;
        
        current->setNext(element->getNext());
        if (element == tail) {
            tail = current;
        }
        delete element;
    }

    // Итеративный поиск по значению
    ImprovedList<T> find(const T& value) {
        ImprovedList<T> result;
        Element<T>* current = head;
        while (current != nullptr) {
            if (current->getInfo() == value) {
                result.push(current->getInfo());
            }
            current = current->getNext();
        }
        return result;
    }

    // Рекурсивный поиск по значению
    ImprovedList<T> findRecursive(const T& value) {
        ImprovedList<T> result;
        findRecursiveHelper(head, value, result);
        return result;
    }

private:
    void findRecursiveHelper(Element<T>* current, const T& value, ImprovedList<T>& result) {
        if (current == nullptr) return;
        if (current->getInfo() == value) {
            result.push(current->getInfo());
        }
        findRecursiveHelper(current->getNext(), value, result);
    }

public:
    // Фильтр по полю (итеративный)
    ImprovedList<T> filter(function<bool(const T&, const T&)> comp, const T& value) {
        ImprovedList<T> result;
        Element<T>* current = head;
        while (current != nullptr) {
            if (comp(current->getInfo(), value)) {
                result.push(current->getInfo());
            }
            current = current->getNext();
        }
        return result;
    }

    // Фильтр по полю (рекурсивный)
    ImprovedList<T> filterRecursive(function<bool(const T&, const T&)> comp, const T& value) {
        ImprovedList<T> result;
        filterRecursiveHelper(head, comp, value, result);
        return result;
    }

private:
    void filterRecursiveHelper(Element<T>* current, function<bool(const T&, const T&)> comp, 
                               const T& value, ImprovedList<T>& result) {
        if (current == nullptr) return;
        if (comp(current->getInfo(), value)) {
            result.push(current->getInfo());
        }
        filterRecursiveHelper(current->getNext(), comp, value, result);
    }

public:
    // Универсальный фильтр
    ImprovedList<T> filterUniversal(bool (*func)(const T&)) {
        ImprovedList<T> result;
        Element<T>* current = head;
        while (current != nullptr) {
            if (func(current->getInfo())) {
                result.push(current->getInfo());
            }
            current = current->getNext();
        }
        return result;
    }

    // Сохранение в файл
    void save(const string& filename) const {
        ofstream out(filename);
        if (!out) throw runtime_error("Cannot open file for writing");
        
        out << "FILE_FORMAT\n"; // Маркер формата файла
        Element<T>* current = head;
        while (current != nullptr) {
            out << current->getInfo() << "\n";
            current = current->getNext();
        }
        out.close();
    }

    // Загрузка из файла
    void load(const string& filename) {
        ifstream in(filename);
        if (!in) throw runtime_error("Cannot open file for reading");
        
        // Очищаем текущий список
        while (head != nullptr) {
            pop();
        }
        
        string marker;
        getline(in, marker);
        if (marker != "FILE_FORMAT") {
            throw runtime_error("Invalid file format");
        }
        
        T value;
        while (in >> value) {
            push(value);
        }
        
        in.close();
    }

    void print() const {
        Element<T>* current = head;
        while (current != nullptr) {
            cout << current->getInfo() << "\n";
            current = current->getNext();
        }
    }

    friend ostream& operator<<(ostream& os, const ImprovedList<T>& list) {
        Element<T>* current = list.head;
        while (current != nullptr) {
            os << current->getInfo();
            if (current->getNext() != nullptr) {
                os << "\n";
            }
            current = current->getNext();
        }
        return os;
    }
};

// Структура "Ноутбук"
class Laptop {
private:
    string brand; // Производитель
    string model; // Модель
    double screenSize; // Размер экрана
    string cpu; // Процессор
    int cores; // Кол-во ядер
    int ram; // Объем оперативной памяти
    int diskSize; // Объем диска
    string diskType; // Тип диска
    double price; // Цена

public:
    Laptop(string b = "", string m = "", double s = 0.0, string c = "", 
           int cr = 0, int r = 0, int d = 0, string dt = "", double p = 0.0) : 
        brand(b), model(m), screenSize(s), cpu(c), cores(cr), 
        ram(r), diskSize(d), diskType(dt), price(p) {}

    string getBrand() const { return brand; }
    string getModel() const { return model; }
    double getScreenSize() const { return screenSize; }
    string getCpu() const { return cpu; }
    int getCores() const { return cores; }
    int getRam() const { return ram; }
    int getDiskSize() const { return diskSize; }
    string getDiskType() const { return diskType; }
    double getPrice() const { return price; }

    bool operator==(const Laptop& other) const {
        return brand == other.brand && model == other.model;
    }

    // Оператор для поиска по бренду
    bool operator==(const string& searchBrand) const {
        return brand == searchBrand;
    }

    friend ostream& operator<<(ostream& os, const Laptop& laptop);
    friend istream& operator>>(istream& is, Laptop& laptop);
};

ostream& operator<<(ostream& os, const Laptop& laptop) {
    // Вывод в консоль в табличном формате
    os << left 
       << setw(15) << laptop.brand
       << setw(20) << laptop.model
       << setw(8) << laptop.screenSize
       << setw(12) << laptop.cpu
       << setw(8) << laptop.cores
       << setw(8) << laptop.ram
       << setw(10) << laptop.diskSize
       << setw(8) << laptop.diskType
       << setw(10) << laptop.price;
    return os;
}

ostream& writeToFile(ostream& os, const Laptop& laptop) {
    os << laptop.getBrand() << "," 
       << laptop.getModel() << "," 
       << laptop.getScreenSize() << "," 
       << laptop.getCpu() << "," 
       << laptop.getCores() << "," 
       << laptop.getRam() << "," 
       << laptop.getDiskSize() << "," 
       << laptop.getDiskType() << "," 
       << laptop.getPrice();
    return os;
}

istream& operator>>(istream& is, Laptop& laptop) {
    string brand, model, cpu, diskType;
    double screenSize, price;
    int cores, ram, diskSize;
    char delim;
    
    if (getline(is, brand, ',') && getline(is, model, ',') &&
        is >> screenSize >> delim >> cores >> delim >> ram >> delim >> diskSize >> delim) {
        getline(is, diskType, ',');
        is >> price;
        laptop = Laptop(brand, model, screenSize, cpu, cores, ram, diskSize, diskType, price);
        is.ignore();
    } else {
        is.setstate(ios::failbit);
    }
    return is;
}

// Функции для универсального фильтра
bool isHighEnd(const Laptop& laptop) {
    return laptop.getRam() >= 16 && laptop.getPrice() > 1000;
}

bool isSSD(const Laptop& laptop) {
    return laptop.getDiskType() == "SSD";
}

int main() {
    // Демонстрация работы
    cout << "=== Laptop Management System ===\n\n";
    
    // Создание списка
    ImprovedList<Laptop> laptops;
    
    // Автоматическое заполнение
    cout << "Adding sample laptops...\n";
    laptops.push(Laptop("Apple", "MacBook Pro 14", 14.2, "M2 Pro", 10, 16, 512, "SSD", 1999.99));
    laptops.push(Laptop("Dell", "XPS 15", 15.6, "i7-12700H", 12, 16, 512, "SSD", 1499.99));
    laptops.push(Laptop("Lenovo", "ThinkPad X1", 14.0, "i5-1240P", 12, 16, 256, "SSD", 1299.99));
    laptops.push(Laptop("HP", "Pavilion 15", 15.6, "i5-1135G7", 4, 8, 512, "SSD", 699.99));
    
    cout << laptopHeader;
    laptops.print();
    cout << string(100, '-') << "\n";
    
    // Вставка элемента
    cout << "\nInserting Asus laptop at position 2...\n";
    laptops.insert(2, Laptop("Asus", "ROG Zephyrus", 15.6, "R9-6900HS", 8, 16, 1000, "SSD", 1799.99));
    
    cout << laptopHeader;
    laptops.print();
    cout << string(100, '-') << "\n";
    
    // Поиск по бренду (итеративный)
    cout << "\nSearching for Dell laptops (iterative)...\n";
    Laptop searchLaptop("Dell", "", 0, "", 0, 0, 0, "", 0);
    ImprovedList<Laptop> foundLaptops = laptops.find(searchLaptop);
    cout << laptopHeader;
    foundLaptops.print();
    cout << string(100, '-') << "\n";
    
    // Поиск по бренду (рекурсивный)
    cout << "\nSearching for HP laptops (recursive)...\n";
    Laptop searchHP("HP", "", 0, "", 0, 0, 0, "", 0);
    ImprovedList<Laptop> foundHP = laptops.findRecursive(searchHP);
    cout << laptopHeader;
    foundHP.print();
    cout << string(100, '-') << "\n";
    
    // Фильтр по цене (итеративный)
    cout << "\nFiltering laptops with price > $1000 (iterative)...\n";
    Laptop priceFilter("", "", 0, "", 0, 0, 0, "", 1000.0);
    auto expensiveLaptops = laptops.filter(
        [](const Laptop& a, const Laptop& b) { return a.getPrice() > b.getPrice(); },
        priceFilter
    );
    cout << laptopHeader;
    expensiveLaptops.print();
    cout << string(100, '-') << "\n";
    
    // Фильтр по цене (рекурсивный)
    cout << "\nFiltering laptops with price < $1500 (recursive)...\n";
    Laptop cheapFilter("", "", 0, "", 0, 0, 0, "", 1500.0);
    auto cheapLaptops = laptops.filterRecursive(
        [](const Laptop& a, const Laptop& b) { return a.getPrice() < b.getPrice(); },
        cheapFilter
    );
    cout << laptopHeader;
    cheapLaptops.print();
    cout << string(100, '-') << "\n";
    
    // Универсальный фильтр
    cout << "\nHigh-end laptops (RAM >= 16, Price > $1000)...\n";
    auto highEndLaptops = laptops.filterUniversal(isHighEnd);
    cout << laptopHeader;
    highEndLaptops.print();
    cout << string(100, '-') << "\n";
    
    // Сохранение и загрузка
    cout << "\nSaving laptops to file...\n";
    laptops.save("Task_6_laptops.txt");
    
    cout << "Loading laptops from file...\n";
    ImprovedList<Laptop> loadedLaptops;
    loadedLaptops.load("Task_6_laptops.txt");
    cout << laptopHeader;
    loadedLaptops.print();
    cout << string(100, '-') << "\n";
    
    // Работа с динамической памятью и dynamic_cast
    cout << "\n=== Testing dynamic memory and polymorphism ===\n";
    LinkedList<Laptop>* basePtr = new ImprovedList<Laptop>();
    
    // Добавляем элементы через базовый указатель
    basePtr->push(Laptop("MSI", "Raider GE76", 17.3, "i9-12900HK", 14, 32, 1000, "SSD", 2499.99));
    basePtr->push(Laptop("Acer", "Predator Helios", 15.6, "i7-12700H", 14, 16, 512, "SSD", 1499.99));
    
    cout << "\nAdded laptops through base pointer\n";
    
    // Преобразование с помощью dynamic_cast
    ImprovedList<Laptop>* improvedPtr = dynamic_cast<ImprovedList<Laptop>*>(basePtr);
    if (improvedPtr) {
        cout << "\nSuccessfully converted to ImprovedList*\n";
        cout << laptopHeader;
        improvedPtr->print();
        cout << string(100, '-') << "\n";
        
        // Демонстрация удаления
        cout << "\nRemoving last element...\n";
        Laptop removed = improvedPtr->pop();
        cout << "Removed: " << removed << "\n\n";
        
        cout << "Remaining laptops:\n";
        cout << laptopHeader;
        improvedPtr->print();
        cout << string(100, '-') << "\n";
    }
    
    // Освобождение памяти через базовый указатель
    delete basePtr;
    cout << "\nMemory freed successfully\n";
    
    return 0;
}