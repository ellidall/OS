#include "studentDb.h"
#include <unordered_map>
#include <vector>
#include <string>

class StudentDatabase {
private:
    std::unordered_map<uint32_t, Student> students;

public:
    int AddStudent(const Student& student) {
        if (students.find(student.id) != students.end()) {
            return -1;
        }
        students[student.id] = student;
        return 0;
    }

    int RemoveStudent(uint32_t id) {
        auto it = students.find(id);
        if (it == students.end()) {
            return -1;
        }
        students.erase(it);
        return 0;
    }

    Student* FindStudentById(uint32_t id) {
        auto it = students.find(id);
        if (it != students.end()) {
            return &it->second;
        }
        return nullptr;
    }

    std::vector<Student*> FindStudentsByName(const std::string& name) {
        std::vector<Student*> result;
        for (auto& pair : students) {
            if (pair.second.name == name) {
                result.push_back(&pair.second);
            }
        }
        return result;
    }
};

extern "C" {

EXPORT void* CreateDatabase() {
    return new StudentDatabase();
}

EXPORT void DestroyDatabase(void* db) {
    delete static_cast<StudentDatabase*>(db);
}

EXPORT int AddStudent(void* db, const Student* student) {
    return static_cast<StudentDatabase*>(db)->AddStudent(*student);
}

EXPORT int RemoveStudent(void* db, uint32_t id) {
    return static_cast<StudentDatabase*>(db)->RemoveStudent(id);
}

EXPORT Student* FindStudentById(void* db, uint32_t id) {
    return static_cast<StudentDatabase*>(db)->FindStudentById(id);
}

EXPORT StudentArray FindStudentsByName(void* db, const char* name) {
    auto results = static_cast<StudentDatabase*>(db)->FindStudentsByName(name);

    StudentArray array;
    array.count = results.size();
    array.students = results.data();

    return array;
}

}
