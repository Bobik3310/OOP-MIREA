#include <iostream>
#include <cmath>

using namespace std;

class Matrix
{
private:
    // Поля закрытые
    double a11;
    double a12;
    double a21;
    double a22;

public:

    // Конструктор по умолчанию
    Matrix()
    {
        a11 = 0;
        a12 = 0;
        a21 = 0;
        a22 = 0;
    }

    // Конструктор с параметрами
    Matrix(double x11, double x12, double x21, double x22)
    {
        a11 = x11;
        a12 = x12;
        a21 = x21;
        a22 = x22;
    }

    // Конструктор копирования
    Matrix(const Matrix& M)
    {
        a11 = M.a11;
        a12 = M.a12;
        a21 = M.a21;
        a22 = M.a22;
    }

    // Деструктор
    ~Matrix() {}

    // Компонентная функция №1
    void print()
    {
        cout << a11 << " " << a12 << endl;
        cout << a21 << " " << a22 << endl;
    }

    // Компонентная функция №2
    double operator~()
    {
        return fabs(a11) + fabs(a12) + fabs(a21) + fabs(a22);
    }

    // Компонентная функция №3
    bool operator<(Matrix M)
    {
        return ~(*this) < ~M;
    }

    // Есть доступ к закрытым полям через friend
    friend bool operator<(double x, Matrix M);

    friend double operator!(Matrix M);
};

bool operator<(double x, Matrix M)
{
    return x < ~M;
}

double operator!(Matrix M)
{
    return (M.a11 + M.a12 + M.a21 + M.a22) / 4.0;
}

int main()
{
    Matrix A(1, -2, 3, -4);
    Matrix B(2, 2, 2, 2);

    cout << "Matrix A:" << endl;
    A.print();

    cout << endl;

    cout << "Matrix B:" << endl;
    B.print();

    cout << endl;

    cout << "Sum abs A = " << ~A << endl;
    cout << "Sum abs B = " << ~B << endl;

    cout << endl;

    cout << "Average A = " << !A << endl;
    cout << "Average B = " << !B << endl;

    cout << endl;

    if (A < B)
    {
        cout << "A < B" << endl;
    }
    else
    {
        cout << "A is not less than B" << endl;
    }

    cout << endl;

    if (7 < A)
    {
        cout << "7 < A" << endl;
    }
    else
    {
        cout << "7 is not less than A" << endl;
    }

    return 0;
}