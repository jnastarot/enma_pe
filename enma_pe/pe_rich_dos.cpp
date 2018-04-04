#include "stdafx.h"
#include "pe_rich_dos.h"


pe_dos_stub::pe_dos_stub() {}
pe_dos_stub::~pe_dos_stub() {}

void pe_dos_stub::set_dos_stub(std::vector<uint8_t>& stub) {
    this->dos_stub = stub;
}
std::vector<uint8_t> pe_dos_stub::get_dos_stub() const {
    return this->dos_stub;
}

pe_rich_data::pe_rich_data() :
    type((e_rich_type)0), compiler_build(0), count(0){
}
pe_rich_data::pe_rich_data(const pe_rich_data& data) {
    this->operator=(data);
}

pe_rich_data::pe_rich_data(e_rich_type type, uint16_t compiler_build, uint32_t count) :
    type(type), compiler_build(compiler_build), count(count) {

}

pe_rich_data::~pe_rich_data(){ }


pe_rich_data& pe_rich_data::operator=(const pe_rich_data& data) {
    
    this->type = data.type;
    this->compiler_build = data.compiler_build;
    this->count = data.count;

    return *this;
}

void pe_rich_data::set_type(e_rich_type type) {
    this->type = type;
}
void pe_rich_data::set_compiler_build(uint16_t compiler_build) {
    this->compiler_build = compiler_build;
}
void pe_rich_data::set_count(uint32_t count) {
    this->count = count;
}

e_rich_type pe_rich_data::get_type() const {
    return type;
}
uint16_t pe_rich_data::get_compiler_build() const {
    return compiler_build;
}
uint32_t pe_rich_data::get_count() const {
    return count;
}


struct rich_data_item {
    uint16_t compiler_build;
    uint16_t type;
    uint32_t count;
};

bool has_image_rich_data(const uint8_t * pimage,uint32_t * rich_data_offset,
    uint32_t * rich_data_size, uint32_t * rich_xor_key) {

    image_dos_header* dos_header = (image_dos_header*)pimage;
    uint32_t rich_offset = 0;

    for (size_t rich_end = sizeof(image_dos_header); rich_end < (dos_header->e_lfanew - 8); rich_end += 4) {//check for rich data

        if (*(uint32_t*)(&pimage[rich_end]) == 0x68636952) { //'Rich'
            uint32_t xor_key = *(uint32_t*)(&pimage[rich_end + 4]);

            for (size_t rich_start = sizeof(image_dos_header);
                rich_start < (dos_header->e_lfanew - 8); rich_start += 4) {//get rich offset

                if ((*(uint32_t*)(&pimage[rich_start]) ^ xor_key) == 0x536E6144) { //'DanS'
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

bool get_image_dos_stub(const uint8_t * pimage,pe_dos_stub& dos_stub) {
    image_dos_header* dos_header = (image_dos_header*)pimage;
    uint32_t dos_stub_size = 0;
    uint32_t rich_offset = 0;

    if (has_image_rich_data(pimage, &rich_offset)) {
       dos_stub_size = rich_offset;
    }else {
       dos_stub_size = dos_header->e_lfanew;
    }

    if (dos_stub_size) {
        std::vector<uint8_t> dos_stub_holder;
        dos_stub_holder.resize(dos_stub_size);
        memcpy(dos_stub_holder.data(), (uint8_t*)pimage, dos_stub_size);
        dos_stub.set_dos_stub(dos_stub_holder);
        return true;
    }

    return false;
}



bool get_image_rich_data(const uint8_t * pimage, std::vector<pe_rich_data>& rich_data) {
    rich_data.clear();
    uint32_t rich_offset = 0;
    uint32_t rich_size = 0;
    uint32_t rich_xor_key = 0;

    if (has_image_rich_data(pimage, &rich_offset,&rich_size,&rich_xor_key)) {

        rich_data_item* rich_items = (rich_data_item*)(&pimage[rich_offset]);

        for (size_t item_idx = 2; item_idx < (rich_size / sizeof(rich_data_item)); item_idx++) {
            rich_data_item rich_item = rich_items[item_idx];
            *(uint32_t*)(&rich_item) ^= rich_xor_key;
            *(uint32_t*)((uint8_t*)&rich_item + 4) ^= rich_xor_key;

            rich_data.push_back(
                pe_rich_data((e_rich_type)rich_item.type, rich_item.compiler_build, rich_item.count)
            );

        }

        return true;

    }

    return false;
}

#define GET_RICH_HASH(x,i) (((x) << (i)) | ((x) >> (32 - (i))))
bool checksum_rich(const uint8_t * pimage,uint32_t * correct_rich_xor_key) {

    uint32_t rich_offset = 0;
    uint32_t rich_size = 0;
    uint32_t rich_xor_key = 0;

    if (has_image_rich_data(pimage, &rich_offset,&rich_size,&rich_xor_key)) {
        rich_data_item* rich_items = (rich_data_item*)(&pimage[rich_offset]);

        uint32_t calc_hash = rich_offset;

        for (uint32_t i = 0; i < rich_offset; i++) { //dos header + stub
            if (i >= 0x3C && i < 0x40) { continue; }//skip e_lfanew

            calc_hash += GET_RICH_HASH(uint32_t(pimage[i]), i);
        }

        for (size_t item_idx = 1; item_idx < (rich_size / sizeof(rich_data_item)); item_idx++) {
            rich_data_item rich_item = rich_items[item_idx];
            *(uint32_t*)(&rich_item) ^= rich_xor_key;
            *(uint32_t*)((uint8_t*)&rich_item + 4) ^= rich_xor_key;

            calc_hash += GET_RICH_HASH(*(uint32_t*)(&rich_item), *(uint32_t*)((uint8_t*)&rich_item + 4));
        }

        if (correct_rich_xor_key) {
            *correct_rich_xor_key = calc_hash;
        }

        if (rich_xor_key == calc_hash) {
            return true;
        }
    }
    return false;
}