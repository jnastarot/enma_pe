#pragma once


class pe_dos_stub {
    std::vector<BYTE> dos_stub;
public:
    pe_dos_stub::pe_dos_stub();
    pe_dos_stub::~pe_dos_stub();
public:
    void pe_dos_stub::set_dos_stub(std::vector<BYTE>& stub);

public:
    std::vector<BYTE> pe_dos_stub::get_dos_stub() const;
};

class pe_rich_data {
    DWORD number;
    DWORD version;
    DWORD times;
public:
    pe_rich_data::pe_rich_data();
    pe_rich_data::pe_rich_data(DWORD number, DWORD version, DWORD times);
    pe_rich_data::~pe_rich_data();
public:
    void pe_rich_data::set_number(DWORD number);
    void pe_rich_data::set_version(DWORD version);
    void pe_rich_data::set_times(DWORD times);
public:
    DWORD pe_rich_data::get_number() const;
    DWORD pe_rich_data::get_version() const;
    DWORD pe_rich_data::get_times() const;
};


bool get_image_dos_stub(_In_ const void * pimage, _Out_ pe_dos_stub& dos_stub);
bool get_image_rich_data(_In_ const void * pimage, _Out_ std::vector<pe_rich_data>& rich_data);

bool has_image_rich_data(_In_ const void * pimage ,
    _Out_opt_ unsigned int * rich_data_offset = 0, _Out_opt_ unsigned int * rich_data_size = 0, _Out_opt_ DWORD * rich_xor_key = 0);
bool checksum_rich(_In_ const void * pimage, _Out_opt_ DWORD * correct_rich_xor_key = 0);