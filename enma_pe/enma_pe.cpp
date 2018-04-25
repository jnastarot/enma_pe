#include "stdafx.h"
#include "enma_pe.h"


pe_image_expanded::pe_image_expanded() {
    this->code = directory_code::directory_code_not_present;
}

pe_image_expanded::pe_image_expanded(const pe_image_expanded& image_ex) {
    operator=(image_ex);
}

pe_image_expanded& pe_image_expanded::operator=(const pe_image_expanded& image_ex) {
    this->code    = image_ex.code;
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

    get_bound_import_table(expanded_image.image, expanded_image.bound_imports);
	get_import_table(expanded_image.image, expanded_image.imports, expanded_image.bound_imports);
    get_delay_import_table(expanded_image.image, expanded_image.delay_imports, expanded_image.bound_imports);
	
    get_resources_table(expanded_image.image, expanded_image.resources);//
	get_exception_table(expanded_image.image, expanded_image.exceptions);//
	get_relocation_table(expanded_image.image, expanded_image.relocations);
	get_debug_table(expanded_image.image, expanded_image.debug);
	get_tls_table(expanded_image.image, expanded_image.tls);
    get_load_config_table(expanded_image.image, expanded_image.load_config);//
}

directory_code get_directories_placement(pe_image &image, std::vector<directory_placement>& placement,
    const bound_import_table* bound_imports) {

    if (!bound_imports) {
        bound_imports = &bound_import_table();
    }

    placement.clear();
    get_placement_export_table(image, placement);
    get_placement_import_table(image, placement, *bound_imports);
    get_placement_resources_table(image, placement);
    get_placement_exceptions_table(image, placement);
    get_placement_relocation_table(image, placement);
    get_placement_debug_table(image, placement);
    get_placement_tls_table(image, placement);
    get_placement_load_config_table(image, placement);
    get_placement_bound_import_table(image, placement);
    get_placement_delay_import_table(image, placement, *bound_imports);

    return directory_code::directory_code_success;
}