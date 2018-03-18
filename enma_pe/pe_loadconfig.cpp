#include "stdafx.h"
#include "pe_loadconfig.h"


load_config_table::load_config_table() {
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
}
load_config_table::~load_config_table() {

}


void		load_config_table::set_timestamp(DWORD timestamp) {
	this->timestamp = timestamp;
}
void		load_config_table::set_major_version(WORD major_version) {
	this->major_version = major_version;
}
void		load_config_table::set_minor_version(WORD minor_version) {
	this->minor_version = minor_version;
}
void		load_config_table::set_global_flagsclear(DWORD global_flagsclear) {
	this->global_flagsclear = global_flagsclear;
}
void		load_config_table::set_global_flagsset(DWORD global_flagsset) {
	this->global_flagsset = global_flagsset;
}
void		load_config_table::set_criticalsection_default_timeout(DWORD criticalsection_default_timeout) {
	this->criticalsection_default_timeout = criticalsection_default_timeout;
}
void		load_config_table::set_decommit_freeblock_threshold(ULONGLONG decommit_freeblock_threshold) {
	this->decommit_freeblock_threshold = decommit_freeblock_threshold;
}
void		load_config_table::set_decommit_totalfree_threshold(ULONGLONG decommit_totalfree_threshold) {
	this->decommit_totalfree_threshold = decommit_totalfree_threshold;
}
void		load_config_table::set_lock_prefix_table(DWORD lock_prefix_table) {
	this->lock_prefix_table = lock_prefix_table;
}
void		load_config_table::set_maximum_allocation_size(ULONGLONG maximum_allocation_size) {
	this->maximum_allocation_size = maximum_allocation_size;
}
void		load_config_table::set_virtual_memory_threshold(ULONGLONG virtual_memory_threshold) {
	this->virtual_memory_threshold = virtual_memory_threshold;
}
void		load_config_table::set_process_heap_flags(DWORD process_heap_flags) {
	this->process_heap_flags = process_heap_flags;
}
void		load_config_table::set_process_affinity_mask(ULONGLONG process_affinity_mask) {
	this->process_affinity_mask = process_affinity_mask;
}
void		load_config_table::set_csd_version(WORD csd_version) {
	this->csd_version = csd_version;
}
void		load_config_table::set_editlist(DWORD editlist) {
	this->editlist = editlist;
}
void		load_config_table::set_security_cookie(DWORD security_cookie) {
	this->security_cookie = security_cookie;
}
void		load_config_table::set_se_handler_table(DWORD se_handler_table) {
	this->se_handler_table = se_handler_table;
}
void		load_config_table::set_guard_cf_check_function_pointer(DWORD guard_cf_check_function_pointer) {
	this->guard_cf_check_function_pointer = guard_cf_check_function_pointer;
}
void		load_config_table::set_guard_cf_function_table(DWORD guard_cf_function_table) {
	this->guard_cf_function_table = guard_cf_function_table;
}
void		load_config_table::set_guard_flags(DWORD guard_flags) {
	this->guard_flags = guard_flags;
}
bool        load_config_table::is_empty() const {

    return !(timestamp || major_version || minor_version || global_flagsclear || global_flagsset ||
        criticalsection_default_timeout || decommit_freeblock_threshold || decommit_totalfree_threshold ||
        maximum_allocation_size || virtual_memory_threshold || process_heap_flags || process_affinity_mask ||
        csd_version || editlist || security_cookie || guard_cf_check_function_pointer || guard_flags ||
        se_handlers.size() || lock_prefixes_rva.size() || guard_cf_functions_rva.size());
}
DWORD		load_config_table::get_timestamp() const {
	return this->timestamp;
}
WORD		load_config_table::get_major_version() const {
	return this->major_version;
}
WORD		load_config_table::get_minor_version() const {
	return this->minor_version;
}
DWORD		load_config_table::get_global_flagsclear() const {
	return this->global_flagsclear;
}
DWORD		load_config_table::get_global_flagsset() const {
	return this->global_flagsset;
}
DWORD		load_config_table::get_criticalsection_default_timeout() const {
	return this->criticalsection_default_timeout;
}
ULONGLONG	load_config_table::get_decommit_freeblock_threshold() const {
	return this->decommit_freeblock_threshold;
}
ULONGLONG	load_config_table::get_decommit_totalfree_threshold() const {
	return this->decommit_totalfree_threshold;
}
DWORD	load_config_table::get_lock_prefix_table() const {
	return this->lock_prefix_table;
}
ULONGLONG	load_config_table::get_maximum_allocation_size() const {
	return this->maximum_allocation_size;
}
ULONGLONG	load_config_table::get_virtual_memory_threshold() const {
	return this->virtual_memory_threshold;
}
DWORD		load_config_table::get_process_heap_flags() const {
	return this->process_heap_flags;
}
ULONGLONG	load_config_table::get_process_affinity_mask() const {
	return this->process_affinity_mask;
}
WORD		load_config_table::get_csd_version() const {
	return this->csd_version;
}
DWORD	load_config_table::get_editlist() const {
	return this->editlist;
}
DWORD	load_config_table::get_security_cookie() const {
	return this->security_cookie;
}
DWORD	load_config_table::get_se_handler_table() const {
	return this->se_handler_table;
}
DWORD	load_config_table::get_se_handler_count() const {
	return this->se_handlers.size();
}
DWORD	load_config_table::get_guard_cf_check_function_pointer() const {
	return this->guard_cf_check_function_pointer;
}
DWORD	load_config_table::get_guard_cf_function_table() const {
	return this->guard_cf_function_table;
}
DWORD	    load_config_table::get_guard_cf_function_count() const {
    return this->guard_cf_functions_rva.size();
}
DWORD		load_config_table::get_guard_flags() const {
	return this->guard_flags;
}
std::vector<DWORD>& load_config_table::get_se_handlers() {
	return this->se_handlers;
}
std::vector<DWORD>& load_config_table::get_lock_prefixes() {
	return this->lock_prefixes_rva;
}
std::vector<DWORD>& load_config_table::get_guard_cf_functions() {
    return this->guard_cf_functions_rva;
}


bool get_load_config_table(const pe_image &image, load_config_table& load_config) {
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
    load_config.get_se_handlers().clear();
    load_config.get_lock_prefixes().clear();
    load_config.get_guard_cf_functions().clear();

    DWORD virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG);
	DWORD virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG);

	if (virtual_address && virtual_size) {
        pe_section * load_cfg_section = image.get_section_by_rva(virtual_address);
        if (load_cfg_section) {
            if (image.is_x32_image()) {

                PIMAGE_LOAD_CONFIG_DIRECTORY32 image_load_config = (PIMAGE_LOAD_CONFIG_DIRECTORY32)&load_cfg_section->get_section_data()[virtual_address - load_cfg_section->get_virtual_address()];

                load_config.set_timestamp(image_load_config->TimeDateStamp);
                load_config.set_major_version(image_load_config->MajorVersion);
                load_config.set_minor_version(image_load_config->MinorVersion);
                load_config.set_global_flagsclear(image_load_config->GlobalFlagsClear);
                load_config.set_global_flagsset(image_load_config->GlobalFlagsSet);
                load_config.set_criticalsection_default_timeout(image_load_config->CriticalSectionDefaultTimeout);
                load_config.set_decommit_freeblock_threshold(image_load_config->DeCommitFreeBlockThreshold);
                load_config.set_decommit_totalfree_threshold(image_load_config->DeCommitTotalFreeThreshold);
                if (image_load_config->LockPrefixTable) {
                    load_config.set_lock_prefix_table(image.va_to_rva(image_load_config->LockPrefixTable));
                }
                load_config.set_maximum_allocation_size(image_load_config->MaximumAllocationSize);
                load_config.set_virtual_memory_threshold(image_load_config->VirtualMemoryThreshold);
                load_config.set_process_heap_flags(image_load_config->ProcessHeapFlags);
                load_config.set_process_affinity_mask(image_load_config->ProcessAffinityMask);
                load_config.set_csd_version(image_load_config->CSDVersion);
                if (image_load_config->EditList) {
                    load_config.set_editlist(image.va_to_rva(image_load_config->EditList));
                }
                if (image_load_config->SecurityCookie) {
                    load_config.set_security_cookie(image.va_to_rva(image_load_config->SecurityCookie));
                }
                if (image_load_config->SEHandlerTable) {
                    load_config.set_se_handler_table(image.va_to_rva(image_load_config->SEHandlerTable));
                }
                if (image_load_config->GuardCFCheckFunctionPointer) {
                    load_config.set_guard_cf_check_function_pointer(image.va_to_rva(image_load_config->GuardCFCheckFunctionPointer));
                }
                if (image_load_config->GuardCFFunctionTable) {
                    load_config.set_guard_cf_function_table(image.va_to_rva(image_load_config->GuardCFFunctionTable));
                }
                load_config.set_guard_flags(image_load_config->GuardFlags);

                for (DWORD i = 0; i < image_load_config->SEHandlerCount; i++) {
                    DWORD se_handler_rva;

                    image.get_data_by_rva(image.va_to_rva(image_load_config->SEHandlerTable + i * sizeof(DWORD)),
                        &se_handler_rva, sizeof(se_handler_rva));

                    load_config.get_se_handlers().push_back( se_handler_rva );
                }

                if (image_load_config->LockPrefixTable) {
                    unsigned long current = 0;
                    while (true){

                        DWORD lock_prefix_va;

                        image.get_data_by_rva(image.va_to_rva(image_load_config->LockPrefixTable + current * sizeof(DWORD)),
                            &lock_prefix_va, sizeof(lock_prefix_va));


                        if (!lock_prefix_va) {
                            break;
                        }
                        
                       load_config.get_lock_prefixes().push_back(image.va_to_rva(lock_prefix_va));
                        ++current;
                    }
                }
                for (DWORD i = 0; i < image_load_config->GuardCFFunctionCount; i++) {
                    DWORD cf_function_va;

                    image.get_data_by_rva(image.va_to_rva(image_load_config->GuardCFFunctionTable + i * sizeof(DWORD)) ,
                        &cf_function_va, sizeof(cf_function_va));
                    
                    load_config.get_guard_cf_functions().push_back(image.va_to_rva(cf_function_va));
                }
                return true;
            }
            else {
                PIMAGE_LOAD_CONFIG_DIRECTORY64 image_load_config = (PIMAGE_LOAD_CONFIG_DIRECTORY64)&load_cfg_section->get_section_data()[virtual_address - load_cfg_section->get_virtual_address()];

                load_config.set_timestamp(image_load_config->TimeDateStamp);
                load_config.set_major_version(image_load_config->MajorVersion);
                load_config.set_minor_version(image_load_config->MinorVersion);
                load_config.set_global_flagsclear(image_load_config->GlobalFlagsClear);
                load_config.set_global_flagsset(image_load_config->GlobalFlagsSet);
                load_config.set_criticalsection_default_timeout(image_load_config->CriticalSectionDefaultTimeout);
                load_config.set_decommit_freeblock_threshold(image_load_config->DeCommitFreeBlockThreshold);
                load_config.set_decommit_totalfree_threshold(image_load_config->DeCommitTotalFreeThreshold);
                if (image_load_config->LockPrefixTable) {
                    load_config.set_lock_prefix_table(image.va_to_rva(image_load_config->LockPrefixTable));
                }
                load_config.set_maximum_allocation_size(image_load_config->MaximumAllocationSize);
                load_config.set_virtual_memory_threshold(image_load_config->VirtualMemoryThreshold);
                load_config.set_process_heap_flags(image_load_config->ProcessHeapFlags);
                load_config.set_process_affinity_mask(image_load_config->ProcessAffinityMask);
                load_config.set_csd_version(image_load_config->CSDVersion);
                if (image_load_config->EditList) {
                    load_config.set_editlist(image.va_to_rva(image_load_config->EditList));
                }
                if (image_load_config->SecurityCookie) {
                    load_config.set_security_cookie(image.va_to_rva(image_load_config->SecurityCookie));
                }
                if (image_load_config->SEHandlerTable) {
                    load_config.set_se_handler_table(image.va_to_rva(image_load_config->SEHandlerTable));
                }
                if (image_load_config->GuardCFCheckFunctionPointer) {
                    load_config.set_guard_cf_check_function_pointer(image.va_to_rva(image_load_config->GuardCFCheckFunctionPointer));
                }
                if (image_load_config->GuardCFFunctionTable) {
                    load_config.set_guard_cf_function_table(image.va_to_rva(image_load_config->GuardCFFunctionTable));
                }
                load_config.set_guard_flags(image_load_config->GuardFlags);


                for (DWORD i = 0; i < image_load_config->GuardCFFunctionCount; i++) {
                    DWORD64 cf_function_va;

                    image.get_data_by_rva(image.va_to_rva(image_load_config->GuardCFFunctionTable + i * sizeof(DWORD64)),
                        &cf_function_va, sizeof(cf_function_va));

                    load_config.get_guard_cf_functions().push_back(image.va_to_rva(cf_function_va));
                }
                return true;
            }
        }
	}


	return false;
}

void build_load_config_table(pe_image &image, pe_section& section, load_config_table& load_config, relocation_table& relocs) {

    if (section.get_size_of_raw_data() & 0xF) {
        section.get_section_data().resize(
            section.get_section_data().size() + (0x10 - (section.get_section_data().size()&0xF))
        );
    }

    if (image.is_x32_image()) {
        DWORD load_config_offset =  section.get_size_of_raw_data();

        section.get_section_data().resize(section.get_size_of_raw_data() + 
        sizeof(IMAGE_LOAD_CONFIG_DIRECTORY32) + 
            (load_config.get_se_handlers().size() ? load_config.get_se_handlers().size() * sizeof(DWORD) : 0) +
            (load_config.get_lock_prefixes().size() ? (load_config.get_lock_prefixes().size()+1) * sizeof(DWORD) : 0) +
            (load_config.get_guard_cf_functions().size() ? load_config.get_guard_cf_functions().size() * sizeof(DWORD) : 0)
        );

        PIMAGE_LOAD_CONFIG_DIRECTORY32 image_load_config = (PIMAGE_LOAD_CONFIG_DIRECTORY32)&section.get_section_data().data()[load_config_offset];
        
        image_load_config->Size = sizeof(IMAGE_LOAD_CONFIG_DIRECTORY32);
        image_load_config->TimeDateStamp                 = load_config.get_timestamp();
        image_load_config->MajorVersion                  = load_config.get_major_version();
        image_load_config->MinorVersion                  = load_config.get_minor_version();
        image_load_config->GlobalFlagsClear              = load_config.get_global_flagsclear();
        image_load_config->GlobalFlagsSet                = load_config.get_global_flagsset();
        image_load_config->CriticalSectionDefaultTimeout = load_config.get_criticalsection_default_timeout();
        image_load_config->DeCommitFreeBlockThreshold    = load_config.get_decommit_freeblock_threshold();
        image_load_config->DeCommitTotalFreeThreshold    = load_config.get_decommit_totalfree_threshold();
        image_load_config->MaximumAllocationSize         = load_config.get_maximum_allocation_size();
        image_load_config->VirtualMemoryThreshold        = load_config.get_virtual_memory_threshold();
        image_load_config->ProcessHeapFlags              = load_config.get_process_heap_flags();
        image_load_config->ProcessAffinityMask           = load_config.get_process_affinity_mask();
        image_load_config->CSDVersion                    = load_config.get_csd_version();
        image_load_config->Reserved1 = 0;

        if (load_config.get_editlist()) {
            image_load_config->EditList = image.rva_to_va(load_config.get_editlist());
            relocs.add_item(section.get_virtual_address() + load_config_offset + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY32, EditList), 0);
        }
        if (load_config.get_security_cookie()) {
            image_load_config->SecurityCookie = image.rva_to_va(load_config.get_security_cookie());
            relocs.add_item(section.get_virtual_address() + load_config_offset + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY32, SecurityCookie), 0);
        }
        if (load_config.get_guard_cf_check_function_pointer()) {
            image_load_config->GuardCFCheckFunctionPointer = image.rva_to_va(load_config.get_guard_cf_check_function_pointer());
            relocs.add_item(section.get_virtual_address() + load_config_offset + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY32, GuardCFCheckFunctionPointer), 0);
        }

        image_load_config->Reserved2 = 0;
        image_load_config->GuardFlags = load_config.get_guard_flags();

        

        if (load_config.get_se_handlers().size()) { //only x86
            image_load_config->SEHandlerTable = section.get_virtual_address() + load_config_offset + sizeof(IMAGE_LOAD_CONFIG_DIRECTORY32);
            image_load_config->SEHandlerCount = load_config.get_se_handlers().size();
            relocs.add_item(section.get_virtual_address() + load_config_offset + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY32, SEHandlerTable), 0);

            for (unsigned int item_idx = 0; item_idx < load_config.get_se_handlers().size(); item_idx++) {
                *(DWORD*)&section.get_section_data().data()[
                    load_config_offset + sizeof(IMAGE_LOAD_CONFIG_DIRECTORY32) + item_idx * sizeof(DWORD)
                ] = load_config.get_se_handlers()[item_idx];
            }
        }

        if (load_config.get_lock_prefixes().size()) {//only x86
            image_load_config->LockPrefixTable = section.get_virtual_address() + load_config_offset + sizeof(IMAGE_LOAD_CONFIG_DIRECTORY32) +
                (load_config.get_se_handlers().size() ? load_config.get_se_handlers().size() * sizeof(DWORD) : 0);
            relocs.add_item(section.get_virtual_address() + load_config_offset + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY32, LockPrefixTable), 0);

            for (unsigned int item_idx = 0; item_idx < load_config.get_lock_prefixes().size(); item_idx++) {
                *(DWORD*)&section.get_section_data().data()[
                    load_config_offset + sizeof(IMAGE_LOAD_CONFIG_DIRECTORY32) + item_idx * sizeof(DWORD)
                ] = image.rva_to_va(load_config.get_lock_prefixes()[item_idx]);

                relocs.add_item(section.get_virtual_address() + load_config_offset + sizeof(IMAGE_LOAD_CONFIG_DIRECTORY32) +
                    (load_config.get_se_handlers().size() ? load_config.get_se_handlers().size() * sizeof(DWORD) : 0) +
                    item_idx*sizeof(DWORD)
                    , 0);
            }

            *(DWORD*)&section.get_section_data().data()[
                load_config_offset + sizeof(IMAGE_LOAD_CONFIG_DIRECTORY32) + load_config.get_lock_prefixes().size() * sizeof(DWORD)
            ] = 0;
        }

        if (load_config.get_guard_cf_functions().size()) {
            image_load_config->GuardCFFunctionTable = section.get_virtual_address() + load_config_offset + sizeof(IMAGE_LOAD_CONFIG_DIRECTORY32) +
                (load_config.get_se_handlers().size() ? load_config.get_se_handlers().size() * sizeof(DWORD) : 0) +
                (load_config.get_lock_prefixes().size() ? (load_config.get_lock_prefixes().size() + 1) * sizeof(DWORD) : 0);
            image_load_config->GuardCFFunctionCount = load_config.get_guard_cf_functions().size();
            relocs.add_item(section.get_virtual_address() + load_config_offset + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY32, GuardCFFunctionTable), 0);


            for (unsigned int item_idx = 0; item_idx < load_config.get_guard_cf_functions().size(); item_idx++) {
                *(DWORD*)&section.get_section_data().data()[
                    load_config_offset + sizeof(IMAGE_LOAD_CONFIG_DIRECTORY32) + item_idx * sizeof(DWORD)
                ] = image.rva_to_va(load_config.get_guard_cf_functions()[item_idx]);

                relocs.add_item(section.get_virtual_address() + load_config_offset + sizeof(IMAGE_LOAD_CONFIG_DIRECTORY32) +
                    (load_config.get_se_handlers().size() ? load_config.get_se_handlers().size() * sizeof(DWORD) : 0) +
                    (load_config.get_lock_prefixes().size() ? (load_config.get_lock_prefixes().size() + 1) * sizeof(DWORD) : 0) +
                     item_idx * sizeof(DWORD)
                     , 0);
            }
        }
        

        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG, section.get_virtual_address() + load_config_offset);
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG, sizeof(IMAGE_LOAD_CONFIG_DIRECTORY32));
    }
    else {
        DWORD load_config_offset = section.get_size_of_raw_data();
        unsigned int current_offset = 0;

        section.get_section_data().resize(section.get_size_of_raw_data() +
            sizeof(IMAGE_LOAD_CONFIG_DIRECTORY64) +
            (load_config.get_guard_cf_functions().size() ? load_config.get_guard_cf_functions().size() * sizeof(DWORD64) : 0)
        );

        PIMAGE_LOAD_CONFIG_DIRECTORY64 image_load_config = (PIMAGE_LOAD_CONFIG_DIRECTORY64)&section.get_section_data().data()[load_config_offset];

        image_load_config->Size = sizeof(IMAGE_LOAD_CONFIG_DIRECTORY64);
        image_load_config->TimeDateStamp = load_config.get_timestamp();
        image_load_config->MajorVersion = load_config.get_major_version();
        image_load_config->MinorVersion = load_config.get_minor_version();
        image_load_config->GlobalFlagsClear = load_config.get_global_flagsclear();
        image_load_config->GlobalFlagsSet = load_config.get_global_flagsset();
        image_load_config->CriticalSectionDefaultTimeout = load_config.get_criticalsection_default_timeout();
        image_load_config->DeCommitFreeBlockThreshold = load_config.get_decommit_freeblock_threshold();
        image_load_config->DeCommitTotalFreeThreshold = load_config.get_decommit_totalfree_threshold();
        image_load_config->MaximumAllocationSize = load_config.get_maximum_allocation_size();
        image_load_config->VirtualMemoryThreshold = load_config.get_virtual_memory_threshold();
        image_load_config->ProcessHeapFlags = load_config.get_process_heap_flags();
        image_load_config->ProcessAffinityMask = load_config.get_process_affinity_mask();
        image_load_config->CSDVersion = load_config.get_csd_version();
        image_load_config->Reserved1 = 0;

        if (load_config.get_editlist()) {
            image_load_config->EditList = image.rva_to_va(load_config.get_editlist());
            relocs.add_item(section.get_virtual_address() + load_config_offset + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY64, EditList), 0);
        }
        if (load_config.get_security_cookie()) {
            image_load_config->SecurityCookie = image.rva_to_va(load_config.get_security_cookie());
            relocs.add_item(section.get_virtual_address() + load_config_offset + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY64, SecurityCookie), 0);
        }
        if (load_config.get_guard_cf_check_function_pointer()) {
            image_load_config->GuardCFCheckFunctionPointer = image.rva_to_va(load_config.get_guard_cf_check_function_pointer());
            relocs.add_item(section.get_virtual_address() + load_config_offset + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY64, GuardCFCheckFunctionPointer), 0);
        }

        image_load_config->Reserved2 = 0;
        image_load_config->GuardFlags = load_config.get_guard_flags();

        current_offset += sizeof(IMAGE_LOAD_CONFIG_DIRECTORY64);

        if (load_config.get_guard_cf_functions().size()) {
            image_load_config->GuardCFFunctionTable = section.get_virtual_address() + load_config_offset + sizeof(IMAGE_LOAD_CONFIG_DIRECTORY64);
            image_load_config->GuardCFFunctionCount = load_config.get_guard_cf_functions().size();
            relocs.add_item(section.get_virtual_address() + load_config_offset + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY64, GuardCFFunctionTable), 0);


            for (unsigned int item_idx = 0; item_idx < load_config.get_guard_cf_functions().size(); item_idx++) {
                *(DWORD64*)&section.get_section_data().data()[
                    load_config_offset + sizeof(IMAGE_LOAD_CONFIG_DIRECTORY64) + item_idx * sizeof(DWORD64)
                ] = image.rva_to_va(load_config.get_guard_cf_functions()[item_idx]);

                 relocs.add_item(section.get_virtual_address() + load_config_offset + sizeof(IMAGE_LOAD_CONFIG_DIRECTORY64) +
                     item_idx * sizeof(DWORD64)
                        , 0);
            }
        }


        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG, section.get_virtual_address() + load_config_offset);
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG, sizeof(IMAGE_LOAD_CONFIG_DIRECTORY64));
    }

}

bool erase_load_config_table(pe_image &image, std::vector<erased_zone>* zones, relocation_table* relocs) {

    image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG, 0);
    image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG, 0);


    DWORD virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG);
    DWORD virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG);

    if (virtual_address && virtual_size) {
        pe_section * load_cfg_section = image.get_section_by_rva(virtual_address);
        if (load_cfg_section) {
            if (image.is_x32_image()) {

                PIMAGE_LOAD_CONFIG_DIRECTORY32 image_load_config = (PIMAGE_LOAD_CONFIG_DIRECTORY32)&load_cfg_section->get_section_data()[virtual_address - load_cfg_section->get_virtual_address()];

                if (relocs) {
                    if (image_load_config->LockPrefixTable) {
                        relocs->erase_item(virtual_address + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY32, LockPrefixTable));
                    }
                    if (image_load_config->EditList) {
                        relocs->erase_item(virtual_address + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY32, EditList));
                    }
                    if (image_load_config->SecurityCookie) {
                        relocs->erase_item(virtual_address + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY32, SecurityCookie));
                    }
                    if (image_load_config->SEHandlerTable) {
                        relocs->erase_item(virtual_address + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY32, SEHandlerTable));
                    }
                    if (image_load_config->GuardCFCheckFunctionPointer) {
                        relocs->erase_item(virtual_address + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY32, GuardCFCheckFunctionPointer));
                    }
                    if (image_load_config->GuardCFFunctionTable) {
                        relocs->erase_item(virtual_address + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY32, GuardCFFunctionTable));
                    }
                }

                for (DWORD i = 0; i < image_load_config->SEHandlerCount; i++) {
                    DWORD se_handler_rva  = 0;

                    image.set_data_by_rva(image.va_to_rva(image_load_config->SEHandlerTable + i * sizeof(DWORD)),
                        &se_handler_rva, sizeof(se_handler_rva));
                    if (zones) {
                        zones->push_back({ image.va_to_rva( image_load_config->SEHandlerTable + i * sizeof(DWORD)), sizeof(DWORD) });
                    }
                }

                if (image_load_config->LockPrefixTable) {
                    unsigned long current = 0;
                    while (true) {

                        DWORD lock_prefix_va;

                        image.get_data_by_rva(image.va_to_rva(image_load_config->LockPrefixTable + current * sizeof(DWORD)),
                            &lock_prefix_va, sizeof(lock_prefix_va));


                        if (!lock_prefix_va) {
                            break;
                        }

                        lock_prefix_va = 0;
                        image.set_data_by_rva(image.va_to_rva(image_load_config->LockPrefixTable + current * sizeof(DWORD)),
                            &lock_prefix_va, sizeof(lock_prefix_va));

                        if (relocs) {
                            relocs->erase_item(image.va_to_rva(image_load_config->LockPrefixTable + current * sizeof(DWORD)));
                        }
                        if (zones) {
                            zones->push_back({ image.va_to_rva(image_load_config->LockPrefixTable + current * sizeof(DWORD)) , sizeof(DWORD)*2 });
                        }
                        ++current;
                    }
                }

                for (DWORD i = 0; i < image_load_config->GuardCFFunctionCount; i++) {
                    DWORD cf_function_rva = 0;

                    image.set_data_by_rva(image_load_config->GuardCFFunctionTable + i * sizeof(DWORD) - (DWORD)image.get_image_base(),
                        &cf_function_rva, sizeof(cf_function_rva));

                    if (relocs) {
                        relocs->erase_item(image_load_config->GuardCFFunctionTable + i * sizeof(DWORD) - (DWORD)image.get_image_base());
                    }
                    if (zones) {
                        zones->push_back({ image_load_config->GuardCFFunctionTable + i * sizeof(DWORD) - (DWORD)image.get_image_base() , sizeof(DWORD) });
                    }
                }

                if (zones) {
                    zones->push_back({ virtual_address , sizeof(IMAGE_LOAD_CONFIG_DIRECTORY32) });
                }
                ZeroMemory(image_load_config,sizeof(IMAGE_LOAD_CONFIG_DIRECTORY32));
                return true;
            }
            else {
                PIMAGE_LOAD_CONFIG_DIRECTORY64 image_load_config = (PIMAGE_LOAD_CONFIG_DIRECTORY64)&load_cfg_section->get_section_data()[virtual_address - load_cfg_section->get_virtual_address()];

                if (relocs) {
                    if (image_load_config->LockPrefixTable) {
                        relocs->erase_item(virtual_address + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY64, LockPrefixTable));
                    }
                    if (image_load_config->EditList) {
                        relocs->erase_item(virtual_address + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY64, EditList));
                    }
                    if (image_load_config->SecurityCookie) {
                        relocs->erase_item(virtual_address + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY64, SecurityCookie));
                    }
                    if (image_load_config->SEHandlerTable) {
                        relocs->erase_item(virtual_address + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY64, SEHandlerTable));
                    }
                    if (image_load_config->GuardCFCheckFunctionPointer) {
                        relocs->erase_item(virtual_address + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY64, GuardCFCheckFunctionPointer));
                    }
                    if (image_load_config->GuardCFFunctionTable) {
                        relocs->erase_item(virtual_address + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY64, GuardCFFunctionTable));
                    }
                }

                for (DWORD i = 0; i < image_load_config->GuardCFFunctionCount; i++) {
                    DWORD64 cf_function_rva = 0;

                    image.set_data_by_rva(image.va_to_rva(image_load_config->GuardCFFunctionTable + i * sizeof(DWORD64)),
                        &cf_function_rva, sizeof(cf_function_rva));

                    if (relocs) {
                        relocs->erase_item(image.va_to_rva(image_load_config->GuardCFFunctionTable + i * sizeof(DWORD64)));
                    }
                    if (zones) {
                        zones->push_back({image.va_to_rva(image_load_config->GuardCFFunctionTable + i * sizeof(DWORD64)), sizeof(DWORD64) });
                    }
                }

                if (zones) {
                    zones->push_back({virtual_address , sizeof(IMAGE_LOAD_CONFIG_DIRECTORY64) });
                }
                ZeroMemory(image_load_config, sizeof(IMAGE_LOAD_CONFIG_DIRECTORY64));
                return true;
            }
        }
    }

	return false;
}