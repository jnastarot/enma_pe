#pragma once


#include "pe_image.h"

struct erased_zone { DWORD rva; unsigned int size; };

#include "pe_export.h"
#include "pe_import.h"
#include "pe_resources.h"
#include "pe_exceptions.h"
#include "pe_security.h"
#include "pe_relocations.h"
#include "pe_debug.h"
#include "pe_tls.h"
#include "pe_loadconfig.h"
#include "pe_bound_import.h"
#include "pe_delay_import.h"
#include "pe_dotnet.h"

struct pe_image_expanded {
    export_table        exports;
    import_table        imports;
    resource_directory  resources;
    exceptions_table    exceptions;
    relocation_table    relocations;
    debug_table         debug;
    tls_table           tls;
    load_config_table   load_config;
    delay_import_table  delay_imports;
    bound_import_table  bound_imports;

    pe_image image;

    pe_image_expanded& pe_image_expanded::operator=(const pe_image_expanded& image_ex) {
        this->exports       = image_ex.exports;
        this->imports       = image_ex.imports;
        this->resources     = image_ex.resources;
        this->exceptions    = image_ex.exceptions;
        this->relocations   = image_ex.relocations;
        this->debug         = image_ex.debug;
        this->tls           = image_ex.tls;
        this->load_config   = image_ex.load_config;
        this->delay_imports = image_ex.delay_imports;
        this->bound_imports = image_ex.bound_imports;
        this->image         = image_ex.image;
        return *this;
    }
};


void do_expanded_pe_image(_Inout_ pe_image_expanded& expanded_image, _In_ const pe_image &image);
void erase_directories_pe_image(_Inout_ pe_image &image, _Out_ std::vector<erased_zone>& zones,
    _Inout_opt_ relocation_table* relocs = 0,
    _In_opt_ bool delete_empty_sections = false);


#include "pe_helper.h"