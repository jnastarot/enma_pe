#pragma once

namespace enma {

    struct pe_load_config_directory {

    private:
        uint32_t _size;
        uint32_t _timestamp;
        uint16_t _major_version;
        uint16_t _minor_version;
        uint32_t _global_flagsclear;
        uint32_t _global_flagsset;
        uint32_t _criticalsection_default_timeout;
        uint64_t _decommit_freeblock_threshold;
        uint64_t _decommit_totalfree_threshold;
        uint32_t _lock_prefix_table;
        uint64_t _maximum_allocation_size;
        uint64_t _virtual_memory_threshold;
        uint32_t _process_heap_flags;
        uint64_t _process_affinity_mask;
        uint16_t _csd_version;
        uint16_t _dependent_load_flags;
        uint32_t _editlist;
        uint32_t _security_cookie;
        uint32_t _se_handler_table;
        uint32_t _se_handler_count;
        uint32_t _guard_cf_check_function_pointer;
        uint32_t _guard_cf_dispatch_function_pointer; // VA
        uint32_t _guard_cf_function_table;
        uint32_t _guard_cf_function_count;
        uint32_t _guard_flags;
        image_load_config_code_integrity _code_integrity;
        uint32_t _guard_address_taken_iat_entry_table;
        uint32_t _guard_address_taken_iat_entry_count;
        uint32_t _guard_long_jump_target_table;
        uint32_t _guard_long_jump_target_count;
        uint32_t _dynamic_value_reloc_table;
        uint32_t _chpe_meta_data_pointer;
        uint32_t _guard_rf_failure_routine;
        uint32_t _guard_rf_failure_routine_function_pointer;
        uint32_t _dynamic_value_reloc_table_offset;
        uint16_t _dynamic_value_reloc_table_section;
        uint32_t _guard_rf_verify_stack_pointer_function_pointer;
        uint32_t _hot_patch_table_offset;
        uint32_t _enclave_configuration_pointer;

        std::vector<uint32_t> _se_handlers;
        std::vector<uint32_t> _lock_prefixes_rva;
        std::vector<uint32_t> _guard_cf_functions_rva;
        std::vector<uint32_t> _guard_iat_entries_rva;
        std::vector<uint32_t> _guard_long_jump_targets_rva;

    public:
        pe_load_config_directory()
            : _size(0)
            , _timestamp(0)
            , _major_version(0)
            , _minor_version(0)
            , _global_flagsclear(0)
            , _global_flagsset(0)
            , _criticalsection_default_timeout(0)
            , _decommit_freeblock_threshold(0)
            , _decommit_totalfree_threshold(0)
            , _lock_prefix_table(0)
            , _maximum_allocation_size(0)
            , _virtual_memory_threshold(0)
            , _process_heap_flags(0)
            , _process_affinity_mask(0)
            , _csd_version(0)
            , _dependent_load_flags(0)
            , _editlist(0)
            , _security_cookie(0)
            , _se_handler_table(0)
            , _se_handler_count(0)
            , _guard_cf_check_function_pointer(0)
            , _guard_cf_dispatch_function_pointer(0)
            , _guard_cf_function_table(0)
            , _guard_cf_function_count(0)
            , _guard_flags(0)
            , _code_integrity({ 0 })
            , _guard_address_taken_iat_entry_table(0)
            , _guard_address_taken_iat_entry_count(0)
            , _guard_long_jump_target_table(0)
            , _guard_long_jump_target_count(0)
            , _dynamic_value_reloc_table(0)
            , _chpe_meta_data_pointer(0)
            , _guard_rf_failure_routine(0)
            , _guard_rf_failure_routine_function_pointer(0)
            , _dynamic_value_reloc_table_offset(0)
            , _dynamic_value_reloc_table_section(0)
            , _guard_rf_verify_stack_pointer_function_pointer(0)
            , _hot_patch_table_offset(0)
            , _enclave_configuration_pointer(0) {}

        ~pe_load_config_directory() = default;

        pe_load_config_directory(const pe_load_config_directory&) = default;
        pe_load_config_directory& operator=(const pe_load_config_directory&) = default;
        pe_load_config_directory(pe_load_config_directory&&) = default;
        pe_load_config_directory& operator=(pe_load_config_directory&&) = default;

    public:
        constexpr void set_size(uint32_t  size) {
            _size = size;
        }
        constexpr void set_timestamp(uint32_t timestamp) {
            _timestamp = timestamp;
        }
        constexpr void set_major_version(uint16_t major_version) {
            _major_version = major_version;
        }
        constexpr void set_minor_version(uint16_t minor_version) {
            _minor_version = minor_version;
        }
        constexpr void set_global_flagsclear(uint32_t global_flagsclear) {
            _global_flagsclear = global_flagsclear;
        }
        constexpr void set_global_flagsset(uint32_t global_flagsset) {
            _global_flagsset = global_flagsset;
        }
        constexpr void set_criticalsection_default_timeout(uint32_t criticalsection_default_timeout) {
            _criticalsection_default_timeout = criticalsection_default_timeout;
        }
        constexpr void set_decommit_freeblock_threshold(uint64_t  decommit_freeblock_threshold) {
            _decommit_freeblock_threshold = decommit_freeblock_threshold;
        }
        constexpr void set_decommit_totalfree_threshold(uint64_t  decommit_totalfree_threshold) {
            _decommit_totalfree_threshold = decommit_totalfree_threshold;
        }
        constexpr void set_lock_prefix_table(uint32_t lock_prefix_table) {
            _lock_prefix_table = lock_prefix_table;
        }
        constexpr void set_maximum_allocation_size(uint64_t  maximum_allocation_size) {
            _maximum_allocation_size = maximum_allocation_size;
        }
        constexpr void set_virtual_memory_threshold(uint64_t  virtual_memory_threshold) {
            _virtual_memory_threshold = virtual_memory_threshold;
        }
        constexpr void set_process_heap_flags(uint32_t process_heap_flags) {
            _process_heap_flags = process_heap_flags;
        }
        constexpr void set_process_affinity_mask(uint64_t  process_affinity_mask) {
            _process_affinity_mask = process_affinity_mask;
        }
        constexpr void set_csd_version(uint16_t csd_version) {
            _csd_version = csd_version;
        }
        constexpr void set_dependent_load_flags(uint16_t dependent_load_flags) {
            _dependent_load_flags = dependent_load_flags;
        }
        constexpr void set_editlist(uint32_t editlist) {
            _editlist = editlist;
        }
        constexpr void set_security_cookie(uint32_t security_cookie) {
            _security_cookie = security_cookie;
        }
        constexpr void set_se_handler_table(uint32_t se_handler_table) {
            _se_handler_table = se_handler_table;
        }
        constexpr void set_se_handler_count(uint32_t  se_handler_count) {
            _se_handler_count = se_handler_count;
        }
        constexpr void set_guard_cf_check_function_pointer(uint32_t guard_cf_check_function_pointer) {
            _guard_cf_check_function_pointer = guard_cf_check_function_pointer;
        }
        constexpr void set_guard_cf_dispatch_function_pointer(uint32_t  guard_cf_dispatch_function_pointer) {
            _guard_cf_dispatch_function_pointer = guard_cf_dispatch_function_pointer;
        }
        constexpr void set_guard_cf_function_table(uint32_t guard_cf_function_table) {
            _guard_cf_function_table = guard_cf_function_table;
        }
        constexpr void set_guard_cf_function_count(uint32_t  guard_cf_function_count) {
            _guard_cf_function_count = guard_cf_function_count;
        }
        constexpr void set_guard_flags(uint32_t guard_flags) {
            _guard_flags = guard_flags;
        }
        constexpr void set_code_integrity(image_load_config_code_integrity& code_integrity) {
            _code_integrity = code_integrity;
        }
        constexpr void set_guard_address_taken_iat_entry_table(uint32_t guard_address_taken_iat_entry_table) {
            _guard_address_taken_iat_entry_table = guard_address_taken_iat_entry_table;
        }
        constexpr void set_guard_address_taken_iat_entry_count(uint32_t guard_address_taken_iat_entry_count) {
            _guard_address_taken_iat_entry_count = guard_address_taken_iat_entry_count;
        }
        constexpr void set_guard_long_jump_target_table(uint32_t guard_long_jump_target_table) {
            _guard_long_jump_target_table = guard_long_jump_target_table;
        }
        constexpr void set_guard_long_jump_target_count(uint32_t guard_long_jump_target_count) {
            _guard_long_jump_target_count = guard_long_jump_target_count;
        }
        constexpr void set_dynamic_value_reloc_table(uint32_t dynamic_value_reloc_table) {
            _dynamic_value_reloc_table = dynamic_value_reloc_table;
        }
        constexpr void set_chpe_meta_data_pointer(uint32_t chpe_meta_data_pointer) {
            _chpe_meta_data_pointer = chpe_meta_data_pointer;
        }
        constexpr void set_guard_rf_failure_routine(uint32_t guard_rf_failure_routine) {
            _guard_rf_failure_routine = guard_rf_failure_routine;
        }
        constexpr void set_guard_rf_failure_routine_function_pointer(uint32_t guard_rf_failure_routine_function_pointer) {
            _guard_rf_failure_routine_function_pointer = guard_rf_failure_routine_function_pointer;
        }
        constexpr void set_dynamic_value_reloc_table_offset(uint32_t dynamic_value_reloc_table_offset) {
            _dynamic_value_reloc_table_offset = dynamic_value_reloc_table_offset;
        }
        constexpr void set_dynamic_value_reloc_table_section(uint16_t dynamic_value_reloc_table_section) {
            _dynamic_value_reloc_table_section = dynamic_value_reloc_table_section;
        }
        constexpr void set_guard_rf_verify_stack_pointer_function_pointer(uint32_t guard_rf_verify_stack_pointer_function_pointer) {
            _guard_rf_verify_stack_pointer_function_pointer = guard_rf_verify_stack_pointer_function_pointer;
        }
        constexpr void set_hot_patch_table_offset(uint32_t hot_patch_table_offset) {
            _hot_patch_table_offset = hot_patch_table_offset;
        }
        constexpr void set_enclave_configuration_pointer(uint32_t enclave_configuration_pointer) {
            _enclave_configuration_pointer = enclave_configuration_pointer;
        }

    public:
        constexpr uint32_t get_size() const {
            return _size;
        }
        constexpr uint32_t get_timestamp() const {
            return _timestamp;
        }
        constexpr uint16_t get_major_version() const {
            return _major_version;
        }
        constexpr uint16_t get_minor_version() const {
            return _minor_version;
        }
        constexpr uint32_t get_global_flagsclear() const {
            return _global_flagsclear;
        }
        constexpr uint32_t get_global_flagsset() const {
            return _global_flagsset;
        }
        constexpr uint32_t get_criticalsection_default_timeout() const {
            return _criticalsection_default_timeout;
        }
        constexpr uint64_t get_decommit_freeblock_threshold() const {
            return _decommit_freeblock_threshold;
        }
        constexpr uint64_t get_decommit_totalfree_threshold() const {
            return _decommit_totalfree_threshold;
        }
        constexpr uint32_t get_lock_prefix_table() const {
            return _lock_prefix_table;
        }
        constexpr uint64_t get_maximum_allocation_size() const {
            return _maximum_allocation_size;
        }
        constexpr uint64_t get_virtual_memory_threshold() const {
            return _virtual_memory_threshold;
        }
        constexpr uint32_t get_process_heap_flags() const {
            return _process_heap_flags;
        }
        constexpr uint64_t get_process_affinity_mask() const {
            return _process_affinity_mask;
        }
        constexpr uint16_t get_csd_version() const {
            return _csd_version;
        }
        constexpr uint16_t get_dependent_load_flags() const {
            return _dependent_load_flags;
        }
        constexpr uint32_t get_editlist() const {
            return _editlist;
        }
        constexpr uint32_t get_security_cookie() const {
            return _security_cookie;
        }
        constexpr uint32_t get_se_handler_table() const {
            return _se_handler_table;
        }
        constexpr uint32_t get_se_handler_count() const {
            return _se_handler_count;
        }
        constexpr uint32_t get_guard_cf_check_function_pointer() const {
            return _guard_cf_check_function_pointer;
        }
        constexpr uint32_t get_guard_cf_dispatch_function_pointer() const {
            return _guard_cf_dispatch_function_pointer;
        }
        constexpr uint32_t get_guard_cf_function_table() const {
            return _guard_cf_function_table;
        }
        constexpr uint32_t get_guard_cf_function_count() const {
            return _guard_cf_function_count;
        }
        constexpr uint32_t get_guard_flags() const {
            return _guard_flags;
        }
        constexpr const std::vector<uint32_t >& get_se_handlers() const { //x86 only
            return _se_handlers;
        }
        constexpr const std::vector<uint32_t >& get_lock_prefixes() const { //x86 only
            return _lock_prefixes_rva;
        }
        constexpr const std::vector<uint32_t >& get_guard_cf_functions() const {
            return _guard_cf_functions_rva;
        }
        constexpr const std::vector<uint32_t >& get_guard_iat_entries() const {
            return _guard_iat_entries_rva;
        }
        constexpr const std::vector<uint32_t >& get_guard_long_jump_targets() const {
            return _guard_long_jump_targets_rva;
        }
        constexpr std::vector<uint32_t>& get_se_handlers() { //x86 only
            return _se_handlers;
        }
        constexpr std::vector<uint32_t>& get_lock_prefixes() { //x86 only
            return _lock_prefixes_rva;
        }
        constexpr std::vector<uint32_t>& get_guard_cf_functions() {
            return _guard_cf_functions_rva;
        }
        constexpr std::vector<uint32_t >& get_guard_iat_entries() {
            return _guard_iat_entries_rva;
        }
        constexpr std::vector<uint32_t >& get_guard_long_jump_targets() {
            return _guard_long_jump_targets_rva;
        }
        constexpr const image_load_config_code_integrity& get_code_integrity() const {
            return _code_integrity;
        }
        constexpr uint32_t get_guard_address_taken_iat_entry_table() const {
            return _guard_address_taken_iat_entry_table;
        }
        constexpr uint32_t get_guard_address_taken_iat_entry_count() const {
            return _guard_address_taken_iat_entry_count;
        }
        constexpr uint32_t get_guard_long_jump_target_table() const {
            return _guard_long_jump_target_table;
        }
        constexpr uint32_t get_guard_long_jump_target_count() const {
            return _guard_long_jump_target_count;
        }
        constexpr uint32_t get_dynamic_value_reloc_table() const {
            return _dynamic_value_reloc_table;
        }
        constexpr uint32_t get_chpe_meta_data_pointer() const {
            return _chpe_meta_data_pointer;
        }
        constexpr uint32_t get_guard_rf_failure_routine() const {
            return _guard_rf_failure_routine;
        }
        constexpr uint32_t get_guard_rf_failure_routine_function_pointer() const {
            return _guard_rf_failure_routine_function_pointer;
        }
        constexpr uint32_t get_dynamic_value_reloc_table_offset() const {
            return _dynamic_value_reloc_table_offset;
        }
        constexpr uint16_t get_dynamic_value_reloc_table_section() const {
            return _dynamic_value_reloc_table_section;
        }
        constexpr uint32_t get_guard_rf_verify_stack_pointer_function_pointer() const {
            return _guard_rf_verify_stack_pointer_function_pointer;
        }
        constexpr uint32_t get_hot_patch_table_offset() const {
            return _hot_patch_table_offset;
        }
        constexpr uint32_t get_enclave_configuration_pointer() const {
            return _enclave_configuration_pointer;
        }
    };


    enum load_config_table_build_id {
        load_config_table_build_se_handlers = 1 << 1,
        load_config_table_build_lock_prefixes = 1 << 2,
        load_config_table_build_guard_cf_functions = 1 << 3,
        load_config_table_build_guard_iat_entrys = 1 << 4,
        load_config_table_build_guard_long_jump_targets = 1 << 5,
    };

    pe_directory_code get_load_config_directory(_In_ const pe_image& image, _Out_ pe_load_config_directory& load_config);

    bool build_internal_load_config_directory_data(_Inout_ pe_image& image, _Inout_ pe_section& section,
        _Inout_ pe_load_config_directory& load_config, _In_ uint32_t build_items_ids/*import_table_build_id*/,
        _Inout_ pe_relocations_directory& relocs);

    bool build_load_config_directory_only(_Inout_ pe_image& image, _Inout_ pe_section& section,
        _Inout_ pe_load_config_directory& load_config, _Inout_ pe_relocations_directory& relocs);
    bool build_load_config_directory_full(_Inout_ pe_image& image, _Inout_ pe_section& section,
        _Inout_ pe_load_config_directory& load_config, _Inout_ pe_relocations_directory& relocs);


    pe_directory_code get_placement_load_config_directory(_Inout_ const pe_image& image, _Inout_ pe_placement& placement);

}