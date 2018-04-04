#pragma once

class dotnet_table {
    uint16_t   major_version;
    uint16_t   minor_version;
    uint32_t   flags;

    union {
        uint32_t   entry_point_token;
        uint32_t   entry_point_rva;
    };

    image_data_directory   meta_data;
    image_data_directory   resources;
    image_data_directory   strong_name_signature;
    image_data_directory   code_manager_table;
    image_data_directory   vtable_fixups;
    image_data_directory   export_address_table_jumps;
public:
    dotnet_table::dotnet_table();
    dotnet_table::~dotnet_table();


public:
    void dotnet_table::set_major_version(uint16_t major_version);
    void dotnet_table::set_minor_version(uint16_t minor_version);

    void dotnet_table::set_flags(uint32_t flags);
    void dotnet_table::set_entry_point(uint32_t entry_point);

    void dotnet_table::set_meta_data(image_data_directory meta_data);
    void dotnet_table::set_resources(image_data_directory resources);
    void dotnet_table::set_strong_name_signature(image_data_directory strong_name_signature);
    void dotnet_table::set_code_manager_table(image_data_directory code_manager_table);
    void dotnet_table::set_vtable_fixups(image_data_directory vtable_fixups);
    void dotnet_table::set_export_address_table_jumps(image_data_directory export_address_table_jumps);
public:
    uint16_t dotnet_table::get_major_version() const;
    uint16_t dotnet_table::get_minor_version() const;

    uint32_t dotnet_table::get_flags() const;
    uint32_t dotnet_table::get_entry_point() const;

    image_data_directory dotnet_table::get_meta_data() const;
    image_data_directory dotnet_table::get_resources() const;
    image_data_directory dotnet_table::get_strong_name_signature() const;
    image_data_directory dotnet_table::get_code_manager_table() const;
    image_data_directory dotnet_table::get_vtable_fixups() const;
    image_data_directory dotnet_table::get_export_address_table_jumps() const;
};

bool get_dotnet_table(_In_ const pe_image &image, _Out_ dotnet_table& dotnet);