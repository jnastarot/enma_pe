#include "stdafx.h"
#include "pe_dotnet.h"



dotnet_table::dotnet_table() {

}
dotnet_table::~dotnet_table() {

}

WORD dotnet_table::get_major_version() const {
    return major_version;
}
WORD dotnet_table::get_minor_version() const {
    return minor_version;
}

DWORD dotnet_table::get_flags()const {
    return flags;
}
DWORD dotnet_table::get_entry_point()const {
    return entry_point_rva;
}

IMAGE_DATA_DIRECTORY dotnet_table::get_meta_data() const {
    return meta_data;
}
IMAGE_DATA_DIRECTORY dotnet_table::get_resources() const {
    return resources;
}
IMAGE_DATA_DIRECTORY dotnet_table::get_strong_name_signature()const {
    return strong_name_signature;
}
IMAGE_DATA_DIRECTORY dotnet_table::get_code_manager_table()const {
    return code_manager_table;
}
IMAGE_DATA_DIRECTORY dotnet_table::get_vtable_fixups()const {
    return vtable_fixups;
}
IMAGE_DATA_DIRECTORY dotnet_table::get_export_address_table_jumps() const {
    return export_address_table_jumps;
}
IMAGE_DATA_DIRECTORY dotnet_table::get_managed_native_header() const {
    return managed_native_header;
}

void dotnet_table::set_major_version(WORD major_version) {
    this->major_version = major_version;
}
void dotnet_table::set_minor_version(WORD minor_version) {
    this->minor_version = minor_version;
}

void dotnet_table::set_flags(DWORD flags) {
    this->flags = flags;
}
void dotnet_table::set_entry_point(DWORD entry_point) {
    this->entry_point_rva = entry_point;
}

void dotnet_table::set_meta_data(IMAGE_DATA_DIRECTORY meta_data) {
    this->meta_data = meta_data;
}
void dotnet_table::set_resources(IMAGE_DATA_DIRECTORY resources) {
    this->resources = resources;
}
void dotnet_table::set_strong_name_signature(IMAGE_DATA_DIRECTORY strong_name_signature) {
    this->strong_name_signature = strong_name_signature;
}
void dotnet_table::set_code_manager_table(IMAGE_DATA_DIRECTORY code_manager_table) {
    this->code_manager_table = code_manager_table;
}
void dotnet_table::set_vtable_fixups(IMAGE_DATA_DIRECTORY vtable_fixups) {
    this->vtable_fixups = vtable_fixups;
}
void dotnet_table::set_export_address_table_jumps(IMAGE_DATA_DIRECTORY export_address_table_jumps) {
    this->export_address_table_jumps = export_address_table_jumps;
}
void dotnet_table::set_managed_native_header(IMAGE_DATA_DIRECTORY managed_native_header) {
    this->managed_native_header = managed_native_header;
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
    dotnet.set_managed_native_header({ 0,0 });

    DWORD virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR);
    DWORD virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR);

    if (virtual_address) {
        pe_section * dotnet_section = image.get_section_by_rva(virtual_address);

        if (dotnet_section) {
            PIMAGE_COR20_HEADER dotnet_header = 
                (PIMAGE_COR20_HEADER)&dotnet_section->get_section_data().data()[(DWORD)(virtual_address - dotnet_section->get_virtual_address())];

            dotnet.set_major_version(dotnet_header->MajorRuntimeVersion);
            dotnet.set_minor_version(dotnet_header->MinorRuntimeVersion);

            dotnet.set_flags(dotnet_header->Flags);
            dotnet.set_entry_point(dotnet_header->EntryPointRVA);

            dotnet.set_meta_data(dotnet_header->MetaData);
            dotnet.set_resources(dotnet_header->Resources);
            dotnet.set_strong_name_signature(dotnet_header->StrongNameSignature);
            dotnet.set_code_manager_table(dotnet_header->CodeManagerTable);
            dotnet.set_vtable_fixups(dotnet_header->VTableFixups);
            dotnet.set_export_address_table_jumps(dotnet_header->ExportAddressTableJumps);
            dotnet.set_managed_native_header(dotnet_header->ManagedNativeHeader);
            return true;
        }
    }

    return false;
}
