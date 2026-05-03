#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <iomanip>
#include <algorithm>
#include <cmath>

using namespace std;

struct Pixel {
    int r;
    int g;
    int b;
};

using Image = vector<vector<Pixel>>;

using ImageBatch = vector<Image>;

using Matrix = vector<vector<double>>;

using Filter = vector<vector<double>>;

Pixel generateRandomPixel(mt19937& generator) {
    uniform_int_distribution<int> distribution(0, 255);

    Pixel pixel;
    pixel.r = distribution(generator);
    pixel.g = distribution(generator);
    pixel.b = distribution(generator);

    return pixel;
}

Image generateRandomImage(int height, int width) {
    random_device rd;
    mt19937 generator(rd());

    Image image(height, vector<Pixel>(width));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            image[y][x] = generateRandomPixel(generator);
        }
    }

    return image;
}

ImageBatch generateRandomBatch(int imageCount, int height, int width) {
    ImageBatch batch;

    for (int i = 0; i < imageCount; i++) {
        batch.push_back(generateRandomImage(height, width));
    }

    return batch;
}

int getChannelValue(const Pixel& pixel, int channel) {
    if (channel == 0) {
        return pixel.r;
    }

    if (channel == 1) {
        return pixel.g;
    }

    return pixel.b;
}

Matrix convertToGray(const Image& image) {
    int height = image.size();
    int width = image[0].size();

    Matrix gray(height, vector<double>(width, 0));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int R = image[y][x].r;
            int G = image[y][x].g;
            int B = image[y][x].b;

            gray[y][x] = 0.3 * R + 0.59 * G + 0.11 * B;
        }
    }

    return gray;
}

Matrix convolutionGray(const Matrix& image, const Filter& filter) {
    int height = image.size();
    int width = image[0].size();

    int filterSize = 3;

    int resultHeight = height - filterSize + 1;
    int resultWidth = width - filterSize + 1;

    Matrix result(resultHeight, vector<double>(resultWidth, 0));

    for (int y = 0; y < resultHeight; y++) {
        for (int x = 0; x < resultWidth; x++) {
            double sum = 0;

            for (int fy = 0; fy < filterSize; fy++) {
                for (int fx = 0; fx < filterSize; fx++) {
                    sum += image[y + fy][x + fx] * filter[fy][fx];
                }
            }

            result[y][x] = sum;
        }
    }

    return result;
}

Matrix convolutionEvenVariant(const Image& image, const Filter& filter) {
    int height = image.size();
    int width = image[0].size();

    int filterSize = 3;

    int resultHeight = height - filterSize + 1;
    int resultWidth = width - filterSize + 1;

    Matrix result(resultHeight, vector<double>(resultWidth, 0));

    for (int y = 0; y < resultHeight; y++) {
        for (int x = 0; x < resultWidth; x++) {
            double totalSum = 0;

            // Перебираем каналы R, G, B
            for (int channel = 0; channel < 3; channel++) {
                double channelSum = 0;

                for (int fy = 0; fy < filterSize; fy++) {
                    for (int fx = 0; fx < filterSize; fx++) {
                        int pixelValue = getChannelValue(
                            image[y + fy][x + fx],
                            channel
                        );

                        channelSum += pixelValue * filter[fy][fx];
                    }
                }

                totalSum += channelSum;
            }

            result[y][x] = totalSum;
        }
    }

    return result;
}

Matrix normalizeToByteRange(const Matrix& image) {
    int height = image.size();
    int width = image[0].size();

    Matrix normalized(height, vector<double>(width, 0));

    double minValue = image[0][0];
    double maxValue = image[0][0];

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            minValue = min(minValue, image[y][x]);
            maxValue = max(maxValue, image[y][x]);
        }
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (maxValue == minValue) {
                normalized[y][x] = 0;
            } else {
                normalized[y][x] =
                    (image[y][x] - minValue) * 255.0 / (maxValue - minValue);
            }
        }
    }

    return normalized;
}

void savePGM(const Matrix& image, const string& filename) {
    Matrix normalized = normalizeToByteRange(image);

    int height = normalized.size();
    int width = normalized[0].size();

    ofstream file(filename);

    if (!file.is_open()) {
        cout << "Ошибка открытия файла: " << filename << endl;
        return;
    }

    file << "P2\n";
    file << width << " " << height << "\n";
    file << "255\n";

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int value = static_cast<int>(round(normalized[y][x]));

            if (value < 0) {
                value = 0;
            }

            if (value > 255) {
                value = 255;
            }

            file << value << " ";
        }

        file << "\n";
    }

    file.close();
}

void saveMatrixToTXT(const Matrix& matrix, const string& filename) {
    ofstream file(filename);

    if (!file.is_open()) {
        cout << "Ошибка открытия файла: " << filename << endl;
        return;
    }

    for (int y = 0; y < matrix.size(); y++) {
        for (int x = 0; x < matrix[y].size(); x++) {
            file << fixed << setprecision(3) << matrix[y][x] << " ";
        }

        file << "\n";
    }

    file.close();
}

void printImage(const Image& image, const string& title) {
    cout << title << endl;

    for (int y = 0; y < image.size(); y++) {
        for (int x = 0; x < image[y].size(); x++) {
            cout << "("
                 << setw(3) << image[y][x].r << ","
                 << setw(3) << image[y][x].g << ","
                 << setw(3) << image[y][x].b << ") ";
        }

        cout << endl;
    }

    cout << endl;
}

void printMatrix(const Matrix& matrix, const string& title) {
    cout << title << endl;

    for (int y = 0; y < matrix.size(); y++) {
        for (int x = 0; x < matrix[y].size(); x++) {
            cout << setw(10) << fixed << setprecision(3) << matrix[y][x] << " ";
        }

        cout << endl;
    }

    cout << endl;
}

int main() {
    // Размер исходного изображения
    const int height = 6;
    const int width = 6;

    // Количество изображений во входном наборе
    const int imageCount = 2;

    // Фильтр верхних частот для варианта 30:
    Filter highPassFilter = {
        {-1.0 / 9.0, -1.0 / 9.0, -1.0 / 9.0},
        {-1.0 / 9.0,  8.0 / 9.0, -1.0 / 9.0},
        {-1.0 / 9.0, -1.0 / 9.0, -1.0 / 9.0}
    };

    // Генерируем набор изображений.
    ImageBatch batch = generateRandomBatch(imageCount, height, width);

    for (int i = 0; i < imageCount; i++) {
        cout << "Изображение #" << i + 1 << endl;

        printImage(batch[i], "Исходное RGB-изображение:");

        Matrix result;
        
        result = convolutionEvenVariant(batch[i], highPassFilter);

        printMatrix(result, "Результат свертки:");

        string pgmFilename = "result_" + to_string(i + 1) + ".pgm";
        string txtFilename = "result_" + to_string(i + 1) + ".txt";

        savePGM(result, pgmFilename);
        saveMatrixToTXT(result, txtFilename);

        cout << "Изображение сохранено в файл: " << pgmFilename << endl;
        cout << "Числовая матрица сохранена в файл: " << txtFilename << endl;
        cout << "----------------------------------------" << endl;
    }

    return 0;
}