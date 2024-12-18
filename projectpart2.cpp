#include <iostream>
#include <vector>
#include <iterator>
#include <omp.h>
#include <chrono>
#include <type_traits>
#include <typeinfo>

// Multi-threaded find_all using OpenMP
template <typename Iterator, typename T>
auto find_all_omp(Iterator begin, Iterator end, const T& value) {
    using ValueType = typename std::iterator_traits<Iterator>::value_type;
    
    // Check for type compatibility at runtime
    if (typeid(ValueType) != typeid(T)) {
        std::cout << "Error: Incorrect type. Expected type '" << typeid(ValueType).name()
                  << "' but got '" << typeid(T).name() << "'." << std::endl;
        return std::vector<Iterator>{};  // Return an empty vector
    }

    std::vector<Iterator> result;
    size_t container_size = std::distance(begin, end);
    int num_threads = (container_size < 1000) ? 2 : (container_size < 10000 ? 4 : 8);
    int num_threads_used = 0;

#pragma omp parallel num_threads(num_threads)
    {
        std::vector<Iterator> local_result;

#pragma omp single
        num_threads_used = omp_get_num_threads();

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

    std::cout << "Threads used: " << num_threads_used << std::endl;
    return result;
}

// Single-threaded find_all without OpenMP
template <typename Iterator, typename T>
auto find_all_single_thread(Iterator begin, Iterator end, const T& value) {
    using ValueType = typename std::iterator_traits<Iterator>::value_type;
    
    // Check for type compatibility at runtime
    if (typeid(ValueType) != typeid(T)) {
        std::cout << "Error: Incorrect type. Expected type '" << typeid(ValueType).name()
                  << "' but got '" << typeid(T).name() << "'." << std::endl;
        return std::vector<Iterator>{};  // Return an empty vector
    }

    std::vector<Iterator> result;
    for (Iterator it = begin; it != end; ++it) {
        if (*it == value) {
            result.push_back(it);
        }
    }
    return result;
}

// Function to test and time multi-threaded OpenMP execution
template <typename Container, typename T>
void test_find_all_omp(const Container& container, const T& value) {
    auto start = std::chrono::high_resolution_clock::now();

    auto result = find_all_omp(container.begin(), container.end(), value);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Time taken with OpenMP: " << elapsed.count() << " seconds\n";
    std::cout << "Number of matches found: " << result.size() << std::endl;
}

// Function to test and time single-threaded execution
template <typename Container, typename T>
void test_find_all_single_thread(const Container& container, const T& value) {
    auto start = std::chrono::high_resolution_clock::now();

    auto result = find_all_single_thread(container.begin(), container.end(), value);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Time taken without OpenMP (single-thread): " << elapsed.count() << " seconds\n";
    std::cout << "Number of matches found: " << result.size() << std::endl;
}

int main() {
    // Test with an int vector
    std::vector<int> v1(1000000, 1);
    v1[100000] = 2;
    v1[500000] = 2;
    std::cout << "Test with int vector, searching for 2:" << std::endl;
    test_find_all_single_thread(v1, 2);
    test_find_all_omp(v1, 2);
    

    return 0;
}
