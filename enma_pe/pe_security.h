#pragma once

class security_item {
    uint16_t   revision;
    uint16_t   certificate_type;

    std::vector<uint8_t> certificate_data;
public:
    security_item::security_item();
    security_item::security_item(uint16_t revision, uint16_t certificate_type, std::vector<uint8_t>& certificate_data);
    security_item::security_item(const security_item& item);
    security_item::~security_item();

    security_item& security_item::operator=(const security_item& item);
public:
    void   security_item::set_revision(uint16_t revision);
    void   security_item::set_certificate_type(uint16_t type);

    void security_item::set_certificate_data(std::vector<uint8_t>& data);

public:
    uint16_t   security_item::get_revision() const;
    uint16_t   security_item::get_certificate_type() const;

    std::vector<uint8_t>& security_item::get_certificate_data();
};


class security_table {
    std::vector<security_item> certificates;
public:
    security_table::security_table();
    security_table::security_table(const security_table& security);
    security_table::~security_table();

    security_table& security_table::operator=(const security_table& security);
public:
    void security_table::add_certificate(const security_item& item);
    void security_table::clear();
public:
    size_t security_table::get_certificates_count() const;

    std::vector<security_item>& security_table::get_certificates();
};

directory_code get_security_table(_In_ const pe_image &image,_Out_ security_table& security);
directory_code get_placement_security_table(_Inout_ pe_image &image,_Inout_ std::vector<directory_placement>& placement);