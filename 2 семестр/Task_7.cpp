#include <iostream>
#include <fstream>
#include <ctime>
#include <cmath>
#include <string>

using namespace std;

#define N 1000

// Шаблонная функция копирования массива
template<class T>
void copyArray(T from[], T to[], long size) {
    for (long i = 0; i < size; i++) {
        to[i] = from[i];
    }
}

// Сортировка расческой
template<class T>
T* combsort(T* a, size_t len) {
    T* arr = new T[len];
    copyArray(a, arr, len);
    
    size_t step = len;
    bool swapped = true;
    T temp;
    
    // Подбираем оптимальный фактор уменьшения
    const double factor = 1.247;
    
    while (step > 1 || swapped) {
        // Уменьшаем шаг
        if (step > 1) {
            step = (size_t)(step / factor);
            if (step < 1) step = 1;
        }
        
        swapped = false;
        
        // Сравниваем элементы на расстоянии step
        for (size_t i = 0; i < len - step; i++) {
            if (arr[i] > arr[i + step]) {
                // Меняем элементы местами
                temp = arr[i];
                arr[i] = arr[i + step];
                arr[i + step] = temp;
                swapped = true;
            }
        }
    }
    
    return arr;
}

// Сортировка слияением
template<class T>
void merge(T arr[], size_t left, size_t mid, size_t right) {
    size_t n1 = mid - left + 1;  // Размер левого подмассива
    size_t n2 = right - mid;      // Размер правого подмассива
    
    // Создаем временные массивы
    T* L = new T[n1];
    T* R = new T[n2];
    
    // Копируем данные во временные массивы
    for (size_t i = 0; i < n1; i++) {
        L[i] = arr[left + i];
    }
    for (size_t j = 0; j < n2; j++) {
        R[j] = arr[mid + 1 + j];
    }
    
    // Слияние временных массивов обратно в arr[left..right]
    size_t i = 0;    // Индекс левого подмассива
    size_t j = 0;    // Индекс правого подмассива
    size_t k = left; // Индекс результирующего массива
    
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    
    // Копируем оставшиеся элементы L[], если есть
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    
    // Копируем оставшиеся элементы R[], если есть
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
    
    delete[] L;
    delete[] R;
}

template<class T>
void mergeSortRecursive(T arr[], size_t left, size_t right) {
    if (left < right) {
        // Находим середину
        size_t mid = left + (right - left) / 2;
        
        // Сортируем левую и правую половины
        mergeSortRecursive(arr, left, mid);
        mergeSortRecursive(arr, mid + 1, right);
        
        // Объединяем отсортированные половины
        merge(arr, left, mid, right);
    }
}

template<class T>
T* mergesort(T* a, size_t len) {
    // Копируем массив - будем сортировать его копию
    T* arr = new T[len];
    copyArray(a, arr, len);
    
    // Вызываем рекурсивную сортировку слиянием
    mergeSortRecursive(arr, 0, len - 1);
    
    return arr;
}

// Функция чтения массива из файла, сортировки и записи результата
template<class T>
void sortArrayFromFile(const string& inputFilename, 
                       const string& outputFilename,
                       T* (*sortFunction)(T*, size_t),
                       const string& sortName) {
    
    // Чтение массива из файла
    ifstream fin(inputFilename);
    T* array = nullptr;
    size_t len = 0;
    
    if (fin) {
        fin >> len;
        array = new T[len];
        for (size_t i = 0; i < len; i++) {
            fin >> array[i];
        }
        fin.close();
        
        cout << "Прочитан массив из " << len << " элементов из файла " << inputFilename << endl;
        
        // Замер времени сортировки
        clock_t time1 = clock();
        
        // Сортировка
        T* sorted = sortFunction(array, len);
        
        clock_t time2 = clock();
        double elapsed = ((double)(time2 - time1)) / CLOCKS_PER_SEC;
        
        cout << sortName << ": " << elapsed << " секунд" << endl;
        
        // Запись отсортированного массива в файл
        ofstream fout(outputFilename);
        if (fout) {
            fout << len << "\n";
            for (size_t i = 0; i < len; i++) {
                fout << sorted[i] << " ";
            }
            fout.close();
            cout << "Отсортированный массив записан в файл " << outputFilename << endl;
        }
        
        delete[] sorted;
        delete[] array;
    } else {
        cout << "Ошибка открытия файла " << inputFilename << endl;
    }
}

// Функция генерации случайного массива и записи в файл
void generateRandomArray(const string& filename, size_t size) {
    int* arr = new int[size];
    
    for (size_t i = 0; i < size; i++) {
        srand(static_cast<unsigned int>(time(nullptr) + i));
        arr[i] = (int)(sin((double)rand()) * N);
    }
    
    ofstream fout(filename);
    if (fout) {
        fout << size << "\n";
        for (size_t i = 0; i < size; i++) {
            fout << arr[i] << " ";
        }
        fout.close();
        cout << "Сгенерирован массив из " << size << " элементов в файле " << filename << endl;
    }
    
    delete[] arr;
}

int main() {
    const size_t size = 10000;

    cout << "=== Тестирование на массиве из 10000 элементов ===\n\n";

    generateRandomArray("array.txt", size);

    cout << "\n";

    // Сортировка расческой
    sortArrayFromFile<int>(
        "array.txt",
        "comb_sorted_large.txt",
        combsort,
        "Сортировка расческой"
    );

    cout << "\n";

    // Сортировка слиянием
    sortArrayFromFile<int>(
        "array.txt",
        "merge_sorted_large.txt",
        mergesort,
        "Сортировка слиянием"
    );

    cout << "\n=== Демонстрация с double ===\n\n";

    // Генерируем небольшой массив double
    double* doubleArr = new double[20];

    for (int i = 0; i < 20; i++) {
        doubleArr[i] = static_cast<double>(rand() % 1000) / 10.0;
    }

    // Записываем массив double в файл
    ofstream fout("double_array.txt");
    if (fout) {
        fout << 20 << "\n";

        for (int i = 0; i < 20; i++) {
            fout << doubleArr[i] << " ";
        }

        fout.close();
    }

    // Сортируем double-массив
    sortArrayFromFile<double>(
        "double_array.txt",
        "double_sorted.txt",
        combsort,
        "Сортировка расческой (double)"
    );

    delete[] doubleArr;

    cout << "\n=== Проверка первых 10 элементов после сортировки ===\n\n";

    // Проверка результата сортировки расческой
    ifstream finComb("comb_sorted_large.txt");
    if (finComb) {
        size_t len;
        finComb >> len;

        cout << "Первые 10 элементов после сортировки расческой: ";

        int value;
        for (size_t i = 0; i < 10 && i < len; i++) {
            finComb >> value;
            cout << value << " ";
        }

        cout << "\n";
        finComb.close();
    } else {
        cout << "Ошибка открытия файла comb_sorted_large.txt\n";
    }

    // Проверка результата сортировки слиянием
    ifstream finMerge("merge_sorted_large.txt");
    if (finMerge) {
        size_t len;
        finMerge >> len;

        cout << "Первые 10 элементов после сортировки слиянием: ";

        int value;
        for (size_t i = 0; i < 10 && i < len; i++) {
            finMerge >> value;
            cout << value << " ";
        }

        cout << "\n";
        finMerge.close();
    } else {
        cout << "Ошибка открытия файла merge_sorted_large.txt\n";
    }

    return 0;
}