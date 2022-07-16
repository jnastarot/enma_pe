#pragma once

namespace enma {

    namespace hl {

        struct pe_image_icon_group {

        private:
            bool _valid_group;
            
            uint16_t _type;

            std::list<pe_resource_internal::group_icon_dir_entry> _entries;

            std::vector<uint8_t> _group_data;

        public:
            pe_image_icon_group()
                : _valid_group(false)
                , _type(0) {}
            pe_image_icon_group(const uint8_t* group_info, size_t group_info_size);

            ~pe_image_icon_group() {}

        public:
            bool parse(const uint8_t* group_info, size_t group_info_size);
            std::vector<uint8_t> compile();

            bool get_icon(pe_resource_directory& _resources,
                uint16_t used_language_id,
                std::vector<uint8_t>& icon_buf);

        public:
            constexpr bool is_valid_group() const {
                return _valid_group;
            }
            constexpr uint16_t get_type() const {
                return _type;
            }
            constexpr std::list<pe_resource_internal::group_icon_dir_entry>& get_entries() {
                return _entries;
            }
            constexpr const std::list<pe_resource_internal::group_icon_dir_entry>& get_entries() const{
                return _entries;
            }

        public:
            constexpr void set_valid_group(bool valid) {
                _valid_group = valid;
            }
        };
    }
}