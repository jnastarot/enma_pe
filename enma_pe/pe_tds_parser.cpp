#include "stdafx.h"
#include "pe_tds_parser.h"

/*
   rewrited code from
 http://denisenkomik.narod.ru/main.cpp
*/



struct TdsHeader {
    char Magic[4];
    int SubSectionDirOffset;
};

enum SubSectTypes {
    sstModule = 0x120,
    sstAlignSym = 0x125,
    sstSrcModule = 0x127,
    sstGlobalSym = 0x129,
    sstGlobalTypes = 0x12b,
    sstNames = 0x130,
};

enum SymbolAlignType
{
    S_COMPILE = 0x01,
    S_REGISTER = 0x02,
    S_CONST = 0x03,
    S_UDT = 0x04,
    S_SSEARCH = 0x05,
    S_END = 0x06,
    S_SKIP = 0x07,
    S_CVRESERVE = 0x08,
    S_OBJNAME = 0x09,
    S_ENDARG = 0x0A,
    S_COBOLUDT = 0x0B,
    S_MANYREG = 0x0C,
    S_RETURN = 0x0D,
    S_ENTRYTHIS = 0x0E,
    /*0xF-0x1F unused */
    S_GPROCREF = 0x20, /* [Address 32][Type 32][Name 32][Browser offset 32][Offset 32][Segment 16] */
    S_GDATAREF = 0x21, /* [Address 32][Type 32][Name 32][Browser offset 32][Offset 32][Segment 16] */
    S_EDATA = 0x22, /* [Type 32][Name 32][unk 16][EI 16][Browser offset 32]*/
    S_EPROC = 0x23, /* [Type 32][Name 32][unk 16][EI 16][Browser offset 32]*/
    S_USES = 0x24,
    S_NAMESPACE = 0x25,
    S_USING = 0x26,
    S_PCONSTANT = 0x27,
};

enum SymbolAlignSizedType
{
    S_BPREL = 0x0,
    S_LDATA = 0x1,
    S_GDATA = 0x2,
    S_PUB = 0x3, /* [Offset 32][Segment 16][Flags 16][Type 32][Name 32][Browser offset 32]*/
    S_LPROC = 0x4,
    S_GPROC = 0x5,
    S_THUNK = 0x6, /*[Parent 32][End 32][Next 32][Offset 32][Segment 16][Length 16][Name 32][Ordinal 8][Some flags 8]*/
    S_BLOCK = 0x7,
    S_WITH = 0x8,
    S_LABEL = 0x9, /*[Offset 32][Segment 16][Some byte 8][Name 32]*/
                   /*0x unused*/
                   S_VFTPATH = 0xB,
                   S_VFTREGREL = 0xC,
                   S_LTHREAD = 0xD,
                   S_GTHREAD = 0xE,
                   /*0xF unused*/
                   S_ENTRY = 0x10, /*[Offset 32][Segment 16]*/
                   S_OPTVAR = 0x11,
                   S_PROCRET = 0x12,
                   S_SAVREGS = 0x13,
                   /*0x14-2F*/
                   S_SLINK = 0x30,
};



struct SubSectDirItem
{
    short Type; // SubSectTypes
    short Index;
    int Offset;
    int Size;
};

struct SubSectDir
{
    char Unknown[0x4];
    int Num;
    char Unknown2[0x8];
    SubSectDirItem Items[1];
};

struct GlobalSymHeader
{
    unsigned short SymHash;
    unsigned short AddrHash;
    unsigned int cbSymbols;
    unsigned int cbSymHash;
    unsigned int cbAddrHash;
    unsigned int cUDTs;
    unsigned int cOthers;
    unsigned int Total;
    unsigned int cNamespaces;
};

#pragma pack(2)

struct SData32
{
    int Offset;
    short Segment;
    short Flags;
    int Type;
    int Name;
    int BrowserOffset;
};

struct SProc32
{
    int Parent;
    int End;
    int Next;
    int Size1;
    char Unknown[0x04];
    int Size;
    int Start;
    short Segment;
    short Unknown2;
    short Type;
    short Unknown3;
    short Name;
    char Unknown4[0x6];
    // for global only
    unsigned char LinkerNameLen;
    char LinkerName[1];
};




inline unsigned char EatUChar(const char *& pos)
{
    unsigned char result = *reinterpret_cast<const unsigned char*>(pos);
    pos += sizeof(BYTE);
    return result;
}

inline short EatShort(const char *& pos)
{
    short result = *reinterpret_cast<const short*>(pos);
    pos += sizeof(WORD);
    return result;
}

inline int EatInt(const char *& pos)
{
    int result = *reinterpret_cast<const int*>(pos);
    pos += sizeof(DWORD);
    return result;
}


tds_parser::tds_parser(std::string filepath, pe_image& image, map_root& map, e_map_result &result) {
    OFSTRUCT ofs = { 0 };
    DWORD readed;

    HANDLE hfile = (HANDLE)OpenFile(filepath.c_str(), &ofs, OF_READ);
    if (hfile != INVALID_HANDLE_VALUE) {
        unsigned int file_size = GetFileSize(hfile, 0);
        tds_file.resize(file_size);

        if (!ReadFile(hfile, tds_file.data(), file_size, &readed, 0) || readed != file_size) {
            result = e_map_result::map_error_readfile;
            return;
        }

        CloseHandle(hfile);

        result = parse(image, map);
    }
    else {
        result = e_map_result::map_error_openfile;
    }
}
tds_parser::tds_parser(std::vector<BYTE>& tds_file, pe_image& image, map_root& map, e_map_result &result) {
    this->tds_file = tds_file;
    result = parse(image, map);
}

tds_parser::~tds_parser() {}


e_map_result tds_parser::parse(pe_image& image, map_root& map) {
    const TdsHeader * header = reinterpret_cast<const TdsHeader *>(tds_file.data());

    if (tds_file.size() > 0x20 && *(WORD*)header->Magic == 0x4246 && (*(WORD*)&header->Magic[2] - 0x3930) < 0x800) {//FB0X

        cout << header->Magic[0] << header->Magic[1] << header->Magic[2] << header->Magic[3] << endl;
        const SubSectDir * ssdir = reinterpret_cast<const SubSectDir *>(tds_file.data() + header->SubSectionDirOffset);
        const SubSectDirItem * items = &ssdir->Items[0];

        for (int i = 0; i < ssdir->Num; i++) {

            const SubSectDirItem * curr = &items[i];

            switch (curr->Type) {

            case sstNames:
                p_names_section = (const char*)tds_file.data() + curr->Offset;
                break;
            }
        }

        for (int i = 0; i < ssdir->Num; i++) {

            const SubSectDirItem * curr = &items[i];

            switch (curr->Type) {

            case sstAlignSym:
                parse_align_symbols( (const char*)tds_file.data() + curr->Offset, (const char*)tds_file.data() + curr->Offset + curr->Size, curr->Index);
                break;
            case sstGlobalSym:
                parse_global_symbols( (const char*)tds_file.data() + curr->Offset);
                break;
            }
        }

        for (auto& section:  image.get_sections()) {
            map_segment seg;
            seg._class = section->is_executable() ? segment_class_code : segment_class_data;
            seg.offset = 0;
            seg.section_number = segments.size() + 1;
            seg.length = section->get_virtual_size();
            seg.segment_name = section->get_name();
            segments.push_back(seg);
        }

        map_finalize_items(image, segments, items_raw, map);
        return e_map_result::map_error_ok;
    }
    else {
        return e_map_result::map_error_parsefile;
    }
}

std::string tds_parser::get_name_by_id(int name_id){
    const char * pos = p_names_section;
    int num = EatInt(pos);
    if (name_id > num)
        return std::string();
    for (int i = 0; i < name_id - 1; i++)
        pos += EatUChar(pos) + 1;
    return std::string(pos + 1);
}

void tds_parser::parse_item_path(map_item_raw& item, std::string name) {

    std::string path_step;
    for (unsigned int i = 0; i < name.length(); i++) {

        if (name[i] == '$') {
            item.item_name = path_step;
            break;
        }
        else if (name[i] == '@'){
            if (!i) {
                continue;
            }
            else {
                item.path.push_back(path_step);
                continue;
            }
        }
        else {
            path_step += name[i];
        }
    }

    if (!item.item_name.length()) {
        if (path_step.length()) {
            item.item_name = path_step;
        }
        else {
            if (item.path.size()) {
                item.item_name = item.path[item.path.size() - 1];
                item.path.pop_back();
            }
            else {
                item.item_name = "unknown item =,(";
            }
        }
    }
}

void tds_parser::parse_symbols(const char * start, const char * end){
    const char * pos = start;

    while (pos != end) {

        int size = EatShort(pos);
        int type = EatShort(pos);

        switch (type & 0xFF00){

        case 0x200:
            
            switch (type & 0xFF) {

            case S_LDATA: case S_GDATA: {
                const SData32 * dat = reinterpret_cast<const SData32 *>(pos);

                map_item_raw item;
                item.section_number = dat->Segment;
                item._class = data_class_data;
                item.offset = dat->Offset;
                item.length = 0;

                parse_item_path(item, get_name_by_id(dat->Name));
                
                items_raw.push_back(item);
                break;
            }

            case S_LPROC:case S_GPROC: {
                const SProc32 * proc = reinterpret_cast<const SProc32 *>(pos);

                map_item_raw item;
                item.section_number = proc->Segment;
                item._class = data_class_function;
                item.offset = proc->Start;
                item.length = proc->Size;
                parse_item_path(item, get_name_by_id(proc->Name));

                items_raw.push_back(item);
                break;
            }
            default:
                break;
            }
            break;
        default:
            break;
        }
        pos += size - 2;
    }
}
void tds_parser::parse_align_symbols(const char * start, const char * end, int moduleIndex){
    const char * pos = start;
    int Unknown1 = EatInt(pos);
    parse_symbols(pos, end);
}
void tds_parser::parse_global_symbols(const char * start){
    const GlobalSymHeader * hdr = reinterpret_cast<const GlobalSymHeader *>(start);
    const char * pos = start + sizeof(GlobalSymHeader);
    parse_symbols(pos, pos + hdr->cbSymbols);
}
