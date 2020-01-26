#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <stdlib.h>

extern void profiler_print_results();

#define DB_PATH "test.db"

int main(int argc, char** argv) {
    remove(DB_PATH);

    sqlite3* db;
    sqlite3_open(DB_PATH, &db);

    if (sqlite3_exec(db, "CREATE TABLE PEOPLE (ID, NAME, AGE)", 0, 0, 0) != SQLITE_OK) {
        printf("Create table failed!\n");
        return 1;
    }

    struct Human {
        int id;
        const char* name;
        int age;
    } people[] = {
        { .id = 0, .name = "Nick", .age = 45 },
        { .id = 1, .name = "Alice", .age = 20 },
        { .id = 2, .name = "John", .age = 27 },
    };

    for (int i = 0; i < 3; ++i) {
        sqlite3_stmt* insertStatement;
        if (sqlite3_prepare_v2(db, "INSERT INTO PEOPLE VALUES (?, ?, ?)", -1, &insertStatement, 0) != SQLITE_OK) {
            printf("Insert statement preparation failed !\n");
            return 1;
        }

        if (sqlite3_bind_int(insertStatement, 1, people[i].id) != SQLITE_OK) {
            printf("Id bind failed!\n");
            return 1;
        }
        if (sqlite3_bind_text(insertStatement, 2, people[i].name, strlen(people[i].name), SQLITE_STATIC) != SQLITE_OK) {
            printf("Name bind failed!\n");
            return 1;
        }
        if (sqlite3_bind_int(insertStatement, 3, people[i].age) != SQLITE_OK) {
            printf("Age bind failed!\n");
            return 1;
        }

        if (sqlite3_step(insertStatement) != SQLITE_DONE) {
            printf("Insertion failed");
            return 1;
        }

        sqlite3_finalize(insertStatement);
    }


    sqlite3_close(db);


    sqlite3_open(DB_PATH, &db);
    sqlite3_stmt* cursor;
    if (sqlite3_prepare_v2(db, "SELECT id, name, age FROM PEOPLE", -1, &cursor, 0) != SQLITE_OK) {
        printf("Select failed!\n");
        return 1;
    }

    while(sqlite3_step(cursor) == SQLITE_ROW) {
        int id = sqlite3_column_int(cursor, 0);
        const char* name = (const char*)sqlite3_column_text(cursor, 1);
        int age = sqlite3_column_int(cursor, 2);

        printf("[%s] Id: %d, Age: %d\n", name, id, age);
    }
    sqlite3_finalize(cursor);

    profiler_print_results();
    return 0;
}
