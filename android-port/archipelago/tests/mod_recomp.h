// Test adapter for upstream OfflineModRecomp's runtime function-pointer interface.
#pragma once
#define get_function unused_get_function_declaration
#define section_addresses unused_section_addresses_declaration
#define cop0_status_write unused_cop0_status_write_declaration
#define cop0_status_read unused_cop0_status_read_declaration
#define switch_error unused_switch_error_declaration
#define do_break unused_do_break_declaration
#include "recomp.h"
#undef get_function
#undef section_addresses
#undef cop0_status_write
#undef cop0_status_read
#undef switch_error
#undef do_break
#define RECOMP_EXPORT
#define REF_RELOC_HI16(index, offset) HI16(reference_section_addresses[index] + (offset))
#define REF_RELOC_LO16(index, offset) LO16(reference_section_addresses[index] + (offset))
