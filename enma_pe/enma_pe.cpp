#include "stdafx.h"
#include "enma_pe.h"


pe_image_expanded::pe_image_expanded() {

}

pe_image_expanded::pe_image_expanded(const pe_image_expanded& image_ex) {
    operator=(image_ex);
}

pe_image_expanded& pe_image_expanded::operator=(const pe_image_expanded& image_ex) {
    this->exports = image_ex.exports;
    this->imports = image_ex.imports;
    this->resources = image_ex.resources;
    this->exceptions = image_ex.exceptions;
    this->relocations = image_ex.relocations;
    this->debug = image_ex.debug;
    this->tls = image_ex.tls;
    this->load_config = image_ex.load_config;
    this->delay_imports = image_ex.delay_imports;
    this->bound_imports = image_ex.bound_imports;
    this->image = image_ex.image;
    return *this;
}


void do_expanded_pe_image(pe_image_expanded& expanded_image,const pe_image &image) {
    expanded_image.image = image;
	get_export_table(expanded_image.image, expanded_image.exports);
	get_import_table(expanded_image.image, expanded_image.imports);
//	get_resources_table(expanded_image.image, expanded_image.resources);
//	get_exception_table(expanded_image.image, expanded_image.exceptions);
	get_relocation_table(expanded_image.image, expanded_image.relocations);
	get_debug_table(expanded_image.image, expanded_image.debug);
	get_tls_table(expanded_image.image, expanded_image.tls);
 //   get_load_config_table(expanded_image.image, expanded_image.load_config);
    get_bound_import_table(expanded_image.image, expanded_image.bound_imports);
	get_delay_import_table(expanded_image.image, expanded_image.delay_imports);
}

directory_code get_directories_placement(pe_image &image, std::vector<directory_placement>& placement) {
    placement.clear();
    get_placement_export_table(image, placement);
    get_placement_import_table(image, placement);
    get_placement_resources_table(image, placement);
    get_placement_exceptions_table(image, placement);
    get_placement_relocation_table(image, placement);
    get_placement_debug_table(image, placement);
    get_placement_tls_table(image, placement);
    get_placement_load_config_table(image, placement);
    get_placement_bound_import_table(image, placement);
    get_placement_delay_import_table(image, placement);

    return directory_code::directory_code_success;
}

void erase_directories_pe_image(pe_image &image, std::vector<directory_placement>& placement, relocation_table* relocs, bool delete_empty_sections) {
    
	std::sort(placement.begin(), placement.end(), [](directory_placement& lhs, directory_placement& rhs) {
		return lhs.rva < rhs.rva;
	});

	for (size_t parent_zone_idx = 0; parent_zone_idx + 1 < placement.size(); parent_zone_idx++) { //link zones

		if (placement[parent_zone_idx].rva <= placement[parent_zone_idx + 1].rva &&
			(placement[parent_zone_idx].rva + placement[parent_zone_idx].size) >= placement[parent_zone_idx + 1].rva
			) {

			if ((placement[parent_zone_idx + 1].rva + 
                    placement[parent_zone_idx + 1].size) > (placement[parent_zone_idx].rva + placement[parent_zone_idx].size)) {

                placement[parent_zone_idx].size = 
                    ((placement[parent_zone_idx + 1].rva + placement[parent_zone_idx + 1].size) - placement[parent_zone_idx].rva);
			}

            placement.erase(placement.begin() + parent_zone_idx + 1);
			parent_zone_idx--;
		}
	}


    for (size_t placement_idx = 0; placement_idx < placement.size(); placement_idx--) {


    }


	if (delete_empty_sections && image.get_sections().size()) {
		for (size_t section_idx = image.get_sections().size() - 1; section_idx > 0; section_idx--) {
			auto _section = image.get_sections()[section_idx];

			for (auto& zone : placement) {
				if (_section->get_virtual_address() == zone.rva &&
					_section->get_virtual_size() <= zone.size) {

					image.get_sections().erase(image.get_sections().begin() + section_idx);
					goto go_next_;
				}
			}

			return;
		go_next_:;
		}
	}
}