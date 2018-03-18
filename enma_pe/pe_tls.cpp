#include "stdafx.h"
#include "pe_tls.h"


tls_table::tls_table() {
	start_address_raw_data	= 0;
	end_address_of_raw_data = 0;
	address_of_index		= 0;
	address_of_callbacks	= 0;
	raw_data.clear();
	callbacks.clear();
}
tls_table::~tls_table(){}


tls_table& tls_table::operator=(const tls_table& tls) {

	start_address_raw_data	= tls.start_address_raw_data;
	end_address_of_raw_data = tls.end_address_of_raw_data;
	address_of_index		= tls.address_of_index;
	address_of_callbacks	= tls.address_of_callbacks;

	raw_data = tls.raw_data;
	callbacks = tls.callbacks;

	return *this;
}


void tls_table::set_start_address_raw_data(DWORD   start_address_raw_data) {
	this->start_address_raw_data = start_address_raw_data;
}
void tls_table::set_end_address_of_raw_data(DWORD   end_address_of_raw_data) {
	this->end_address_of_raw_data = end_address_of_raw_data;
}
void tls_table::set_address_of_index(DWORD   address_of_index) {
	this->address_of_index = address_of_index;
}
void tls_table::set_address_of_callbacks(DWORD   address_of_callbacks){
	this->address_of_callbacks = address_of_callbacks;
}
void tls_table::set_size_of_zero_fill(DWORD   size_of_zero_fill) {
    this->size_of_zero_fill = size_of_zero_fill;
}
void tls_table::set_characteristics(DWORD   characteristics) {
	this->characteristics = characteristics;
}

DWORD tls_table::get_start_address_raw_data() const {
	return this->start_address_raw_data;
}
DWORD tls_table::get_end_address_of_raw_data() const {
	return this->end_address_of_raw_data;
}
DWORD tls_table::get_address_of_index() const {
	return this->address_of_index;
}
DWORD tls_table::get_address_of_callbacks() const {
	return this->address_of_callbacks;
}
DWORD tls_table::get_size_of_zero_fill() const {
    return this->size_of_zero_fill;
}
DWORD tls_table::get_characteristics() const {
	return this->characteristics;
}
std::vector<BYTE>& tls_table::get_raw_data() {
	return raw_data;
}
std::vector<tls_table::tls_callback>& tls_table::get_callbacks() {
	return callbacks;
}

bool get_tls_table(const pe_image &image, tls_table& tls) {

    tls.set_start_address_raw_data(0);
    tls.set_end_address_of_raw_data(0);
    tls.set_address_of_index(0);
    tls.set_address_of_callbacks(0);
    tls.set_characteristics(0);
    tls.get_callbacks().clear();

	DWORD virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_TLS);
	DWORD virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_TLS);

	if (virtual_address && virtual_size) {
		pe_section * tls_section = image.get_section_by_rva(virtual_address);

		if (tls_section) {
			BYTE * tls_raw = &tls_section->get_section_data().data()[(DWORD)(virtual_address - tls_section->get_virtual_address())];
			if (tls_section->get_size_of_raw_data() >= virtual_size) {

				if (image.is_x32_image()) {
					IMAGE_TLS_DIRECTORY32 * tls_desc = (IMAGE_TLS_DIRECTORY32 *)tls_raw;
					if (tls_desc->StartAddressOfRawData) {
						tls.set_start_address_raw_data(image.va_to_rva(tls_desc->StartAddressOfRawData));
						tls.set_end_address_of_raw_data(image.va_to_rva(tls_desc->EndAddressOfRawData));

						pe_section * raw_data_section = image.get_section_by_va(tls_desc->StartAddressOfRawData);

						DWORD data_raw_size = (tls_desc->EndAddressOfRawData - tls_desc->StartAddressOfRawData);
						DWORD data_raw = image.va_to_raw(tls_desc->StartAddressOfRawData);
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
						tls.set_end_address_of_raw_data(0);
						tls.get_raw_data().clear();
					}

					if (tls_desc->AddressOfIndex) {
						tls.set_address_of_index(image.va_to_rva(tls_desc->AddressOfIndex));
					}
					else {
						tls.set_address_of_index(0);
					}

					if (tls_desc->AddressOfCallBacks) {
						pe_section * callback_data_section = image.get_section_by_va(tls_desc->AddressOfCallBacks);
						if (callback_data_section) {
							tls.get_callbacks().clear();

							DWORD * raw_callbacks = (DWORD*)&callback_data_section->get_section_data().data()[
								image.va_to_rva(tls_desc->AddressOfCallBacks) - callback_data_section->get_virtual_address()
							];

							while (*raw_callbacks) {
								tls.get_callbacks().push_back({ image.va_to_rva(*raw_callbacks),true });
								raw_callbacks++;
							}

						}

						tls.set_address_of_callbacks(image.va_to_rva(tls_desc->AddressOfCallBacks));
					}
					else {
						tls.set_address_of_callbacks(0);
					}

					tls.set_characteristics(tls_desc->Characteristics);
                    tls.set_size_of_zero_fill(tls_desc->SizeOfZeroFill);
				}
				else {
					IMAGE_TLS_DIRECTORY64 * tls_desc = (IMAGE_TLS_DIRECTORY64 *)tls_raw;
					if (tls_desc->StartAddressOfRawData) {
						tls.set_start_address_raw_data(image.va_to_rva(tls_desc->StartAddressOfRawData));
						tls.set_end_address_of_raw_data(image.va_to_rva(tls_desc->EndAddressOfRawData));

						pe_section * raw_data_section = image.get_section_by_va(tls_desc->StartAddressOfRawData);

						DWORD data_raw_size = DWORD(tls_desc->EndAddressOfRawData - tls_desc->StartAddressOfRawData);
						DWORD data_raw = image.va_to_raw(tls_desc->StartAddressOfRawData);
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
						tls.set_end_address_of_raw_data(0);
						tls.get_raw_data().clear();
					}

					if (tls_desc->AddressOfIndex) {
						tls.set_address_of_index(image.va_to_rva(tls_desc->AddressOfIndex));
					}
					else {
						tls.set_address_of_index(0);
					}

					if (tls_desc->AddressOfCallBacks) {
						pe_section * callback_data_section = image.get_section_by_va(tls_desc->AddressOfCallBacks);
						if (callback_data_section) {
							tls.get_callbacks().clear();

							DWORD64 * raw_callbacks = (DWORD64*)&callback_data_section->get_section_data().data()[
								image.va_to_rva(tls_desc->AddressOfCallBacks) - callback_data_section->get_virtual_address()
							];

							while (*raw_callbacks) {
								tls.get_callbacks().push_back({ image.va_to_rva(*raw_callbacks),true });
								raw_callbacks++;
							}

						}

						tls.set_address_of_callbacks(image.va_to_rva(tls_desc->AddressOfCallBacks));
					}
					else {
						tls.set_address_of_callbacks(0);
					}

					tls.set_characteristics(tls_desc->Characteristics);
                    tls.set_size_of_zero_fill(tls_desc->SizeOfZeroFill);
				}
				return true;
			}
		}
	}

	return false;
}

void build_tls_table(pe_image &image, pe_section& section, tls_table& tls, relocation_table& relocs) {

    if (section.get_size_of_raw_data() & 0xF) {
        section.get_section_data().resize(
            section.get_section_data().size() + (0x10 - (section.get_section_data().size() & 0xF))
        );
    }

    if (image.is_x32_image()) {
        IMAGE_TLS_DIRECTORY32 tls_desc;
        ZeroMemory(&tls_desc, sizeof(tls_desc));

        DWORD tls_directory_rva = section.get_virtual_address() + section.get_size_of_raw_data();
        DWORD rva_tls_index = tls_directory_rva + sizeof(IMAGE_TLS_DIRECTORY32);
        DWORD rva_tls_callbacks = 0;
        DWORD nulldata = 0;

        tls_desc.AddressOfIndex = (DWORD)image.rva_to_va(tls_directory_rva + sizeof(IMAGE_TLS_DIRECTORY32));
        relocs.add_item(tls_directory_rva + offsetof(IMAGE_TLS_DIRECTORY32, AddressOfIndex),0);

        if (tls.get_raw_data().size()) {
            tls_desc.StartAddressOfRawData = (DWORD)image.rva_to_va(tls_directory_rva + sizeof(IMAGE_TLS_DIRECTORY32) + sizeof(DWORD));

            tls_desc.EndAddressOfRawData =
                tls_desc.StartAddressOfRawData + tls.get_raw_data().size();

            relocs.add_item(tls_directory_rva + offsetof(IMAGE_TLS_DIRECTORY32, StartAddressOfRawData),0);
            relocs.add_item(tls_directory_rva + offsetof(IMAGE_TLS_DIRECTORY32, EndAddressOfRawData),0);
        }
        else {
            tls_desc.StartAddressOfRawData = 0;
            tls_desc.EndAddressOfRawData = 0;
        }

        if (tls.get_callbacks().size()) {
            tls_desc.AddressOfCallBacks = (DWORD)image.rva_to_va(tls_directory_rva + sizeof(IMAGE_TLS_DIRECTORY32) + sizeof(DWORD));
            if (tls.get_raw_data().size()) {
                tls_desc.AddressOfCallBacks += tls.get_raw_data().size();
            }
            rva_tls_callbacks = image.va_to_rva(tls_desc.AddressOfCallBacks);

            relocs.add_item(tls_directory_rva + offsetof(IMAGE_TLS_DIRECTORY32, AddressOfCallBacks),0);
        }

        tls_desc.SizeOfZeroFill = tls.get_size_of_zero_fill();
        tls_desc.Characteristics = tls.get_characteristics();


        section.add_data((BYTE*)&tls_desc, sizeof(tls_desc));//desc
        section.add_data((BYTE*)&nulldata, sizeof(nulldata));

        if (tls.get_raw_data().size()) {
            section.add_data(tls.get_raw_data().data(), tls.get_raw_data().size());
        }

        if (tls.get_callbacks().size()) {
            
            for (auto& callback_item : tls.get_callbacks()) {
                DWORD call_back_va =  callback_item.rva_callback;
                if (callback_item.use_relocation) {
                    call_back_va = (DWORD)image.rva_to_va(call_back_va);
                    relocs.add_item(section.get_virtual_address() + section.get_size_of_raw_data(),0);//fixup
                }

                section.add_data((BYTE*)&call_back_va, sizeof(call_back_va));
            }
        }

        section.add_data((BYTE*)&nulldata, sizeof(nulldata));


        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_TLS, tls_directory_rva);
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_TLS, 
            (section.get_virtual_address() + section.get_size_of_raw_data()) - tls_directory_rva);
    }
    else {
        IMAGE_TLS_DIRECTORY64 tls_desc;
        ZeroMemory(&tls_desc, sizeof(tls_desc));

        DWORD tls_directory_rva = section.get_virtual_address() + section.get_size_of_raw_data();
        DWORD rva_tls_index = tls_directory_rva + sizeof(IMAGE_TLS_DIRECTORY64);
        DWORD rva_tls_callbacks = 0;
        DWORD nulldata = 0;

        tls_desc.AddressOfIndex = image.rva_to_va(tls_directory_rva + sizeof(IMAGE_TLS_DIRECTORY64));
        relocs.add_item(tls_directory_rva + offsetof(IMAGE_TLS_DIRECTORY64, AddressOfIndex),0);


        if (tls.get_raw_data().size()) {
            tls_desc.StartAddressOfRawData = image.rva_to_va(tls_directory_rva + sizeof(IMAGE_TLS_DIRECTORY64) + sizeof(DWORD));
            tls_desc.EndAddressOfRawData = tls_desc.StartAddressOfRawData + tls.get_raw_data().size();
            relocs.add_item(tls_directory_rva + offsetof(IMAGE_TLS_DIRECTORY64, StartAddressOfRawData),0);
            relocs.add_item(tls_directory_rva + offsetof(IMAGE_TLS_DIRECTORY64, EndAddressOfRawData),0);
        }
        else {
            tls_desc.StartAddressOfRawData = 0;
            tls_desc.EndAddressOfRawData = 0;
        }

        if (tls.get_callbacks().size()) {
            tls_desc.AddressOfCallBacks = image.rva_to_va(tls_directory_rva + sizeof(IMAGE_TLS_DIRECTORY64) + sizeof(DWORD));
            if (tls.get_raw_data().size()) {
                tls_desc.AddressOfCallBacks += tls.get_raw_data().size();
            }
            rva_tls_callbacks = image.va_to_rva(tls_desc.AddressOfCallBacks);
            relocs.add_item(tls_directory_rva + offsetof(IMAGE_TLS_DIRECTORY64, AddressOfCallBacks),0);
        }

        tls_desc.SizeOfZeroFill = tls.get_size_of_zero_fill();
        tls_desc.Characteristics = tls.get_characteristics();


        section.add_data((BYTE*)&tls_desc, sizeof(tls_desc));//desc
        section.add_data((BYTE*)&nulldata, sizeof(DWORD));

        if (tls.get_raw_data().size()) {
            section.add_data(tls.get_raw_data().data(), tls.get_raw_data().size());
        }

        if (tls.get_callbacks().size()) {

            for (auto& callback_item : tls.get_callbacks()) {
                DWORD64 call_back_va = callback_item.rva_callback;
                if (callback_item.use_relocation) {
                    call_back_va = image.rva_to_va(callback_item.rva_callback);
                    relocs.add_item(section.get_virtual_address() + section.get_size_of_raw_data(),0);//fixup
                }

                section.add_data((BYTE*)&call_back_va, sizeof(call_back_va));
            }
        }

        section.add_data((BYTE*)&nulldata, sizeof(nulldata));

        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_TLS, tls_directory_rva);
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_TLS,
            (section.get_virtual_address() + section.get_size_of_raw_data()) - tls_directory_rva);
    }


}

bool erase_tls_table(pe_image &image,std::vector<erased_zone>* zones, relocation_table* relocs) {

	DWORD virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_TLS);
	DWORD virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_TLS);

	if (virtual_address && virtual_size) {
		pe_section * tls_section = image.get_section_by_rva(virtual_address);
		
		if (tls_section) {
			BYTE * tls_raw = &tls_section->get_section_data().data()[(DWORD)(virtual_address - tls_section->get_virtual_address())];
			if (tls_section->get_size_of_raw_data() >= virtual_size) {

				if (image.is_x32_image()) {
					IMAGE_TLS_DIRECTORY32 * tls_desc = (IMAGE_TLS_DIRECTORY32 *)tls_raw;
					if (tls_desc->StartAddressOfRawData) {
						pe_section * raw_data_section = image.get_section_by_va(tls_desc->StartAddressOfRawData);
						if (raw_data_section) {				
							ZeroMemory(
								&raw_data_section->get_section_data().data()[
									image.va_to_rva(tls_desc->StartAddressOfRawData) - raw_data_section->get_virtual_address()],
								min(tls_desc->EndAddressOfRawData - tls_desc->StartAddressOfRawData, raw_data_section->get_section_data().size())
							);
							if (zones) {
								zones->push_back({
									image.va_to_rva(tls_desc->StartAddressOfRawData) ,
									max(tls_desc->EndAddressOfRawData - tls_desc->StartAddressOfRawData, raw_data_section->get_section_data().size())
								});
							}
						}
						if (relocs) {
                            relocs->erase_item(virtual_address + offsetof(IMAGE_TLS_DIRECTORY32, StartAddressOfRawData));
                            relocs->erase_item(virtual_address + offsetof(IMAGE_TLS_DIRECTORY32, EndAddressOfRawData));
						}
					}
					if (tls_desc->AddressOfIndex) {
						pe_section * index_data_section = image.get_section_by_va(tls_desc->AddressOfIndex);
						if (index_data_section) {
							*(DWORD*)&index_data_section->get_section_data().data()[
								image.va_to_rva(tls_desc->AddressOfIndex) - index_data_section->get_virtual_address()] = 0;
						}
						if (relocs) {
                            relocs->erase_item(virtual_address + offsetof(IMAGE_TLS_DIRECTORY32, AddressOfIndex));
						}
						if (zones) {
							zones->push_back({
								image.va_to_rva(tls_desc->AddressOfIndex) ,
								sizeof(DWORD)
							});
						}
					}
					if (tls_desc->AddressOfCallBacks) {
						pe_section * callback_data_section = image.get_section_by_va(tls_desc->AddressOfCallBacks);
						if (callback_data_section) {
							DWORD rva_callbacks = image.va_to_rva(tls_desc->AddressOfCallBacks);
							DWORD * raw_callbacks = (DWORD*)&callback_data_section->get_section_data().data()[
								image.va_to_rva(tls_desc->AddressOfCallBacks) - callback_data_section->get_virtual_address()
							];

							while (*raw_callbacks) {
								if (relocs) { relocs->erase_item(rva_callbacks);}
								if (zones)  {zones->push_back({rva_callbacks ,sizeof(DWORD)});}

								*raw_callbacks = 0;
								raw_callbacks++;
								rva_callbacks += sizeof(DWORD);
							}

						}

						if (relocs) {
                            relocs->erase_item(virtual_address + offsetof(IMAGE_TLS_DIRECTORY32, AddressOfCallBacks));
						}
					}

					ZeroMemory(tls_raw, virtual_size);

					if (zones) {zones->push_back({virtual_address ,virtual_size});}
				}
				else {
					IMAGE_TLS_DIRECTORY64 * tls_desc = (IMAGE_TLS_DIRECTORY64 *)tls_raw;
					if (tls_desc->StartAddressOfRawData) {
						pe_section * raw_data_section = image.get_section_by_va(tls_desc->StartAddressOfRawData);
						if (raw_data_section) {
							ZeroMemory(
								&raw_data_section->get_section_data().data()[
									image.va_to_rva(tls_desc->StartAddressOfRawData) - raw_data_section->get_virtual_address()],
								DWORD(tls_desc->EndAddressOfRawData - tls_desc->StartAddressOfRawData)
										);
							if (zones) {
								zones->push_back({
									image.va_to_rva(tls_desc->StartAddressOfRawData) ,
									(unsigned int)max(tls_desc->EndAddressOfRawData - tls_desc->StartAddressOfRawData, raw_data_section->get_section_data().size())
								});
							}
						}
						if (relocs) {
                            relocs->erase_item(virtual_address + offsetof(IMAGE_TLS_DIRECTORY64, StartAddressOfRawData));
                            relocs->erase_item(virtual_address + offsetof(IMAGE_TLS_DIRECTORY64, EndAddressOfRawData));
						}
					}
					if (tls_desc->AddressOfIndex) {
						pe_section * index_data_section = image.get_section_by_va(tls_desc->AddressOfIndex);
						if (index_data_section) {
							*(DWORD*)&index_data_section->get_section_data().data()[
								image.va_to_rva(tls_desc->AddressOfIndex) - index_data_section->get_virtual_address()] = 0;
						}
						if (relocs) {
                            relocs->erase_item(virtual_address + offsetof(IMAGE_TLS_DIRECTORY64, AddressOfIndex));
						}
						if (zones) {
							zones->push_back({
								image.va_to_rva(tls_desc->AddressOfIndex) ,
								sizeof(DWORD)
							});
						}
					}
					if (tls_desc->AddressOfCallBacks) {
						pe_section * callback_data_section = image.get_section_by_va(tls_desc->AddressOfCallBacks);
						if (callback_data_section) {
							DWORD rva_callbacks = image.va_to_rva(tls_desc->AddressOfCallBacks);
							DWORD64 * raw_callbacks = (DWORD64*)&callback_data_section->get_section_data().data()[
								image.va_to_rva(tls_desc->AddressOfCallBacks) - callback_data_section->get_virtual_address()
							];

							while (*raw_callbacks) {
								if (relocs) { relocs->erase_item(rva_callbacks); }
								if (zones) { zones->push_back({ rva_callbacks ,sizeof(DWORD64) }); }

								*raw_callbacks = 0;
								raw_callbacks++;
								rva_callbacks += sizeof(DWORD64);
							}

						}
						if (relocs) {
                            relocs->erase_item(virtual_address + offsetof(IMAGE_TLS_DIRECTORY64, AddressOfCallBacks));
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