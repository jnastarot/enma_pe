#pragma once


template<
    typename ptr_size_type,
    typename image_nt_headers_type,
    typename image_thunk_data_type,
    typename image_tls_directory_type,
    typename image_load_config_directory_type,
    typename image_dynamic_relocation_type,
    typename image_dynamic_relocation_v2_type,

    uint16_t _image_magic,
    ptr_size_type _ordinal_flag
>
struct image_types{
    typedef ptr_size_type ptr_size; 
    typedef image_nt_headers_type image_nt_headers;
    typedef image_thunk_data_type image_thunk_data;
    typedef image_tls_directory_type image_tls_directory;

    typedef image_load_config_directory_type image_load_config_directory;
    typedef image_dynamic_relocation_type    image_dynamic_relocation;
    typedef image_dynamic_relocation_v2_type image_dynamic_relocation_v2;


    static const uint16_t image_magic  = _image_magic;
    static const ptr_size ordinal_flag = _ordinal_flag;
};


typedef image_types<
    uint32_t,
    image_nt_headers32,
    image_thunk_data32,
    image_tls_directory32,
    image_load_config_directory32,
    image_dynamic_relocation32,
    image_dynamic_relocation32_v2,

    IMAGE_NT_OPTIONAL_HDR32_MAGIC,
    IMAGE_ORDINAL_FLAG32> image_32;

typedef image_types<
    uint64_t,
    image_nt_headers64,
    image_thunk_data64,
    image_tls_directory64,
    image_load_config_directory64,
    image_dynamic_relocation64,
    image_dynamic_relocation64_v2,

    IMAGE_NT_OPTIONAL_HDR64_MAGIC,
    IMAGE_ORDINAL_FLAG64> image_64;