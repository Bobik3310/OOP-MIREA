#include <iostream>
using namespace std;

class A1
{
protected:
    int a1;

public:
    A1(int v1 = 0)
    {
        a1 = v1;
        cout << "\nClass A1 constructor";
    }

    virtual ~A1() {}

    virtual void print()
    {
        cout << "\nClass A1";
    }

    virtual void show()
    {
        cout << "\na1 = " << a1;
    }
};

class B1 : virtual public A1
{
protected:
    int b1;

public:
    B1(int v1 = 0, int v2 = 0) : A1(v1)
    {
        b1 = v2;
        cout << "\nClass B1 constructor";
    }

    virtual ~B1() {}

    void print() override
    {
        cout << "\nClass B1";
    }

    void show() override
    {
        cout << "\na1 = " << a1
             << ", b1 = " << b1;
    }
};

class B2 : virtual public A1
{
protected:
    int b2;

public:
    B2(int v1 = 0, int v2 = 0) : A1(v1)
    {
        b2 = v2;
        cout << "\nClass B2 constructor";
    }

    virtual ~B2() {}

    void print() override
    {
        cout << "\nClass B2";
    }

    void show() override
    {
        cout << "\na1 = " << a1
             << ", b2 = " << b2;
    }
};

class B3 : virtual public A1
{
protected:
    int b3;

public:
    B3(int v1 = 0, int v2 = 0) : A1(v1)
    {
        b3 = v2;
        cout << "\nClass B3 constructor";
    }

    virtual ~B3() {}

    void print() override
    {
        cout << "\nClass B3";
    }

    void show() override
    {
        cout << "\na1 = " << a1
             << ", b3 = " << b3;
    }
};

class C1 : virtual public B1, virtual public B2, virtual public B3
{
protected:
    int c1;

public:
    C1(int v1 = 0, int v2 = 0, int v3 = 0, int v4 = 0, int v5 = 0)
        : A1(v1), B1(v1, v2), B2(v1, v3), B3(v1, v4)
    {
        c1 = v5;
        cout << "\nClass C1 constructor";
    }

    virtual ~C1() {}

    void print() override
    {
        cout << "\nClass C1";
    }

    void show() override
    {
        cout << "\na1 = " << a1
             << ", b1 = " << b1
             << ", b2 = " << b2
             << ", b3 = " << b3
             << ", c1 = " << c1;
    }
};

class C2 : virtual public B1, virtual public B2, virtual public B3
{
protected:
    int c2;

public:
    C2(int v1 = 0, int v2 = 0, int v3 = 0, int v4 = 0, int v5 = 0)
        : A1(v1), B1(v1, v2), B2(v1, v3), B3(v1, v4)
    {
        c2 = v5;
        cout << "\nClass C2 constructor";
    }

    virtual ~C2() {}

    void print() override
    {
        cout << "\nClass C2";
    }

    void show() override
    {
        cout << "\na1 = " << a1
             << ", b1 = " << b1
             << ", b2 = " << b2
             << ", b3 = " << b3
             << ", c2 = " << c2;
    }
};

class D1 : public C1, public C2
{
protected:
    int d1;

public:
    D1(int v1 = 0, int v2 = 0, int v3 = 0, int v4 = 0,
       int v5 = 0, int v6 = 0, int v7 = 0)
        : A1(v1),
          B1(v1, v2),
          B2(v1, v3),
          B3(v1, v4),
          C1(v1, v2, v3, v4, v5),
          C2(v1, v2, v3, v4, v6)
    {
        d1 = v7;
        cout << "\nClass D1 constructor";
    }

    virtual ~D1() {}

    void print() override
    {
        cout << "\nClass D1";
    }

    void show() override
    {
        cout << "\na1 = " << a1
             << ", b1 = " << b1
             << ", b2 = " << b2
             << ", b3 = " << b3
             << ", c1 = " << c1
             << ", c2 = " << c2
             << ", d1 = " << d1;
    }
};

int main()
{
    cout << "\n\n--- Для B1: ---";
    B1 objB1(1, 2);
    objB1.print();
    objB1.show();

    cout << "\n\n--- Для C1: ---";
    C1 objC1(1, 2, 3, 4, 5);
    objC1.print();
    objC1.show();

    cout << "\n\n--- Для C2: ---";
    C2 objC2(10, 20, 30, 40, 50);
    objC2.print();
    objC2.show();

    cout << "\n\n--- Для D1: ---";
    D1 objD1(1, 2, 3, 4, 5, 6, 7);
    objD1.print();
    objD1.show();

    cout << "\n\n--- Полиморфизм: ---";
    // вызывается метод объекта-наследника, а не базового класса
    A1* ptr = &objD1;
    ptr->print();
    ptr->show();

    cout << "\n";

    return 0;
}