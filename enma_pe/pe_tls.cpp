#include "stdafx.h"
#include "pe_tls.h"


tls_table::tls_table() {
	start_address_raw_data	= 0;
	end_address_raw_data = 0;
	address_of_index		= 0;
	address_of_callbacks	= 0;
    size_of_zero_fill       = 0;
    characteristics         = 0;
	raw_data.clear();
	callbacks.clear();
}

tls_table::tls_table(const tls_table& tls) {
    this->operator=(tls);
}

tls_table::~tls_table(){}


tls_table& tls_table::operator=(const tls_table& tls) {

	start_address_raw_data	= tls.start_address_raw_data;
	end_address_raw_data    = tls.end_address_raw_data;
	address_of_index		= tls.address_of_index;
	address_of_callbacks	= tls.address_of_callbacks;
    size_of_zero_fill       = tls.size_of_zero_fill;
    characteristics         = tls.characteristics;
	raw_data = tls.raw_data;
	callbacks = tls.callbacks;

	return *this;
}


void tls_table::set_start_address_raw_data(uint32_t   start_address_raw_data) {
	this->start_address_raw_data = start_address_raw_data;
}
void tls_table::set_end_address_raw_data(uint32_t   end_address_raw_data) {
	this->end_address_raw_data = end_address_raw_data;
}
void tls_table::set_address_of_index(uint32_t   address_of_index) {
	this->address_of_index = address_of_index;
}
void tls_table::set_address_of_callbacks(uint32_t   address_of_callbacks){
	this->address_of_callbacks = address_of_callbacks;
}
void tls_table::set_size_of_zero_fill(uint32_t   size_of_zero_fill) {
    this->size_of_zero_fill = size_of_zero_fill;
}
void tls_table::set_characteristics(uint32_t   characteristics) {
	this->characteristics = characteristics;
}

uint32_t tls_table::get_start_address_raw_data() const {
	return this->start_address_raw_data;
}
uint32_t tls_table::get_end_address_raw_data() const {
	return this->end_address_raw_data;
}
uint32_t tls_table::get_address_of_index() const {
	return this->address_of_index;
}
uint32_t tls_table::get_address_of_callbacks() const {
	return this->address_of_callbacks;
}
uint32_t tls_table::get_size_of_zero_fill() const {
    return this->size_of_zero_fill;
}
uint32_t tls_table::get_characteristics() const {
	return this->characteristics;
}
std::vector<uint8_t>& tls_table::get_raw_data() {
	return raw_data;
}
std::vector<tls_table::tls_callback>& tls_table::get_callbacks() {
	return callbacks;
}

template<typename image_format>
directory_code _get_tls_table(const pe_image &image, tls_table& tls) {

    tls.set_start_address_raw_data(0);
    tls.set_end_address_raw_data(0);
    tls.set_address_of_index(0);
    tls.set_address_of_callbacks(0);
    tls.set_characteristics(0);
    tls.get_callbacks().clear();
    tls.get_raw_data().clear();

    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_TLS);


    if (virtual_address) {
        pe_image_io tls_io((pe_image &)image);
       
        typename image_format::image_tls_directory tls_directory;

        if (tls_io.set_image_offset(virtual_address).read(&tls_directory, sizeof(tls_directory)) == enma_io_success) {

            if (tls_directory.start_address_of_raw_data) {
                tls.set_start_address_raw_data(image.va_to_rva(tls_directory.start_address_of_raw_data));
                tls.set_end_address_raw_data(image.va_to_rva(tls_directory.end_address_of_raw_data));

                tls_io.set_image_offset(image.va_to_rva(tls_directory.start_address_of_raw_data));

                tls.get_raw_data().resize(tls_directory.end_address_of_raw_data - tls_directory.start_address_of_raw_data);

                if (tls_io.read(tls.get_raw_data().data(), tls.get_raw_data().size()) != enma_io_success) {
                    return directory_code::directory_code_currupted;
                }
            }

            if (tls_directory.address_of_index) {
                tls.set_address_of_index(image.va_to_rva(tls_directory.address_of_index));
            }
            if (tls_directory.address_of_callbacks) {
                tls.set_address_of_callbacks(image.va_to_rva(tls_directory.address_of_callbacks));

                tls_io.set_image_offset(image.va_to_rva(tls_directory.address_of_callbacks));

                typename image_format::ptr_size tls_callback = 0;

                do {
                    tls_io >> tls_callback;
                    if (tls_io.get_last_code() != enma_io_success) { return directory_code::directory_code_currupted; }

                    if (tls_callback) {
                        tls.get_callbacks().push_back({ image.va_to_rva(tls_callback),true });
                    }

                } while (tls_callback);
            }

            tls.set_characteristics(tls_directory.characteristics);
            tls.set_size_of_zero_fill(tls_directory.size_of_zero_fill);

            return directory_code::directory_code_success;
        }

        return directory_code::directory_code_currupted;
    }
    
    return directory_code::directory_code_not_present;
}

template<typename image_format>
bool _build_internal_tls_data(const pe_image &image, pe_section& section,
    tls_table& tls, relocation_table& relocs, uint32_t build_items_ids/*tls_table_build_id*/) {

    pe_section_io tls_io(section,(pe_image &)image,enma_io_mode_allow_expand);
    tls_io.seek_to_end();

    if (build_items_ids&tls_table_build_raw_data) {

        if (tls.get_raw_data().size()) {
            tls_io.align_up(0x10);

            tls.set_start_address_raw_data(tls_io.get_section_offset());
            tls.set_end_address_raw_data(tls_io.get_section_offset() + tls.get_raw_data().size());

            if (tls_io.write(tls.get_raw_data().data(), tls.get_raw_data().size()) != enma_io_success) {
                return false; //err
            }
        }
        else {
            tls.set_start_address_raw_data(0);
            tls.set_end_address_raw_data(0);
        }
    }

    if (build_items_ids&tls_table_build_address_of_index) {
        tls_io.align_up(sizeof(typename image_format::ptr_size));

        tls.set_address_of_index(tls_io.get_section_offset());

        uint32_t index__ = 0;

        if (tls_io.write(&index__, sizeof(index__)) != enma_io_success) {
            return false; //err
        }
    }

    if (build_items_ids&tls_table_build_callbacks) {

        if (tls.get_callbacks().size()) {
            tls_io.align_up(sizeof(typename image_format::ptr_size));

            tls.set_address_of_callbacks(tls_io.get_section_offset());

            for (auto& callback_item : tls.get_callbacks()) {
                typename image_format::ptr_size call_back_va = callback_item.rva_callback;

                if (callback_item.use_relocation) {
                    call_back_va = (typename image_format::ptr_size)image.rva_to_va(callback_item.rva_callback);
                    relocs.add_item(tls_io.get_section_offset(), 0);
                }

                if (tls_io.write(&call_back_va,sizeof(call_back_va)) != enma_io_success) {
                    return false;//err
                }
            }

            tls_io.add_size(sizeof(typename image_format::ptr_size));
        }
        else {
            tls.set_address_of_callbacks(0);
        }
    }
    return true;
}


template<typename image_format>
bool _build_tls_table_only(pe_image &image, pe_section& section, tls_table& tls, relocation_table& relocs) {

    pe_section_io tls_io(section, (pe_image &)image, enma_io_mode_allow_expand);
    tls_io.align_up(0x10).seek_to_end();

    uint32_t tls_rva = tls_io.get_section_offset();

    typename image_format::image_tls_directory tls_directory;
    memset(&tls_directory, 0, sizeof(tls_directory));

    if (tls.get_start_address_raw_data()) {
        tls_directory.start_address_of_raw_data = image.rva_to_va(tls.get_start_address_raw_data());
        tls_directory.end_address_of_raw_data   = image.rva_to_va(tls.get_end_address_raw_data());

        relocs.add_item(tls_io.get_section_offset() + offsetof(typename image_format::image_tls_directory, start_address_of_raw_data), 0);
        relocs.add_item(tls_io.get_section_offset() + offsetof(typename image_format::image_tls_directory, end_address_of_raw_data),   0);
    }

    if (tls.get_address_of_index()) {
        tls_directory.address_of_index = image.rva_to_va(tls.get_address_of_index());
        relocs.add_item(tls_io.get_section_offset() + offsetof(typename image_format::image_tls_directory, address_of_index), 0);
    }

    if (tls.get_address_of_callbacks()) {
        tls_directory.address_of_callbacks = image.rva_to_va(tls.get_address_of_callbacks());
        relocs.add_item(tls_io.get_section_offset() + offsetof(typename image_format::image_tls_directory, address_of_callbacks), 0);
    }
    
    tls_directory.size_of_zero_fill     = tls.get_size_of_zero_fill();
    tls_directory.characteristics       = tls.get_characteristics();

    if (tls_io.write(&tls_directory, sizeof(tls_directory)) == enma_io_success) {
        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_TLS, tls_rva);
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_TLS, sizeof(typename image_format::image_tls_directory));

        return true;
    }
    else {

        return false;
    }
}


template<typename image_format>
directory_code _get_placement_tls_table(pe_image &image, std::vector<directory_placement>& placement) {

    tls_table tls;

    directory_code code = get_tls_table(image, tls);

    if (code == directory_code::directory_code_success) {
        placement.push_back({ 
            image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_TLS) ,
            sizeof(typename image_format::image_tls_directory),dp_id_tls_desc 
        });

        if (tls.get_start_address_raw_data()) {
            placement.push_back({
                tls.get_start_address_raw_data() ,
                tls.get_raw_data().size(),dp_id_tls_raw_data
            });
        }

        if (tls.get_address_of_index()) {
            placement.push_back({
                tls.get_address_of_index() ,
                sizeof(uint32_t),dp_id_tls_index
            });
        }

        if (tls.get_address_of_callbacks()) {
            placement.push_back({
                tls.get_address_of_callbacks() ,
                (tls.get_callbacks().size() + 1 ) * sizeof(typename image_format::ptr_size),dp_id_tls_callbacks
            });
            
        }
        
        return directory_code::directory_code_success;
    }

    return code;
}

directory_code get_tls_table(const pe_image &image, tls_table& tls) {

    if (image.is_x32_image()) {
        return _get_tls_table<image_32>(image, tls);
    }
    else {
        return _get_tls_table<image_64>(image, tls);
    }
}

bool build_internal_tls_data(const pe_image &image,pe_section& section,
    tls_table& tls, relocation_table& relocs, uint32_t build_items_ids/*tls_table_build_id*/) {

    if (image.is_x32_image()) {
        return _build_internal_tls_data<image_32>(image, section, tls, relocs, build_items_ids);
    }
    else {
        return _build_internal_tls_data<image_64>(image, section, tls, relocs, build_items_ids);
    }
}

bool build_tls_table_only(pe_image &image, pe_section& section, tls_table& tls, relocation_table& relocs) {
    if (image.is_x32_image()) {
        return _build_tls_table_only<image_32>(image, section, tls, relocs);
    }
    else {
        return _build_tls_table_only<image_64>(image, section, tls, relocs);
    }
}

bool build_tls_full(pe_image &image,pe_section& section,
    tls_table& tls,relocation_table& relocs) {

    if (build_internal_tls_data(image, section, tls, relocs,
        tls_table_build_raw_data | tls_table_build_address_of_index | tls_table_build_callbacks) &&
        build_tls_table_only(image, section, tls, relocs)) {

        return true;
    }
    else {
        return false;
    }
}


directory_code get_placement_tls_table(pe_image &image, std::vector<directory_placement>& placement) {

    if (image.is_x32_image()) {
        return _get_placement_tls_table<image_32>(image, placement);
    }
    else {
        return _get_placement_tls_table<image_64>(image, placement);
    } 
}