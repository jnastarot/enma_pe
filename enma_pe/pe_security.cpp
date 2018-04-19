#include "stdafx.h"
#include "pe_security.h"








directory_code get_placement_security_table(pe_image &image, std::vector<directory_placement>& placement) {

    uint32_t raw_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_SECURITY);
    uint32_t virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_SECURITY);
    
    if (raw_address && virtual_size) {

        pe_image_io reloc_io(image,enma_io_address_raw);

        uint32_t _offset_real = 0;
        uint32_t available_size = 0;
        uint32_t down_oversize = 0;
        uint32_t up_oversize = 0;

        reloc_io.view_image(
            raw_address, virtual_size,
            _offset_real,
            available_size, down_oversize, up_oversize
        );

        uint32_t sec_rva = image.raw_to_rva(raw_address);
        
        if (sec_rva) {
            placement.push_back({ sec_rva , available_size, dp_id_security_desc });
        }

        if (!down_oversize && !up_oversize) {
            return directory_code::directory_code_success;
        }

        return directory_code::directory_code_currupted;
    }

    return directory_code::directory_code_not_present;
}