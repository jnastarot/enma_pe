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

bool get_tls_table(const pe_image &image, tls_table& tls) {

    tls.set_start_address_raw_data(0);
    tls.set_end_address_raw_data(0);
    tls.set_address_of_index(0);
    tls.set_address_of_callbacks(0);
    tls.set_characteristics(0);
    tls.get_callbacks().clear();

    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_TLS);
    uint32_t virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_TLS);

	if (virtual_address && virtual_size) {
		pe_section * tls_section = image.get_section_by_rva(virtual_address);

		if (tls_section) {
			uint8_t * tls_raw = &tls_section->get_section_data().data()[virtual_address - tls_section->get_virtual_address()];
			if (tls_section->get_size_of_raw_data() >= virtual_size) {

				if (image.is_x32_image()) {
					image_tls_directory32 * tls_desc = (image_tls_directory32 *)tls_raw;
					if (tls_desc->start_address_of_raw_data) {
						tls.set_start_address_raw_data(image.va_to_rva(tls_desc->start_address_of_raw_data));
						tls.set_end_address_raw_data(image.va_to_rva(tls_desc->end_address_of_raw_data));

						pe_section * raw_data_section = image.get_section_by_va(tls_desc->start_address_of_raw_data);

                        uint32_t data_raw_size = (tls_desc->end_address_of_raw_data - tls_desc->start_address_of_raw_data);
                        uint32_t data_raw = image.va_to_raw(tls_desc->start_address_of_raw_data);
						tls.get_raw_data().resize(data_raw_size);
						ZeroMemory(tls.get_raw_data().data(), tls.get_raw_data().size());

						if (raw_data_section && 
							(raw_data_section->get_pointer_to_raw() <= data_raw &&
							raw_data_section->get_pointer_to_raw() + raw_data_section->get_size_of_raw_data() > data_raw + data_raw_size)
							) {
							
							memcpy(tls.get_raw_data().data(),
								&raw_data_section->get_section_data().data()[data_raw - raw_data_section->get_pointer_to_raw()],
								data_raw_size
							);
						}
					}
					else {
						tls.set_start_address_raw_data(0);
						tls.set_end_address_raw_data(0);
						tls.get_raw_data().clear();
					}

					if (tls_desc->address_of_index) {
						tls.set_address_of_index(image.va_to_rva(tls_desc->address_of_index));
					}
					else {
						tls.set_address_of_index(0);
					}

					if (tls_desc->address_of_callbacks) {
						pe_section * callback_data_section = image.get_section_by_va(tls_desc->address_of_callbacks);
						if (callback_data_section) {
							tls.get_callbacks().clear();

                            uint32_t * raw_callbacks = (uint32_t*)&callback_data_section->get_section_data().data()[
								image.va_to_rva(tls_desc->address_of_callbacks) - callback_data_section->get_virtual_address()
							];

							while (*raw_callbacks) {
								tls.get_callbacks().push_back({ image.va_to_rva(*raw_callbacks),true });
								raw_callbacks++;
							}

						}

						tls.set_address_of_callbacks(image.va_to_rva(tls_desc->address_of_callbacks));
					}
					else {
						tls.set_address_of_callbacks(0);
					}

					tls.set_characteristics(tls_desc->characteristics);
                    tls.set_size_of_zero_fill(tls_desc->size_of_zero_fill);
				}
				else {
					image_tls_directory64 * tls_desc = (image_tls_directory64 *)tls_raw;
					if (tls_desc->start_address_of_raw_data) {
						tls.set_start_address_raw_data(image.va_to_rva(tls_desc->start_address_of_raw_data));
						tls.set_end_address_raw_data(image.va_to_rva(tls_desc->end_address_of_raw_data));

						pe_section * raw_data_section = image.get_section_by_va(tls_desc->start_address_of_raw_data);

                        uint32_t data_raw_size = uint32_t(tls_desc->end_address_of_raw_data - tls_desc->start_address_of_raw_data);
                        uint32_t data_raw = image.va_to_raw(tls_desc->start_address_of_raw_data);
						tls.get_raw_data().resize(data_raw_size);
						ZeroMemory(tls.get_raw_data().data(), tls.get_raw_data().size());

						if (raw_data_section &&
							(raw_data_section->get_pointer_to_raw() <= data_raw &&
								raw_data_section->get_pointer_to_raw() + raw_data_section->get_size_of_raw_data() > data_raw + data_raw_size)
							) {

							memcpy(tls.get_raw_data().data(),
								&raw_data_section->get_section_data().data()[data_raw - raw_data_section->get_pointer_to_raw()],
								data_raw_size
							);
						}
					}
					else {
						tls.set_start_address_raw_data(0);
						tls.set_end_address_raw_data(0);
						tls.get_raw_data().clear();
					}

					if (tls_desc->address_of_index) {
						tls.set_address_of_index(image.va_to_rva(tls_desc->address_of_index));
					}
					else {
						tls.set_address_of_index(0);
					}

					if (tls_desc->address_of_callbacks) {
						pe_section * callback_data_section = image.get_section_by_va(tls_desc->address_of_callbacks);
						if (callback_data_section) {
							tls.get_callbacks().clear();

							uint64_t * raw_callbacks = (uint64_t*)&callback_data_section->get_section_data().data()[
								image.va_to_rva(tls_desc->address_of_callbacks) - callback_data_section->get_virtual_address()
							];

							while (*raw_callbacks) {
								tls.get_callbacks().push_back({ image.va_to_rva(*raw_callbacks),true });
								raw_callbacks++;
							}

						}

						tls.set_address_of_callbacks(image.va_to_rva(tls_desc->address_of_callbacks));
					}
					else {
						tls.set_address_of_callbacks(0);
					}

					tls.set_characteristics(tls_desc->characteristics);
                    tls.set_size_of_zero_fill(tls_desc->size_of_zero_fill);
				}
				return true;
			}
		}
	}

	return false;
}



void build_internal_tls_data(const pe_image &image,pe_section& section,
    tls_table& tls, relocation_table& relocs, uint32_t build_items_ids/*tls_table_build_id*/) {

    if (build_items_ids&tls_table_build_raw_data) {

        if (tls.get_raw_data().size()) {

            if (section.get_size_of_raw_data() & 0xF) {
                section.get_section_data().resize(
                    section.get_section_data().size() + (0x10 - (section.get_section_data().size() & 0xF))
                );
            }

            tls.set_start_address_raw_data(section.get_virtual_address() + section.get_section_data().size());
            tls.set_end_address_raw_data(
                section.get_virtual_address() + section.get_section_data().size() + tls.get_raw_data().size());

            section.add_data(tls.get_raw_data().data(), tls.get_raw_data().size());
        }
        else {
            tls.set_start_address_raw_data(0);
            tls.set_end_address_raw_data(0);
        }
    }

    if (build_items_ids&tls_table_build_address_of_index) {
        if (section.get_size_of_raw_data() & 0x3) {
            section.get_section_data().resize(
                section.get_section_data().size() + (0x4 - (section.get_section_data().size() & 0x3))
            );
        }
        
        tls.set_address_of_index(section.get_virtual_address() + section.get_section_data().size());

        uint32_t index__ = 0;
        section.add_data((const uint8_t*)&index__, sizeof(index__));
    }

    if (build_items_ids&tls_table_build_callbacks) {
        if (section.get_size_of_raw_data() & 0x3) {
            section.get_section_data().resize(
                section.get_section_data().size() + (0x4 - (section.get_section_data().size() & 0x3))
            );
        }

        if (tls.get_callbacks().size()) {
            tls.set_address_of_callbacks(section.get_virtual_address() + section.get_section_data().size());
        }
        else {
            tls.set_address_of_callbacks(0);
        }

        for (auto& callback_item : tls.get_callbacks()) {
           
            if (image.is_x32_image()) {
                uint32_t call_back_va = callback_item.rva_callback;
                if (callback_item.use_relocation) {
                    call_back_va = (uint32_t)image.rva_to_va(callback_item.rva_callback);
                    relocs.add_item(section.get_virtual_address() + section.get_size_of_raw_data(), 0);
                }
                section.add_data((uint8_t*)&call_back_va, sizeof(call_back_va));
            }
            else {
                uint64_t call_back_va = callback_item.rva_callback;
                if (callback_item.use_relocation) {
                    call_back_va = image.rva_to_va(callback_item.rva_callback);
                    relocs.add_item(section.get_virtual_address() + section.get_size_of_raw_data(), 0);
                }
                section.add_data((uint8_t*)&call_back_va, sizeof(call_back_va));
            }
        }
    }
}

void build_tls_table_only(pe_image &image, pe_section& section, tls_table& tls, relocation_table& relocs) {

    if (section.get_size_of_raw_data() & 0xF) {
        section.get_section_data().resize(
            section.get_section_data().size() + (0x10 - (section.get_section_data().size() & 0xF))
        );
    }

    if (image.is_x32_image()) {
        uint32_t tls_rva = section.get_virtual_address() + section.get_section_data().size();
        section.get_section_data().resize(section.get_section_data().size() + sizeof(image_tls_directory32));

        image_tls_directory32* tls_desc = (image_tls_directory32*)&section.get_section_data().data()[
            tls_rva - section.get_virtual_address()
        ];

        ZeroMemory(tls_desc, sizeof(image_tls_directory32));

        
        tls_desc->start_address_of_raw_data = (uint32_t)image.rva_to_va(tls.get_start_address_raw_data());
        tls_desc->end_address_of_raw_data   = (uint32_t)image.rva_to_va(tls.get_end_address_raw_data());
        tls_desc->address_of_index          = (uint32_t)image.rva_to_va(tls.get_address_of_index());

        relocs.add_item(tls_rva + offsetof(image_tls_directory32, start_address_of_raw_data), 0);
        relocs.add_item(tls_rva + offsetof(image_tls_directory32, end_address_of_raw_data), 0);
        relocs.add_item(tls_rva + offsetof(image_tls_directory32, address_of_index), 0);

        if (tls.get_address_of_callbacks()) {
            tls_desc->address_of_callbacks = (uint32_t)image.rva_to_va(tls.get_address_of_callbacks());
            relocs.add_item(tls_rva + offsetof(image_tls_directory32, address_of_callbacks), 0);
        }
        tls_desc->size_of_zero_fill = tls.get_size_of_zero_fill();
        tls_desc->characteristics = tls.get_characteristics();

      
        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_TLS, tls_rva);
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_TLS,sizeof(image_tls_directory32));
    }
    else {
        uint32_t tls_rva = section.get_virtual_address() + section.get_section_data().size();
        section.get_section_data().resize(section.get_section_data().size() + sizeof(image_tls_directory64));

        image_tls_directory64* tls_desc = (image_tls_directory64*)&section.get_section_data().data()[
            tls_rva - section.get_virtual_address()
        ];

        ZeroMemory(tls_desc, sizeof(image_tls_directory64));


        tls_desc->start_address_of_raw_data = image.rva_to_va(tls.get_start_address_raw_data());
        tls_desc->end_address_of_raw_data   = image.rva_to_va(tls.get_end_address_raw_data());
        tls_desc->address_of_index          = image.rva_to_va(tls.get_address_of_index());

        relocs.add_item(tls_rva + offsetof(image_tls_directory64, start_address_of_raw_data), 0);
        relocs.add_item(tls_rva + offsetof(image_tls_directory64, end_address_of_raw_data), 0);
        relocs.add_item(tls_rva + offsetof(image_tls_directory64, address_of_index), 0);

        if (tls.get_address_of_callbacks()) {
            tls_desc->address_of_callbacks = image.rva_to_va(tls.get_end_address_raw_data());
            relocs.add_item(tls_rva + offsetof(image_tls_directory64, address_of_callbacks), 0);
        }
        tls_desc->size_of_zero_fill = tls.get_size_of_zero_fill();
        tls_desc->characteristics   = tls.get_characteristics();


        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_TLS, tls_rva);
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_TLS, sizeof(image_tls_directory64));
    }
}

void build_tls_full(pe_image &image,pe_section& section,
    tls_table& tls,relocation_table& relocs) {

    build_internal_tls_data(image, section, tls, relocs,
        tls_table_build_raw_data | tls_table_build_address_of_index | tls_table_build_callbacks);

    build_tls_table_only(image, section, tls, relocs);
}


bool erase_tls_table(pe_image &image,std::vector<erased_zone>* zones, relocation_table* relocs) {

	uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_TLS);
    uint32_t virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_TLS);

	if (virtual_address) {
		pe_section * tls_section = image.get_section_by_rva(virtual_address);
		
		if (tls_section) {
			uint8_t * tls_raw = &tls_section->get_section_data().data()[virtual_address - tls_section->get_virtual_address()];
			if (tls_section->get_size_of_raw_data() >= virtual_size) {

				if (image.is_x32_image()) {
					image_tls_directory32 * tls_desc = (image_tls_directory32 *)tls_raw;
					if (tls_desc->start_address_of_raw_data) {
						pe_section * raw_data_section = image.get_section_by_va(tls_desc->start_address_of_raw_data);
						if (raw_data_section) {				
							ZeroMemory(
								&raw_data_section->get_section_data().data()[
									image.va_to_rva(tls_desc->start_address_of_raw_data) - raw_data_section->get_virtual_address()],
								min(tls_desc->end_address_of_raw_data - tls_desc->start_address_of_raw_data, raw_data_section->get_section_data().size())
							);
							if (zones) {
								zones->push_back({
									image.va_to_rva(tls_desc->start_address_of_raw_data) ,
									(uint32_t)max(tls_desc->end_address_of_raw_data - tls_desc->start_address_of_raw_data, raw_data_section->get_section_data().size())
								});
							}
						}
						if (relocs) {
                            relocs->erase_item(virtual_address + offsetof(image_tls_directory32, start_address_of_raw_data));
                            relocs->erase_item(virtual_address + offsetof(image_tls_directory32, end_address_of_raw_data));
						}
					}
					if (tls_desc->address_of_index) {
						pe_section * index_data_section = image.get_section_by_va(tls_desc->address_of_index);
						if (index_data_section) {
							*(uint32_t*)&index_data_section->get_section_data().data()[
								image.va_to_rva(tls_desc->address_of_index) - index_data_section->get_virtual_address()] = 0;
						}
						if (relocs) {
                            relocs->erase_item(virtual_address + offsetof(image_tls_directory32, address_of_index));
						}
						if (zones) {
							zones->push_back({
								image.va_to_rva(tls_desc->address_of_index) ,
								sizeof(uint32_t)
							});
						}
					}
					if (tls_desc->address_of_callbacks) {
						pe_section * callback_data_section = image.get_section_by_va(tls_desc->address_of_callbacks);
						if (callback_data_section) {
                            uint32_t rva_callbacks = image.va_to_rva(tls_desc->address_of_callbacks);
                            uint32_t * raw_callbacks = (uint32_t*)&callback_data_section->get_section_data().data()[
								image.va_to_rva(tls_desc->address_of_callbacks) - callback_data_section->get_virtual_address()
							];

							while (*raw_callbacks) {
								if (relocs) { relocs->erase_item(rva_callbacks);}
								if (zones)  {zones->push_back({rva_callbacks ,sizeof(uint32_t)});}

								*raw_callbacks = 0;
								raw_callbacks++;
								rva_callbacks += sizeof(uint32_t);
							}

						}

						if (relocs) {
                            relocs->erase_item(virtual_address + offsetof(image_tls_directory32, address_of_callbacks));
						}
					}

					ZeroMemory(tls_raw, virtual_size);

					if (zones) {zones->push_back({virtual_address ,virtual_size});}
				}
				else {
					image_tls_directory64 * tls_desc = (image_tls_directory64 *)tls_raw;
					if (tls_desc->start_address_of_raw_data) {
						pe_section * raw_data_section = image.get_section_by_va(tls_desc->start_address_of_raw_data);
						if (raw_data_section) {
							ZeroMemory(
								&raw_data_section->get_section_data().data()[
									image.va_to_rva(tls_desc->start_address_of_raw_data) - raw_data_section->get_virtual_address()],
                                uint32_t(tls_desc->end_address_of_raw_data - tls_desc->start_address_of_raw_data)
										);
							if (zones) {
								zones->push_back({
									image.va_to_rva(tls_desc->start_address_of_raw_data) ,
									(uint32_t)max(tls_desc->end_address_of_raw_data - tls_desc->start_address_of_raw_data, raw_data_section->get_section_data().size())
								});
							}
						}
						if (relocs) {
                            relocs->erase_item(virtual_address + offsetof(image_tls_directory64, start_address_of_raw_data));
                            relocs->erase_item(virtual_address + offsetof(image_tls_directory64, end_address_of_raw_data));
						}
					}
					if (tls_desc->address_of_index) {
						pe_section * index_data_section = image.get_section_by_va(tls_desc->address_of_index);
						if (index_data_section) {
							*(uint32_t*)&index_data_section->get_section_data().data()[
								image.va_to_rva(tls_desc->address_of_index) - index_data_section->get_virtual_address()] = 0;
						}
						if (relocs) {
                            relocs->erase_item(virtual_address + offsetof(image_tls_directory64, address_of_index));
						}
						if (zones) {
							zones->push_back({
								image.va_to_rva(tls_desc->address_of_index) ,
								sizeof(uint32_t)
							});
						}
					}
					if (tls_desc->address_of_callbacks) {
						pe_section * callback_data_section = image.get_section_by_va(tls_desc->address_of_callbacks);
						if (callback_data_section) {
                            uint32_t rva_callbacks = image.va_to_rva(tls_desc->address_of_callbacks);
                            uint64_t * raw_callbacks = (uint64_t*)&callback_data_section->get_section_data().data()[
								image.va_to_rva(tls_desc->address_of_callbacks) - callback_data_section->get_virtual_address()
							];

							while (*raw_callbacks) {
								if (relocs) { relocs->erase_item(rva_callbacks); }
								if (zones) { zones->push_back({ rva_callbacks ,sizeof(uint64_t) }); }

								*raw_callbacks = 0;
								raw_callbacks++;
								rva_callbacks += sizeof(uint64_t);
							}

						}
						if (relocs) {
                            relocs->erase_item(virtual_address + offsetof(image_tls_directory64, address_of_callbacks));
						}
					}

					ZeroMemory(tls_raw, virtual_size);
					if (zones) { zones->push_back({ virtual_address ,virtual_size }); }
				}
				
				image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_TLS, 0);
				image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_TLS, 0);
				return true;
			}
		}
	}

	return false;
}