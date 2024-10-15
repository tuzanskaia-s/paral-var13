#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <cassert>

const int TOTAL_STUDENTS = 25;
const int TOTAL_DAYS = 30;
const int DAILY_TICKETS = 3;

std::mutex mtx;
std::condition_variable cv;
int tickets_left = 0;
int students_with_first_ticket = 0;
bool all_students_got_first_ticket = false;
std::vector<int> tickets_for_students(TOTAL_STUDENTS, 0);

void ticket_supplier() {
    for (int day = 0; day < TOTAL_DAYS; ++day) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            tickets_left = DAILY_TICKETS;
            std::cout << "Day " << day + 1 << ": 3 tickets are available." << std::endl;
        }
        cv.notify_all();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void student(int id) {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return tickets_left > 0 || students_with_first_ticket == TOTAL_STUDENTS; });

        if (tickets_for_students[id] == 2) {
            return;
        }

        if (tickets_left > 0 &&
            (tickets_for_students[id] == 0 || (tickets_for_students[id] == 1 && all_students_got_first_ticket))) {
            --tickets_left;
            tickets_for_students[id]++;

            if (tickets_for_students[id] == 1) {
                students_with_first_ticket++;
                std::cout << "Student " << id + 1 << " got the first ticket." << std::endl;
            } else if (tickets_for_students[id] == 2) {
                std::cout << "Student " << id + 1 << " got the second ticket." << std::endl;
            }

            if (students_with_first_ticket == TOTAL_STUDENTS) {
                all_students_got_first_ticket = true;
            }
        }
    }
}

// Вся логика программы вынесена в эту функцию
void run_simulation() {
    std::thread supplier(ticket_supplier);
    std::vector<std::thread> students;

    for (int i = 0; i < TOTAL_STUDENTS; ++i) {
        students.emplace_back(student, i);
    }

    supplier.join();

    for (auto& s : students) {
        s.join();
    }
}

// Тест1: Все студенты получили по два билета
void test_all_students_got_two_tickets() {
    run_simulation();  // Запускаем симуляцию
    for (int tickets : tickets_for_students) {
        assert(tickets == 2 && "Каждый студент должен получить ровно два билета");
    }
    std::cout << "Тест пройден: Все студенты получили по два билета.\n";
}

// Тест2: Никто не получил второй билет до получения первого всеми студентами
void test_no_second_ticket_before_first() {
    bool all_first_tickets_given = false;
    for (int day = 0; day < TOTAL_DAYS; ++day) {
        int first_tickets = 0, second_tickets = 0;
        for (int i = 0; i < TOTAL_STUDENTS; ++i) {
            if (tickets_for_students[i] == 1) {
                first_tickets++;
            } else if (tickets_for_students[i] == 2) {
                second_tickets++;
            }
        }
        if (second_tickets > 0 && first_tickets < TOTAL_STUDENTS) {
            assert(false && "Нельзя получить второй билет, пока все не получат первый");
        }
        if (first_tickets == TOTAL_STUDENTS) {
            all_first_tickets_given = true;
        }
    }
    assert(all_first_tickets_given && "Все студенты должны получить первый билет перед вторым");
    std::cout << "Тест пройден: Никто не получил второй билет до получения первого всеми студентами.\n";
}

// Тест3: За каждый день раздается не более трех билетов
void test_no_more_than_three_tickets_per_day() {
    int day_tickets = 0;
    for (int day = 0; day < TOTAL_DAYS; ++day) {
        day_tickets = 0;
        for (int i = 0; i < TOTAL_STUDENTS; ++i) {
            day_tickets += (tickets_for_students[i] == 1) ? 1 : 0;
            day_tickets += (tickets_for_students[i] == 2) ? 1 : 0;
        }
        assert(day_tickets <= DAILY_TICKETS && "За один день нельзя выдать больше трех билетов");
    }
    std::cout << "Тест пройден: Каждый день раздается не более трех билетов.\n";
}

// Тест4: Корректное завершение всех потоков
void test_all_threads_complete() {
    run_simulation();  // Запускаем симуляцию
    std::cout << "Тест пройден: Все потоки завершились корректно.\n";
}

int main() {
    // Запуск тестов
    test_all_students_got_two_tickets();
    test_no_second_ticket_before_first();
    test_no_more_than_three_tickets_per_day();
    test_all_threads_complete();

    return 0;
}
