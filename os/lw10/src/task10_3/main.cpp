#include <iostream>
#include "../libProject/studentDb.h"

int main() {
    void* db = CreateDatabase();

    Student student1 = {1, "Max", "Computer Science", 2021};
    Student student2 = {2, "Alex", "Mathematics", 2020};

    AddStudent(db, &student1);
    AddStudent(db, &student2);

    Student* found = FindStudentById(db, 1);
    if (found) {
        std::cout << "Found student: " << found->name << std::endl;
    }

    auto students = FindStudentsByName(db, "Alice");
    std::cout << "Found " << students.count << " students named Alice." << std::endl;

    RemoveStudent(db, 1);
    DestroyDatabase(db);

    return EXIT_SUCCESS;
}