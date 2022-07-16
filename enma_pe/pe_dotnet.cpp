#include "stdafx.h"

using namespace enma;

pe_directory_code enma::get_dotnet_directory(const pe_image &image,pe_dotnet_directory& dotnet) {

    dotnet.set_major_version(0);
    dotnet.set_minor_version(0);
    dotnet.set_flags(0);
    dotnet.set_entry_point(0);
    dotnet.set_meta_data({ 0,0 });
    dotnet.set_resources({ 0,0 });
    dotnet.set_strong_name_signature({ 0,0 });
    dotnet.set_code_manager_table({ 0,0 });
    dotnet.set_vtable_fixups({ 0,0 });
    dotnet.set_export_address_table_jumps({ 0,0 });

    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR);

    if (virtual_address) {
        pe_image_io dotnet_io(image);
        dotnet_io.set_image_offset(virtual_address);

        image_cor20_header dotnet_header;

        if (dotnet_io.read(&dotnet_header, sizeof(dotnet_header)) != enma_io_success) {
            return pe_directory_code::pe_directory_code_currupted;
        }

        dotnet.set_major_version(dotnet_header.major_runtime_version);
        dotnet.set_minor_version(dotnet_header.minor_runtime_version);

        dotnet.set_flags(dotnet_header.flags);
        dotnet.set_entry_point(dotnet_header.entry_point_rva);

        dotnet.set_meta_data(dotnet_header.meta_data);
        dotnet.set_resources(dotnet_header.resources);
        dotnet.set_strong_name_signature(dotnet_header.strong_name_signature);
        dotnet.set_code_manager_table(dotnet_header.code_manager_table);
        dotnet.set_vtable_fixups(dotnet_header.vtable_fixups);
        dotnet.set_export_address_table_jumps(dotnet_header.export_address_table_jumps);

        return pe_directory_code::pe_directory_code_success;

    }

    return pe_directory_code::pe_directory_code_not_present;
}
