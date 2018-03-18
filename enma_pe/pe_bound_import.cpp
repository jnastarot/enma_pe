#include "stdafx.h"
#include "pe_bound_import.h"


bound_imported_library::bound_imported_library() {
    timestamp = 0;
    number_of_module_forwarder_refs = 0;
}
bound_imported_library::~bound_imported_library() {

}

bound_imported_library& bound_imported_library::operator=(const bound_imported_library& lib) {
    this->timestamp = lib.timestamp;
    this->number_of_module_forwarder_refs = lib.number_of_module_forwarder_refs;
    this->name = lib.name;

    return *this;
}

void bound_imported_library::set_name(std::string name) {
    this->name = name;
}
void bound_imported_library::set_timestamp(DWORD timestamp) {
    this->timestamp = timestamp;
}
void bound_imported_library::set_number_of_forwarder_refs(unsigned int number) {
    this->number_of_module_forwarder_refs = number;
}
std::string bound_imported_library::get_name() const{
    return this->name;
}
DWORD bound_imported_library::get_timestamp() const {
    return this->timestamp;
}
unsigned int bound_imported_library::get_number_of_forwarder_refs() const {
    return this->number_of_module_forwarder_refs;
}

bound_import_table::bound_import_table() {

}
bound_import_table::~bound_import_table() {

}

bound_import_table& bound_import_table::operator=(const bound_import_table& imports) {
    this->libs = imports.libs;

    return *this;
}
void bound_import_table::add_lib(bound_imported_library& lib) {
    libs.push_back(lib);
}
std::vector<bound_imported_library>& bound_import_table::get_libs() {
    return libs;
}


bool get_bound_import_table(const pe_image &image, bound_import_table& imports) {

    imports.get_libs().clear();

    DWORD virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT);
    DWORD virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT);

    if (virtual_address && virtual_size) {
        pe_section * imp_section = image.get_section_by_rva(virtual_address);
        if (imp_section) {
            BYTE * raw_decs = &imp_section->get_section_data().data()[virtual_address - imp_section->get_virtual_address()];

            for (unsigned int imp_size = 0; imp_size < virtual_size; imp_size += sizeof(PIMAGE_BOUND_IMPORT_DESCRIPTOR)) {
                PIMAGE_BOUND_IMPORT_DESCRIPTOR imp_description = (PIMAGE_BOUND_IMPORT_DESCRIPTOR)(&raw_decs[imp_size]);

                bound_imported_library lib;
                lib.set_timestamp(imp_description->TimeDateStamp);
                lib.set_number_of_forwarder_refs(imp_description->NumberOfModuleForwarderRefs);

                pe_section * name_section = image.get_section_by_rva(virtual_address + imp_description->OffsetModuleName);
                if (name_section) {
                    lib.set_name((char*)&name_section->get_section_data().data()[
                        (virtual_address + imp_description->OffsetModuleName) - name_section->get_virtual_address()
                    ]);          
                }

                imports.add_lib(lib);
            }

            return true;
        }
    }


    return false;
}

bool erase_bound_import_table(pe_image &image,std::vector<erased_zone>* zones) {

    DWORD virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT);
    DWORD virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT);

    if (virtual_address && virtual_size) {
        pe_section * imp_section = image.get_section_by_rva(virtual_address);
        if (imp_section) {
            BYTE * raw_decs = &imp_section->get_section_data().data()[virtual_address - imp_section->get_virtual_address()];

            for (unsigned int imp_size = 0; imp_size < virtual_size; imp_size += sizeof(PIMAGE_BOUND_IMPORT_DESCRIPTOR)) {
                PIMAGE_BOUND_IMPORT_DESCRIPTOR imp_description = (PIMAGE_BOUND_IMPORT_DESCRIPTOR)(&raw_decs[imp_size]);

                pe_section * name_section = image.get_section_by_rva(virtual_address + imp_description->OffsetModuleName);
                if (name_section) {
                    char * name = (char*)&name_section->get_section_data().data()[
                        (virtual_address + imp_description->OffsetModuleName) - name_section->get_virtual_address()
                    ];
                    if (zones) { zones->push_back({ DWORD(virtual_address + imp_description->OffsetModuleName) ,DWORD(lstrlen(name)) }); }
                    ZeroMemory(name, lstrlen(name));
                }
                if (zones) { zones->push_back({ (virtual_address + imp_size) ,sizeof(PIMAGE_BOUND_IMPORT_DESCRIPTOR) }); }
                ZeroMemory(&raw_decs[imp_size], sizeof(PIMAGE_BOUND_IMPORT_DESCRIPTOR));
            }

            image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT, 0);
            image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT, 0);
            return true;
        }
    }

    return false;
}