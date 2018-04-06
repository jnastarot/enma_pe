#include "stdafx.h"
#include "pe_security.h"








bool get_placement_security_table(pe_image &image, std::vector<directory_placement>& placement) {

    uint32_t raw_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_SECURITY);
    uint32_t virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_SECURITY);

    if (raw_address && virtual_size) {

        pe_section * security_section = image.get_section_by_raw(raw_address);

        if (security_section) {
            placement.push_back({
              security_section->get_virtual_address() + (raw_address - security_section->get_pointer_to_raw()) ,
              virtual_size,
              dp_id_security_desc
            });
        }

        return true;
    }

    return false;
}