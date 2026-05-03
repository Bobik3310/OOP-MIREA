#include <iostream>
#include <cstring>

using namespace std;

// Базовый класс "Строка"
class BaseString
{
protected:
    char* p;
    int len;
    int capacity;

public:
    // Конструктор
    BaseString(const char* ptr)
    {
        cout << "\nBase Constructor 1\n";

        len = strlen(ptr) + 1;
        capacity = 256;

        if (len > capacity)
            capacity = len;

        p = new char[capacity];

        for (int i = 0; i < len; i++)
        {
            p[i] = ptr[i];
        }
    }

    // Конструктор по-умолчанию
    BaseString(int Capacity = 256)
    {
        cout << "\nBase Constructor 0\n";

        capacity = Capacity;
        if (capacity <= 0)
            capacity = 256;

        p = new char[capacity];
        p[0] = '\0';
        len = 1;
    }

    // Конструктор копирования
    BaseString(const BaseString& s)
    {
        cout << "\nBase Copy Constructor\n";

        len = s.len;
        capacity = s.capacity;
        p = new char[capacity];

        for (int i = 0; i < len; i++)
        {
            p[i] = s.p[i];
        }
    }

    // Виртуальный деструктор
    // - виртуальный, чтобы вначале вызывался деструктор наследника
    virtual ~BaseString()
    {
        cout << "\nBase Destructor\n";

        if (p != NULL)
        {
            delete[] p;
            p = NULL;
        }

        len = 0;
        capacity = 0;
    }

    int Length() const
    {
        return len;
    }

    int Capacity() const
    {
        return capacity;
    }

    char& operator[](int i)
    {
        return p[i];
    }

    const char& operator[](int i) const
    {
        return p[i];
    }

    // Оператор присваивания
    BaseString& operator=(const BaseString& s)
    {
        cout << "\nBase Operator =\n";

        if (this == &s)
            return *this;

        delete[] p;

        len = s.len;
        capacity = s.capacity;
        p = new char[capacity];

        for (int i = 0; i < len; i++)
        {
            p[i] = s.p[i];
        }

        return *this;
    }

    virtual void print() const
    {
        int i = 0;

        while (p[i] != '\0')
        {
            cout << p[i];
            i++;
        }
    }
};

// Свой класс
class PunctuationString : public BaseString
{
private:
    // проверка символа
    bool isPunctuation(char c) const
    {
        return c == '.' || c == ',' || c == '!' ||
               c == '?' || c == ':' || c == ';';
    }

public:
    // Конструктор наследника
    // - сначала вызывается конструктор базового класса, который выделяет память и копирует строку
    PunctuationString(const char* ptr) : BaseString(ptr)
    {
        cout << "\nDerived Constructor 1\n";
    }

    // Конструктор наследника
    // - сначала вызывается BaseString(Capacity), который создает пустую строку зад. длины
    PunctuationString(int Capacity = 256) : BaseString(Capacity)
    {
        cout << "\nDerived Constructor 0\n";
    }

    // Конструктор копирования наследника
    // - вызывает конструктор копирования базового класса
    PunctuationString(const PunctuationString& s) : BaseString(s)
    {
        cout << "\nDerived Copy Constructor\n";
    }

    // Деструктор наследника
    // - после него вызовется деструктор базового класса
    ~PunctuationString()
    {
        cout << "\nDerived Destructor\n";
    }

    // Оператор присваивания для наследника
    PunctuationString& operator=(const PunctuationString& s)
    {
        cout << "\nDerived Operator =\n";

        if (this != &s)
        {
            BaseString::operator=(s);
        }

        return *this;
    }

    // Проверка на пробел
    bool HasSpaceAfterEachPunctuation() const
    {
        for (int i = 0; p[i] != '\0'; i++)
        {
            if (isPunctuation(p[i]))
            {
                if (p[i + 1] == '\0')
                    continue;

                if (p[i + 1] != ' ')
                    return false;
            }
        }

        return true;
    }

    // Индекс первой неправильно пунктуации
    int FirstBadPunctuationIndex() const
    {
        for (int i = 0; p[i] != '\0'; i++)
        {
            if (isPunctuation(p[i]))
            {
                if (p[i + 1] == '\0')
                    continue;

                if (p[i + 1] != ' ')
                    return i;
            }
        }

        return -1;
    }
};

int main()
{
    const char* text1 = "Hello, World! How are you?";
    const char* text2 = "Hello,World! How are you?";
    const char* text3 = "One; two: three, four.";

    PunctuationString s1(text1);
    PunctuationString s2(text2);
    PunctuationString s3(text3);

    cout << "\n\nString 1: ";
    s1.print();

    if (s1.HasSpaceAfterEachPunctuation())
        cout << "\nAfter each punctuation mark there is a space\n";
    else
        cout << "\nError at index: " << s1.FirstBadPunctuationIndex() << "\n";


    cout << "\nString 2: ";
    s2.print();

    if (s2.HasSpaceAfterEachPunctuation())
        cout << "\nAfter each punctuation mark there is a space\n";
    else
        cout << "\nError at index: " << s2.FirstBadPunctuationIndex() << "\n";

    cout << "\nString 3: ";
    s3.print();

    if (s3.HasSpaceAfterEachPunctuation())
        cout << "\nAfter each punctuation mark there is a space\n";
    else
        cout << "\nError at index: " << s3.FirstBadPunctuationIndex() << "\n";

    cout << "\n\nCopy constructor test:";
    PunctuationString copy = s1;
    cout << "\nCopy: ";
    copy.print();

    cout << "\n\nOperator = test:";
    PunctuationString assigned;
    assigned = s3;
    cout << "\nAssigned: ";
    assigned.print();

    return 0;
}