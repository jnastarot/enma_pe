#pragma once


class load_config_table{
    uint32_t        size;
	uint32_t 		timestamp;
	uint16_t 		major_version;
	uint16_t 		minor_version;
	uint32_t 		global_flagsclear;
	uint32_t 		global_flagsset;
	uint32_t 		criticalsection_default_timeout;
    uint64_t        decommit_freeblock_threshold;
    uint64_t        decommit_totalfree_threshold;
    uint32_t 	    lock_prefix_table;      
    uint64_t        maximum_allocation_size;
    uint64_t        virtual_memory_threshold;
	uint32_t 		process_heap_flags;
    uint64_t        process_affinity_mask;
	uint16_t 		csd_version;
    uint16_t        dependent_load_flags;
    uint32_t 	    editlist;			
    uint32_t 	    security_cookie;		
    uint32_t 	    se_handler_table;
    uint32_t 	    se_handler_count;
    uint32_t 	    guard_cf_check_function_pointer;
    uint32_t        guard_cf_dispatch_function_pointer; // VA
    uint32_t 	    guard_cf_function_table;
    uint32_t 	    guard_cf_function_count;
	uint32_t 		guard_flags;
    image_load_config_code_integrity code_integrity;
    uint32_t        guard_address_taken_iat_entry_table;
    uint32_t        guard_address_taken_iat_entry_count;
    uint32_t        guard_long_jump_target_table;
    uint32_t        guard_long_jump_target_count;
    uint32_t        dynamic_value_reloc_table;
    uint32_t        chpe_meta_data_pointer;
    uint32_t        guard_rf_failure_routine;
    uint32_t        guard_rf_failure_routine_function_pointer;
    uint32_t        dynamic_value_reloc_table_offset;
    uint16_t        dynamic_value_reloc_table_section;
    uint32_t        guard_rf_verify_stack_pointer_function_pointer;
    uint32_t        hot_patch_table_offset;
    uint32_t        enclave_configuration_pointer;

	std::vector<uint32_t> se_handlers;
	std::vector<uint32_t> lock_prefixes_rva;
    std::vector<uint32_t> guard_cf_functions_rva;
    std::vector<uint32_t> guard_iat_entries_rva;
    std::vector<uint32_t> guard_long_jump_targets_rva;
public:
	load_config_table::load_config_table();
	load_config_table::~load_config_table();
public:
    void		load_config_table::set_size(uint32_t  size);
	void		load_config_table::set_timestamp(uint32_t  timestamp);
	void		load_config_table::set_major_version(uint16_t  major_version);
	void		load_config_table::set_minor_version(uint16_t  minor_version);
	void		load_config_table::set_global_flagsclear(uint32_t  global_flagsclear);
	void		load_config_table::set_global_flagsset(uint32_t  global_flagsset);
	void		load_config_table::set_criticalsection_default_timeout(uint32_t  criticalsection_default_timeout);
	void		load_config_table::set_decommit_freeblock_threshold(uint64_t  decommit_freeblock_threshold);
	void		load_config_table::set_decommit_totalfree_threshold(uint64_t  decommit_totalfree_threshold);
	void		load_config_table::set_lock_prefix_table(uint32_t  lock_prefix_table);	
	void		load_config_table::set_maximum_allocation_size(uint64_t  maximum_allocation_size);
	void		load_config_table::set_virtual_memory_threshold(uint64_t  virtual_memory_threshold);
	void		load_config_table::set_process_heap_flags(uint32_t  process_heap_flags);
	void		load_config_table::set_process_affinity_mask(uint64_t  process_affinity_mask);
	void		load_config_table::set_csd_version(uint16_t  csd_version);
    void        load_config_table::set_dependent_load_flags(uint16_t dependent_load_flags);
	void		load_config_table::set_editlist(uint32_t  editlist);						
	void		load_config_table::set_security_cookie(uint32_t  security_cookie);			
	void		load_config_table::set_se_handler_table(uint32_t  se_handler_table);
    void		load_config_table::set_se_handler_count(uint32_t  se_handler_count);
	void		load_config_table::set_guard_cf_check_function_pointer(uint32_t  guard_cf_check_function_pointer);
    void        load_config_table::set_guard_cf_dispatch_function_pointer(uint32_t  guard_cf_dispatch_function_pointer);
	void		load_config_table::set_guard_cf_function_table(uint32_t  guard_cf_function_table);	
    void		load_config_table::set_guard_cf_function_count(uint32_t  guard_cf_function_count);
	void		load_config_table::set_guard_flags(uint32_t  guard_flags);
    void        load_config_table::set_code_integrity(image_load_config_code_integrity& code_integrity);
    void        load_config_table::set_guard_address_taken_iat_entry_table(uint32_t guard_address_taken_iat_entry_table);
    void        load_config_table::set_guard_address_taken_iat_entry_count(uint32_t guard_address_taken_iat_entry_count);
    void        load_config_table::set_guard_long_jump_target_table(uint32_t guard_long_jump_target_table);
    void        load_config_table::set_guard_long_jump_target_count(uint32_t guard_long_jump_target_count);
    void        load_config_table::set_dynamic_value_reloc_table(uint32_t dynamic_value_reloc_table);
    void        load_config_table::set_chpe_meta_data_pointer(uint32_t chpe_meta_data_pointer);
    void        load_config_table::set_guard_rf_failure_routine(uint32_t guard_rf_failure_routine);
    void        load_config_table::set_guard_rf_failure_routine_function_pointer(uint32_t guard_rf_failure_routine_function_pointer);
    void        load_config_table::set_dynamic_value_reloc_table_offset(uint32_t dynamic_value_reloc_table_offset);
    void        load_config_table::set_dynamic_value_reloc_table_section(uint16_t dynamic_value_reloc_table_section);
    void        load_config_table::set_guard_rf_verify_stack_pointer_function_pointer(uint32_t guard_rf_verify_stack_pointer_function_pointer);
    void        load_config_table::set_hot_patch_table_offset(uint32_t hot_patch_table_offset);
    void        load_config_table::set_enclave_configuration_pointer(uint32_t enclave_configuration_pointer);

public:
    uint32_t		load_config_table::get_size() const;
	uint32_t 		load_config_table::get_timestamp() const;
	uint16_t 		load_config_table::get_major_version() const;
	uint16_t 		load_config_table::get_minor_version() const;
	uint32_t 		load_config_table::get_global_flagsclear() const;
	uint32_t 		load_config_table::get_global_flagsset() const;
	uint32_t 		load_config_table::get_criticalsection_default_timeout() const;
    uint64_t 	    load_config_table::get_decommit_freeblock_threshold() const;
    uint64_t 	    load_config_table::get_decommit_totalfree_threshold() const;
    uint32_t 	    load_config_table::get_lock_prefix_table() const;		
    uint64_t    	load_config_table::get_maximum_allocation_size() const;
    uint64_t 	    load_config_table::get_virtual_memory_threshold() const;
	uint32_t 		load_config_table::get_process_heap_flags() const;
    uint64_t 	    load_config_table::get_process_affinity_mask() const;
	uint16_t 		load_config_table::get_csd_version() const;
    uint16_t        load_config_table::get_dependent_load_flags() const;
    uint32_t 	    load_config_table::get_editlist() const;				
    uint32_t 	    load_config_table::get_security_cookie() const;	
    uint32_t 	    load_config_table::get_se_handler_table() const;
    uint32_t 	    load_config_table::get_se_handler_count() const;
    uint32_t 	    load_config_table::get_guard_cf_check_function_pointer() const; 
    uint32_t        load_config_table::get_guard_cf_dispatch_function_pointer() const;
    uint32_t 	    load_config_table::get_guard_cf_function_table() const;
    uint32_t 	    load_config_table::get_guard_cf_function_count() const;
	uint32_t 		load_config_table::get_guard_flags() const;
    image_load_config_code_integrity load_config_table::get_code_integrity() const;
    uint32_t        load_config_table::get_guard_address_taken_iat_entry_table() const;
    uint32_t        load_config_table::get_guard_address_taken_iat_entry_count() const;
    uint32_t        load_config_table::get_guard_long_jump_target_table() const;
    uint32_t        load_config_table::get_guard_long_jump_target_count() const;
    uint32_t        load_config_table::get_dynamic_value_reloc_table() const;
    uint32_t        load_config_table::get_chpe_meta_data_pointer() const;
    uint32_t        load_config_table::get_guard_rf_failure_routine() const;
    uint32_t        load_config_table::get_guard_rf_failure_routine_function_pointer() const;
    uint32_t        load_config_table::get_dynamic_value_reloc_table_offset() const;
    uint16_t        load_config_table::get_dynamic_value_reloc_table_section() const;
    uint32_t        load_config_table::get_guard_rf_verify_stack_pointer_function_pointer() const;
    uint32_t        load_config_table::get_hot_patch_table_offset() const;
    uint32_t        load_config_table::get_enclave_configuration_pointer() const;

    const std::vector<uint32_t >& load_config_table::get_se_handlers() const;   //x86 only
    const std::vector<uint32_t >& load_config_table::get_lock_prefixes() const; //x86 only
    const std::vector<uint32_t >& load_config_table::get_guard_cf_functions() const;
    const std::vector<uint32_t >& load_config_table::get_guard_iat_entries() const;
    const std::vector<uint32_t >& load_config_table::get_guard_long_jump_targets() const;

	std::vector<uint32_t >& load_config_table::get_se_handlers();   //x86 only
	std::vector<uint32_t >& load_config_table::get_lock_prefixes(); //x86 only
    std::vector<uint32_t >& load_config_table::get_guard_cf_functions();
    std::vector<uint32_t >& load_config_table::get_guard_iat_entries();
    std::vector<uint32_t >& load_config_table::get_guard_long_jump_targets();
};


enum load_config_table_build_id {
    load_config_table_build_se_handlers             = 1 << 1,
    load_config_table_build_lock_prefixes           = 1 << 2,
    load_config_table_build_guard_cf_functions      = 1 << 3,
    load_config_table_build_guard_iat_entrys        = 1 << 4,
    load_config_table_build_guard_long_jump_targets = 1 << 5,
};

directory_code get_load_config_table(_In_ const pe_image &image, _Out_ load_config_table& load_config);

bool build_internal_load_config_data(_Inout_ pe_image &image, _Inout_ pe_section& section,
    _Inout_ load_config_table& load_config, _In_ uint32_t build_items_ids/*import_table_build_id*/,
    _Inout_ relocation_table& relocs);

bool build_load_config_table_only(_Inout_ pe_image &image, _Inout_ pe_section& section,
    _Inout_ load_config_table& load_config, _Inout_ relocation_table& relocs);
bool build_load_config_table_full(_Inout_ pe_image &image, _Inout_ pe_section& section,
    _Inout_ load_config_table& load_config, _Inout_ relocation_table& relocs);


directory_code get_placement_load_config_table(_Inout_ pe_image &image, _Inout_ std::vector<directory_placement>& placement);