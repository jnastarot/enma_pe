#include "stdafx.h"
#include "pe_rich_dos.h"


pe_dos_stub::pe_dos_stub() {}
pe_dos_stub::~pe_dos_stub() {}

void pe_dos_stub::set_dos_stub(std::vector<BYTE>& stub) {
    this->dos_stub = stub;
}
std::vector<BYTE> pe_dos_stub::get_dos_stub() const {
    return this->dos_stub;
}

pe_rich_data::pe_rich_data() :
    number(0), version(0), times(0){
}

pe_rich_data::pe_rich_data(DWORD number, DWORD version, DWORD times) :
    number(number), version(version), times(times) {

}

pe_rich_data::~pe_rich_data(){ }

void pe_rich_data::set_number(DWORD number) {
    this->number = number;
}
void pe_rich_data::set_version(DWORD version) {
    this->version = version;
}
void pe_rich_data::set_times(DWORD times) {
    this->times = times;
}

DWORD pe_rich_data::get_number() const {
    return number;
}
DWORD pe_rich_data::get_version() const {
    return version;
}
DWORD pe_rich_data::get_times() const {
    return times;
}


struct rich_data_item {
    WORD number;
    WORD version;
    DWORD times;
};

bool has_rich_data(const void * pimage,unsigned int * rich_data_offset,
    unsigned int * rich_data_size, DWORD * rich_xor_key) {

    PIMAGE_DOS_HEADER dos_header = PIMAGE_DOS_HEADER(pimage);
    unsigned int rich_offset = 0;

    for (int rich_end = sizeof(IMAGE_DOS_HEADER); rich_end < (dos_header->e_lfanew - 8); rich_end += 4) {//check for rich data

        if (*(DWORD*)((BYTE*)pimage + (rich_end)) == 0x68636952) { //'Rich'
            DWORD xor_key = *(DWORD*)((BYTE*)pimage + (rich_end + 4));

            for (int rich_start = sizeof(IMAGE_DOS_HEADER);
                rich_start < (dos_header->e_lfanew - 8); rich_start += 4) {//get rich offset

                if ((*(DWORD*)((BYTE*)pimage + rich_start) ^ xor_key) == 0x536E6144) { //'DanS'
                    rich_offset = rich_start;

                    if (rich_data_offset) {
                        *rich_data_offset = rich_start;
                    }
                    if (rich_data_size) {
                        *rich_data_size = rich_end-rich_start;
                    }
                    if (rich_xor_key) {
                        *rich_xor_key = xor_key;
                    }
                    return true;
                }
            }
        }
    }

    return false;
}

bool get_image_dos_stub(const void * pimage,pe_dos_stub& dos_stub) {
    PIMAGE_DOS_HEADER dos_header = PIMAGE_DOS_HEADER(pimage);
    unsigned int dos_stub_size = 0;
    unsigned int rich_offset = 0;

    if (has_rich_data(pimage, &rich_offset)) {
       dos_stub_size = rich_offset - sizeof(IMAGE_DOS_HEADER);
    }else {
       dos_stub_size = dos_header->e_lfanew - sizeof(IMAGE_DOS_HEADER);
    }

    if (dos_stub_size) {
        std::vector<BYTE> dos_stub_holder;
        dos_stub_holder.resize(dos_stub_size);
        memcpy(dos_stub_holder.data(), (BYTE*)pimage + sizeof(IMAGE_DOS_HEADER), dos_stub_size);
        dos_stub.set_dos_stub(dos_stub_holder);
        return true;
    }

    return false;
}



bool get_image_rich_data(const void * pimage, std::vector<pe_rich_data>& rich_data) {
    rich_data.clear();
    unsigned int rich_offset = 0;
    unsigned int rich_size = 0;
    DWORD rich_xor_key = 0;

    if (has_rich_data(pimage, &rich_offset,&rich_size,&rich_xor_key)) {

        rich_data_item* rich_items = (rich_data_item*)((BYTE*)pimage + rich_offset);

        for (int item_idx = 2; item_idx < (rich_size / sizeof(rich_data_item)); item_idx++) {
            rich_data_item rich_item = rich_items[item_idx];
            *(DWORD*)(&rich_item) ^= rich_xor_key;
            *(DWORD*)((BYTE*)&rich_item + 4) ^= rich_xor_key;

            rich_data.push_back(
                pe_rich_data(rich_item.number, rich_item.version, rich_item.times)
            );

        }

        return true;

    }

    return false;
}
