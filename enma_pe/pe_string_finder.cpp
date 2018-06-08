#include "stdafx.h"
#include "pe_string_finder.h"




void get_strings_from_image_by_locale(const pe_image& image, string_base_table& string_table,
    const char * locale,
    bool find_in_clean_image,
    bool find_in_execute_sections,
    uint32_t min_string_size,
    uint32_t max_string_size) {

    pe_image current_image = image;

    if (find_in_clean_image) {
        std::vector<directory_placement> placement;
        get_directories_placement(current_image, placement);
        erase_directories_placement(current_image, placement, 0, true);
    }

    if (!locale) {
        locale = "en-EN";
    }

    string_table.locale_name = locale;
    string_table.ansi_base.clear();
    string_table.wide_base.clear();

    /*
    _locale_t loc = _create_locale(LC_CTYPE, locale);


    //find ansi strings
    {
        pe_image_io ansi_string_io(current_image);
        ansi_string_io.seek_to_start();

        char image_byte;
        uint32_t pre_string_rva;
        std::string pre_string;
        while (ansi_string_io.read(&image_byte, sizeof(image_byte)) == enma_io_success) {
            if ( (!find_in_execute_sections || ansi_string_io.is_executable_rva(ansi_string_io.get_image_offset() - 1)) &&
                _isprint_l(image_byte, loc) || _isblank_l(image_byte, loc)) {

                if (!pre_string.size()) {
                    pre_string_rva = ansi_string_io.get_image_offset() - 1;
                }

                pre_string += image_byte;
            }
            else {
                if (image_byte == 0 &&
                    pre_string.size() >= MIN_STRING_SIZE && pre_string.size() <= MAX_STRING_SIZE) {

                    string_table.ansi_base.push_back({ pre_string_rva , pre_string });
                    pre_string.clear();
                }
                else {
                    pre_string.clear();
                }
            }
        }

    }

    //find wide strings
    {
        pe_image_io wide_string_io(current_image);
        wide_string_io.seek_to_start();

        wchar_t image_byte;
        uint32_t pre_string_rva;
        std::wstring pre_string;

        while (wide_string_io.read(&image_byte, sizeof(image_byte)) == enma_io_success) {
            if ((!find_in_execute_sections || wide_string_io.is_executable_rva(wide_string_io.get_image_offset() - 1)) &&
                _iswprint_l(image_byte, loc) || _iswblank_l(image_byte, loc)) {

                if (!pre_string.size()) {
                    pre_string_rva = wide_string_io.get_image_offset() - 1;
                }

                pre_string += image_byte;
            }
            else {
                if (image_byte == 0 &&
                    pre_string.size() >= MIN_STRING_SIZE && pre_string.size() <= MAX_STRING_SIZE) {

                    string_table.wide_base.push_back({ pre_string_rva , pre_string });
                    pre_string.clear();
                }
                else {
                    pre_string.clear();
                }
            }
        }

    }

    _free_locale(loc);
    */
}

