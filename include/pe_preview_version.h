#pragma once

namespace enma {

    namespace hl {

        struct pe_image_preview_version_info {

        private:
            std::list<std::pair<uint16_t, pe_resource_directory_entry*>> _available_languages;

            pe_resource_directory& _resources;

            bool parse_resources();

        public:
            pe_image_preview_version_info(pe_resource_directory& resources);
            ~pe_image_preview_version_info() {}

        public:
            bool get_default_version(pe_image_version_info& ver, uint16_t& language_id);
            bool get_version_by_language(uint16_t language_id, pe_image_version_info& ver);

        public:
            constexpr std::list<std::pair<uint16_t, pe_resource_directory_entry*>>& get_available_languages() {
                return _available_languages;
            }
            constexpr const std::list<std::pair<uint16_t, pe_resource_directory_entry*>>& get_available_languages() const {
                return _available_languages;
            }
        };
    }
}