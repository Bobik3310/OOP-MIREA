#include <iostream>
#include <math.h>

using namespace std;

class MyArrayParent
{
protected:
    int capacity;
    int count;
    double* ptr;

public:
    MyArrayParent(int Dimension = 100)
    {
        cout << "\nMyArray constructor";
        ptr = new double[Dimension];
        capacity = Dimension;
        count = 0;
    }

    // Конструктор принимает и копирует
    MyArrayParent(double* arr, int len)
    {
        cout << "\nMyArray constructor";

        capacity = len;
        count = len;
        ptr = new double[capacity];

        for (int i = 0; i < len; i++)
        {
            ptr[i] = arr[i];
        }
    }


    // Конструктор копий
    MyArrayParent(const MyArrayParent& v)
    {
        cout << "\nCopy constructor";

        capacity = v.capacity;
        count = v.count;
        ptr = new double[capacity];

        for (int i = 0; i < count; i++)
        {
            ptr[i] = v.ptr[i];
        }
    }

    virtual ~MyArrayParent()
    {
        cout << "\nMyArray destructor";

        if (ptr != NULL)
        {
            delete[] ptr;
            ptr = NULL;
        }
    }

    int Capacity() { return capacity; }

    int Size() { return count; }

    double GetComponent(int index)
    {
        if (index >= 0 && index < count)
            return ptr[index];

        return -1;
    }

    void SetComponent(int index, double value)
    {
        if (index >= 0 && index < count)
            ptr[index] = value;
    }

    virtual void push(double value)
    {
        if (count < capacity)
        {
            ptr[count] = value;
            count++;
        }
    }

    void RemoveLastValue()
    {
        if (count > 0)
            count--;

        return;
    }

    // Оператор []
    double& operator[](int index)
    {
        if (index >= 0 && index < count)
        {
            return ptr[index];
        }

        static double error = -1;
        return error;
    }

    // Оператор =
    MyArrayParent& operator=(const MyArrayParent& v)
    {
        cout << "\noperator = ";

        if (this == &v)
        {
            return *this;
        }

        if (capacity < v.capacity)
        {
            delete[] ptr;
            ptr = new double[v.capacity];
            capacity = v.capacity;
        }

        count = v.count;

        for (int i = 0; i < count; i++)
        {
            ptr[i] = v.ptr[i];
        }

        return *this;
    }

    double Mean()
    {
        if (count == 0)
        {
            return 0;
        }

        double sum = 0;

        for (int i = 0; i < count; i++)
        {
            sum += ptr[i];
        }

        return sum / count;
    }

    void print()
    {
        cout << "\nMyArr, size: " << count << ", values: {";

        for (int i = 0; i < count; ++i)
        {
            cout << ptr[i];

            if (i < count - 1)
                cout << ", ";
        }

        cout << "}";
    }

    int IndexOf(double value, bool fromStart = true)
    {
        if (fromStart)
        {
            for (int i = 0; i < count; i++)
                if (fabs(ptr[i] - value) < 0.001) return i;

            return -1;
        }
        else
        {
            for (int i = count - 1; i >= 0; i--)
                if (fabs(ptr[i] - value) < 0.001) return i;

            return -1;
        }
    }
};

class MyArrayChild : public MyArrayParent
{
public:
    MyArrayChild(int Dimension = 100) : MyArrayParent(Dimension)
    {
        cout << "\nMyArrayChild constructor";
    }

    MyArrayChild(double* arr, int len) : MyArrayParent(arr, len)
    {
        cout << "\nMyArrayChild constructor";
    }

    ~MyArrayChild()
    {
        cout << "\nMyArrayChild destructor\n";
    }

    // Удаление элемента из произв. места в массиве
    void RemoveAt(int index)
    {
        if (index == -1)
        {
            RemoveLastValue();
            return;
        }

        if (index < 0 || index >= count)
            return;

        for (int i = index; i < count - 1; i++)
        {
            ptr[i] = ptr[i + 1];
        }

        count--;
    }

    // Вставка элемента в некоторое место
    void InsertAt(double value, int index)
    {
        if (count >= capacity)
            return;

        if (index == -1 || index == count)
        {
            MyArrayParent::push(value);
            return;
        }

        if (index < 0 || index > count)
            return;

        for (int i = count; i > index; i--)
        {
            ptr[i] = ptr[i - 1];
        }

        ptr[index] = value;
        count++;
    }

    void operator+(double value)
    {
        push(value);
    }

    MyArrayChild NewArray(double p)
    {
        MyArrayChild Res(capacity);

        double mean = Mean();
        double limit = fabs(p * mean);

        for (int i = 0; i < count; i++)
        {
            if (fabs(ptr[i] - mean) <= limit)
            {
                Res.push(ptr[i]);
            }
        }

        return Res;
    }
};

class MySortedArray : public MyArrayChild
{
public:
    MySortedArray(int Dimension = 100) : MyArrayChild(Dimension)
    {
        cout << "\nMySortedArray constructor";
    }

    MySortedArray(double* arr, int len) : MyArrayChild(len)
    {
        cout << "\nMySortedArray constructor";

        for (int i = 0; i < len; i++)
        {
            push(arr[i]);
        }
    }

    ~MySortedArray()
    {
        cout << "\nMySortedArray destructor\n";
    }

    void push(double value)
    {
        if (count >= capacity)
            return;

        if (count == 0)
        {
            MyArrayParent::push(value);
            return;
        }

        int left = 0;
        int right = count - 1;
        int index = count;

        while (left <= right)
        {
            int middle = left + (right - left) / 2;

            if (ptr[middle] < value)
            {
                left = middle + 1;
            }
            else
            {
                index = middle;
                right = middle - 1;
            }
        }

        for (int i = count; i > index; i--)
        {
            ptr[i] = ptr[i - 1];
        }

        ptr[index] = value;
        count++;
    }

    // Поиск элемента в массиве
    int IndexOf(double value)
    {
        int left = 0;
        int right = count - 1;

        while (left <= right)
        {
            int middle = left + (right - left) / 2;

            if (fabs(ptr[middle] - value) < 0.001)
                return middle;

            if (ptr[middle] < value)
                left = middle + 1;
            else
                right = middle - 1;
        }

        return -1;
    }

    MySortedArray NewArray(double p)
    {
        MySortedArray Res(capacity);

        double mean = Mean();
        double limit = fabs(p * mean);

        double leftBorder = mean - limit;
        double rightBorder = mean + limit;

        for (int i = 0; i < count; i++)
        {
            if (ptr[i] < leftBorder)
                continue;

            if (ptr[i] > rightBorder)
                break;

            Res.push(ptr[i]);
        }

        return Res;
    }
};

int main()
{
    double arr[] = {10, 12, 11, 100, 9, 13, -50, 10};
    int len = 8;

    cout << "\n\n--- MyArrayChild ---";

    MyArrayChild A(arr, len);

    cout << "\nInitial array:";
    A.print();

    cout << "\nMean = " << A.Mean();

    MyArrayChild B = A.NewArray(1);

    cout << "\nNew array:";
    B.print();

    cout << "\n\n--- MySortedArray ---";

    MySortedArray S(arr, len);

    cout << "\nSorted array:";
    S.print();

    cout << "\nMean = " << S.Mean();

    MySortedArray S2 = S.NewArray(1);

    cout << "\nNew sorted array:";
    S2.print();

    cout << "\n\nIndex of 12 = " << S.IndexOf(12);

    S.push(11.5);

    cout << "\nAfter push(11.5):";
    S.print();

    cout << "\n";

    return 0;
}