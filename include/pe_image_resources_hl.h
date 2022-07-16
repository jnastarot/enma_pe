#pragma once

namespace enma {

    namespace hl {

		enum pe_resource_type {
			kResourceCursor = 1,
			kResourceBitmap = 2,
			kResourceIcon	= 3,
			kResourceMenu	= 4,
			kResourceDialog = 5,
			kResourceString = 6,
			kResourceFontDir = 7,
			kResourceFont	 = 8,
			kResourceAccelerator = 9,
			kResourceRCData		 = 10,
			kResourceMessageTable = 11,
			kResourceCursorGroup  = 12,
			kResourceIconGroup	  = 14,
			kResourceversion	  = 16,
			kResourceDlgInclude	  = 17,
			kResourcePlugPlay	  = 19,
			kResourceVxd		  = 20,
			kResourceAniCursor	  = 21,
			kResourceAniIcon	  = 22,
			kResourceHtml		  = 23,
			kResourceManifest	  = 24
		};

        struct pe_image_resources_hl {

        private:
            pe_resource_directory& _resources;

        public:
            pe_image_resources_hl(pe_resource_directory& resources)
                : _resources(resources) {}

            ~pe_image_resources_hl() {}

        public:
            pe_resource_directory_entry* get_directory(uint16_t type_id);
            pe_resource_directory_entry* get_directory(const std::wstring& type_name);
            pe_resource_directory_entry* get_directory(uint16_t type_id, uint16_t resource_id);
            pe_resource_directory_entry* get_directory(const std::wstring& type_name, uint16_t resource_id);
            pe_resource_directory_entry* get_directory(uint16_t type_id, const std::wstring& resource_name);
            pe_resource_directory_entry* get_directory(const std::wstring& type_name, const std::wstring& resource_name);
            pe_resource_directory_entry* get_directory(const std::wstring& type_name, uint16_t resource_id, uint16_t language_id);
            pe_resource_directory_entry* get_directory(uint16_t type_id, const std::wstring& resource_name, uint16_t language_id);
            pe_resource_directory_entry* get_directory(uint16_t type_id, uint16_t resource_id, uint16_t language_id);
			pe_resource_directory_entry* get_directory(const std::wstring& type_name, const std::wstring& resource_name, uint16_t language_id);

        };
    }
}


#include "pe_image_version_info.h"
#include "pe_image_icon_groups.h"
#include "pe_preview_icon.h"
#include "pe_preview_version.h"