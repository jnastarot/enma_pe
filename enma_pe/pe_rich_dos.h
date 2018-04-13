#pragma once

enum e_rich_type {
  rich_type_unknown  = 0,
  rich_type_unmarked = 1,

  //[ C ]
  rich_type_C_0x0104 = 260,//VS2015 - VS2017
  rich_type_C_0x00E0 = 224,//VS2013
  rich_type_C_0x00CE = 206,//VS2012
  rich_type_C_0x00AA = 170,//VS2010
  rich_type_C_0x0085 = 133,
  rich_type_C_0x0083 = 131,//VS2008
  rich_type_C_0x006F = 111,
  rich_type_C_0x006D = 109,//VS2005
  rich_type_C_0x0061 = 97,
  rich_type_C_0x005F = 95, //VS2003
  rich_type_C_0x004E = 78,
  rich_type_C_0x004C = 76,
  rich_type_C_0x0039 = 57,
  rich_type_C_0x0032 = 50,
  rich_type_C_0x0030 = 48,
  rich_type_C_0x0023 = 35,
  rich_type_C_0x001C = 28, //VS2002
  rich_type_C_0x0015 = 21, //MSVC++ 6.0 SP5
  rich_type_C_0x000A = 10, //MSVS6.0
  rich_type_C_0x0008 = 8,
  //[ASM]
  rich_type_ASM_0x0103 = 259,//VS2015 - VS2017
  rich_type_ASM_0x00DF = 223,//VS2013
  rich_type_ASM_0x00CD = 205,//VS2012
  rich_type_ASM_0x00BB = 187,
  rich_type_ASM_0x009E = 158,//VS2010
  rich_type_ASM_0x0095 = 149,//VS2008
  rich_type_ASM_0x007D = 125,//VS2005
  rich_type_ASM_0x004B = 75,
  rich_type_ASM_0x0040 = 64, //VS2002
  rich_type_ASM_0x002D = 45,
  rich_type_ASM_0x002A = 42,
  rich_type_ASM_0x0012 = 18,
  rich_type_ASM_0x000F = 15, //VS2003
  rich_type_ASM_0x000E = 14,
  //[C++]
  rich_type_CPP_0x0105 = 261,//VS2015 - VS2017
  rich_type_CPP_0x00E1 = 225,//VS2013
  rich_type_CPP_0x00CF = 207,//VS2012
  rich_type_CPP_0x00AB = 171,//VS2010
  rich_type_CPP_0x0086 = 134,
  rich_type_CPP_0x0084 = 132,//VS2008
  rich_type_CPP_0x0070 = 112,
  rich_type_CPP_0x006E = 110,//VS2005
  rich_type_CPP_0x0062 = 98,
  rich_type_CPP_0x0060 = 96, //VS2003
  rich_type_CPP_0x004F = 79,
  rich_type_CPP_0x004D = 77,
  rich_type_CPP_0x003A = 58,
  rich_type_CPP_0x0033 = 51,
  rich_type_CPP_0x0031 = 49,
  rich_type_CPP_0x0024 = 36,
  rich_type_CPP_0x001D = 29, //VS2002
  rich_type_CPP_0x0016 = 22,
  rich_type_CPP_0x000B = 11, //MSVS98 6.0
  //[RES]
  rich_type_RES_0x00FF = 255,//VS2015 - VS2017
  rich_type_RES_0x00DB = 219,//VS2013
  rich_type_RES_0x00C9 = 201,//VS2012
  rich_type_RES_0x00BA = 186,
  rich_type_RES_0x009A = 154,//VS2010
  rich_type_RES_0x0094 = 148,//VS2008
  rich_type_RES_0x007C = 124,//VS2005
  rich_type_RES_0x005E = 94, //VS2003
  rich_type_RES_0x0046 = 70,
  rich_type_RES_0x0045 = 69, //VS2002
  rich_type_RES_0x0038 = 56,
  rich_type_RES_0x0006 = 6,  //MSVC++ 6.0
  //[LNK]
  rich_type_LNK_0x0102 = 258,//VS2015 - VS2017
  rich_type_LNK_0x00DE = 222,//VS2013
  rich_type_LNK_0x00CC = 204,//VS2012
  rich_type_LNK_0x00B7 = 183,
  rich_type_LNK_0x009D = 157,//VS2010
  rich_type_LNK_0x0091 = 145,//VS2008
  rich_type_LNK_0x0078 = 120,//VS2005
  rich_type_LNK_0x005A = 90, //VS2003
  rich_type_LNK_0x0056 = 86,
  rich_type_LNK_0x0047 = 71,
  rich_type_LNK_0x003D = 61, //VS2002
  rich_type_LNK_0x003C = 60,
  rich_type_LNK_0x0028 = 40,
  rich_type_LNK_0x0025 = 37,
  rich_type_LNK_0x0020 = 32,
  rich_type_LNK_0x001E = 30,
  rich_type_LNK_0x0013 = 19,
  rich_type_LNK_0x0010 = 16,
  rich_type_LNK_0x0004 = 4, //MSVS6.0
  rich_type_LNK_0x0002 = 2,
  //[EXP]
  rich_type_EXP_0x0100 = 256,//VS2015 - VS2017
  rich_type_EXP_0x00DC = 220,//VS2013
  rich_type_EXP_0x00CA = 202,//VS2012
  rich_type_EXP_0x00B8 = 184,
  rich_type_EXP_0x009B = 155,//VS2010
  rich_type_EXP_0x0092 = 146,//VS2008
  rich_type_EXP_0x007A = 122,//VS2005
  rich_type_EXP_0x005C = 92, //VS2003
  rich_type_EXP_0x0058 = 88,
  rich_type_EXP_0x0049 = 73,
  rich_type_EXP_0x003F = 63, //VS2002
  rich_type_EXP_0x003E = 62,
  //[IMP]
  rich_type_IMP_0x0101 = 257,//VS2015 - VS2017
  rich_type_IMP_0x00DD = 221,//VS2013
  rich_type_IMP_0x00CB = 203,//VS2012
  rich_type_IMP_0x00B9 = 185,
  rich_type_IMP_0x009C = 156,//VS2010
  rich_type_IMP_0x0093 = 147,//VS2008
  rich_type_IMP_0x007B = 123,//VS2005
  rich_type_IMP_0x005D = 93, //VS2003
  rich_type_IMP_0x0059 = 89,
  rich_type_IMP_0x004A = 74,
  rich_type_IMP_0x0036 = 54,
  rich_type_IMP_0x0019 = 25, //VS2002

  rich_type_max = 0xFFFF
};


class pe_dos_header {
    image_dos_header dos_h;
public:
    pe_dos_header::pe_dos_header();
    pe_dos_header::~pe_dos_header();
public:
    void pe_dos_header::set_header(image_dos_header * header);

public:
    image_dos_header& pe_dos_header::get_header();
};


class pe_dos_stub {
    std::vector<uint8_t> dos_stub;
public:
    pe_dos_stub::pe_dos_stub();
    pe_dos_stub::~pe_dos_stub();
public:
    void pe_dos_stub::set_stub(std::vector<uint8_t>& stub);

public:
    std::vector<uint8_t> pe_dos_stub::get_stub() const;
};



class pe_rich_item {
    e_rich_type type;
    uint16_t compiler_build;
    uint32_t count;

public:
    pe_rich_item::pe_rich_item();
    pe_rich_item::pe_rich_item(const pe_rich_item& item);
    pe_rich_item::pe_rich_item(e_rich_type type,uint16_t compiler_build,uint32_t count);
    pe_rich_item::~pe_rich_item();

    pe_rich_item& pe_rich_item::operator=(const pe_rich_item& item);
public:
    void pe_rich_item::set_type(e_rich_type type);
    void pe_rich_item::set_compiler_build(uint16_t compiler_build);
    void pe_rich_item::set_count(uint32_t count);
public:
    e_rich_type pe_rich_item::get_type() const;
    uint16_t pe_rich_item::get_compiler_build() const;
    uint32_t pe_rich_item::get_count() const;
};

class pe_rich_data {
    std::vector<pe_rich_item> items;
    uint32_t rich_offset;
    uint32_t rich_size;
    uint32_t rich_xorkey;
    uint32_t rich_correct_xorkey;
public:
    pe_rich_data::pe_rich_data();
    pe_rich_data::pe_rich_data(const pe_rich_data& data);
    pe_rich_data::~pe_rich_data();

    pe_rich_data& pe_rich_data::operator=(const pe_rich_data& data);
public:
    void pe_rich_data::set_rich_offset(uint32_t offset);
    void pe_rich_data::set_rich_size(uint32_t size);
    void pe_rich_data::set_rich_xorkey(uint32_t xorkey);
    void pe_rich_data::set_rich_correct_xorkey(uint32_t correct_xorkey);

public:
    uint32_t pe_rich_data::get_rich_offset() const;
    uint32_t pe_rich_data::get_rich_size() const;
    uint32_t pe_rich_data::get_rich_xorkey() const;
    uint32_t pe_rich_data::get_rich_correct_xorkey() const;
    bool     pe_rich_data::is_valid_rich() const;
    bool     pe_rich_data::is_present() const;
    std::vector<pe_rich_item>& pe_rich_data::get_items();
};


bool get_image_dos_header(_In_ const uint8_t * pimage, _Out_ pe_dos_header& dos_header);
bool get_image_dos_stub(_In_ const uint8_t * pimage,   _Out_ pe_dos_stub& dos_stub);
bool get_image_rich_data(_In_ const uint8_t * pimage,  _Out_ pe_rich_data& rich_data);

bool has_image_rich_data(_In_ const uint8_t * pimage ,
    _Out_opt_ uint32_t * rich_data_offset = 0, _Out_opt_ uint32_t * rich_data_size = 0, _Out_opt_ uint32_t * rich_xor_key = 0);
bool checksum_rich(_In_ const uint8_t * pimage, _Out_opt_ uint32_t * correct_rich_xor_key = 0);