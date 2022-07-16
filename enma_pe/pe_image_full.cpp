#include "stdafx.h"

using namespace enma;

pe_image_full::pe_image_full(const pe_image& image) {
    operator=(image);
}
pe_image_full::pe_image_full(const pe_image_full& image) {
    operator=(image);
}
pe_image_full::pe_image_full(bool _pe32, bool init_dos_thunk) {
    _code = pe_directory_code_success;
    _image = pe_image(_pe32, init_dos_thunk);
}

pe_image_full::pe_image_full(const uint8_t* raw_image, uint32_t size) {
    _image = pe_image(raw_image, size);

    _code = 0;
    _code |= get_export_directory(_image, _exports);
    _code |= get_bound_import_directory(_image, _bound_imports);
    _code |= get_import_directory(_image, _imports, _bound_imports);
    _code |= get_delay_import_directory(_image, _delay_imports, _bound_imports);
    _code |= get_resources_directory(_image, _resources);
    _code |= get_exception_directory(_image, _exceptions);
    _code |= get_security_directory(_image, _security);
    _code |= get_relocation_directory(_image, _relocations);
    _code |= get_debug_directory(_image, _debug);
    _code |= get_tls_directory(_image, _tls);
    _code |= get_load_config_directory(_image, _load_config);

    _code &= ~(pe_directory_code_not_present);
}

pe_image_full::pe_image_full(const std::string& file_path) {
    _image = pe_image(file_path);

    _code = 0;
    _code |= get_export_directory(_image, _exports);
    _code |= get_bound_import_directory(_image, _bound_imports);
    _code |= get_import_directory(_image, _imports, _bound_imports);
    _code |= get_delay_import_directory(_image, _delay_imports, _bound_imports);
    _code |= get_resources_directory(_image, _resources);
    _code |= get_exception_directory(_image, _exceptions);
    _code |= get_security_directory(_image, _security);
    _code |= get_relocation_directory(_image, _relocations);
    _code |= get_debug_directory(_image, _debug);
    _code |= get_tls_directory(_image, _tls);
    _code |= get_load_config_directory(_image, _load_config);

    _code &= ~(pe_directory_code_not_present);
}

pe_image_full::pe_image_full(const std::wstring& file_path) {
    _image = pe_image(file_path);

    _code = 0;
    _code |= get_export_directory(_image, _exports);
    _code |= get_bound_import_directory(_image, _bound_imports);
    _code |= get_import_directory(_image, _imports, _bound_imports);
    _code |= get_delay_import_directory(_image, _delay_imports, _bound_imports);
    _code |= get_resources_directory(_image, _resources);
    _code |= get_exception_directory(_image, _exceptions);
    _code |= get_security_directory(_image, _security);
    _code |= get_relocation_directory(_image, _relocations);
    _code |= get_debug_directory(_image, _debug);
    _code |= get_tls_directory(_image, _tls);
    _code |= get_load_config_directory(_image, _load_config);

    _code &= ~(pe_directory_code_not_present);
}

pe_image_full& pe_image_full::operator=(const pe_image_full& image) {

    _code = image._code;
    _exports = image._exports;
    _imports = image._imports;
    _resources = image._resources;
    _exceptions = image._exceptions;
    _security = image._security;
    _relocations = image._relocations;
    _debug = image._debug;
    _tls = image._tls;
    _load_config = image._load_config;
    _delay_imports = image._delay_imports;
    _bound_imports = image._bound_imports;
    _image = image._image;

    _code &= ~(pe_directory_code_not_present);

    return *this;
}

pe_image_full& pe_image_full::operator=(const pe_image& image) {

    _image = image;

    _code = 0;
    _code |= get_export_directory(_image, _exports);
    _code |= get_bound_import_directory(_image, _bound_imports);
    _code |= get_import_directory(_image, _imports, _bound_imports);
    _code |= get_delay_import_directory(_image, _delay_imports, _bound_imports);
    _code |= get_resources_directory(_image, _resources);
    _code |= get_exception_directory(_image, _exceptions);
    _code |= get_security_directory(_image, _security);
    _code |= get_relocation_directory(_image, _relocations);
    _code |= get_debug_directory(_image, _debug);
    _code |= get_tls_directory(_image, _tls);
    _code |= get_load_config_directory(_image, _load_config);

    _code &= ~(pe_directory_code_not_present);

    return *this;
}

pe_directory_code get_directories_placement(pe_image &image, pe_placement& placement,
    const pe_bound_import_directory* bound_imports) {

    placement.clear();
    get_placement_export_directory(image, placement);
    get_placement_import_directory(image, placement, bound_imports ? *bound_imports : pe_bound_import_directory());
    get_placement_resources_directory(image, placement);
    get_placement_exceptions_directory(image, placement);
    get_placement_security_directory(image, placement);
    get_placement_relocation_directory(image, placement);
    get_placement_debug_directory(image, placement);
    get_placement_tls_directory(image, placement);
    get_placement_load_config_directory(image, placement);
    get_placement_bound_import_directory(image, placement);
    get_placement_delay_import_directory(image, placement, bound_imports ? *bound_imports : pe_bound_import_directory());

    return pe_directory_code::pe_directory_code_success;
}