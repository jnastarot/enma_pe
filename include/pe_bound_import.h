#pragma once

namespace enma {

    struct pe_bound_imported_ref {

    private:
        std::string _ref_name;
        uint32_t _timestamp;

    public:
        pe_bound_imported_ref()
            : _timestamp(0) {}
        pe_bound_imported_ref(const std::string& ref_name, uint32_t  timestamp)
            : _ref_name(ref_name)
            , _timestamp(timestamp) {}

        ~pe_bound_imported_ref() = default;

        pe_bound_imported_ref(const pe_bound_imported_ref&) = default;
        pe_bound_imported_ref& operator=(const pe_bound_imported_ref&) = default;
        pe_bound_imported_ref(pe_bound_imported_ref&&) = default;
        pe_bound_imported_ref& operator=(pe_bound_imported_ref&&) = default;

    public:
        inline void set_ref_name(const std::string& ref_name) {
            _ref_name = ref_name;
        }
        constexpr void set_timestamp(uint32_t  timestamp) {
            _timestamp = timestamp;
        }

    public:
        constexpr const std::string& get_ref_name() const {
            return _ref_name;
        }
        constexpr uint32_t get_timestamp() const {
            return _timestamp;
        }
    };

    struct pe_bound_library {

    private:
        std::string _library_name;
        uint32_t _timestamp;
        std::vector<pe_bound_imported_ref> _refs;

    public:
        pe_bound_library()
            : _timestamp(0) {}

        ~pe_bound_library() = default;

        pe_bound_library(const pe_bound_library&) = default;
        pe_bound_library& operator=(const pe_bound_library&) = default;
        pe_bound_library(pe_bound_library&&) = default;
        pe_bound_library& operator=(pe_bound_library&&) = default;

    public:
        inline void set_library_name(const std::string& library_name) {
            _library_name = library_name;
        }
        constexpr void set_timestamp(uint32_t  timestamp) {
            _timestamp = timestamp;
        }
        inline void add_ref(const pe_bound_imported_ref& ref) {
            _refs.push_back(ref);
        }

    public:
        constexpr const std::string& get_library_name() const {
            return _library_name;
        }
        constexpr uint32_t get_timestamp() const {
            return _timestamp;
        }
        inline size_t get_number_of_forwarder_refs() const {
            return _refs.size();
        }
        constexpr const std::vector<pe_bound_imported_ref>& get_refs() const {
            return _refs;
        }
        constexpr std::vector<pe_bound_imported_ref>& get_refs() {
            return _refs;
        }
    };

    struct pe_bound_import_directory {

    private:
        std::vector<pe_bound_library> _libraries;

    public:
        pe_bound_import_directory() = default;
        ~pe_bound_import_directory() = default;

        pe_bound_import_directory(const pe_bound_import_directory&) = default;
        pe_bound_import_directory& operator=(const pe_bound_import_directory&) = default;
        pe_bound_import_directory(pe_bound_import_directory&&) = default;
        pe_bound_import_directory& operator=(pe_bound_import_directory&&) = default;

    public:
        inline void add_library(const pe_bound_library& lib) {
            _libraries.push_back(lib);
        }
        inline void clear() {
            _libraries.clear();
        }

    public:
        inline size_t size() const {
            return _libraries.size();
        }
        constexpr const std::vector<pe_bound_library>& get_libraries() const {
            return _libraries;
        }
        constexpr std::vector<pe_bound_library>& get_libraries() {
            return _libraries;
        }

        bool has_library(const std::string& library_name, uint32_t timestamp) const;
        bool has_library(const std::string& library_name) const;

        const pe_bound_library* get_library(const std::string& library_name) const;
    };

    pe_directory_code get_bound_import_directory(_In_ const pe_image& image, _Out_ pe_bound_import_directory& imports);
    bool build_bound_import_directory(_Inout_ pe_image& image, _Inout_ pe_section& section,
        _In_ pe_bound_import_directory& imports);
    pe_directory_code get_placement_bound_import_directory(_In_ const pe_image& image, _Inout_ pe_placement& placement);
}