#pragma once

namespace enma {

    namespace hl {

        typedef struct _vs_string_translation {
            uint16_t language_id;
            uint16_t code_page_id;

            bool operator<(const _vs_string_translation& entry) const {
                return this->language_id < entry.language_id&&
                    this->code_page_id < entry.code_page_id;
            }

        } vs_string_translation;

        typedef std::list<std::pair<std::wstring, std::wstring>> vs_strings; //key - value

        struct pe_image_version_info {

        private:
            bool _valid_info;
            bool _valid_file_info;

            struct image_info_t {
                uint32_t struct_version;
                uint32_t file_version_ms;
                uint32_t file_version_ls;
                uint32_t product_version_ms;
                uint32_t product_version_ls;
                uint32_t file_flags_mask;
                uint32_t file_flags;
                uint32_t file_os;
                uint32_t file_type;
                uint32_t file_sub_type;
                uint32_t file_date_ms;
                uint32_t file_date_ls;
            } _file_info;

            std::map<vs_string_translation, vs_strings> _vs_string_table;
            std::vector<vs_string_translation> _supported_translation;

            bool parse_string(const uint8_t* _string, size_t _string_size, vs_strings& strings);
            bool parse_string_table(const uint8_t* _string_table, size_t _string_table_size);
            bool parse_string_file_info(const uint8_t* _string_file_info, size_t _string_file_info_size);
            bool parse_var_info(const uint8_t* _var_info, size_t _var_info_size);
            bool parse_var_file_info(const uint8_t* _var_file_info, size_t _var_file_info_size);
        public:

            pe_image_version_info()
                : _valid_info(false)
                , _valid_file_info(false)
                , _file_info({ 0 }) {}

            pe_image_version_info(const uint8_t* version_info, size_t version_info_size);
            ~pe_image_version_info() {}

        public:
            bool parse(const uint8_t* version_info, size_t version_info_size);
            std::vector<uint8_t> compile();

        public:
            constexpr bool is_valid_info() const {
                return _valid_info;
            }
            constexpr bool has_file_info() const {
                return _valid_file_info;
            }
            constexpr const std::map<vs_string_translation, vs_strings>& get_string_table() const {
                return _vs_string_table;
            }
            constexpr std::map<vs_string_translation, vs_strings>& get_string_table() {
                return _vs_string_table;
            }
            constexpr const std::vector<vs_string_translation>& get_supported_translation() const {
                return _supported_translation;
            }
            constexpr std::vector<vs_string_translation>& get_supported_translation() {
                return _supported_translation;
            }
            constexpr const image_info_t& get_file_info() const {
                return _file_info;
            }
            constexpr image_info_t& get_file_info() {
                return _file_info;
            }

        public:
            constexpr void set_valid_info(bool valid) {
                _valid_info = valid;
            }
            constexpr void set_valid_file_info(bool valid) {
                _valid_file_info = valid;
            }
        };
    }
}