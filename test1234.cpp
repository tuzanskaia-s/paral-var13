#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>

// Подключаем исходный код программы, для которого будем писать тесты
#include "projectpart1.cpp"

// Переменные, используемые для синхронизации и тестирования
std::atomic<int> total_tickets_issued{0};

// Тест на проверку того, что каждый студент получает не более двух билетов
TEST(StudentTicketProgram, NoMoreThanTwoTicketsPerStudent) {
    for (int tickets : tickets_for_students) {
        EXPECT_LE(tickets, 2) << "Student received more than 2 tickets!";
    }
}

// Тест на проверку того, что каждый студент получает первый билет до второго
TEST(StudentTicketProgram, FirstTicketBeforeSecond) {
    for (int i = 0; i < TOTAL_STUDENTS; ++i) {
        if (tickets_for_students[i] == 2) {
            EXPECT_TRUE(has_first_ticket[i]) << "Student " << i + 1 << " got second ticket without first!";
        }
    }
}

// Тест на проверку того, что все студенты получили первый билет перед тем, как кто-либо из них получил второй билет
TEST(StudentTicketProgram, AllFirstTicketsBeforeAnySecond) {
    int students_with_second_ticket = 0;
    for (int tickets : tickets_for_students) {
        if (tickets == 2) {
            students_with_second_ticket++;
        }
    }
    EXPECT_EQ(students_with_first_ticket, TOTAL_STUDENTS) << "Not all students got their first ticket before any second ticket was issued!";
}

// Тест на проверку, что общее количество выданных билетов не превышает допустимого за все дни
TEST(StudentTicketProgram, TotalTicketsCorrect) {
    total_tickets_issued = 0;
    for (int tickets : tickets_for_students) {
        total_tickets_issued += tickets;
    }
    EXPECT_LE(total_tickets_issued, TOTAL_DAYS * DAILY_TICKETS) << "More tickets were issued than available.";
}

// Тест на проверку корректности завершения работы всех потоков
TEST(StudentTicketProgram, AllThreadsJoinCorrectly) {
    EXPECT_NO_THROW({
        std::thread supplier(ticket_supplier);
        std::vector<std::thread> students;
        for (int i = 0; i < TOTAL_STUDENTS; ++i) {
            students.emplace_back(student, i);
        }

        supplier.join();

        for (auto& s : students) {
            s.join();
        }
    }) << "Error occurred during thread execution.";
}

// Функция для инициализации всех глобальных переменных для запуска каждого теста
void ResetGlobals() {
    tickets_left = 0;
    students_with_first_ticket = 0;
    all_students_got_first_ticket = false;
    std::fill(has_first_ticket.begin(), has_first_ticket.end(), false);
    std::fill(tickets_for_students.begin(), tickets_for_students.end(), 0);
}

// Главная функция тестов
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ResetGlobals();
    return RUN_ALL_TESTS();
}
