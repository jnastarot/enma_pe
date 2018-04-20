#include "stdafx.h"
#include "pe_bound_import.h"


bound_imported_ref::bound_imported_ref() {
    timestamp = 0;
}
bound_imported_ref::bound_imported_ref(const bound_imported_ref& ref) {
    this->operator=(ref);
}
bound_imported_ref::bound_imported_ref(const std::string& ref_name, uint32_t  timestamp):
    ref_name(ref_name), timestamp(timestamp){

}
bound_imported_ref::~bound_imported_ref() {

}

bound_imported_ref& bound_imported_ref::operator=(const bound_imported_ref& ref) {
    this->timestamp = ref.timestamp;
    this->ref_name = ref.ref_name;

    return *this;
}

void bound_imported_ref::set_ref_name(const std::string& ref_name) {
    this->ref_name = ref_name;
}
void bound_imported_ref::set_timestamp(uint32_t  timestamp) {
    this->timestamp = timestamp;
}

std::string bound_imported_ref::get_ref_name() const {
    return this->ref_name;
}
uint32_t  bound_imported_ref::get_timestamp() const {
    return this->timestamp;
}


bound_imported_library::bound_imported_library() {
    timestamp = 0;
}
bound_imported_library::bound_imported_library(const bound_imported_library& lib) {
    this->operator=(lib);
}
bound_imported_library::~bound_imported_library() {

}

bound_imported_library& bound_imported_library::operator=(const bound_imported_library& lib) {
    this->timestamp = lib.timestamp;
    this->refs = lib.refs;
    this->library_name = lib.library_name;

    return *this;
}

void bound_imported_library::set_library_name(const std::string& library_name) {
    this->library_name = library_name;
}
void bound_imported_library::set_timestamp(uint32_t  timestamp) {
    this->timestamp = timestamp;
}

void bound_imported_library::add_ref(const bound_imported_ref& ref) {
    this->refs.push_back(ref);
}

std::string bound_imported_library::get_library_name() const{
    return this->library_name;
}
uint32_t  bound_imported_library::get_timestamp() const {
    return this->timestamp;
}
size_t bound_imported_library::get_number_of_forwarder_refs() const {
    return this->refs.size();
}
std::vector<bound_imported_ref>& bound_imported_library::get_refs() {
    return this->refs;
}

bound_import_table::bound_import_table() {

}
bound_import_table::bound_import_table(const bound_import_table& imports) {
    this->operator=(imports);
}
bound_import_table::~bound_import_table() {

}

bound_import_table& bound_import_table::operator=(const bound_import_table& imports) {
    this->libs = imports.libs;

    return *this;
}
void bound_import_table::add_lib(const bound_imported_library& lib) {
    libs.push_back(lib);
}
std::vector<bound_imported_library>& bound_import_table::get_libs() {
    return libs;
}


directory_code get_bound_import_table(const pe_image &image, bound_import_table& imports) {

    imports.get_libs().clear();

    uint32_t  virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT);

    if (virtual_address) {
        pe_image_io bnd_import_desc_io(image);
        bnd_import_desc_io.set_image_offset(virtual_address);

        image_bound_import_descriptor bound_imp_description;

        if (bnd_import_desc_io.read(&bound_imp_description, sizeof(bound_imp_description)) != enma_io_success) {
            return directory_code::directory_code_currupted;
        }

        if (bound_imp_description.time_date_stamp && bound_imp_description.offset_module_name) {

            do {
                bound_imported_library bound_lib;
                bound_lib.set_timestamp(bound_imp_description.time_date_stamp);

                std::string lib_name;

                if (pe_image_io(image).set_image_offset(virtual_address + bound_imp_description.offset_module_name).read_string(
                    lib_name) != enma_io_success) {

                    return directory_code::directory_code_currupted;
                }


                for (size_t ref_idx = 0; ref_idx < bound_imp_description.number_of_module_forwarder_refs; ref_idx++) {
                    image_bound_forwarded_ref ref_description;

                    if (bnd_import_desc_io.read(&ref_description, sizeof(ref_description)) != enma_io_success) {
                        return directory_code::directory_code_currupted;
                    }

                    std::string ref_name;

                    if (pe_image_io(image).set_image_offset(virtual_address + ref_description.offset_module_name).read_string(
                        ref_name) != enma_io_success) {

                        return directory_code::directory_code_currupted;
                    }

                    bound_lib.add_ref(bound_imported_ref(ref_name, ref_description.time_date_stamp));
                }

                imports.add_lib(bound_lib);

                if (bnd_import_desc_io.read(&bound_imp_description, sizeof(bound_imp_description)) != enma_io_success) {
                    return directory_code::directory_code_currupted;
                }
            } while (bound_imp_description.time_date_stamp && bound_imp_description.offset_module_name);
        }

        return directory_code::directory_code_success;
    }


    return directory_code::directory_code_not_present;
}

bool build_bound_import_table(pe_image &image,pe_section& section,
    bound_import_table& imports) {

    if (!imports.get_libs().size()) { 
        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT,0);
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT,0);
        return true; 
    }

    if (section.get_size_of_raw_data() & 0xF) {
        section.get_section_data().resize(
            section.get_section_data().size() + (0x10 - (section.get_section_data().size() & 0xF))
        );
    }

    size_t bound_import_offset = section.get_section_data().size();
    size_t  descs_size = 0;
    size_t  names_size = 0;

    for (auto& bound_desc : imports.get_libs()) {
        descs_size += sizeof(image_bound_import_descriptor);
        names_size += bound_desc.get_library_name().length() + 1;

        for (auto& bound_ref : bound_desc.get_refs()){
            descs_size += sizeof(image_bound_forwarded_ref);
            names_size += bound_ref.get_ref_name().length() + 1;
        }
    }
    descs_size += sizeof(image_bound_import_descriptor);

    section.get_section_data().resize(
        section.get_section_data().size() + 
        descs_size + names_size
    );

    memset(&section.get_section_data().data()[
        bound_import_offset
    ],0, descs_size + names_size);

    uint8_t  * desc_data = &section.get_section_data().data()[
        bound_import_offset
    ];
    size_t names_offset = 0;

    for (auto& bound_desc : imports.get_libs()) {
        image_bound_import_descriptor* bound_data = (image_bound_import_descriptor*)desc_data;
        bound_data->time_date_stamp = bound_desc.get_timestamp();
        bound_data->number_of_module_forwarder_refs = (uint16_t)bound_desc.get_number_of_forwarder_refs();
        bound_data->offset_module_name = (uint16_t)(descs_size + names_offset);

        memcpy((char*)&section.get_section_data().data()[
            bound_import_offset + descs_size + names_offset
        ], bound_desc.get_library_name().c_str(), bound_desc.get_library_name().length() + 1);

        names_offset+= bound_desc.get_library_name().length() + 1;

        desc_data += sizeof(image_bound_import_descriptor);
        for (auto& bound_ref : bound_desc.get_refs()) {
            image_bound_forwarded_ref* ref_data = (image_bound_forwarded_ref*)desc_data;
            ref_data->time_date_stamp = bound_ref.get_timestamp();
            ref_data->offset_module_name = (uint16_t)(descs_size + names_offset);

            memcpy((char*)&section.get_section_data().data()[
                bound_import_offset + descs_size + names_offset
            ], bound_ref.get_ref_name().c_str(), bound_ref.get_ref_name().length() + 1);

            names_offset += bound_ref.get_ref_name().length() + 1;

            desc_data += sizeof(image_bound_forwarded_ref);
        }
    }
}


directory_code get_placement_bound_import_table(const pe_image &image, std::vector<directory_placement>& placement) {

    uint32_t  virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT);

    if (virtual_address) {
        pe_image_io bnd_import_desc_io(image);
        bnd_import_desc_io.set_image_offset(virtual_address);

        image_bound_import_descriptor bound_imp_description;

        if (bnd_import_desc_io.read(&bound_imp_description, sizeof(bound_imp_description)) != enma_io_success) {
            return directory_code::directory_code_currupted;
        }

        if (bound_imp_description.time_date_stamp && bound_imp_description.offset_module_name) {

            do {
                bound_imported_library bound_lib;
                bound_lib.set_timestamp(bound_imp_description.time_date_stamp);

                std::string lib_name;

                if (pe_image_io(image).set_image_offset(virtual_address + bound_imp_description.offset_module_name).read_string(
                    lib_name) != enma_io_success) {

                    return directory_code::directory_code_currupted;
                }

                placement.push_back({ virtual_address + bound_imp_description.offset_module_name ,ALIGN_UP((lib_name.length() + 1) , 0x2) ,
                    dp_id_bound_import_names
                });


                for (size_t ref_idx = 0; ref_idx < bound_imp_description.number_of_module_forwarder_refs; ref_idx++) {
                    image_bound_forwarded_ref ref_description;

                    if (bnd_import_desc_io.read(&ref_description, sizeof(ref_description)) != enma_io_success) {
                        return directory_code::directory_code_currupted;
                    }

                    std::string ref_name;

                    if (pe_image_io(image).set_image_offset(virtual_address + ref_description.offset_module_name).read_string(
                        ref_name) != enma_io_success) {

                        return directory_code::directory_code_currupted;
                    }


                    placement.push_back({ virtual_address + ref_description.offset_module_name ,ALIGN_UP((ref_name.length() + 1) , 0x2) ,
                        dp_id_bound_import_names
                    });
                }

                if (bnd_import_desc_io.read(&bound_imp_description, sizeof(bound_imp_description)) != enma_io_success) {
                    return directory_code::directory_code_currupted;
                }
            } while (bound_imp_description.time_date_stamp && bound_imp_description.offset_module_name);

            placement.push_back({ virtual_address ,
                ALIGN_UP((bnd_import_desc_io.get_image_offset() - virtual_address) ,0x10), dp_id_bound_import_desc });
        }

        return directory_code::directory_code_success;
    }


    return directory_code::directory_code_not_present;
}