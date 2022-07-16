#pragma once

namespace enma {

#ifndef ENMA_IO_DEFINES 
#define ENMA_IO_DEFINES

    enum enma_io_mode {
        enma_io_mode_default,
        enma_io_mode_allow_expand,
    };

    enum enma_io_addressing_type {
        enma_io_address_raw,
        enma_io_address_rva,
        enma_io_address_va,
    };

    enum enma_io_code {
        enma_io_success,
        enma_io_incomplete, //part of read\write
        enma_io_data_not_present,

        enma_io_type_not_supported,
    };

#endif

    struct pe_image;
    struct pe_image_full;
    struct pe_section;
    struct pe_export_directory;
    struct pe_import_directory;
    struct pe_resource_directory;
    struct pe_exceptions_directory;
    struct pe_security_directory;
    struct pe_relocations_directory;
    struct pe_debug_directory;
    struct pe_tls_directory;
    struct pe_load_config_directory;
    struct pe_delay_import_directory;
    struct pe_bound_import_directory;
    struct pe_dotnet_directory;
}

#pragma warning(push)
#pragma warning(disable: 28251) //disable warning about not included in SAL anotations

#include "enma_pe_internal.h"
#include "enma_pe_properties.h"

#include "pe_headers.h"
#include "pe_image.h"
#include "pe_image_io.h"

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

#include "pe_image_full.h"
#include "pe_builder.h"

#include "pe_highlevel.h"

#pragma warning(pop)
