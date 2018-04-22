#include "stdafx.h"
#include "pe_loadconfig.h"


load_config_table::load_config_table() {
    size                            = 0;
	timestamp						= 0;
	major_version					= 0;
	minor_version					= 0;
	global_flagsclear				= 0;
	global_flagsset					= 0;
	criticalsection_default_timeout = 0;
	decommit_freeblock_threshold	= 0;
	decommit_totalfree_threshold	= 0;
	lock_prefix_table				= 0; // VA
	maximum_allocation_size			= 0;
	virtual_memory_threshold		= 0;
	process_heap_flags				= 0;
	process_affinity_mask			= 0;
	csd_version						= 0;
	editlist						= 0; // VA
	security_cookie					= 0; // VA
	se_handler_table				= 0; // VA
	guard_cf_check_function_pointer = 0; // VA
	guard_cf_function_table			= 0; // VA
	guard_flags						= 0;
    code_integrity                  = {0};
    guard_address_taken_iat_entry_table = 0; // VA
    guard_address_taken_iat_entry_count = 0;
    guard_long_jump_target_table        = 0; // VA
    guard_long_jump_target_count        = 0;
    dynamic_value_reloc_table           = 0; // VA
    chpe_meta_data_pointer              = 0;
    guard_rf_failure_routine            = 0; // VA
    guard_rf_failure_routine_function_pointer = 0; // VA
    dynamic_value_reloc_table_offset    = 0;
    dynamic_value_reloc_table_section   = 0;
    reserved2                           = 0;
    guard_rf_verify_stack_pointer_function_pointer = 0; // VA
    hot_patch_table_offset              = 0;
    reserved3                           = 0;
    enclave_configuration_pointer       = 0; // VA
}
load_config_table::~load_config_table() {

}
void		load_config_table::set_size(uint32_t  size) {
    this->size = size;
}
void		load_config_table::set_timestamp(uint32_t timestamp) {
	this->timestamp = timestamp;
}
void		load_config_table::set_major_version(uint16_t major_version) {
	this->major_version = major_version;
}
void		load_config_table::set_minor_version(uint16_t minor_version) {
	this->minor_version = minor_version;
}
void		load_config_table::set_global_flagsclear(uint32_t global_flagsclear) {
	this->global_flagsclear = global_flagsclear;
}
void		load_config_table::set_global_flagsset(uint32_t global_flagsset) {
	this->global_flagsset = global_flagsset;
}
void		load_config_table::set_criticalsection_default_timeout(uint32_t criticalsection_default_timeout) {
	this->criticalsection_default_timeout = criticalsection_default_timeout;
}
void		load_config_table::set_decommit_freeblock_threshold(uint64_t  decommit_freeblock_threshold) {
	this->decommit_freeblock_threshold = decommit_freeblock_threshold;
}
void		load_config_table::set_decommit_totalfree_threshold(uint64_t  decommit_totalfree_threshold) {
	this->decommit_totalfree_threshold = decommit_totalfree_threshold;
}
void		load_config_table::set_lock_prefix_table(uint32_t lock_prefix_table) {
	this->lock_prefix_table = lock_prefix_table;
}
void		load_config_table::set_maximum_allocation_size(uint64_t  maximum_allocation_size) {
	this->maximum_allocation_size = maximum_allocation_size;
}
void		load_config_table::set_virtual_memory_threshold(uint64_t  virtual_memory_threshold) {
	this->virtual_memory_threshold = virtual_memory_threshold;
}
void		load_config_table::set_process_heap_flags(uint32_t process_heap_flags) {
	this->process_heap_flags = process_heap_flags;
}
void		load_config_table::set_process_affinity_mask(uint64_t  process_affinity_mask) {
	this->process_affinity_mask = process_affinity_mask;
}
void		load_config_table::set_csd_version(uint16_t csd_version) {
	this->csd_version = csd_version;
}
void		load_config_table::set_editlist(uint32_t editlist) {
	this->editlist = editlist;
}
void		load_config_table::set_security_cookie(uint32_t security_cookie) {
	this->security_cookie = security_cookie;
}
void		load_config_table::set_se_handler_table(uint32_t se_handler_table) {
	this->se_handler_table = se_handler_table;
}
void		load_config_table::set_guard_cf_check_function_pointer(uint32_t guard_cf_check_function_pointer) {
	this->guard_cf_check_function_pointer = guard_cf_check_function_pointer;
}
void		load_config_table::set_guard_cf_function_table(uint32_t guard_cf_function_table) {
	this->guard_cf_function_table = guard_cf_function_table;
}
void		load_config_table::set_guard_flags(uint32_t guard_flags) {
	this->guard_flags = guard_flags;
}
void        load_config_table::set_code_integrity(image_load_config_code_integrity& code_integrity) {
    this->code_integrity = code_integrity;
}
void        load_config_table::set_guard_address_taken_iat_entry_table(uint64_t guard_address_taken_iat_entry_table) {
    this->guard_address_taken_iat_entry_table = guard_address_taken_iat_entry_table;
}
void        load_config_table::set_guard_address_taken_iat_entry_count(uint64_t guard_address_taken_iat_entry_count) {
    this->guard_address_taken_iat_entry_count = guard_address_taken_iat_entry_count;
}
void        load_config_table::set_guard_long_jump_target_table(uint64_t guard_long_jump_target_table) {
    this->guard_long_jump_target_table = guard_long_jump_target_table;
}
void        load_config_table::set_guard_long_jump_target_count(uint64_t guard_long_jump_target_count) {
    this->guard_long_jump_target_count = guard_long_jump_target_count;
}
void        load_config_table::set_dynamic_value_reloc_table(uint64_t dynamic_value_reloc_table) {
    this->dynamic_value_reloc_table = dynamic_value_reloc_table;
}
void        load_config_table::set_chpe_meta_data_pointer(uint64_t chpe_meta_data_pointer) {
    this->chpe_meta_data_pointer = chpe_meta_data_pointer;
}
void        load_config_table::set_guard_rf_failure_routine(uint64_t guard_rf_failure_routine) {
    this->guard_rf_failure_routine = guard_rf_failure_routine;
}
void        load_config_table::set_guard_rf_failure_routine_function_pointer(uint64_t guard_rf_failure_routine_function_pointer) {
    this->guard_rf_failure_routine_function_pointer = guard_rf_failure_routine_function_pointer;
}
void        load_config_table::set_dynamic_value_reloc_table_offset(uint32_t dynamic_value_reloc_table_offset) {
    this->dynamic_value_reloc_table_offset = dynamic_value_reloc_table_offset;
}
void        load_config_table::set_dynamic_value_reloc_table_section(uint16_t dynamic_value_reloc_table_section) {
    this->dynamic_value_reloc_table_section = dynamic_value_reloc_table_section;
}
void        load_config_table::set_guard_rf_verify_stack_pointer_function_pointer(uint64_t guard_rf_verify_stack_pointer_function_pointer) {
    this->guard_rf_verify_stack_pointer_function_pointer = guard_rf_verify_stack_pointer_function_pointer;
}
void        load_config_table::set_hot_patch_table_offset(uint32_t hot_patch_table_offset) {
    this->hot_patch_table_offset = hot_patch_table_offset;
}
void        load_config_table::set_enclave_configuration_pointer(uint64_t enclave_configuration_pointer) {
    this->enclave_configuration_pointer = enclave_configuration_pointer;
}

uint32_t		load_config_table::get_size() const {
    return this->size;
}
uint32_t		load_config_table::get_timestamp() const {
	return this->timestamp;
}
uint16_t		load_config_table::get_major_version() const {
	return this->major_version;
}
uint16_t		load_config_table::get_minor_version() const {
	return this->minor_version;
}
uint32_t		load_config_table::get_global_flagsclear() const {
	return this->global_flagsclear;
}
uint32_t		load_config_table::get_global_flagsset() const {
	return this->global_flagsset;
}
uint32_t		load_config_table::get_criticalsection_default_timeout() const {
	return this->criticalsection_default_timeout;
}
uint64_t 	load_config_table::get_decommit_freeblock_threshold() const {
	return this->decommit_freeblock_threshold;
}
uint64_t 	load_config_table::get_decommit_totalfree_threshold() const {
	return this->decommit_totalfree_threshold;
}
uint32_t	load_config_table::get_lock_prefix_table() const {
	return this->lock_prefix_table;
}
uint64_t 	load_config_table::get_maximum_allocation_size() const {
	return this->maximum_allocation_size;
}
uint64_t 	load_config_table::get_virtual_memory_threshold() const {
	return this->virtual_memory_threshold;
}
uint32_t		load_config_table::get_process_heap_flags() const {
	return this->process_heap_flags;
}
uint64_t 	load_config_table::get_process_affinity_mask() const {
	return this->process_affinity_mask;
}
uint16_t		load_config_table::get_csd_version() const {
	return this->csd_version;
}
uint32_t	load_config_table::get_editlist() const {
	return this->editlist;
}
uint32_t	load_config_table::get_security_cookie() const {
	return this->security_cookie;
}
uint32_t	load_config_table::get_se_handler_table() const {
	return this->se_handler_table;
}
uint32_t	load_config_table::get_se_handler_count() const {
	return this->se_handlers.size();
}
uint32_t	load_config_table::get_guard_cf_check_function_pointer() const {
	return this->guard_cf_check_function_pointer;
}
uint32_t	load_config_table::get_guard_cf_function_table() const {
	return this->guard_cf_function_table;
}
uint32_t	    load_config_table::get_guard_cf_function_count() const {
    return this->guard_cf_functions_rva.size();
}
uint32_t		load_config_table::get_guard_flags() const {
	return this->guard_flags;
}
std::vector<uint32_t>& load_config_table::get_se_handlers() {
	return this->se_handlers;
}
std::vector<uint32_t>& load_config_table::get_lock_prefixes() {
	return this->lock_prefixes_rva;
}
std::vector<uint32_t>& load_config_table::get_guard_cf_functions() {
    return this->guard_cf_functions_rva;
}
image_load_config_code_integrity load_config_table::get_code_integrity() const {
    return this->code_integrity;
}
uint64_t        load_config_table::get_guard_address_taken_iat_entry_table() const {
    return this->guard_address_taken_iat_entry_table;
}
uint64_t        load_config_table::get_guard_address_taken_iat_entry_count() const {
    return this->guard_address_taken_iat_entry_count;
}
uint64_t        load_config_table::get_guard_long_jump_target_table() const {
    return this->guard_long_jump_target_table;
}
uint64_t        load_config_table::get_guard_long_jump_target_count() const {
    return this->guard_long_jump_target_count;
}
uint64_t        load_config_table::get_dynamic_value_reloc_table() const {
    return this->dynamic_value_reloc_table;
}
uint64_t        load_config_table::get_chpe_meta_data_pointer() const {
    return this->chpe_meta_data_pointer;
}
uint64_t        load_config_table::get_guard_rf_failure_routine() const {
    return this->guard_rf_failure_routine;
}
uint64_t        load_config_table::get_guard_rf_failure_routine_function_pointer() const {
    return this->guard_rf_failure_routine_function_pointer;
}
uint32_t        load_config_table::get_dynamic_value_reloc_table_offset() const {
    return this->dynamic_value_reloc_table_offset;
}
uint16_t        load_config_table::get_dynamic_value_reloc_table_section() const {
    return this->dynamic_value_reloc_table_section;
}
uint64_t        load_config_table::get_guard_rf_verify_stack_pointer_function_pointer() const {
    return this->guard_rf_verify_stack_pointer_function_pointer;
}
uint32_t        load_config_table::get_hot_patch_table_offset() const {
    return this->hot_patch_table_offset;
}
uint64_t        load_config_table::get_enclave_configuration_pointer() const {
    return this->enclave_configuration_pointer;
}

template <typename image_format>
directory_code _get_load_config_table(const pe_image &image, load_config_table& load_config) {
    load_config.set_size(0);
    load_config.set_timestamp(0);
    load_config.set_major_version(0);
    load_config.set_minor_version(0);
    load_config.set_global_flagsclear(0);
    load_config.set_global_flagsset(0);
    load_config.set_criticalsection_default_timeout(0);
    load_config.set_decommit_freeblock_threshold(0);
    load_config.set_decommit_totalfree_threshold(0);
    load_config.set_lock_prefix_table(0);
    load_config.set_maximum_allocation_size(0);
    load_config.set_virtual_memory_threshold(0);
    load_config.set_process_heap_flags(0);
    load_config.set_process_affinity_mask(0);
    load_config.set_csd_version(0);
    load_config.set_editlist(0);
    load_config.set_security_cookie(0);
    load_config.set_se_handler_table(0);
    load_config.set_guard_cf_check_function_pointer(0);
    load_config.set_guard_cf_function_table(0);
    load_config.set_guard_flags(0);
    image_load_config_code_integrity _code_integrity = { 0 };
    load_config.set_code_integrity(_code_integrity);
    load_config.set_guard_address_taken_iat_entry_table(0);
    load_config.set_guard_address_taken_iat_entry_count(0);
    load_config.set_guard_long_jump_target_table(0);
    load_config.set_guard_long_jump_target_count(0);
    load_config.set_dynamic_value_reloc_table(0);
    load_config.set_chpe_meta_data_pointer(0);
    load_config.set_guard_rf_failure_routine(0);
    load_config.set_guard_rf_failure_routine_function_pointer(0);
    load_config.set_dynamic_value_reloc_table_offset(0);
    load_config.set_dynamic_value_reloc_table_section(0);
    load_config.set_guard_rf_verify_stack_pointer_function_pointer(0);
    load_config.set_hot_patch_table_offset(0);
    load_config.set_enclave_configuration_pointer(0);
    load_config.get_se_handlers().clear();
    load_config.get_lock_prefixes().clear();
    load_config.get_guard_cf_functions().clear();
    
    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG);
    uint32_t virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG);

    if (virtual_address && virtual_size) {
        typename image_format::image_load_config_directory load_config_desc;
        uint32_t desc_size;

        if (pe_image_io(image).set_image_offset(virtual_address).read(&desc_size,sizeof(desc_size)) != enma_io_success) {
            return directory_code::directory_code_currupted;
        }

        if (!desc_size) { return directory_code::directory_code_success; }

        if (pe_image_io(image).set_image_offset(virtual_address).read(&load_config_desc, desc_size) != enma_io_success) {
            return directory_code::directory_code_currupted;
        }

        load_config.set_size(desc_size);
        load_config.set_timestamp(load_config_desc.time_date_stamp);
        load_config.set_major_version(load_config_desc.major_version);
        load_config.set_minor_version(load_config_desc.major_version);
        load_config.set_global_flagsclear(load_config_desc.global_flags_clear);
        load_config.set_global_flagsset(load_config_desc.global_flags_set);
        load_config.set_criticalsection_default_timeout(load_config_desc.critical_section_default_timeout);
        load_config.set_decommit_freeblock_threshold(load_config_desc.decommit_free_block_threshold);
        load_config.set_decommit_totalfree_threshold(load_config_desc.decommit_total_free_threshold);

        if (load_config_desc.lock_prefix_table) {
            load_config.set_lock_prefix_table(image.va_to_rva(load_config_desc.lock_prefix_table));

            pe_image_io loadcfg_lock_prefix_io((pe_image &)image);
            loadcfg_lock_prefix_io.set_image_offset(image.va_to_rva(load_config_desc.lock_prefix_table));

            typename image_format::ptr_size lock_prefix_va = 0;

            do {

                if (loadcfg_lock_prefix_io.read(&lock_prefix_va, sizeof(lock_prefix_va)) != enma_io_success) {
                    return directory_code::directory_code_currupted;
                }

                load_config.get_lock_prefixes().push_back(image.va_to_rva(lock_prefix_va));

            } while (lock_prefix_va);
        }

        load_config.set_maximum_allocation_size(load_config_desc.maximum_allocation_size);
        load_config.set_virtual_memory_threshold(load_config_desc.virtual_memory_threshold);
        load_config.set_process_heap_flags(load_config_desc.process_heap_flags);
        load_config.set_process_affinity_mask(load_config_desc.process_affinity_mask);
        load_config.set_csd_version(load_config_desc.csd_version);


        if (load_config_desc.edit_list) {
            load_config.set_editlist(image.va_to_rva(load_config_desc.edit_list));
        }
        if (load_config_desc.security_cookie) {
            load_config.set_security_cookie(image.va_to_rva(load_config_desc.security_cookie));
        }

        if (offsetof(typename image_format::image_load_config_directory, se_handler_table) >= desc_size) {
            return directory_code::directory_code_success;
        }

        if (load_config_desc.se_handler_table) {
            load_config.set_se_handler_table(image.va_to_rva(load_config_desc.se_handler_table));

            pe_image_io loadcfg_se_handlers_io((pe_image &)image);
            loadcfg_se_handlers_io.set_image_offset(image.va_to_rva(load_config_desc.se_handler_table));

            for (uint32_t i = 0; i < load_config_desc.se_handler_count; i++) {
                uint32_t se_handler_rva;

                if (loadcfg_se_handlers_io.read(&se_handler_rva, sizeof(se_handler_rva)) != enma_io_success) {
                    return directory_code::directory_code_currupted;
                }

                load_config.get_se_handlers().push_back(se_handler_rva);
            }
        }
        
        if (offsetof(typename image_format::image_load_config_directory, guard_cf_check_function_pointer) >= desc_size) {
            return directory_code::directory_code_success;
        }

        if (load_config_desc.guard_cf_check_function_pointer) {
            load_config.set_guard_cf_check_function_pointer(image.va_to_rva(load_config_desc.guard_cf_check_function_pointer));
        }
        if (load_config_desc.guard_cf_function_table) {
            load_config.set_guard_cf_function_table(image.va_to_rva(load_config_desc.guard_cf_function_table));

            pe_image_io loadcfg_guard_cf_function_io((pe_image &)image);
            loadcfg_guard_cf_function_io.set_image_offset(load_config_desc.guard_cf_function_table);
            for (uint32_t i = 0; i < load_config_desc.guard_cf_function_count; i++) {
                typename image_format::ptr_size cf_function_va;

                if (loadcfg_guard_cf_function_io.read(&cf_function_va, sizeof(cf_function_va)) != enma_io_success) {
                    return directory_code::directory_code_currupted;
                }

                load_config.get_guard_cf_functions().push_back(image.va_to_rva(cf_function_va));
            }
        }

        load_config.set_guard_flags(load_config_desc.guard_flags);
        
        if (offsetof(typename image_format::image_load_config_directory, code_integrity) >= desc_size) {
            return directory_code::directory_code_success;
        }

        load_config.set_code_integrity(load_config_desc.code_integrity);

        if (offsetof(typename image_format::image_load_config_directory, guard_address_taken_iat_entry_table) >= desc_size) {
            return directory_code::directory_code_success;
        }

        load_config.set_guard_address_taken_iat_entry_table(load_config_desc.guard_address_taken_iat_entry_table);
        load_config.set_guard_address_taken_iat_entry_count(load_config_desc.guard_address_taken_iat_entry_count);
        load_config.set_guard_long_jump_target_table(load_config_desc.guard_long_jump_target_table);
        load_config.set_guard_long_jump_target_count(load_config_desc.guard_long_jump_target_count);

        if (offsetof(typename image_format::image_load_config_directory, dynamic_value_reloc_table) >= desc_size) {
            return directory_code::directory_code_success;
        }

        load_config.set_dynamic_value_reloc_table(load_config_desc.dynamic_value_reloc_table);
        load_config.set_chpe_meta_data_pointer(load_config_desc.chpe_meta_data_pointer);

        if (offsetof(typename image_format::image_load_config_directory, guard_rf_failure_routine) >= desc_size) {
            return directory_code::directory_code_success;
        }

        load_config.set_guard_rf_failure_routine(load_config_desc.guard_rf_failure_routine);
        load_config.set_guard_rf_failure_routine_function_pointer(load_config_desc.guard_rf_failure_routine_function_pointer);
        load_config.set_dynamic_value_reloc_table_offset(load_config_desc.dynamic_value_reloc_table_offset);
        load_config.set_dynamic_value_reloc_table_section(load_config_desc.dynamic_value_reloc_table_section);

        if (offsetof(typename image_format::image_load_config_directory, guard_rf_verify_stack_pointer_function_pointer) >= desc_size) {
            return directory_code::directory_code_success;
        }

        load_config.set_guard_rf_verify_stack_pointer_function_pointer(load_config_desc.guard_rf_verify_stack_pointer_function_pointer);
        load_config.set_hot_patch_table_offset(load_config_desc.hot_patch_table_offset);

        if (offsetof(typename image_format::image_load_config_directory, enclave_configuration_pointer) >= desc_size) {
            return directory_code::directory_code_success;
        }

        load_config.set_enclave_configuration_pointer(load_config_desc.enclave_configuration_pointer);

        return directory_code::directory_code_success;
    }
    
    return directory_code::directory_code_not_present;
}


directory_code get_load_config_table(const pe_image &image, load_config_table& load_config) {
   
    if (image.is_x32_image()) {
        return _get_load_config_table<image_32>(image, load_config);
    }
    else {
        return _get_load_config_table<image_64>(image, load_config);
    }
}

bool build_load_config_table(pe_image &image, pe_section& section, load_config_table& load_config, relocation_table& relocs) {

    if (section.get_size_of_raw_data() & 0xF) {
        section.get_section_data().resize(
            section.get_section_data().size() + (0x10 - (section.get_section_data().size()&0xF))
        );
    }

    if (image.is_x32_image()) {
        uint32_t load_config_offset =  section.get_size_of_raw_data();

        section.get_section_data().resize(section.get_size_of_raw_data() + 
        sizeof(image_load_config_directory32) +
            (load_config.get_se_handlers().size() ? load_config.get_se_handlers().size() * sizeof(uint32_t) : 0) +
            (load_config.get_lock_prefixes().size() ? (load_config.get_lock_prefixes().size()+1) * sizeof(uint32_t) : 0) +
            (load_config.get_guard_cf_functions().size() ? load_config.get_guard_cf_functions().size() * sizeof(uint32_t) : 0)
        );

        image_load_config_directory32* image_load_config = (image_load_config_directory32*)&section.get_section_data().data()[load_config_offset];
        
        image_load_config->size = sizeof(image_load_config_directory32);
        image_load_config->time_date_stamp                 = load_config.get_timestamp();
        image_load_config->major_version                  = load_config.get_major_version();
        image_load_config->minor_version                  = load_config.get_minor_version();
        image_load_config->global_flags_clear              = load_config.get_global_flagsclear();
        image_load_config->global_flags_set                = load_config.get_global_flagsset();
        image_load_config->critical_section_default_timeout = load_config.get_criticalsection_default_timeout();
        image_load_config->decommit_free_block_threshold    = (uint32_t)load_config.get_decommit_freeblock_threshold();
        image_load_config->decommit_total_free_threshold    = (uint32_t)load_config.get_decommit_totalfree_threshold();
        image_load_config->maximum_allocation_size         = (uint32_t)load_config.get_maximum_allocation_size();
        image_load_config->virtual_memory_threshold        = (uint32_t)load_config.get_virtual_memory_threshold();
        image_load_config->process_heap_flags              = load_config.get_process_heap_flags();
        image_load_config->process_affinity_mask           = (uint32_t)load_config.get_process_affinity_mask();
        image_load_config->csd_version                    = load_config.get_csd_version();
       // image_load_config->Reserved1 = 0;

        if (load_config.get_editlist()) {
            image_load_config->edit_list = (uint32_t)image.rva_to_va(load_config.get_editlist());
            relocs.add_item(section.get_virtual_address() + load_config_offset + offsetof(image_load_config_directory32, edit_list), 0);
        }
        if (load_config.get_security_cookie()) {
            image_load_config->security_cookie = (uint32_t)image.rva_to_va(load_config.get_security_cookie());
            relocs.add_item(section.get_virtual_address() + load_config_offset + offsetof(image_load_config_directory32, security_cookie), 0);
        }
        if (load_config.get_guard_cf_check_function_pointer()) {
            image_load_config->guard_cf_check_function_pointer = (uint32_t)image.rva_to_va(load_config.get_guard_cf_check_function_pointer());
            relocs.add_item(section.get_virtual_address() + load_config_offset + offsetof(image_load_config_directory32, guard_cf_check_function_pointer), 0);
        }

        image_load_config->reserved2 = 0;
        image_load_config->guard_flags = load_config.get_guard_flags();

        

        if (load_config.get_se_handlers().size()) { //only x86
            image_load_config->se_handler_table = section.get_virtual_address() + load_config_offset + sizeof(image_load_config_directory32);
            image_load_config->se_handler_count = load_config.get_se_handlers().size();
            relocs.add_item(section.get_virtual_address() + load_config_offset + offsetof(image_load_config_directory32, se_handler_table), 0);

            for (size_t item_idx = 0; item_idx < load_config.get_se_handlers().size(); item_idx++) {
                *(uint32_t*)&section.get_section_data().data()[
                    load_config_offset + sizeof(image_load_config_directory32) + item_idx * sizeof(uint32_t)
                ] = load_config.get_se_handlers()[item_idx];
            }
        }

        if (load_config.get_lock_prefixes().size()) {//only x86
            image_load_config->lock_prefix_table = section.get_virtual_address() + load_config_offset + sizeof(image_load_config_directory32) +
                (load_config.get_se_handlers().size() ? load_config.get_se_handlers().size() * sizeof(uint32_t) : 0);
            relocs.add_item(section.get_virtual_address() + load_config_offset + offsetof(image_load_config_directory32, lock_prefix_table), 0);

            for (size_t item_idx = 0; item_idx < load_config.get_lock_prefixes().size(); item_idx++) {
                *(uint32_t*)&section.get_section_data().data()[
                    load_config_offset + sizeof(image_load_config_directory32) + item_idx * sizeof(uint32_t)
                ] = (uint32_t)image.rva_to_va(load_config.get_lock_prefixes()[item_idx]);

                relocs.add_item(section.get_virtual_address() + load_config_offset + sizeof(image_load_config_directory32) +
                    (load_config.get_se_handlers().size() ? load_config.get_se_handlers().size() * sizeof(uint32_t) : 0) +
                    item_idx*sizeof(uint32_t)
                    , 0);
            }

            *(uint32_t*)&section.get_section_data().data()[
                load_config_offset + sizeof(image_load_config_directory32) + load_config.get_lock_prefixes().size() * sizeof(uint32_t)
            ] = 0;
        }

        if (load_config.get_guard_cf_functions().size()) {
            image_load_config->guard_cf_function_table = section.get_virtual_address() + load_config_offset + sizeof(image_load_config_directory32) +
                (load_config.get_se_handlers().size() ? load_config.get_se_handlers().size() * sizeof(uint32_t) : 0) +
                (load_config.get_lock_prefixes().size() ? (load_config.get_lock_prefixes().size() + 1) * sizeof(uint32_t) : 0);
            image_load_config->guard_cf_function_count = load_config.get_guard_cf_functions().size();
            relocs.add_item(section.get_virtual_address() + load_config_offset + offsetof(image_load_config_directory32, guard_cf_function_table), 0);


            for (size_t item_idx = 0; item_idx < load_config.get_guard_cf_functions().size(); item_idx++) {
                *(uint32_t*)&section.get_section_data().data()[
                    load_config_offset + sizeof(image_load_config_directory32) + item_idx * sizeof(uint32_t)
                ] = (uint32_t)image.rva_to_va(load_config.get_guard_cf_functions()[item_idx]);

                relocs.add_item(section.get_virtual_address() + load_config_offset + sizeof(image_load_config_directory32) +
                    (load_config.get_se_handlers().size() ? load_config.get_se_handlers().size() * sizeof(uint32_t) : 0) +
                    (load_config.get_lock_prefixes().size() ? (load_config.get_lock_prefixes().size() + 1) * sizeof(uint32_t) : 0) +
                     item_idx * sizeof(uint32_t)
                     , 0);
            }
        }
        

        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG, section.get_virtual_address() + load_config_offset);
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG, sizeof(image_load_config_directory32));
    }
    else {
        uint32_t load_config_offset = section.get_size_of_raw_data();
        size_t current_offset = 0;

        section.get_section_data().resize(section.get_size_of_raw_data() +
            sizeof(image_load_config_directory64) +
            (load_config.get_guard_cf_functions().size() ? load_config.get_guard_cf_functions().size() * sizeof(uint64_t) : 0)
        );

        image_load_config_directory64* image_load_config = (image_load_config_directory64*)&section.get_section_data().data()[load_config_offset];

        image_load_config->size = sizeof(image_load_config_directory64);
        image_load_config->time_date_stamp = load_config.get_timestamp();
        image_load_config->major_version = load_config.get_major_version();
        image_load_config->minor_version = load_config.get_minor_version();
        image_load_config->global_flags_clear = load_config.get_global_flagsclear();
        image_load_config->global_flags_set = load_config.get_global_flagsset();
        image_load_config->critical_section_default_timeout = load_config.get_criticalsection_default_timeout();
        image_load_config->decommit_free_block_threshold = load_config.get_decommit_freeblock_threshold();
        image_load_config->decommit_total_free_threshold = load_config.get_decommit_totalfree_threshold();
        image_load_config->maximum_allocation_size = load_config.get_maximum_allocation_size();
        image_load_config->virtual_memory_threshold = load_config.get_virtual_memory_threshold();
        image_load_config->process_heap_flags = load_config.get_process_heap_flags();
        image_load_config->process_affinity_mask = load_config.get_process_affinity_mask();
        image_load_config->csd_version = load_config.get_csd_version();
       // image_load_config->Reserved1 = 0;

        if (load_config.get_editlist()) {
            image_load_config->edit_list = image.rva_to_va(load_config.get_editlist());
            relocs.add_item(section.get_virtual_address() + load_config_offset + offsetof(image_load_config_directory64, edit_list), 0);
        }
        if (load_config.get_security_cookie()) {
            image_load_config->security_cookie = image.rva_to_va(load_config.get_security_cookie());
            relocs.add_item(section.get_virtual_address() + load_config_offset + offsetof(image_load_config_directory64, security_cookie), 0);
        }
        if (load_config.get_guard_cf_check_function_pointer()) {
            image_load_config->guard_cf_check_function_pointer = image.rva_to_va(load_config.get_guard_cf_check_function_pointer());
            relocs.add_item(section.get_virtual_address() + load_config_offset + offsetof(image_load_config_directory64, guard_cf_check_function_pointer), 0);
        }

        image_load_config->reserved2 = 0;
        image_load_config->guard_flags = load_config.get_guard_flags();

        current_offset += sizeof(image_load_config_directory64);

        if (load_config.get_guard_cf_functions().size()) {
            image_load_config->guard_cf_function_table = section.get_virtual_address() + load_config_offset + sizeof(image_load_config_directory64);
            image_load_config->guard_cf_function_count = load_config.get_guard_cf_functions().size();
            relocs.add_item(section.get_virtual_address() + load_config_offset + offsetof(image_load_config_directory64, guard_cf_function_table), 0);


            for (size_t item_idx = 0; item_idx < load_config.get_guard_cf_functions().size(); item_idx++) {
                *(uint64_t*)&section.get_section_data().data()[
                    load_config_offset + sizeof(image_load_config_directory64) + item_idx * sizeof(uint64_t)
                ] = image.rva_to_va(load_config.get_guard_cf_functions()[item_idx]);

                 relocs.add_item(section.get_virtual_address() + load_config_offset + sizeof(image_load_config_directory64) +
                     item_idx * sizeof(uint64_t)
                        , 0);
            }
        }


        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG, section.get_virtual_address() + load_config_offset);
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG, sizeof(image_load_config_directory64));
    }

    return true;
}

directory_code get_placement_load_config_table(pe_image &image, std::vector<directory_placement>& placement) {

    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG);
    uint32_t virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG);

    if (virtual_address && virtual_size) {
        pe_section * load_cfg_section = image.get_section_by_rva(virtual_address);
        if (load_cfg_section) {
            if (image.is_x32_image()) {

                image_load_config_directory32* image_load_config = (image_load_config_directory32*)&load_cfg_section->get_section_data()[virtual_address - load_cfg_section->get_virtual_address()];

            
                if (offsetof(image_load_config_directory32, se_handler_table) < virtual_size &&
                    image_load_config->se_handler_count &&
                    image_load_config->se_handler_table) {

                    placement.push_back({ image.va_to_rva(image_load_config->se_handler_table) ,
                        image_load_config->se_handler_count * sizeof(uint32_t),
                        dp_id_loadconfig_se_table
                    });
                }

                if (offsetof(image_load_config_directory32, lock_prefix_table) < virtual_size &&
                    image_load_config->lock_prefix_table) {

                    size_t lock_prefix_count = 0;

                    pe_section * lock_pref_section = image.get_section_by_va(image_load_config->lock_prefix_table);

                    if (lock_pref_section) {
                        uint32_t * lock_item = (uint32_t *)lock_pref_section->get_section_data().data()[
                            image.va_to_rva(image_load_config->lock_prefix_table) - lock_pref_section->get_virtual_address()
                        ];
                        
                        for (; *lock_item; lock_item++, lock_prefix_count++) {}

                        placement.push_back({ image.va_to_rva(image_load_config->se_handler_table) ,
                            (image_load_config->se_handler_count+1) * sizeof(uint32_t),
                            dp_id_loadconfig_se_table
                        });
                    }
                }

                if (offsetof(image_load_config_directory32, guard_cf_function_table) < virtual_size &&
                    image_load_config->guard_cf_function_count &&
                    image_load_config->guard_cf_function_table) {

                        placement.push_back({ image.va_to_rva(image_load_config->guard_cf_function_table) ,
                            image_load_config->guard_cf_function_count * sizeof(uint32_t),
                            dp_id_loadconfig_cf_table
                        });
                }


                placement.push_back({ virtual_address , sizeof(image_load_config_directory32) ,dp_id_loadconfig_desc });
                return directory_code::directory_code_success;
            }
            else {
                image_load_config_directory64* image_load_config = (image_load_config_directory64*)&load_cfg_section->get_section_data()[virtual_address - load_cfg_section->get_virtual_address()];

                if(offsetof(image_load_config_directory64, guard_cf_function_table) < virtual_size &&
                    image_load_config->guard_cf_function_count &&
                    image_load_config->guard_cf_function_table) {

                    placement.push_back({ image.va_to_rva(image_load_config->guard_cf_function_table) ,
                        size_t(image_load_config->guard_cf_function_count * sizeof(uint64_t)),
                        dp_id_loadconfig_cf_table
                    });
                }

                placement.push_back({ virtual_address , sizeof(image_load_config_directory64),dp_id_loadconfig_desc });
                return directory_code::directory_code_success;
            }
        }
    }

	return directory_code::directory_code_not_present;
}