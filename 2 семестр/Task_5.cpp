#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <typeinfo>

using namespace std;

// Система исключений
class Exception : public std::exception {
protected:
    char* str;
public:
    Exception(const char* s) {
        str = new char[strlen(s) + 1];
        strcpy(str, s);
    }
    
    Exception(const Exception& e) {
        str = new char[strlen(e.str) + 1];
        strcpy(str, e.str);
    }
    
    ~Exception() {
        delete[] str;
    }
    
    virtual void print() {
        cout << "Exception: " << str;
    }
    
    const char* what() const noexcept override {
        return str;
    }
};

class IndexOutOfBoundsException : public Exception {
public:
    IndexOutOfBoundsException(const char* message) : Exception(message) {}
};

class WrongDimensionsException : public Exception {
public:
    WrongDimensionsException(const char* message) : Exception(message) {}
};

class WrongSizeException : public Exception {
public:
    WrongSizeException(const char* message) : Exception(message) {}
};

class NonPositiveSizeException : public WrongSizeException {
public:
    NonPositiveSizeException(const char* message) : WrongSizeException(message) {}
};

class TooLargeSizeException : public WrongSizeException {
public:
    TooLargeSizeException(const char* message) : WrongSizeException(message) {}
};

class NegativeElementException : public Exception {
public:
    NegativeElementException(const char* message) : Exception(message) {}
};

// Многомерный массив
template<typename T>
class BaseMatrix {
protected:
    T** ptr;
    int height;
    int width;

public:
    BaseMatrix(int Height = 2, int Width = 2) {
        if (Height <= 0 || Width <= 0)
            throw NonPositiveSizeException("Non-positive size of matrix");
        if (Height > 100 || Width > 100)
            throw TooLargeSizeException("Matrix is too large");
        height = Height;
        width = Width;
        ptr = new T*[height]; // Выделяем память
        for (int i = 0; i < height; i++)
            ptr[i] = new T[width]();
    }

    BaseMatrix(const BaseMatrix& M) {
        height = M.height;
        width = M.width;
        ptr = new T*[height]; // Выделяем память
        for (int i = 0; i < height; i++) {
            ptr[i] = new T[width];
            for (int j = 0; j < width; j++)
                ptr[i][j] = M.ptr[i][j];
        }
    }

    ~BaseMatrix() {
        if (ptr != nullptr) {
            for (int i = 0; i < height; i++)
                delete[] ptr[i];
            delete[] ptr; // Удаляем память
            ptr = nullptr;
        }
    }

    void print() {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++)
                cout << ptr[i][j] << " ";
            cout << "\n";
        }
    }

    T& operator()(int row, int column) {
        if (row < 0 || column < 0 || row >= height || column >= width)
            throw IndexOutOfBoundsException("Index is out of bounds");
        return ptr[row][column];
    }

    template<typename U>
    friend ostream& operator<<(ostream& ustream, const BaseMatrix<U>& obj);
    
    template<typename U>
    friend istream& operator>>(istream& ustream, BaseMatrix<U>& obj);

    int Height() const { return height; }
    int Width() const { return width; }

    void saveToFile(const string& name) {
        ofstream file(name);
        if (!file.is_open())
            throw Exception("Cannot open file for writing");
        
        file << height << " " << width << "\n";
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                file << ptr[i][j] << " ";
            }
            file << "\n";
        }
        file.close();
    }

    void loadFromFile(const string& name) {
        ifstream file(name);
        if (!file.is_open())
            throw Exception("Cannot open file for reading");
        
        int newHeight, newWidth;
        file >> newHeight >> newWidth;
        
        if (ptr != nullptr) {
            for (int i = 0; i < height; i++)
                delete[] ptr[i];
            delete[] ptr;
        }
        
        height = newHeight;
        width = newWidth;
        ptr = new T*[height];
        for (int i = 0; i < height; i++) {
            ptr[i] = new T[width];
            for (int j = 0; j < width; j++) {
                file >> ptr[i][j];
            }
        }
        file.close();
    }
};

template<typename T>
ostream& operator<<(ostream& ustream, const BaseMatrix<T>& obj) {
    if (typeid(ustream) == typeid(ofstream)) {
        ustream << obj.height << " " << obj.width << "\n";
        for (int i = 0; i < obj.height; i++) {
            for (int j = 0; j < obj.width; j++)
                ustream << obj.ptr[i][j] << " ";
            ustream << "\n";
        }
        return ustream;
    }
    
    for (int i = 0; i < obj.height; i++) {
        for (int j = 0; j < obj.width; j++)
            ustream << obj.ptr[i][j] << " ";
        ustream << "\n";
    }
    return ustream;
}

template<typename T>
istream& operator>>(istream& ustream, BaseMatrix<T>& obj) {
    if (typeid(ustream) == typeid(ifstream))
        ustream >> obj.height >> obj.width;
    
    for (int i = 0; i < obj.height; i++)
        for (int j = 0; j < obj.width; j++)
            ustream >> obj.ptr[i][j];
    
    return ustream;
}

// Унаследованный класс
template<typename T>
class DerivedMatrix : public BaseMatrix<T> {
public:
    DerivedMatrix(int Height = 2, int Width = 2) : BaseMatrix<T>(Height, Width) {}
    
    DerivedMatrix(T** data, int Height, int Width) : BaseMatrix<T>(Height, Width) {
        for (int i = 0; i < this->height; i++) {
            for (int j = 0; j < this->width; j++) {
                this->ptr[i][j] = data[i][j];
            }
        }
    }

    DerivedMatrix(const DerivedMatrix& M) : BaseMatrix<T>(M) {}

    DerivedMatrix& operator=(const DerivedMatrix& M) {
        if (this == &M) return *this;
        
        if (this->ptr != nullptr) {
            for (int i = 0; i < this->height; i++)
                delete[] this->ptr[i];
            delete[] this->ptr;
        }
        
        this->height = M.height;
        this->width = M.width;
        this->ptr = new T*[this->height];
        for (int i = 0; i < this->height; i++) {
            this->ptr[i] = new T[this->width];
            for (int j = 0; j < this->width; j++) {
                this->ptr[i][j] = M.ptr[i][j];
            }
        }
        return *this;
    }

    void fillSpecial() {
        for (int i = 0; i < this->height; i++) {
            for (int j = 0; j < this->width; j++) {
                this->ptr[i][j] = static_cast<T>((i + 1) * (j + 1) + i * j);
            }
        }
    }

    // Нетривиальное заполнение
    void fillRandom() {
        for (int i = 0; i < this->height; i++) {
            for (int j = 0; j < this->width; j++) {
                this->ptr[i][j] = static_cast<T>(rand() % 10 + 1);
            }
        }
    }

    // Задание варианта
    double* getHarmonicMeans() {
        if (this->height == 0 || this->width == 0)
            throw WrongDimensionsException("Matrix is empty");
        
        double* result = new double[this->height];
        
        for (int i = 0; i < this->height; i++) {
            double sumReciprocals = 0.0;
            int count = this->width;
            
            for (int j = 0; j < this->width; j++) {
                if (this->ptr[i][j] < 0)
                    throw NegativeElementException("Matrix contains negative elements");
                if (this->ptr[i][j] == 0)
                    throw NegativeElementException("Matrix contains zero elements");
                    
                sumReciprocals += 1.0 / this->ptr[i][j];
            }
            
            result[i] = count / sumReciprocals;
        }
        
        return result;
    }

    DerivedMatrix operator+(const DerivedMatrix& M) {
        if (this->height != M.height || this->width != M.width)
            throw WrongDimensionsException("Matrices have different dimensions");
        
        DerivedMatrix result(this->height, this->width);
        for (int i = 0; i < this->height; i++) {
            for (int j = 0; j < this->width; j++) {
                result.ptr[i][j] = this->ptr[i][j] + M.ptr[i][j];
            }
        }
        return result;
    }
};

int main() {
    cout << "=== Testing exception handling ===\n";
      
    try {
        DerivedMatrix<double> Wrong(-2, 3);
    }
    catch (NonPositiveSizeException& e) {
        cout << "Caught NonPositiveSizeException: " << e.what() << endl;
    }
        
    try {
            DerivedMatrix<double> Wrong(150, 3);
        }
        catch (TooLargeSizeException& e) {
            cout << "Caught TooLargeSizeException: " << e.what() << endl;
        }
        
        try {
            DerivedMatrix<double> M(3, 3);
            M(5, 2) = 10;
        }
        catch (IndexOutOfBoundsException& e) {
            cout << "Caught IndexOutOfBoundsException: " << e.what() << endl;
        }
        
        try {
            DerivedMatrix<double> M1(2, 3);
            DerivedMatrix<double> M2(4, 5);
            M1.fillRandom();
            M2.fillRandom();
            auto M3 = M1 + M2;
        }
        catch (WrongDimensionsException& e) {
            cout << "Caught WrongDimensionsException: " << e.what() << endl;
        }
        



        cout << "\n=== Matrix operations ===\n";
        
        DerivedMatrix<double> matrix1(3, 4);
        matrix1.fillSpecial();
        cout << "Matrix 1 (special fill):\n" << matrix1 << endl;
        
        double* harmonicMeans = matrix1.getHarmonicMeans();
        cout << "Harmonic means vector: ";
        for (int i = 0; i < matrix1.Height(); i++) {
            cout << harmonicMeans[i] << " ";
        }
        cout << "\n\n";
        delete[] harmonicMeans;
        
        DerivedMatrix<double> matrix2(3, 4);
        matrix2.fillRandom();
        cout << "Matrix 2 (random fill):\n" << matrix2 << endl;
        
        harmonicMeans = matrix2.getHarmonicMeans();
        cout << "Harmonic means vector: ";
        for (int i = 0; i < matrix2.Height(); i++) {
            cout << harmonicMeans[i] << " ";
        }
        cout << "\n";
        delete[] harmonicMeans;
        



        cout << "=== File operations ===";

        cout << "\n\n";

        double* loadedHarmonics = matrix1.getHarmonicMeans();
        cout << "Harmonic means from saved to file matrix: ";
        for (int i = 0; i < matrix1.Height(); i++) {
            cout << loadedHarmonics[i] << " ";
        }

        cout << "\n\n";

        matrix1.saveToFile("matrix_result.txt");
        cout << "Matrix saved to file\n";

        cout << "\n";

        DerivedMatrix<double> loadedMatrix(1, 1);
        loadedMatrix.loadFromFile("matrix_result.txt");
        cout << "Matrix loaded from file:\n" << loadedMatrix << endl;
        
        loadedHarmonics = loadedMatrix.getHarmonicMeans();
        cout << "Harmonic means from loaded matrix: ";
        for (int i = 0; i < loadedMatrix.Height(); i++) {
            cout << loadedHarmonics[i] << " ";
        }
    
    return 0;
}