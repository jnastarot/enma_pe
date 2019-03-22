#pragma once

struct pe_image_expanded;
class pe_image;
class pe_section;


#include "enma_pe_internal.h"
#include "enma_pe_properties.h"

#include "pe_image.h"
#include "pe_image_io.h"

enum id_pe_placement {
    id_pe_none,

    id_pe_export_descriptor,
    id_pe_export_functions_table,
    id_pe_export_names_table,
    id_pe_export_ordinals_table,
    id_pe_export_name,
    id_pe_export_function_name,
    id_pe_export_function_forwarded_name,

    id_pe_import_descriptor,
    id_pe_import_original_first_think,
    id_pe_import_first_think,
    id_pe_import_library_name,
    id_pe_import_function_name,

    id_pe_bound_import_descriptor,
    id_pe_bound_import_ref_desc,
    id_pe_bound_import_library_name,

    id_pe_delay_import_descriptor,
    id_pe_delay_import_name_table,
    id_pe_delay_import_bound_table,
    id_pe_delay_import_unload_table,
    id_pe_delay_import_library_name,
    id_pe_delay_import_function_name,

    id_pe_resources,

    id_pe_exception_descriptors,
    id_pe_exception_unwindinfo,

    id_pe_security_descriptor,
    id_pe_security_certificate,

    id_pe_relocations_descriptor,
    id_pe_relocations_block,

    id_pe_debug_descriptor,
    id_pe_debug_item_data,
    
    id_pe_tls_descriptor,
    id_pe_tls_raw_data,
    id_pe_tls_index,
    id_pe_tls_callbacks,

    id_pe_loadconfig_descriptor,
    id_pe_loadconfig_se_table,
    id_pe_loadconfig_lock_table,
    id_pe_loadconfig_cf_table,
    id_pe_loadconfig_iat_table,
    id_pe_loadconfig_long_jump_table,   
};

struct directory_placement {
    size_t size;
    id_pe_placement id;
    std::string name;

    directory_placement()
        :size(0), id(id_pe_none), name() {};
    directory_placement(size_t size, id_pe_placement id, std::string name)
        :size(size), id(id), name(name) {};
};

typedef std::map<uint32_t, directory_placement> pe_directory_placement;

enum directory_code {
    directory_code_success,
    directory_code_not_present,
    directory_code_currupted,
};

#include "pe_relocations.h"
#include "pe_export.h"
#include "pe_bound_import.h"
#include "pe_import.h"
#include "pe_resources.h"
#include "pe_exceptions.h"
#include "pe_security.h"
#include "pe_debug.h"
#include "pe_tls.h"
#include "pe_loadconfig.h"
#include "pe_delay_import.h"
#include "pe_dotnet.h"

struct pe_image_expanded {
    directory_code code;
    
    export_table        exports;
    import_table        imports;
    resource_directory  resources;
    exceptions_table    exceptions;
    security_table      security;
    relocation_table    relocations;
    debug_table         debug;
    tls_table           tls;
    load_config_table   load_config;
    delay_import_table  delay_imports;
    bound_import_table  bound_imports;

    pe_image image;

    pe_image_expanded();
    pe_image_expanded(const pe_image_expanded& image_ex);
    pe_image_expanded& operator=(const pe_image_expanded& image_ex);
};


void get_expanded_pe_image(_Inout_ pe_image_expanded& expanded_image, _In_ const pe_image &image);
directory_code get_directories_placement(_Inout_ pe_image &image, _Out_ pe_directory_placement& placement,
    _In_opt_ const bound_import_table* bound_imports = 0);


#include "pe_helper.h"