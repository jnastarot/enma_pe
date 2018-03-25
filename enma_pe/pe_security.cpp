#include "stdafx.h"
#include "pe_security.h"








bool erase_security_table(pe_image &image,std::vector<erased_zone>* zones){

    DWORD raw_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_SECURITY);
    DWORD virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_SECURITY);

    if (raw_address && virtual_size) {

        pe_section * security_section = image.get_section_by_raw(raw_address);

        if (security_section) {
            if (zones) {
                zones->push_back({
                  security_section->get_virtual_address() + (raw_address - security_section->get_pointer_to_raw()) ,
                  virtual_size
                });
            }
            ZeroMemory(&security_section->get_section_data().data()[
                raw_address - security_section->get_pointer_to_raw()], virtual_size);
        }

        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_SECURITY, 0);
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_SECURITY, 0);
        return true;
    }

    return false;
}