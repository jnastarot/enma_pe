#include "stdafx.h"
#include "pe_dotnet.h"



dotnet_table::dotnet_table() {

}
dotnet_table::~dotnet_table() {

}

WORD dotnet_table::get_major_version() {
    return major_version;
}
WORD dotnet_table::get_minor_version() {
    return minor_version;
}

DWORD dotnet_table::get_flags() {
    return flags;
}
DWORD dotnet_table::get_entry_point() {
    return entry_point_rva;
}

IMAGE_DATA_DIRECTORY dotnet_table::get_meta_data() {
    return meta_data;
}
IMAGE_DATA_DIRECTORY dotnet_table::get_resources() {
    return resources;
}
IMAGE_DATA_DIRECTORY dotnet_table::get_strong_name_signature() {
    return strong_name_signature;
}
IMAGE_DATA_DIRECTORY dotnet_table::get_code_manager_table() {
    return code_manager_table;
}
IMAGE_DATA_DIRECTORY dotnet_table::get_vtable_fixups() {
    return vtable_fixups;
}
IMAGE_DATA_DIRECTORY dotnet_table::get_export_address_table_jumps() {
    return export_address_table_jumps;
}
IMAGE_DATA_DIRECTORY dotnet_table::get_managed_native_header() {
    return managed_native_header;
}

void dotnet_table::get_major_version(WORD major_version) {
    this->major_version = major_version;
}
void dotnet_table::get_minor_version(WORD minor_version) {
    this->minor_version = minor_version;
}

void dotnet_table::get_flags(DWORD flags) {
    this->flags = flags;
}
void dotnet_table::get_entry_point(DWORD entry_point) {
    this->entry_point_rva = entry_point;
}

void dotnet_table::get_meta_data(IMAGE_DATA_DIRECTORY meta_data) {
    this->meta_data = meta_data;
}
void dotnet_table::get_resources(IMAGE_DATA_DIRECTORY resources) {
    this->resources = resources;
}
void dotnet_table::get_strong_name_signature(IMAGE_DATA_DIRECTORY strong_name_signature) {
    this->strong_name_signature = strong_name_signature;
}
void dotnet_table::get_code_manager_table(IMAGE_DATA_DIRECTORY code_manager_table) {
    this->code_manager_table = code_manager_table;
}
void dotnet_table::get_vtable_fixups(IMAGE_DATA_DIRECTORY vtable_fixups) {
    this->vtable_fixups = vtable_fixups;
}
void dotnet_table::get_export_address_table_jumps(IMAGE_DATA_DIRECTORY export_address_table_jumps) {
    this->export_address_table_jumps = export_address_table_jumps;
}
void dotnet_table::get_managed_native_header(IMAGE_DATA_DIRECTORY managed_native_header) {
    this->managed_native_header = managed_native_header;
}




