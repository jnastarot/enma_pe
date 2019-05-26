#pragma once

#define PE_IMAGE_BUILD_SEP_SECTION_RDATA 0x01 //separated section for load config, debug
#define PE_IMAGE_BUILD_SEP_SECTION_EDATA 0x02 //separated section for imports, exports, tls
#define PE_IMAGE_BUILD_SEP_SECTION_PDATA 0x04 //separated section for exceptions
#define PE_IMAGE_BUILD_SEP_SECTION_RSRC  0x08 //separated section for resources
#define PE_IMAGE_BUILD_SEP_SECTION_RELOC 0x10 //separated section for relocations
#define PE_IMAGE_BUILD_UPD_RICH_DATA     0x20 //update header from pe_image->rich_data to headers
#define PE_IMAGE_BUILD_UPD_CHECKSUM      0x40 //update field nt_header->checksum
#define PE_IMAGE_BUILD_OVERLAY           0x80 //add pe_image->overlay_data

#define PE_IMAGE_BUILD_DIR_EXPORT        0x100
#define PE_IMAGE_BUILD_DIR_IMPORT        0x200
#define PE_IMAGE_BUILD_DIR_RESOURCES     0x400
#define PE_IMAGE_BUILD_DIR_EXCEPTIONS    0x800
#define PE_IMAGE_BUILD_DIR_SECURITY      0x1000  //not implimented
#define PE_IMAGE_BUILD_DIR_RELOCATIONS   0x2000
#define PE_IMAGE_BUILD_DIR_DEBUG         0x4000  //not implimented
#define PE_IMAGE_BUILD_DIR_TLS           0x8000
#define PE_IMAGE_BUILD_DIR_LOAD_CONFIG   0x10000
#define PE_IMAGE_BUILD_DIR_BOUND_IMPORT  0x20000
#define PE_IMAGE_BUILD_DIR_DELAY_IMPORT  0x40000 //not implimented
#define PE_IMAGE_BUILD_DIR_IAT           0x80000 //not implimented



#define PE_IMAGE_BUILD_ALL_EXTENDED_SECTIONS  (PE_IMAGE_BUILD_SEP_SECTION_RDATA | PE_IMAGE_BUILD_SEP_SECTION_EDATA\
    PE_IMAGE_BUILD_SEP_SECTION_RSRC | PE_IMAGE_BUILD_SEP_SECTION_RELOC)

#define PE_IMAGE_BUILD_ALL_DIRECTORIES  (PE_IMAGE_BUILD_DIR_EXPORT | PE_IMAGE_BUILD_DIR_IMPORT\
    PE_IMAGE_BUILD_DIR_RESOURCES | PE_IMAGE_BUILD_DIR_EXCEPTIONS\
    PE_IMAGE_BUILD_DIR_SECURITY | PE_IMAGE_BUILD_DIR_RELOCATIONS\
    PE_IMAGE_BUILD_DIR_DEBUG | PE_IMAGE_BUILD_DIR_TLS\
    PE_IMAGE_BUILD_DIR_LOAD_CONFIG | PE_IMAGE_BUILD_DIR_BOUND_IMPORT\
    PE_IMAGE_BUILD_DIR_DELAY_IMPORT | PE_IMAGE_BUILD_DIR_IAT\)



bool build_pe_image_full(pe_image_full& image_full, uint32_t build_flags, std::vector<uint8_t>& out_image);
bool build_pe_image(pe_image& image, uint32_t build_flags, std::vector<uint8_t>& out_image);


