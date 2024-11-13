#include <iostream>
#include <vector>
#include <iterator>
#include <omp.h>
#include <chrono>
#include <type_traits>

// Функция find_all с использованием OpenMP
template <typename Iterator, typename T>
auto find_all(Iterator begin, Iterator end, const T& value) {
    static_assert(std::is_default_constructible<T>::value, "Value type must be default constructible");
    
    std::vector<Iterator> result;
    size_t container_size = std::distance(begin, end);
    int num_threads = (container_size < 1000) ? 2 : (container_size < 10000 ? 4 : 8);  // Подбор потоков в зависимости от размера

    #pragma omp parallel num_threads(num_threads)
    {
        std::vector<Iterator> local_result;
        
        #pragma omp for nowait
        for (size_t i = 0; i < container_size; ++i) {
            Iterator it = begin;
            std::advance(it, i);
            if (*it == value) {
                local_result.push_back(it);
            }
        }

        #pragma omp critical
        result.insert(result.end(), local_result.begin(), local_result.end());
    }

    return result;
}

// Функция для тестирования и замера времени работы
template <typename Container, typename T>
void test_find_all(const Container& container, const T& value) {
    auto start = std::chrono::high_resolution_clock::now();
    
    auto result = find_all(container.begin(), container.end(), value);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    
    std::cout << "Time taken with OpenMP: " << elapsed.count() << " seconds\n";
    std::cout << "Number of matches found: " << result.size() << std::endl;
}

// Пример использования
int main() {
    // Тесты на числовом типе
    std::vector<int> v1(1000000, 1);
    v1[100000] = 2;
    v1[500000] = 2;
    std::cout << "Test with int vector, searching for 2:" << std::endl;
    test_find_all(v1, 2);

   ]

    return 0;
}
