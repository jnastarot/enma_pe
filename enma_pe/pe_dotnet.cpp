#include "stdafx.h"
#include "pe_dotnet.h"



dotnet_table::dotnet_table() {

}
dotnet_table::~dotnet_table() {

}

uint16_t dotnet_table::get_major_version() const {
    return major_version;
}
uint16_t dotnet_table::get_minor_version() const {
    return minor_version;
}

uint32_t dotnet_table::get_flags()const {
    return flags;
}
uint32_t dotnet_table::get_entry_point()const {
    return entry_point_rva;
}

image_data_directory dotnet_table::get_meta_data() const {
    return meta_data;
}
image_data_directory dotnet_table::get_resources() const {
    return resources;
}
image_data_directory dotnet_table::get_strong_name_signature()const {
    return strong_name_signature;
}
image_data_directory dotnet_table::get_code_manager_table()const {
    return code_manager_table;
}
image_data_directory dotnet_table::get_vtable_fixups()const {
    return vtable_fixups;
}
image_data_directory dotnet_table::get_export_address_table_jumps() const {
    return export_address_table_jumps;
}

void dotnet_table::set_major_version(uint16_t major_version) {
    this->major_version = major_version;
}
void dotnet_table::set_minor_version(uint16_t minor_version) {
    this->minor_version = minor_version;
}

void dotnet_table::set_flags(uint32_t flags) {
    this->flags = flags;
}
void dotnet_table::set_entry_point(uint32_t entry_point) {
    this->entry_point_rva = entry_point;
}

void dotnet_table::set_meta_data(image_data_directory meta_data) {
    this->meta_data = meta_data;
}
void dotnet_table::set_resources(image_data_directory resources) {
    this->resources = resources;
}
void dotnet_table::set_strong_name_signature(image_data_directory strong_name_signature) {
    this->strong_name_signature = strong_name_signature;
}
void dotnet_table::set_code_manager_table(image_data_directory code_manager_table) {
    this->code_manager_table = code_manager_table;
}
void dotnet_table::set_vtable_fixups(image_data_directory vtable_fixups) {
    this->vtable_fixups = vtable_fixups;
}
void dotnet_table::set_export_address_table_jumps(image_data_directory export_address_table_jumps) {
    this->export_address_table_jumps = export_address_table_jumps;
}


bool get_dotnet_table(const pe_image &image,dotnet_table& dotnet) {

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
    uint32_t virtual_size    = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR);

    if (virtual_address) {
        pe_section * dotnet_section = image.get_section_by_rva(virtual_address);

        if (dotnet_section) {
            image_cor20_header* dotnet_header =
                (image_cor20_header*)&dotnet_section->get_section_data().data()[virtual_address - dotnet_section->get_virtual_address()];

            dotnet.set_major_version(dotnet_header->major_runtime_version);
            dotnet.set_minor_version(dotnet_header->minor_runtime_version);

            dotnet.set_flags(dotnet_header->flags);
            dotnet.set_entry_point(dotnet_header->entry_point_rva);

            dotnet.set_meta_data(dotnet_header->meta_data);
            dotnet.set_resources(dotnet_header->resources);
            dotnet.set_strong_name_signature(dotnet_header->strong_name_signature);
            dotnet.set_code_manager_table(dotnet_header->code_manager_table);
            dotnet.set_vtable_fixups(dotnet_header->vtable_fixups);
            dotnet.set_export_address_table_jumps(dotnet_header->export_address_table_jumps);
            return true;
        }
    }

    return false;
}
