#pragma once

namespace enma {

    struct pe_security_entry {

    private:
        uint16_t _revision;
        uint16_t _certificate_type;

        std::vector<uint8_t> _certificate_data;

    public:
        pe_security_entry()
            : _revision(0)
            , _certificate_type(0) {}

        pe_security_entry(uint16_t revision, uint16_t certificate_type, std::vector<uint8_t>& certificate_data)
            : _revision(revision)
            , _certificate_type(certificate_type)
            , _certificate_data(certificate_data) {}

        ~pe_security_entry() = default;

        pe_security_entry(const pe_security_entry&) = default;
        pe_security_entry& operator=(const pe_security_entry&) = default;
        pe_security_entry(pe_security_entry&&) = default;
        pe_security_entry& operator=(pe_security_entry&&) = default;

    public:
        constexpr void set_revision(uint16_t revision) {
            _revision = revision;
        }
        constexpr void set_certificate_type(uint16_t type) {
            _certificate_type = type;
        }
        inline void set_certificate_data(const std::vector<uint8_t>& data) {
            _certificate_data = data;
        }

    public:
        constexpr uint16_t get_revision() const {
            return _revision;
        }
        constexpr uint16_t get_certificate_type() const {
            return _certificate_type;
        }
        constexpr const std::vector<uint8_t>& get_certificate_data() const {
            return _certificate_data;
        }
        constexpr std::vector<uint8_t>& get_certificate_data() {
            return _certificate_data;
        }
    };


    struct pe_security_directory {

    private:
        std::vector<pe_security_entry> _certificates;

    public:
        pe_security_directory() = default;
        ~pe_security_directory() = default;

        pe_security_directory(const pe_security_directory&) = default;
        pe_security_directory& operator=(const pe_security_directory&) = default;
        pe_security_directory(pe_security_directory&&) = default;
        pe_security_directory& operator=(pe_security_directory&&) = default;

    public:
        inline void add_certificate(const pe_security_entry& item) {
            _certificates.push_back(item);
        }
        inline void clear() {
            _certificates.clear();
        }

    public:
        inline size_t get_certificates_count() const {
            return _certificates.size();
        }
        constexpr const std::vector<pe_security_entry>& get_certificates() const {
            return _certificates;
        }
        constexpr std::vector<pe_security_entry>& get_certificates() {
            return _certificates;
        }
    };

    pe_directory_code get_security_directory(_In_ const pe_image& image, _Out_ pe_security_directory& security);
    pe_directory_code get_placement_security_directory(_Inout_ const pe_image& image, _Inout_ pe_placement& placement);

}