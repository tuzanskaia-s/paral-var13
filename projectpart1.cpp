#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>

const int TOTAL_STUDENTS = 25;
const int TOTAL_DAYS = 30;
const int DAILY_TICKETS = 3;

std::mutex mtx;  // Mutex for synchronizing access to tickets
std::condition_variable cv;
int tickets_left = 0;
int students_with_first_ticket = 0;
bool all_students_got_first_ticket = false;
std::vector<bool> has_first_ticket(TOTAL_STUDENTS, false);
std::vector<int> tickets_for_students(TOTAL_STUDENTS, 0);  // Number of tickets each student has received

void ticket_supplier() {
    for (int day = 0; day < TOTAL_DAYS; ++day) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            tickets_left = DAILY_TICKETS;  // At the start of the day, 3 tickets become available
            std::cout << "Day " << day + 1 << ": 3 tickets are available." << std::endl;
        }
        cv.notify_all();  // Notify students that tickets are available

        // Wait for the next day
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void student(int id) {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);

        // Wait until tickets become available
        cv.wait(lock, [] { return tickets_left > 0 || students_with_first_ticket == TOTAL_STUDENTS; });

        // If the student has already received two tickets, exit the loop
        if (tickets_for_students[id] == 2) {
            return;
        }

        // The student tries to get a ticket
        if (tickets_left > 0 &&
            (tickets_for_students[id] == 0 || (tickets_for_students[id] == 1 && all_students_got_first_ticket))) {

            --tickets_left;
            tickets_for_students[id]++;

            if (tickets_for_students[id] == 1) {
                students_with_first_ticket++;
                std::cout << "Student " << id + 1 << " got the first ticket." << std::endl;
            }
            else if (tickets_for_students[id] == 2) {
                std::cout << "Student " << id + 1 << " got the second ticket." << std::endl;
            }

            // Check if all students have received their first ticket
            if (students_with_first_ticket == TOTAL_STUDENTS) {
                all_students_got_first_ticket = true;
            }
        }
    }
}

int main() {
    std::thread supplier(ticket_supplier);
    std::vector<std::thread> students;

    for (int i = 0; i < TOTAL_STUDENTS; ++i) {
        students.emplace_back(student, i);
    }

    supplier.join();

    for (auto& s : students) {
        s.join();
    }

    return 0;
}
