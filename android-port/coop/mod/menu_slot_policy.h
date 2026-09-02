#ifndef DKCOOP_MENU_SLOT_POLICY_H
#define DKCOOP_MENU_SLOT_POLICY_H

// The ordinary menu has file 1, file 2, delete, file 3. Co-op campaigns use
// only physical file 1, so the visible menu becomes file 1, delete.
static inline unsigned coop_file_menu_count(unsigned single_slot) {
    return single_slot ? 2 : 4;
}

static inline unsigned coop_file_menu_delete_index(unsigned single_slot) {
    return single_slot ? 1 : 2;
}

static inline unsigned coop_file_menu_file(unsigned single_slot, unsigned selection) {
    if (single_slot) return 0;
    return selection == 3 ? 2 : selection;
}

static inline unsigned coop_delete_menu_count(unsigned single_slot) {
    return single_slot ? 1 : 3;
}

#endif
