#include "stdafx.h"
#include "pe_helper.h"


double get_data_entropy(const std::vector<uint8_t> &data) {

    uint32_t bytes_count[256];
    memset(bytes_count, 0, sizeof(bytes_count));

    for (size_t offset = 0; offset < data.size(); offset++) {
        bytes_count[data[offset]]++;
    }

    double total_entropy = 0.;

    for (size_t i = 0; i < 256; i++) {
        double temp = (double)bytes_count[i] / data.size();
        if (temp > 0.) {
            total_entropy += fabs(temp * (log(temp) * 1.44269504088896340736));
        }
    }

    return total_entropy;
}

double   get_section_entropy(const pe_section& section) {
    return get_data_entropy(section.get_section_data());
}

uint32_t calculate_checksum(const std::vector<uint8_t> &pe_image) {
    image_dos_header* p_dos_header = (image_dos_header*)pe_image.data();
    if (p_dos_header->e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

    uint32_t checksum_field_offset = p_dos_header->e_lfanew +
        offsetof(image_nt_headers32, optional_header.checksum);

    uint64_t checksum = 0;

    for (size_t i = 0; i < pe_image.size(); i += 4) {
        if (i == checksum_field_offset) { continue; }

        checksum = (checksum & 0xffffffff) + *(uint32_t*)&pe_image.data()[i] + (checksum >> 32);

        if (checksum > 0xffffffff) {
            checksum = (checksum & 0xffffffff) + (checksum >> 32);
        }
    }

    checksum = (checksum & 0xffff) + (checksum >> 16);
    checksum = (checksum)+(checksum >> 16);
    checksum = checksum & 0xffff;
    checksum += pe_image.size();

    return uint32_t(checksum & 0xffffffff);
}


void erase_directories_placement(pe_image &image, std::vector<directory_placement>& placement, relocation_table* relocs, bool delete_empty_sections) {

    std::sort(placement.begin(), placement.end(), [](directory_placement& lhs, directory_placement& rhs) {
        return lhs.rva < rhs.rva;
    });

    std::vector<directory_placement> placement__ = placement;

    for (size_t parent_zone_idx = 0; parent_zone_idx + 1 < placement.size(); parent_zone_idx++) { //link zones

        if (placement[parent_zone_idx].rva <= placement[parent_zone_idx + 1].rva &&
            (placement[parent_zone_idx].rva + placement[parent_zone_idx].size) >= placement[parent_zone_idx + 1].rva
            ) {

            if ((placement[parent_zone_idx + 1].rva +
                placement[parent_zone_idx + 1].size) > (placement[parent_zone_idx].rva + placement[parent_zone_idx].size)) {

                placement[parent_zone_idx].size =
                    ((placement[parent_zone_idx + 1].rva + placement[parent_zone_idx + 1].size) - placement[parent_zone_idx].rva);
            }

            placement.erase(placement.begin() + parent_zone_idx + 1);
            parent_zone_idx--;
        }
    }

    pe_image_io image_io(image);

    for (auto& item : placement) {
        if (relocs) {
            relocs->erase_all_items_in_zone(item.rva, uint32_t(item.size));
        }

        image_io.set_image_offset(item.rva).memory_set(uint32_t(item.size), 0);

        item.id = db_id_none;
    }


    if (delete_empty_sections && image.get_sections().size()) {
        for (int32_t section_idx = int32_t(image.get_sections().size() - 1); section_idx >= 0; section_idx--) {
            auto _section = image.get_sections()[section_idx];

            for (size_t zone_idx = 0; zone_idx < placement.size(); zone_idx++) {
                
                if (_section->get_virtual_address() >= placement[zone_idx].rva && //if it cover full section
                    (_section->get_virtual_address() + _section->get_virtual_size()) <= (placement[zone_idx].rva + placement[zone_idx].size)) {

                    if (_section->get_virtual_address() == placement[zone_idx].rva &&
                        (_section->get_virtual_address() + _section->get_virtual_size()) <= (placement[zone_idx].rva + placement[zone_idx].size)) {
                        placement.erase(placement.begin() + zone_idx);
                    }
                    else {
                        placement[zone_idx].size = (placement[zone_idx].rva - _section->get_virtual_address());
                    }

                    image.get_sections().erase(image.get_sections().begin() + section_idx);
                    goto go_next_;
                }

                if (_section->get_virtual_address() < placement[zone_idx].rva && //if it cover up part of the section
                    (_section->get_virtual_address() + _section->get_virtual_size()) <= (placement[zone_idx].rva + placement[zone_idx].size)) {
                    uint32_t minus_size = (_section->get_virtual_address() + _section->get_virtual_size()) - placement[zone_idx].rva;
                    
                    if (minus_size < placement[zone_idx].size) {
                        placement[zone_idx].size -= minus_size;
                    }
                    else {
                        placement.erase(placement.begin() + zone_idx);
                    }

                    _section->set_size_of_raw_data(_section->get_size_of_raw_data() - minus_size);
                }
            }

            return;
        go_next_:;
        }
    }
}