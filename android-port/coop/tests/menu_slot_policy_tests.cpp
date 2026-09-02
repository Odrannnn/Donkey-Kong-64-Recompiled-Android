#include "../mod/menu_slot_policy.h"
#include <cstdio>
#include <cstdlib>

static unsigned checks;
#define CHECK(x) do { ++checks; if (!(x)) { std::fprintf(stderr, "MENU SLOT POLICY FAIL %d: %s\n", __LINE__, #x); std::exit(1); } } while (0)

int main() {
    CHECK(coop_file_menu_count(0) == 4);
    CHECK(coop_file_menu_delete_index(0) == 2);
    CHECK(coop_file_menu_file(0, 0) == 0);
    CHECK(coop_file_menu_file(0, 1) == 1);
    CHECK(coop_file_menu_file(0, 3) == 2);
    CHECK(coop_delete_menu_count(0) == 3);

    CHECK(coop_file_menu_count(1) == 2);
    CHECK(coop_file_menu_delete_index(1) == 1);
    CHECK(coop_file_menu_file(1, 0) == 0);
    CHECK(coop_file_menu_file(1, 3) == 0);
    CHECK(coop_delete_menu_count(1) == 1);
    std::printf("PASS: %u single-slot co-op menu policy checks\n", checks);
}
