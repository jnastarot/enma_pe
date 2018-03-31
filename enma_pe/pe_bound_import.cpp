#include "stdafx.h"
#include "pe_bound_import.h"


bound_imported_ref::bound_imported_ref() {
    timestamp = 0;
}
bound_imported_ref::bound_imported_ref(const bound_imported_ref& ref) {
    this->operator=(ref);
}
bound_imported_ref::bound_imported_ref(const std::string& ref_name, DWORD timestamp) {
    this->ref_name = ref_name;
    this->timestamp = timestamp;
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
void bound_imported_ref::set_timestamp(DWORD timestamp) {
    this->timestamp = timestamp;
}

std::string bound_imported_ref::get_ref_name() const {
    return this->ref_name;
}
DWORD bound_imported_ref::get_timestamp() const {
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
void bound_imported_library::set_timestamp(DWORD timestamp) {
    this->timestamp = timestamp;
}

void bound_imported_library::add_ref(const bound_imported_ref& ref) {
    this->refs.push_back(ref);
}

std::string bound_imported_library::get_library_name() const{
    return this->library_name;
}
DWORD bound_imported_library::get_timestamp() const {
    return this->timestamp;
}
unsigned int bound_imported_library::get_number_of_forwarder_refs() const {
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


bool get_bound_import_table(const pe_image &image, bound_import_table& imports) {

    imports.get_libs().clear();

    DWORD virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT);

    if (virtual_address) {

        pe_section * bound_imp_section = image.get_section_by_rva(virtual_address);
        if (bound_imp_section) {
            BYTE * raw_decs = &bound_imp_section->get_section_data().data()[
                virtual_address - bound_imp_section->get_virtual_address()
            ];

            for (unsigned int bound_imp_size = 0;; bound_imp_size += sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR)) {
                PIMAGE_BOUND_IMPORT_DESCRIPTOR bound_imp_description = (PIMAGE_BOUND_IMPORT_DESCRIPTOR)(&raw_decs[bound_imp_size]);

                if (!bound_imp_description->TimeDateStamp &&
                    !bound_imp_description->OffsetModuleName &&
                    !bound_imp_description->NumberOfModuleForwarderRefs) {
                    break;
                }

                bound_imported_library bound_lib;
                bound_lib.set_timestamp(bound_imp_description->TimeDateStamp);

                for (unsigned long ref_idx = 0; ref_idx < bound_imp_description->NumberOfModuleForwarderRefs; ref_idx++) {
                    PIMAGE_BOUND_FORWARDER_REF ref_description = (PIMAGE_BOUND_FORWARDER_REF)(&raw_decs[
                        bound_imp_size + sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR) +
                            sizeof(IMAGE_BOUND_FORWARDER_REF)*ref_idx
                    ]);

                    pe_section * ref_name_section = image.get_section_by_rva(virtual_address + ref_description->OffsetModuleName);
                    if (ref_name_section) {
                        bound_lib.add_ref(bound_imported_ref(
                            (char*)&ref_name_section->get_section_data().data()[
                                (virtual_address + bound_imp_description->OffsetModuleName) - ref_name_section->get_virtual_address()
                            ], ref_description->TimeDateStamp
                        ));
                    }
                }

                bound_imp_size += sizeof(IMAGE_BOUND_FORWARDER_REF)*bound_imp_description->NumberOfModuleForwarderRefs;

                pe_section * bound_name_section = image.get_section_by_rva(virtual_address + bound_imp_description->OffsetModuleName);
                if (bound_name_section) {
                    bound_lib.set_library_name((char*)&bound_name_section->get_section_data().data()[
                        (virtual_address + bound_imp_description->OffsetModuleName) - bound_name_section->get_virtual_address()
                    ]);          
                }

                imports.add_lib(bound_lib);
            }

            return true;
        }
    }


    return false;
}

void build_bound_import_table(const pe_image &image,pe_section& section,
    bound_import_table& imports) {

    if (section.get_size_of_raw_data() & 0xF) {
        section.get_section_data().resize(
            section.get_section_data().size() + (0x10 - (section.get_section_data().size() & 0xF))
        );
    }

    unsigned int bound_import_offset = section.get_section_data().size();
    unsigned int descs_size = 0;
    unsigned int names_size = 0;

    for (auto& bound_desc : imports.get_libs()) {
        descs_size += sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR);
        names_size += bound_desc.get_library_name().length() + 1;

        for (auto& bound_ref : bound_desc.get_refs()){
            descs_size += sizeof(IMAGE_BOUND_FORWARDER_REF);
            names_size += bound_ref.get_ref_name().length() + 1;
        }
    }
    descs_size += sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR);

    section.get_section_data().resize(
        section.get_section_data().size() + 
        descs_size + names_size
    );

    ZeroMemory(&section.get_section_data().data()[
        bound_import_offset
    ], descs_size + names_size);

    BYTE * desc_data = &section.get_section_data().data()[
        bound_import_offset
    ];
    unsigned int names_offset = 0;

    for (auto& bound_desc : imports.get_libs()) {
        PIMAGE_BOUND_IMPORT_DESCRIPTOR bound_data = (PIMAGE_BOUND_IMPORT_DESCRIPTOR)desc_data;
        bound_data->TimeDateStamp = bound_desc.get_timestamp();
        bound_data->NumberOfModuleForwarderRefs = bound_desc.get_number_of_forwarder_refs();
        bound_data->OffsetModuleName = descs_size + names_offset;

        lstrcpyA((char*)&section.get_section_data().data()[
            bound_import_offset + descs_size + names_offset
        ], bound_desc.get_library_name().c_str());

        names_offset+= bound_desc.get_library_name().length() + 1;

        desc_data += sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR);
        for (auto& bound_ref : bound_desc.get_refs()) {
            PIMAGE_BOUND_FORWARDER_REF ref_data = (PIMAGE_BOUND_FORWARDER_REF)desc_data;
            ref_data->TimeDateStamp = bound_ref.get_timestamp();
            ref_data->OffsetModuleName = descs_size + names_offset;

            lstrcpyA((char*)&section.get_section_data().data()[
                bound_import_offset + descs_size + names_offset
            ], bound_ref.get_ref_name().c_str());

            names_offset += bound_ref.get_ref_name().length() + 1;

            desc_data += sizeof(IMAGE_BOUND_FORWARDER_REF);
        }
    }
}


bool erase_bound_import_table(pe_image &image,std::vector<erased_zone>* zones) {

    DWORD virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT);

    if (virtual_address) {
        pe_section * bound_imp_section = image.get_section_by_rva(virtual_address);
        if (bound_imp_section) {
            BYTE * raw_decs = &bound_imp_section->get_section_data().data()[
                virtual_address - bound_imp_section->get_virtual_address()
            ];

            for (unsigned int bound_imp_size = 0;; bound_imp_size += sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR)) {
                PIMAGE_BOUND_IMPORT_DESCRIPTOR bound_imp_description = (PIMAGE_BOUND_IMPORT_DESCRIPTOR)(&raw_decs[bound_imp_size]);
                
                if (!bound_imp_description->TimeDateStamp &&
                    !bound_imp_description->OffsetModuleName &&
                    !bound_imp_description->NumberOfModuleForwarderRefs) {
                    break;
                }


                for (unsigned long ref_idx = 0; ref_idx < bound_imp_description->NumberOfModuleForwarderRefs; ref_idx++) {
                    PIMAGE_BOUND_FORWARDER_REF ref_description = (PIMAGE_BOUND_FORWARDER_REF)(&raw_decs[
                        bound_imp_size + sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR) +
                            sizeof(IMAGE_BOUND_FORWARDER_REF)*ref_idx
                    ]);

                    pe_section * ref_name_section = image.get_section_by_rva(virtual_address + ref_description->OffsetModuleName);
                    if (ref_name_section) {
                        char * ref_name = (char*)&ref_name_section->get_section_data().data()[
                            (virtual_address + ref_description->OffsetModuleName) - ref_name_section->get_virtual_address()
                        ];

                        if (zones) { zones->push_back({ 
                            DWORD(virtual_address + ref_description->OffsetModuleName) ,
                            DWORD(lstrlenA(ref_name) + 1) }
                        );}

                        ZeroMemory(ref_name, lstrlenA(ref_name) + 1);
                    }

                    if (zones) { zones->push_back({ (virtual_address + bound_imp_size + sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR) +
                        sizeof(IMAGE_BOUND_FORWARDER_REF)*ref_idx) ,sizeof(IMAGE_BOUND_FORWARDER_REF) }); }

                    ZeroMemory(ref_description, sizeof(IMAGE_BOUND_FORWARDER_REF));
                }

                pe_section * bound_name_section = image.get_section_by_rva(virtual_address + bound_imp_description->OffsetModuleName);
                if (bound_name_section) {
                    char * name = (char*)&bound_name_section->get_section_data().data()[
                        (virtual_address + bound_imp_description->OffsetModuleName) - bound_name_section->get_virtual_address()
                    ];

                    if (zones) { zones->push_back({
                        DWORD(virtual_address + bound_imp_description->OffsetModuleName) ,
                        DWORD(lstrlenA(name) + 1) }
                    ); }

                    ZeroMemory(name, lstrlenA(name)+1);
                }

                if (zones) { zones->push_back({ (virtual_address + bound_imp_size) ,sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR) }); }
                ZeroMemory(&raw_decs[bound_imp_size], sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR));
            }

            image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT, 0);
            image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT, 0);
            return true;
        }
    }

    return false;
}