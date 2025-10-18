#pragma once

#include <cstdint>
#include <cstdlib>

#if defined(_WIN32) || defined(_WIN64)
    #if defined(COMPILING_DLL)
        #define EXPORT __declspec(dllexport)
    #else
        #define EXPORT __declspec(dllimport)
    #endif
#else
    #if defined(COMPILING_SO)
        #define EXPORT __attribute__((visibility("default")))
    #else
        #define EXPORT
    #endif
#endif

extern "C" {

typedef struct
{
    uint32_t id;
    const char* name;
    const char* major;
    uint32_t yearOfAdmission;
} Student;

EXPORT void* CreateDatabase();

EXPORT void DestroyDatabase(void* db);

EXPORT int AddStudent(void* db, const Student* student);

EXPORT int RemoveStudent(void* db, uint32_t id);

EXPORT Student* FindStudentById(void* db, uint32_t id);

typedef struct {
    Student** students;
    size_t count;
} StudentArray;

EXPORT StudentArray  FindStudentsByName(void* db, const char* name);

}