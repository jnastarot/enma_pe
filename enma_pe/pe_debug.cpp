#include "stdafx.h"

using namespace enma;

pe_debug_entry::pe_debug_entry(uint32_t characteristics, uint32_t timestamp, uint16_t major_version, uint16_t minor_version,
    uint32_t type, uint32_t size_of_data, uint32_t address_of_raw_data, uint32_t pointer_to_raw_data,
    void* data)
    : _characteristics(characteristics)
    , _timestamp(timestamp)
    , _major_version(major_version)
    , _minor_version(minor_version)
    , _type(type)
    , _size_of_data(size_of_data)
    , _address_of_raw_data(address_of_raw_data)
    , _pointer_to_raw_data(pointer_to_raw_data) {

    _item_data.resize(_size_of_data);
    memcpy(_item_data.data(), data, _size_of_data);
}

pe_directory_code enma::get_debug_directory(const pe_image &image, pe_debug_directory& debug) {
    debug.get_entries().clear();

    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_DEBUG);
    uint32_t virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_DEBUG);

    if (virtual_address && virtual_size) {
        pe_image_io debug_io(image);

        if (!debug_io.is_present_rva(virtual_address)) {
            return pe_directory_code::pe_directory_code_not_present;
        }

        debug_io.set_image_offset(virtual_address);
        while (debug_io.get_image_offset() < virtual_address + virtual_size) {
            image_debug_directory debug_desc;

            if (debug_io.read(&debug_desc,sizeof(debug_desc)) != enma_io_success) {
                return pe_directory_code::pe_directory_code_currupted;
            }

            pe_debug_entry item;
            item.set_characteristics(debug_desc.characteristics);
            item.set_timestamp(debug_desc.time_date_stamp);
            item.set_major_version(debug_desc.major_version);
            item.set_minor_version(debug_desc.minor_version);
            item.set_type(debug_desc.type);
            item.set_size_of_data(debug_desc.size_of_data);
            item.set_address_of_raw_data(debug_desc.address_of_raw_data);
            item.set_pointer_to_raw_data(debug_desc.pointer_to_raw_data);

           
            if (debug_desc.size_of_data && debug_desc.address_of_raw_data) {
                size_t _offset_real = 0;
                size_t available_size = 0;
                size_t down_oversize = 0;
                size_t up_oversize = 0;

                debug_io.view_image(
                    debug_desc.address_of_raw_data, debug_desc.size_of_data,
                    _offset_real,
                    available_size, down_oversize, up_oversize
                );

                if (down_oversize || up_oversize) {
                    return pe_directory_code::pe_directory_code_currupted;
                }

                if (pe_image_io(image).set_image_offset(debug_desc.address_of_raw_data).read(
                    item.get_item_data(), debug_desc.size_of_data) != enma_io_success) {
                    return pe_directory_code::pe_directory_code_currupted;
                }
            }

            debug.add_entry(item);
        }

        return pe_directory_code::pe_directory_code_success;
    }

    return pe_directory_code::pe_directory_code_not_present;
}

pe_directory_code enma::get_placement_debug_directory(const pe_image &image, pe_placement& placement) {

    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_DEBUG);
    uint32_t virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_DEBUG);

    if (virtual_address && virtual_size) {
        pe_image_io debug_io(image);

        if (!debug_io.is_present_rva(virtual_address)) {
            return pe_directory_code::pe_directory_code_not_present;
        }

        uint32_t total_desc_size = 0;

        debug_io.set_image_offset(virtual_address);
        while (debug_io.get_image_offset() < virtual_address + virtual_size) {
            total_desc_size += (uint32_t)sizeof(image_debug_directory);

            image_debug_directory debug_desc;

            if (debug_io.read(&debug_desc, sizeof(debug_desc)) != enma_io_success) {
                return pe_directory_code::pe_directory_code_currupted;
            }

            if (debug_desc.size_of_data && debug_desc.address_of_raw_data) {
                size_t _offset_real = 0;
                size_t available_size = 0;
                size_t down_oversize = 0;
                size_t up_oversize = 0;

                debug_io.view_image(
                    debug_desc.address_of_raw_data, ALIGN_UP(debug_desc.size_of_data, 0x10),
                    _offset_real,
                    available_size, down_oversize, up_oversize
                );

                if (down_oversize || up_oversize) {
                    return pe_directory_code::pe_directory_code_currupted;
                }


                placement[debug_desc.address_of_raw_data] = pe_placement_entry(available_size, id_pe_placement::id_pe_debug_item_data, "");
            }
        }

        placement[virtual_address] = pe_placement_entry(ALIGN_UP(total_desc_size, 0x10), id_pe_placement::id_pe_debug_descriptor, "");

        return pe_directory_code::pe_directory_code_success;
    }

    return pe_directory_code::pe_directory_code_not_present;
}