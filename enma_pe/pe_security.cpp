#include "stdafx.h"
#include "pe_security.h"


security_item::security_item() {
    revision = 0;
    certificate_type = 0;
}

security_item::security_item(uint16_t revision, uint16_t certificate_type, std::vector<BYTE>& certificate_data)
:revision(revision), certificate_type(certificate_type), certificate_data(certificate_data){}

security_item::security_item(const security_item& item) {
    this->operator=(item);
}
security_item::~security_item() {

}

security_item& security_item::operator=(const security_item& item) {

    revision = item.revision;
    certificate_type = item.revision;
    certificate_data = item.certificate_data;

    return *this;
}

void   security_item::set_revision(uint16_t revision) {
    this->revision = revision;
}
void   security_item::set_certificate_type(uint16_t type) {
    this->certificate_type = type;
}

void security_item::set_certificate_data(std::vector<BYTE>& data) {
    this->certificate_data = data;
}

uint16_t   security_item::get_revision() const {
    return this->revision;
}

uint16_t   security_item::get_certificate_type() const {
    return this->certificate_type;
}

std::vector<BYTE>& security_item::get_certificate_data() {
    return this->certificate_data;
}

security_table::security_table() {

}

security_table::security_table(const security_table& security) {
    this->operator=(security);
}

security_table::~security_table() {

}

security_table& security_table::operator=(const security_table& security) {

    this->certificates = security.certificates;

    return *this;
}


void security_table::add_certificate(const security_item& item) {
    this->certificates.push_back(item);
}

void security_table::clear() {
    this->certificates.clear();
}

size_t security_table::get_certificates_count() const {
    return this->certificates.size();
}

std::vector<security_item>& security_table::get_certificates() {
    return this->certificates;
}

directory_code get_security_table(const pe_image &image, security_table& security) {
    security.clear();

    uint32_t raw_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_SECURITY);
    uint32_t virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_SECURITY);

    if (raw_address && virtual_size) {
        pe_image_io security_io(image, enma_io_address_raw);
        
        uint32_t total_size = 0;
        security_io.set_image_offset(raw_address);

        do {
            win_certificate win_cert;
            std::vector<BYTE> data;

            if (security_io.read(&win_cert, sizeof(win_cert)) != enma_io_success) {
                return directory_code::directory_code_currupted;
            }
            
            if (security_io.read(data, win_cert.length - sizeof(win_certificate)) != enma_io_success) {
                return directory_code::directory_code_currupted;
            }

            security.add_certificate(security_item(win_cert.w_revision, win_cert.w_certificate_type, data));

            total_size += win_cert.length;
        } while (total_size < virtual_size);


        return directory_code::directory_code_success;
    }

    return directory_code::directory_code_not_present;
}


directory_code get_placement_security_table(pe_image &image, std::vector<directory_placement>& placement) {

    uint32_t raw_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_SECURITY);
    uint32_t virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_SECURITY);
    
    if (raw_address && virtual_size) {

        pe_image_io security_io(image,enma_io_address_raw);

        uint32_t _offset_real = 0;
        uint32_t available_size = 0;
        uint32_t down_oversize = 0;
        uint32_t up_oversize = 0;

        security_io.view_image(
            raw_address, virtual_size,
            _offset_real,
            available_size, down_oversize, up_oversize
        );

        uint32_t sec_rva = image.raw_to_rva(raw_address);
        
        if (sec_rva) {
            placement.push_back({ sec_rva , available_size, dp_id_security_desc });
        }

        if (!down_oversize && !up_oversize) {
            return directory_code::directory_code_success;
        }

        return directory_code::directory_code_currupted;
    }

    return directory_code::directory_code_not_present;
}