#include "stdafx.h"

using namespace enma;

constexpr static unsigned char th_dos_stub[64] = {
    0x0E, 0x1F, 0xBA, 0x0E, 0x00, 0xB4, 0x09, 0xCD, 0x21, 0xB8, 0x01, 0x4C, 0xCD, 0x21, 0x54, 0x68,
    0x69, 0x73, 0x20, 0x70, 0x72, 0x6F, 0x67, 0x72, 0x61, 0x6D, 0x20, 0x63, 0x61, 0x6E, 0x6E, 0x6F,
    0x74, 0x20, 0x62, 0x65, 0x20, 0x72, 0x75, 0x6E, 0x20, 0x69, 0x6E, 0x20, 0x44, 0x4F, 0x53, 0x20,
    0x6D, 0x6F, 0x64, 0x65, 0x2E, 0x0D, 0x0D, 0x0A, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

pe_image::pe_image(const pe_image& image) {
    this->operator=(image);
}

pe_image::pe_image(bool _pe32, bool init_dos_thunk) {
    clear_image();

    _headers_data.resize(sizeof(image_dos_header) + (init_dos_thunk ? sizeof(th_dos_stub) : 0));
    memset(_headers_data.data(), 0, _headers_data.size());

    if (init_dos_thunk) {
        memcpy(&_headers_data[sizeof(image_dos_header)], th_dos_stub, sizeof(th_dos_stub));
    }

    image_dos_header * dos_header = (image_dos_header*)_headers_data.data();
    dos_header->e_magic = IMAGE_DOS_SIGNATURE;
    dos_header->e_cblp = 0x90;
    dos_header->e_cp = 0x03;
    dos_header->e_cparhdr = 0x04;
    dos_header->e_maxalloc = 0xFFFF;
    dos_header->e_sp = 0xB8;
    dos_header->e_lfanew = sizeof(image_dos_header) + (init_dos_thunk ? sizeof(th_dos_stub) : 0);


    if (_pe32) {
        
        _machine = IMAGE_FILE_MACHINE_I386;
        _characteristics = IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_32BIT_MACHINE;
        _magic = IMAGE_NT_OPTIONAL_HDR32_MAGIC;
        _image_base = 0x400000;
    }
    else {

        _machine = IMAGE_FILE_MACHINE_AMD64;
        _characteristics = IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_LARGE_ADDRESS_AWARE;
        _magic = IMAGE_NT_OPTIONAL_HDR64_MAGIC;
        _image_base = 0x0000000140000000;
    }

    {
        _image_size = 0x1000;
        _headers_size = 0x200;
        _sub_system = IMAGE_SUBSYSTEM_WINDOWS_GUI;
        _characteristics_dll =
            IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE | 
            IMAGE_DLLCHARACTERISTICS_NX_COMPAT | 
            IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE;
    }

    {
        _section_align = 0x1000;
        _file_align = 0x200;
    }

    {
        _os_ver_major = 5;
        _os_ver_minor = 1;

    }

    {
        _subsystem_ver_major = 5;
        _subsystem_ver_minor = 1;
    }

    _stack_reserve_size = 0x00100000;
    _stack_commit_size  = 0x1000;
    _heap_reserve_size  = 0x00100000;
    _heap_commit_size   = 0x1000;

    _image_status = pe_image_status::pe_image_status_ok;
}

pe_image::pe_image(const uint8_t* raw_image, uint32_t size) {
    clear_image();
    init_from_file(raw_image, size);
}

pe_image::pe_image(const std::string& file_path) {
    clear_image();

    FILE* hfile = 0;
    fopen_s(&hfile, file_path.c_str(), "rb");

    if (hfile != nullptr) {
        fseek(hfile, 0, SEEK_END);
        size_t file_size = ftell(hfile);
        fseek(hfile, 0, SEEK_SET);

        uint8_t * file_buffer = new uint8_t[file_size];


        if (fread(file_buffer, file_size, 1, hfile)) {

            init_from_file(file_buffer, file_size);
        }
        else {
            _image_status = pe_image_status_bad_format;
        }

        delete[] file_buffer;

        fclose(hfile);
    }
    else {
        _image_status = pe_image_status_unknown;
    }
}

pe_image::pe_image(const std::wstring& file_path) {
    clear_image();

    FILE* hfile = 0;
    _wfopen_s(&hfile, file_path.c_str(), L"rb");


    if (hfile != nullptr) {
        fseek(hfile, 0, SEEK_END);
        size_t file_size = ftell(hfile);
        fseek(hfile, 0, SEEK_SET);

        uint8_t* file_buffer = new uint8_t[file_size];


        if (fread(file_buffer, file_size, 1, hfile)) {

            init_from_file(file_buffer, file_size);
        }
        else {
            _image_status = pe_image_status_bad_format;
        }

        delete[] file_buffer;

        fclose(hfile);
    }
    else {
        _image_status = pe_image_status_unknown;
    }
}

pe_image::~pe_image() {
    clear_image();
}

pe_image& pe_image::operator=(const pe_image& image) {
    clear_image();

    _headers_data = image._headers_data;
    _rich_header = image._rich_header;
    _image_status = image._image_status;
    _machine = image._machine;
    _timestamp = image._timestamp;
    _pointer_to_symbol_table = image._pointer_to_symbol_table;
    _number_of_symbols = image._number_of_symbols;
    _size_of_optional_header = image._size_of_optional_header;
    _characteristics = image._characteristics;
    _magic = image._magic;
    _major_linker = image._major_linker;
    _minor_linker = image._minor_linker;
    _size_of_code = image._size_of_code;
    _size_of_init_data = image._size_of_init_data;
    _size_of_uninit_data = image._size_of_uninit_data;
    _entry_point = image._entry_point;
    _base_of_code = image._base_of_code;
    _base_of_data = image._base_of_data;
    _image_base = image._image_base;
    _section_align = image._section_align;
    _file_align = image._file_align;
    _os_ver_major = image._os_ver_major;
    _os_ver_minor = image._os_ver_minor;
    _image_ver_major = image._image_ver_major;
    _image_ver_minor = image._image_ver_minor;
    _subsystem_ver_major = image._subsystem_ver_major;
    _subsystem_ver_minor = image._subsystem_ver_minor;
    _win32_version_value = image._win32_version_value;
    _image_size = image._image_size;
    _headers_size = image._headers_size;
    _checksum = image._checksum;
    _sub_system = image._sub_system;
    _characteristics_dll = image._characteristics_dll;
    _stack_reserve_size = image._stack_reserve_size;
    _stack_commit_size = image._stack_commit_size;
    _heap_reserve_size = image._heap_reserve_size;
    _heap_commit_size = image._heap_commit_size;
    _loader_flags = image._loader_flags;

    for (size_t idx = 0; idx < 16; idx++) {
        
        _directories[idx].virtual_address = image._directories[idx].virtual_address;
        _directories[idx].size = image._directories[idx].size;
    }

    for (size_t idx = 0; idx < image.get_sections_number(); idx++) {

        add_section(*image._sections[idx]);
    }

    _overlay_data = image._overlay_data;

    return *this;
}

template <typename image_format>
bool init_nt_header(pe_image& image,void * nt_header,uint32_t& sections_offset,uint32_t& number_of_sections) {

    typename image_format::image_nt_headers * header = (typename image_format::image_nt_headers *)nt_header;

    if (header->optional_header.magic == image_format::image_magic) {

        image.set_machine(header->file_header.machine)
            .set_timestamp(header->file_header.time_date_stamp)
            .set_pointer_to_symbol_table(header->file_header.pointer_to_symbol_table)
            .set_number_of_symbols(header->file_header.number_of_symbols)
            .set_characteristics(header->file_header.characteristics)
            .set_magic(header->optional_header.magic)
            .set_major_linker(header->optional_header.major_linker_version)
            .set_minor_linker(header->optional_header.minor_linker_version)
            .set_size_of_code(header->optional_header.size_of_code)
            .set_size_of_init_data(header->optional_header.size_of_initialized_data)
            .set_size_of_uninit_data(header->optional_header.size_of_uninitialized_data)
            .set_entry_point(header->optional_header.address_of_entry_point)
            .set_base_of_code(header->optional_header.base_of_code)
            .set_image_base(header->optional_header.image_base)
            .set_section_align(header->optional_header.section_alignment)
            .set_file_align(header->optional_header.file_alignment)
            .set_os_ver_major(header->optional_header.major_operating_system_version)
            .set_os_ver_minor(header->optional_header.minor_operating_system_version)
            .set_image_ver_major(header->optional_header.major_image_version)
            .set_image_ver_minor(header->optional_header.minor_image_version)
            .set_subsystem_ver_major(header->optional_header.major_subsystem_version)
            .set_subsystem_ver_minor(header->optional_header.minor_subsystem_version)
            .set_win32_version_value(header->optional_header.win32_version_value)
            .set_image_size(header->optional_header.size_of_image)
            .set_headers_size(header->optional_header.size_of_headers)
            .set_checksum(header->optional_header.checksum)
            .set_sub_system(header->optional_header.subsystem)
            .set_characteristics_dll(header->optional_header.dll_characteristics)
            .set_stack_reserve_size(header->optional_header.size_of_stack_reserve)
            .set_stack_commit_size(header->optional_header.size_of_stack_commit)
            .set_heap_reserve_size(header->optional_header.size_of_heap_reserve)
            .set_heap_commit_size(header->optional_header.size_of_heap_commit)
            .set_loader_flags(header->optional_header.loader_flags);

        for (uint32_t i = 0; i < 16; i++) {
            image.set_directory_virtual_address(i, header->optional_header.data_directory[i].virtual_address)
                .set_directory_virtual_size(i, header->optional_header.data_directory[i].size);
        }

        sections_offset += header->file_header.size_of_optional_header;
        number_of_sections = header->file_header.number_of_sections;
        return true;
    }

    return false;
}

void pe_image::init_from_file(const uint8_t * image, size_t size) {

    if (size < sizeof(image_dos_header)) {_image_status = pe_image_status_bad_format; return;};

    image_dos_header dos_header;

    memcpy(&dos_header, image, sizeof(image_dos_header));

    if (dos_header.e_magic == IMAGE_DOS_SIGNATURE || //check MZ sign
        dos_header.e_magic == 'MZ') {  //check ZM sign

        if (size <= ((size_t)dos_header.e_lfanew + INT32_SIZE + sizeof(image_file_header))) {
            _image_status = pe_image_status_bad_format; return; 
        };

        if (*(uint32_t*)(&image[dos_header.e_lfanew]) == IMAGE_NT_SIGNATURE) { //check PE00 sign
            uint32_t section_offset = dos_header.e_lfanew + INT32_SIZE + (uint32_t)sizeof(image_file_header);
            uint32_t number_of_sections = 0;
            
            if (size < section_offset + sizeof(image_nt_headers32)) { _image_status = pe_image_status_bad_format; return; };

            if (init_nt_header<pe_image_32>(*this, (void*)&image[dos_header.e_lfanew], section_offset, number_of_sections)) {
                set_base_of_data(pimage_nt_headers32(&image[dos_header.e_lfanew])->optional_header.base_of_data);
            }
            else {
                if (size < section_offset + sizeof(image_nt_headers64)) { _image_status = pe_image_status_bad_format; return; };

                if (!init_nt_header<pe_image_64>(*this, (void*)&image[dos_header.e_lfanew], section_offset, number_of_sections)) {
                    _image_status = pe_image_status_bad_format;
                    return;
                }

                set_base_of_data(0);
            }

            _headers_data.resize(min(_headers_size, size));
            memset(_headers_data.data(), 0, _headers_data.size());

            memcpy(_headers_data.data(), image, min(_headers_size, size));

            get_image_rich_header(_headers_data, _rich_header);

            {
                size_t image_top_size = 0;
                for (size_t i = 0; i < number_of_sections; i++) {
                    image_section_header* section_image = (image_section_header*)(&image[section_offset]);

                    if (size < section_offset + sizeof(image_section_header) ||
                        size < (size_t)ALIGN_DOWN(section_image->pointer_to_raw_data + section_image->size_of_raw_data, get_file_align())
                        ) {

                        _image_status = pe_image_status_bad_format; return;
                    };

                    std::vector<uint8_t> section_data;

                    if (section_image->pointer_to_raw_data < size) {
                        
                        section_data.resize(
                            (size_t)ALIGN_DOWN(min(section_image->size_of_raw_data, (uint32_t)(size - section_image->pointer_to_raw_data)), get_file_align())
                        );
                    }
                    else {
                        _image_status = pe_image_status_bad_format; return;
                    }

                    memcpy(section_data.data(), &image[section_image->pointer_to_raw_data], section_data.size());

                    add_section(pe_section(*section_image, section_data));
                    section_offset += (uint16_t)sizeof(image_section_header);

                    size_t current_size = ALIGN_DOWN( (uint32_t)(section_image->pointer_to_raw_data + section_data.size()), get_file_align());

                    if (image_top_size < current_size) {
                        image_top_size = current_size;
                    }
                }

                if (image_top_size < size) {
                    _overlay_data.resize(size - image_top_size);
                    memcpy(_overlay_data.data(), &image[image_top_size], size - image_top_size);
                }
            }

            _image_status = pe_image_status_ok;
            return;
        }

    }

   _image_status = pe_image_status_bad_format;
}

void pe_image::clear_image() {

    _image_status = pe_image_status_unknown;
    
    _machine = 0;
    _timestamp = 0;
    _pointer_to_symbol_table = 0;
    _number_of_symbols = 0;
    _size_of_optional_header = 0;
    _characteristics = 0;
    _magic = 0;
    _major_linker = 0;
    _minor_linker = 0;
    _size_of_code = 0;
    _size_of_init_data = 0;
    _size_of_uninit_data = 0;
    _entry_point = 0;
    _base_of_code = 0;
    _base_of_data = 0;
    _image_base = 0;
    _section_align = 0;
    _file_align = 0;
    _os_ver_major = 0;
    _os_ver_minor = 0;
    _image_ver_major = 0;
    _image_ver_minor = 0;
    _subsystem_ver_major = 0;
    _subsystem_ver_minor = 0;
    _win32_version_value = 0;
    _image_size = 0;
    _headers_size = 0;
    _checksum = 0;
    _sub_system = 0;
    _characteristics_dll = 0;
    _stack_reserve_size = 0;
    _stack_commit_size = 0;
    _heap_reserve_size = 0;
    _heap_commit_size = 0;
    _loader_flags = 0;

    for (size_t i = 0; i < 16; i++) {
        _directories[i].virtual_address = 0;
        _directories[i].size = 0;
    }

    while (_sections.size()) { 
    
        delete _sections[_sections.size() - 1]; 
        _sections.pop_back(); 
    }

    _rich_header.get_entries().clear();
    _headers_data.clear();
}

pe_image_status load_virtual_pe_image(const uint8_t* hmodule, pe_image& image, bool check_signs,
    bool(*readmem)(void* dst, const uint8_t* src, size_t size)) {

    image.clear_image();

    if (!readmem) {
        image.set_image_status(pe_image_status_unknown);

        return image.get_image_status();
    }


    image.set_image_status(pe_image_status_unknown);

    image_dos_header dos_header;

    readmem(&dos_header, hmodule, sizeof(image_dos_header));

    if ( !check_signs || (dos_header.e_magic == IMAGE_DOS_SIGNATURE || //check MZ sign
        dos_header.e_magic == 'MZ')) {  //check ZM sign

        image_nt_headers32 nt32_header;
        readmem(&nt32_header, &hmodule[dos_header.e_lfanew], sizeof(image_nt_headers32));

        if ( !check_signs  || nt32_header.signature == IMAGE_NT_SIGNATURE) { //check PE00 sign

            uint32_t section_offset = dos_header.e_lfanew + INT32_SIZE + (uint32_t)sizeof(image_file_header);
            uint32_t number_of_sections = 0;

            if (init_nt_header<pe_image_32>(image, (void*)& nt32_header, section_offset, number_of_sections)) {
                image.set_base_of_data(nt32_header.optional_header.base_of_data);
            }
            else {

                image_nt_headers64 nt64_header;
                readmem(&nt64_header, &hmodule[dos_header.e_lfanew], sizeof(image_nt_headers64));

                if (!init_nt_header<pe_image_64>(image, (void*)&nt64_header, section_offset, number_of_sections)) {

                    image.set_image_status(pe_image_status_bad_format);

                    return image.get_image_status();
                }

                image.set_base_of_data(0);
            }

            image.get_headers_data().resize(image.get_headers_size());
            memset(image.get_headers_data().data(), 0, image.get_headers_size());
            readmem(image.get_headers_data().data(), hmodule, image.get_headers_size());

            get_image_rich_header(image.get_headers_data(), image.get_rich_header());

            {
                for (size_t i = 0; i < number_of_sections; i++) {

                    image_section_header section_image;
                    readmem(&section_image, &hmodule[section_offset], sizeof(image_section_header));

                    std::vector<uint8_t> section_data;

                    section_data.resize(ALIGN_UP((size_t)section_image.virtual_size, (size_t)image.get_section_align()));

                    readmem(section_data.data(), &hmodule[section_image.virtual_address], section_data.size());

                    image.add_section(pe_section(section_image, section_data));
                    section_offset += (uint16_t)sizeof(image_section_header);
                }
            }

            image.set_image_status(pe_image_status_ok);
        }

    }


    return image.get_image_status();
}