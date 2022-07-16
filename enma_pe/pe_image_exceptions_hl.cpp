#include "stdafx.h"

using namespace enma;
using namespace hl;

exceptions_handler_specific_data::exceptions_handler_specific_data(const exceptions_handler_specific_data& data) {
    operator=(data);
}

exceptions_handler_specific_data::~exceptions_handler_specific_data() {

    switch (_data_type) {

    case __c_specific_handler: {
        delete get_c_specific_handler_parameters_data(); break;
    }
    case __delphi_specific_handler: {
        delete get_delphi_specific_handler_parameters_data(); break;
    }
    case __llvm_specific_handler: {
        delete get_llvm_specific_handler_parameters_data(); break;
    }
    case __gs_handler_check: {
        delete get_gs_handler_check_parameters_data(); break;
    }
    case __gs_handler_check_seh: {
        delete get_gs_handler_check_seh_parameters_data(); break;
    }
    case __cxx_frame_handler3: {
        delete get_cxx_frame_handler3_parameters_data(); break;
    }
    case __gs_handler_check_eh: {
        delete get_gs_handler_check_eh_parameters_data(); break;
    }
    }

    _data = 0;
    _data_type = unknown_handler;
}

exceptions_handler_specific_data& exceptions_handler_specific_data::operator=(const exceptions_handler_specific_data& data) {

    switch (data._data_type) {

    case __c_specific_handler: {
        set_c_specific_handler_parameters_data(*data.get_c_specific_handler_parameters_data());
        return *this;
    }
    case __delphi_specific_handler: {
        set_delphi_specific_handler_parameters_data(*data.get_delphi_specific_handler_parameters_data());
        return *this;
    }
    case __llvm_specific_handler: {
        set_llvm_specific_handler_parameters_data(*data.get_llvm_specific_handler_parameters_data());
        return *this;
    }
    case __gs_handler_check: {
        set_gs_handler_check_parameters_data(*data.get_gs_handler_check_parameters_data());
        return *this;
    }
    case __gs_handler_check_seh: {
        set_gs_handler_check_seh_parameters_data(*data.get_gs_handler_check_seh_parameters_data());
        return *this;
    }
    case __cxx_frame_handler3: {
        set_cxx_frame_handler3_parameters_data(*data.get_cxx_frame_handler3_parameters_data());
        return *this;
    }
    case __gs_handler_check_eh: {
        set_gs_handler_check_eh_parameters_data(*data.get_gs_handler_check_eh_parameters_data());
        return *this;
    }
    }

    return *this;
}

#define GET_EXCEPTION_PARAMS_RVA(unwind_entry) \
        uint32_t(unwind_entry.get_unwind_info_rva() +\
        sizeof(unwind_info) +\
        ALIGN_UP(sizeof(unwind_code) * unwind_entry.get_codes().size(), 4) +\
        sizeof(uint32_t))

static bool get_placement___c_specific_handler_data(const pe_image_full& image_full, pe_placement& placement, uint32_t handler_rva);
static bool get_placement___delphi_specific_handler_data(const pe_image_full& image_full, pe_placement& placement, uint32_t handler_rva);
static bool get_placement__llvm_specific_handler_data(const pe_image_full& image_full, pe_placement& placement, uint32_t handler_rva);
static bool get_placement__gs_handler_check_data(const pe_image_full& image_full, pe_placement& placement, uint32_t handler_rva);
static bool get_placement__gs_handler_check_seh_data(const pe_image_full& image_full, pe_placement& placement, uint32_t handler_rva);
static bool get_placement__gs_handler_check_eh_data(const pe_image_full& image_full, pe_placement& placement, uint32_t handler_rva);
static bool get_placement__cxx_frame_handler3_data(const pe_image_full& image_full, pe_placement& placement, uint32_t handler_rva);

static bool init_data___c_specific_handler_data(pe_image_full& image_full, uint32_t handler_rva);
static bool init_data___delphi_specific_handler_data(pe_image_full& image_full, uint32_t handler_rva);
static bool init_data__llvm_specific_handler_data(pe_image_full& image_full, uint32_t handler_rva);
static bool init_data__gs_handler_check_data(pe_image_full& image_full, uint32_t handler_rva);
static bool init_data__gs_handler_check_seh_data(pe_image_full& image_full, uint32_t handler_rva);
static bool init_data__gs_handler_check_eh_data(pe_image_full& image_full, uint32_t handler_rva);
static bool init_data__cxx_frame_handler3_data(pe_image_full& image_full, uint32_t handler_rva);

static exception_handler_type get_handler_params_pattern(const pe_image_full& image_full, const std::vector<uint32_t>& unwindinfo_handler_parameters_rvas) {

    std::vector<uint32_t> handler_hits;
    handler_hits.resize(handler_type_max);
    memset(handler_hits.data(), 0, handler_hits.size() * sizeof(uint32_t));

    std::map<uint32_t, size_t> unwind_info_regions;


    for (auto& unwind_entry : image_full.get_exceptions().get_unwind_entries()) { //get unwind info regions

        size_t entry_size = sizeof(unwind_info) +
            ALIGN_UP(sizeof(unwind_code) * unwind_entry.get_codes().size(), 4);

        if (unwind_entry.get_flags() & (UNW_FLAG_EHANDLER | UNW_FLAG_UHANDLER)) {

            entry_size += sizeof(uint32_t);

        }
        else if (unwind_entry.get_flags() & UNW_FLAG_CHAININFO) {
            entry_size += sizeof(runtime_function_entry);
        }

        unwind_info_regions[unwind_entry.get_unwind_info_rva()] = entry_size;
    }


    pe_image_io image_io(image_full.get_image());

    for (auto param_rva : unwindinfo_handler_parameters_rvas) {

        size_t allowed_size = 0;

        {
            bool found = false;
            for (auto& unwind_reg_entry : unwind_info_regions) {
                if (unwind_reg_entry.first > param_rva) { //todo optimize it 
                    allowed_size = (unwind_reg_entry.first - param_rva); found = true;
                    break;
                }
            }

            if (!found) {
                allowed_size = 0x1000;
            }
        }

        if (allowed_size) {

            { //__C_specific_handler & __GSHandlerCheck_SEH & __Delphi_specific_handler

                size_t __C_specific_handler_idxer = 0;
                size_t __GSHandlerCheck_SEH_idxer = 0;
                size_t __Delphi_specific_handler_idxer = 0;
                enma_io_code last_code = enma_io_success;

                uint32_t count_entries = 0;

                if (allowed_size >= sizeof(uint32_t)) {
                    last_code = image_io.set_image_offset(param_rva).read(&count_entries, sizeof(count_entries));
                    if (last_code == enma_io_success) {

                        if ((allowed_size - sizeof(uint32_t)) >= (count_entries * sizeof(cxx_scope_table_entry))) {

                            for (uint32_t scope_idx = 0; scope_idx < count_entries; scope_idx++) {
                                cxx_scope_table_entry scope_entry;

                                last_code = image_io.read(&scope_entry, sizeof(scope_entry));

                                if (last_code == enma_io_success) {

                                    if (scope_entry.begin_address < scope_entry.end_address &&
                                        image_io.is_executable_rva(scope_entry.begin_address) &&
                                        image_io.is_executable_rva(scope_entry.end_address)) {


                                        if (image_io.is_executable_rva(scope_entry.jump_target)) {
                                            __C_specific_handler_idxer++;
                                            __GSHandlerCheck_SEH_idxer++;
                                        }
                                        else {

                                            if (scope_entry.handler_address > 2) {

                                                if (image_io.is_executable_rva(scope_entry.handler_address)) {
                                                    __C_specific_handler_idxer++;
                                                    __GSHandlerCheck_SEH_idxer++;
                                                }
                                            }
                                            else {
                                                if (scope_entry.handler_address == 0) {
                                                    __C_specific_handler_idxer++;
                                                    __GSHandlerCheck_SEH_idxer++;
                                                }
                                                __Delphi_specific_handler_idxer++;
                                            }
                                        }

                                    }
                                    else {
                                        break;
                                    }
                                }
                                else {
                                    break;
                                }
                            }

                            if (__GSHandlerCheck_SEH_idxer &&
                                allowed_size - (sizeof(uint32_t) + (count_entries * sizeof(cxx_scope_table_entry))) >= sizeof(uint32_t)) {
                                uint32_t gs_value = 0;
                                if (image_io.read(&gs_value, sizeof(gs_value)) == enma_io_success) {

                                    if (gs_value > 0 && gs_value < 0xFFFF) {
                                        __GSHandlerCheck_SEH_idxer++;
                                    }
                                }
                            }
                        }
                    }
                }

                if (last_code == enma_io_success && count_entries) {

                    if (__C_specific_handler_idxer >= __GSHandlerCheck_SEH_idxer) {

                        if (__C_specific_handler_idxer >= __Delphi_specific_handler_idxer) {
                            if (__C_specific_handler_idxer) {
                                handler_hits[__c_specific_handler]++;
                            }
                        }
                        else {
                            if (__Delphi_specific_handler_idxer) {
                                handler_hits[__delphi_specific_handler]++;
                            }
                        }

                    }
                    else {
                        if (__GSHandlerCheck_SEH_idxer) {
                            handler_hits[__gs_handler_check_seh]++;
                        }
                    }
                }
            }

            { //__llvm_specific_handler

                if (allowed_size >= sizeof(uint64_t)) {
                    std::vector<pe_relocation_entry> relocs;
                    image_full.get_relocations().get_items_in_segment(relocs, param_rva, allowed_size);

                    if (relocs.size()) {
                        handler_hits[__llvm_specific_handler]++;
                    }
                }
            }

            { //__GSHandlerCheck

                if (allowed_size >= sizeof(uint32_t)) {

                    uint32_t gs_value = 0;

                    if (image_io.set_image_offset(
                        uint32_t(param_rva)).read(&gs_value, sizeof(gs_value)) == enma_io_success) {

                        if (gs_value > 0 && gs_value < 0xFFFF) {
                            handler_hits[__gs_handler_check]++;
                        }
                    }
                }

            }

            { //__CxxFrameHandler3 & __GSHandlerCheck_EH

                enma_io_code last_code = enma_io_success;

                uint32_t func_info_rva = 0;
                bool is_valid_func_info = false;

                if (allowed_size >= sizeof(uint32_t)) {
                    if (image_io.set_image_offset(param_rva).read(&func_info_rva, sizeof(func_info_rva)) == enma_io_success) {
                        if (image_io.is_readable_rva(func_info_rva)) {
                            cxx_function_desc func_desc;

                            if (image_io.set_image_offset(func_info_rva).read(&func_desc, sizeof(func_desc)) == enma_io_success) {

                                if (func_desc.magic_number >= CXX_FRAME_MAGIC_VC6 &&
                                    func_desc.magic_number <= CXX_FRAME_MAGIC_VC8 &&
                                    func_desc.max_state ? (image_io.is_readable_rva(func_desc.p_unwind_map)) : true &&
                                    func_desc.try_blocks ? (image_io.is_readable_rva(func_desc.p_try_block_map)) : true &&
                                    func_desc.ip_map_entries ? (image_io.is_readable_rva(func_desc.p_ip_to_state_map)) : true
                                    ) {
                                    is_valid_func_info = true;
                                }
                            }
                        }
                    }
                }

                if (is_valid_func_info) {
                    if (allowed_size >= (sizeof(uint32_t) * 2)) {
                        uint32_t gs_value = 0;

                        if (image_io.set_image_offset(
                            uint32_t(param_rva + sizeof(uint32_t))).read(&gs_value, sizeof(gs_value)) == enma_io_success) {

                            if (gs_value > 0 && gs_value < 0xFFFF) {
                                handler_hits[__gs_handler_check_eh]++;
                            }
                            else {
                                handler_hits[__cxx_frame_handler3]++;
                            }

                        }
                    }
                    else {
                        handler_hits[__cxx_frame_handler3]++;
                    }
                }

            }
        }
        else {
            handler_hits[unknown_handler]++;
        }
    }

    exception_handler_type result_ = unknown_handler;

    {
        uint32_t last_idxer = 0;
        for (size_t idx = 0; idx < handler_hits.size(); idx++) {
            if (last_idxer < handler_hits[idx]) {
                result_ = exception_handler_type(idx);
                last_idxer = handler_hits[idx];
            }
        }
    }

    return result_;
}

ex_exceptions_info_result enma::hl::get_extended_exception_info(pe_image_full& image_full) {

    if (image_full.get_image().is_x32_image()) { return ex_exceptions_info_ok; }

    ex_exceptions_info_result result = ex_exceptions_info_ok;

    struct exc_handler_desc {
        std::vector<uint32_t> unwindinfo_handler_parameters_rvas;
        exception_handler_type type = unknown_handler;
    };

    std::map<uint32_t, exc_handler_desc> availables_handlers;

    for (auto& unwind_entry : image_full.get_exceptions().get_unwind_entries()) { //get all used handlers with unwind info

        if (unwind_entry.get_flags() & (UNW_FLAG_EHANDLER | UNW_FLAG_UHANDLER)) {
            auto handler_desc = availables_handlers.find(unwind_entry.get_handler_rva());

            if (handler_desc != availables_handlers.end()) {
                handler_desc->second.unwindinfo_handler_parameters_rvas.push_back(uint32_t(
                    GET_EXCEPTION_PARAMS_RVA(unwind_entry)
                ));
            }
            else {
                exc_handler_desc desc;
                desc.type = unknown_handler;
                desc.unwindinfo_handler_parameters_rvas.push_back(uint32_t(
                    GET_EXCEPTION_PARAMS_RVA(unwind_entry)
                ));

                availables_handlers[unwind_entry.get_handler_rva()] = desc;
            }
        }
    }


    for (auto& handler_desc_entry : availables_handlers) { //get handler type
        handler_desc_entry.second.type = get_handler_params_pattern(image_full, handler_desc_entry.second.unwindinfo_handler_parameters_rvas);
    }


    for (auto& handler_desc_entry : availables_handlers) {
        switch (handler_desc_entry.second.type) {

        case unknown_handler: {
            result = ex_exceptions_info_has_unknown;
            break;
        }
        case __c_specific_handler: {
            if (!init_data___c_specific_handler_data(image_full, handler_desc_entry.first)) {
                return ex_exceptions_info_result::ex_exceptions_info_has_error;
            }
            break;
        }
        case __delphi_specific_handler: {
            if (!init_data___delphi_specific_handler_data(image_full, handler_desc_entry.first)) {
                return ex_exceptions_info_result::ex_exceptions_info_has_error;
            }
            break;
        }
        case __llvm_specific_handler: {
            if (!init_data__llvm_specific_handler_data(image_full, handler_desc_entry.first)) {
                return ex_exceptions_info_result::ex_exceptions_info_has_error;
            }
            break;
        }
        case __gs_handler_check: {
            if (!init_data__gs_handler_check_data(image_full, handler_desc_entry.first)) {
                return ex_exceptions_info_result::ex_exceptions_info_has_error;
            }
            break;
        }
        case __gs_handler_check_seh: {
            if (!init_data__gs_handler_check_seh_data(image_full, handler_desc_entry.first)) {
                return ex_exceptions_info_result::ex_exceptions_info_has_error;
            }
            break;
        }
        case __cxx_frame_handler3: {
            if (!init_data__cxx_frame_handler3_data(image_full, handler_desc_entry.first)) {
                return ex_exceptions_info_result::ex_exceptions_info_has_error;
            }
            break;
        }
        case __gs_handler_check_eh: {
            if (!init_data__gs_handler_check_eh_data(image_full, handler_desc_entry.first)) {
                return ex_exceptions_info_result::ex_exceptions_info_has_error;
            }
            break;
        }
        }
    }

    return result;
}

ex_exceptions_info_result enma::hl::get_extended_exception_info_placement(const pe_image_full& image_full, pe_placement& placement) {

    if (image_full.get_image().is_x32_image()) { return ex_exceptions_info_ok; }

    ex_exceptions_info_result result = ex_exceptions_info_ok;

    struct exc_handler_desc {
        std::vector<uint32_t> unwindinfo_handler_parameters_rvas;
        exception_handler_type type = unknown_handler;
    };

    std::map<uint32_t, exc_handler_desc> availables_handlers;

    for (auto& unwind_entry : image_full.get_exceptions().get_unwind_entries()) { //get all used handlers with unwind info

        if (unwind_entry.get_flags() & (UNW_FLAG_EHANDLER | UNW_FLAG_UHANDLER)) {
            auto handler_desc = availables_handlers.find(unwind_entry.get_handler_rva());

            if (handler_desc != availables_handlers.end()) {
                handler_desc->second.unwindinfo_handler_parameters_rvas.push_back(uint32_t(
                    unwind_entry.get_unwind_info_rva() +
                    sizeof(unwind_info) +
                    ALIGN_UP(sizeof(unwind_code) * unwind_entry.get_codes().size(), 4) +
                    sizeof(uint32_t)
                ));
            }
            else {
                exc_handler_desc desc;
                desc.type = unknown_handler;
                desc.unwindinfo_handler_parameters_rvas.push_back(uint32_t(
                    unwind_entry.get_unwind_info_rva() +
                    sizeof(unwind_info) +
                    ALIGN_UP(sizeof(unwind_code) * unwind_entry.get_codes().size(), 4) +
                    sizeof(uint32_t)
                ));

                availables_handlers[unwind_entry.get_handler_rva()] = desc;
            }
        }
    }


    for (auto& handler_desc_entry : availables_handlers) { //get handler type
        handler_desc_entry.second.type = get_handler_params_pattern(image_full, handler_desc_entry.second.unwindinfo_handler_parameters_rvas);
    }

    for (auto& handler_desc_entry : availables_handlers) {
        switch (handler_desc_entry.second.type) {

        case unknown_handler: {
            result = ex_exceptions_info_has_unknown;
            break;
        }
        case __c_specific_handler: {
            if (!get_placement___c_specific_handler_data(image_full, placement, handler_desc_entry.first)) {
                return ex_exceptions_info_result::ex_exceptions_info_has_error;
            }
            break;
        }
        case __delphi_specific_handler: {
            if (!get_placement___delphi_specific_handler_data(image_full, placement, handler_desc_entry.first)) {
                return  ex_exceptions_info_result::ex_exceptions_info_has_error;
            }
            break;
        }
        case __llvm_specific_handler: {
            if (!get_placement__llvm_specific_handler_data(image_full, placement, handler_desc_entry.first)) {
                return ex_exceptions_info_result::ex_exceptions_info_has_error;
            }
            break;
        }
        case __gs_handler_check: {
            if (!get_placement__gs_handler_check_data(image_full, placement, handler_desc_entry.first)) {
                return ex_exceptions_info_result::ex_exceptions_info_has_error;
            }
            break;
        }
        case __gs_handler_check_seh: {
            if (!get_placement__gs_handler_check_seh_data(image_full, placement, handler_desc_entry.first)) {
                return ex_exceptions_info_result::ex_exceptions_info_has_error;
            }
            break;
        }
        case __cxx_frame_handler3: {
            if (!get_placement__cxx_frame_handler3_data(image_full, placement, handler_desc_entry.first)) {
                return ex_exceptions_info_result::ex_exceptions_info_has_error;
            }
            break;
        }
        case __gs_handler_check_eh: {
            if (!get_placement__gs_handler_check_eh_data(image_full, placement, handler_desc_entry.first)) {
                return ex_exceptions_info_result::ex_exceptions_info_has_error;
            }
            break;
        }
        }
    }

    return result;
}

static bool get_placement___c_specific_handler_data(const pe_image_full& image_full, pe_placement& placement, uint32_t handler_rva) {

    pe_image_io image_io(image_full.get_image());

    for (auto& unwind_entry : image_full.get_exceptions().get_unwind_entries()) {
        if (unwind_entry.get_handler_rva() == handler_rva) {

            cxx_scope_table_entry scope_entry;

            image_io.set_image_offset(
                GET_EXCEPTION_PARAMS_RVA(unwind_entry)
            );

            uint32_t rva_start = image_io.get_image_offset();

            uint32_t scope_entries_count = 0;

            if (image_io.read(&scope_entries_count, sizeof(scope_entries_count)) != enma_io_success) {
                return false;
            }

            for (uint32_t scope_idx = 0; scope_idx < scope_entries_count; scope_idx++) {
                uint32_t rva_scope = image_io.get_image_offset();

                if (image_io.read(&scope_entry, sizeof(scope_entry)) != enma_io_success) {
                    return false;
                }
            }

            placement[rva_start] = pe_placement_entry(ALIGN_UP(image_io.get_image_offset() - rva_start, 4), id_pe_placement::id_pe_none, "");
        }
    }

    return true;
}

static bool get_placement___delphi_specific_handler_data(const pe_image_full& image_full, pe_placement& placement, uint32_t handler_rva) {

    pe_image_io image_io(image_full.get_image());

    for (auto& unwind_entry : image_full.get_exceptions().get_unwind_entries()) {
        if (unwind_entry.get_handler_rva() == handler_rva) {

            delphi_scope_table_entry scope_entry;

            image_io.set_image_offset(
                GET_EXCEPTION_PARAMS_RVA(unwind_entry)
            );

            uint32_t rva_start = image_io.get_image_offset();

            uint32_t scope_entries_count = 0;

            if (image_io.read(&scope_entries_count, sizeof(scope_entries_count)) != enma_io_success) {
                return false;
            }

            for (uint32_t scope_idx = 0; scope_idx < scope_entries_count; scope_idx++) {
                uint32_t rva_scope = image_io.get_image_offset();

                if (image_io.read(&scope_entry, sizeof(scope_entry)) != enma_io_success) {
                    return false;
                }
            }

            placement[rva_start] = pe_placement_entry(ALIGN_UP(image_io.get_image_offset() - rva_start, 4), id_pe_placement::id_pe_none, "");
        }
    }

    return true;
}

static bool get_placement__llvm_specific_handler_data(const pe_image_full& image_full, pe_placement& placement, uint32_t handler_rva) {

    pe_image_io image_io(image_full.get_image());

    for (auto& unwind_entry : image_full.get_exceptions().get_unwind_entries()) {
        if (unwind_entry.get_handler_rva() == handler_rva) {

            image_io.set_image_offset(
                GET_EXCEPTION_PARAMS_RVA(unwind_entry)
            );

            uint32_t rva_start = image_io.get_image_offset();

            uint64_t llvm_ptr = 0;

            if (image_io.read(&llvm_ptr, sizeof(llvm_ptr)) != enma_io_success) {
                return false;
            }

            placement[rva_start] = pe_placement_entry(sizeof(llvm_ptr), id_pe_placement::id_pe_none, "");
        }
    }

    return true;
}

static bool get_placement__gs_handler_check_data(const pe_image_full& image_full, pe_placement& placement, uint32_t handler_rva) {

    pe_image_io image_io(image_full.get_image());

    for (auto& unwind_entry : image_full.get_exceptions().get_unwind_entries()) {
        if (unwind_entry.get_handler_rva() == handler_rva) {

            image_io.set_image_offset(
                GET_EXCEPTION_PARAMS_RVA(unwind_entry)
            );

            uint32_t rva_start = image_io.get_image_offset();

            uint32_t gs_data = 0;

            if (image_io.read(&gs_data, sizeof(gs_data)) != enma_io_success) {
                return false;
            }

            placement[rva_start] = pe_placement_entry(sizeof(gs_data), id_pe_placement::id_pe_none, "");
        }
    }

    return true;
}

static bool get_placement__gs_handler_check_seh_data(const pe_image_full& image_full, pe_placement& placement, uint32_t handler_rva) {

    pe_image_io image_io(image_full.get_image());

    for (auto& unwind_entry : image_full.get_exceptions().get_unwind_entries()) {
        if (unwind_entry.get_handler_rva() == handler_rva) {

            cxx_scope_table_entry scope_entry;

            image_io.set_image_offset(
                GET_EXCEPTION_PARAMS_RVA(unwind_entry)
            );

            uint32_t rva_start = image_io.get_image_offset();

            uint32_t scope_entries_count = 0;

            if (image_io.read(&scope_entries_count, sizeof(scope_entries_count)) != enma_io_success) {
                return false;
            }

            for (uint32_t scope_idx = 0; scope_idx < scope_entries_count; scope_idx++) {
                uint32_t rva_scope = image_io.get_image_offset();

                if (image_io.read(&scope_entry, sizeof(scope_entry)) != enma_io_success) {
                    return false;
                }
            }

            uint32_t gs_data;
            if (image_io.read(&gs_data, sizeof(gs_data)) != enma_io_success) {
                return false;
            }

            placement[rva_start] = pe_placement_entry(ALIGN_UP(image_io.get_image_offset() - rva_start, 4), id_pe_placement::id_pe_none, "");
        }
    }

    return true;
}

static bool get_placement__gs_handler_check_eh_data(const pe_image_full& image_full, pe_placement& placement, uint32_t handler_rva) {

    pe_image_io image_io(image_full.get_image());

    for (auto& unwind_entry : image_full.get_exceptions().get_unwind_entries()) {
        if (unwind_entry.get_handler_rva() == handler_rva) {

            image_io.set_image_offset(
                GET_EXCEPTION_PARAMS_RVA(unwind_entry)
            );

            uint32_t rva_start = image_io.get_image_offset();

            uint32_t func_desc_rva = 0;
            if (image_io.read(&func_desc_rva, sizeof(func_desc_rva)) != enma_io_success) {
                return false;
            }

            uint32_t gs_data;
            if (image_io.read(&gs_data, sizeof(gs_data)) != enma_io_success) {
                return false;
            }

            placement[rva_start] = pe_placement_entry(ALIGN_UP(image_io.get_image_offset() - rva_start, 4), id_pe_placement::id_pe_none, "");

            cxx_function_desc func_desc;
            if (image_io.set_image_offset(func_desc_rva).read(&func_desc, sizeof(func_desc)) != enma_io_success) {
                return false;
            }

            if (func_desc.max_state) {

                image_io.set_image_offset(func_desc.p_unwind_map);
                rva_start = image_io.get_image_offset();

                for (uint32_t ip_state_idx = 0; ip_state_idx < func_desc.max_state; ip_state_idx++) {
                    cxx_unwind_map_entry map_entry;

                    if (image_io.read(&map_entry, sizeof(map_entry)) != enma_io_success) {
                        return false;
                    }
                }

                placement[rva_start] = pe_placement_entry(ALIGN_UP(image_io.get_image_offset() - rva_start, 4), id_pe_placement::id_pe_none, "");
            }

            if (func_desc.try_blocks) {

                image_io.set_image_offset(func_desc.p_try_block_map);
                rva_start = image_io.get_image_offset();

                for (uint32_t try_block_idx = 0; try_block_idx < func_desc.try_blocks; try_block_idx++) {
                    cxx_try_block_map_entry try_map_entry;

                    if (image_io.set_image_offset(func_desc.p_try_block_map + sizeof(cxx_try_block_map_entry) * try_block_idx)
                        .read(&try_map_entry, sizeof(try_map_entry)) != enma_io_success) {
                        return false;
                    }
                    cxx_try_block_map_info try_block_info = try_map_entry;

                    if (try_map_entry.catches) {
                        cxx_handler_type cache_handler;

                        pe_image_io cth_image_io(image_full.get_image());
                        cth_image_io.set_image_offset(try_map_entry.p_handler_array);

                        uint32_t cth_rva_start = cth_image_io.get_image_offset();

                        for (uint32_t cache_handler_idx = 0; cache_handler_idx < try_map_entry.catches; cache_handler_idx++) {

                            if (cth_image_io.read(&cache_handler, sizeof(cache_handler)) != enma_io_success) {
                                return false;
                            }
                        }

                        placement[cth_rva_start] = pe_placement_entry(ALIGN_UP(cth_image_io.get_image_offset() - cth_rva_start, 4), id_pe_placement::id_pe_none, "");
                    }
                }

                placement[rva_start] = pe_placement_entry(ALIGN_UP(image_io.get_image_offset() - rva_start, 4), id_pe_placement::id_pe_none, "");
            }

            if (func_desc.ip_map_entries) {

                image_io.set_image_offset(func_desc.p_ip_to_state_map);
                rva_start = image_io.get_image_offset();

                for (uint32_t ip_to_state_idx = 0; ip_to_state_idx < func_desc.ip_map_entries; ip_to_state_idx++) {

                    cxx_ip_to_state_map_entry state_map_entry;

                    if (image_io.read(&state_map_entry, sizeof(state_map_entry)) != enma_io_success) {
                        return false;
                    }
                }

                placement[rva_start] = pe_placement_entry(ALIGN_UP(image_io.get_image_offset() - rva_start, 4), id_pe_placement::id_pe_none, "");
            }
        }
    }

    return true;
}

static bool get_placement__cxx_frame_handler3_data(const pe_image_full& image_full, pe_placement& placement, uint32_t handler_rva) {

    pe_image_io image_io(image_full.get_image());

    for (auto& unwind_entry : image_full.get_exceptions().get_unwind_entries()) {
        if (unwind_entry.get_handler_rva() == handler_rva) {

            image_io.set_image_offset(
                GET_EXCEPTION_PARAMS_RVA(unwind_entry)
            );

            uint32_t rva_start = image_io.get_image_offset();

            uint32_t func_desc_rva = 0;
            if (image_io.read(&func_desc_rva, sizeof(func_desc_rva)) != enma_io_success) {
                return false;
            }

            placement[rva_start] = pe_placement_entry(ALIGN_UP(image_io.get_image_offset() - rva_start, 4), id_pe_placement::id_pe_none, "");

            cxx_function_desc func_desc;
            if (image_io.set_image_offset(func_desc_rva).read(&func_desc, sizeof(func_desc)) != enma_io_success) {
                return false;
            }

            if (func_desc.max_state) {

                image_io.set_image_offset(func_desc.p_unwind_map);
                rva_start = image_io.get_image_offset();

                for (uint32_t ip_state_idx = 0; ip_state_idx < func_desc.max_state; ip_state_idx++) {
                    cxx_unwind_map_entry map_entry;

                    if (image_io.read(&map_entry, sizeof(map_entry)) != enma_io_success) {
                        return false;
                    }
                }

                placement[rva_start] = pe_placement_entry(ALIGN_UP(image_io.get_image_offset() - rva_start, 4), id_pe_placement::id_pe_none, "");
            }

            if (func_desc.try_blocks) {

                image_io.set_image_offset(func_desc.p_try_block_map);
                rva_start = image_io.get_image_offset();

                for (uint32_t try_block_idx = 0; try_block_idx < func_desc.try_blocks; try_block_idx++) {
                    cxx_try_block_map_entry try_map_entry;

                    if (image_io.set_image_offset(func_desc.p_try_block_map + sizeof(cxx_try_block_map_entry) * try_block_idx)
                        .read(&try_map_entry, sizeof(try_map_entry)) != enma_io_success) {
                        return false;
                    }
                    cxx_try_block_map_info try_block_info = try_map_entry;

                    if (try_map_entry.catches) {
                        cxx_handler_type cache_handler;

                        pe_image_io cth_image_io(image_full.get_image());
                        cth_image_io.set_image_offset(try_map_entry.p_handler_array);

                        uint32_t cth_rva_start = cth_image_io.get_image_offset();

                        for (uint32_t cache_handler_idx = 0; cache_handler_idx < try_map_entry.catches; cache_handler_idx++) {

                            if (cth_image_io.read(&cache_handler, sizeof(cache_handler)) != enma_io_success) {
                                return false;
                            }
                        }

                        placement[cth_rva_start] = pe_placement_entry(ALIGN_UP(cth_image_io.get_image_offset() - cth_rva_start, 4), id_pe_placement::id_pe_none, "");
                    }
                }

                placement[rva_start] = pe_placement_entry(ALIGN_UP(image_io.get_image_offset() - rva_start, 4), id_pe_placement::id_pe_none, "");
            }

            if (func_desc.ip_map_entries) {

                image_io.set_image_offset(func_desc.p_ip_to_state_map);
                rva_start = image_io.get_image_offset();

                for (uint32_t ip_to_state_idx = 0; ip_to_state_idx < func_desc.ip_map_entries; ip_to_state_idx++) {

                    cxx_ip_to_state_map_entry state_map_entry;

                    if (image_io.read(&state_map_entry, sizeof(state_map_entry)) != enma_io_success) {
                        return false;
                    }
                }

                placement[rva_start] = pe_placement_entry(ALIGN_UP(image_io.get_image_offset() - rva_start, 4), id_pe_placement::id_pe_none, "");
            }
        }
    }

    return true;
}

static bool init_data___c_specific_handler_data(pe_image_full& image_full, uint32_t handler_rva) {

    pe_image_io image_io(image_full.get_image());

    for (auto& unwind_entry : image_full.get_exceptions().get_unwind_entries()) {
        if (unwind_entry.get_handler_rva() == handler_rva) {

            c_specific_handler_parameters_data *data = new c_specific_handler_parameters_data;

            cxx_scope_table_entry scope_entry;

            image_io.set_image_offset(
                GET_EXCEPTION_PARAMS_RVA(unwind_entry)
            );

            uint32_t scope_entries_count = 0;

            if (image_io.read(&scope_entries_count, sizeof(scope_entries_count)) != enma_io_success) {
                delete data;
                return false;
            }

            for (uint32_t scope_idx = 0; scope_idx < scope_entries_count; scope_idx++) {

                if (image_io.read(&scope_entry, sizeof(scope_entry)) != enma_io_success) {
                    delete data;
                    return false;
                }

                data->table.push_back(scope_entry);
            }

            unwind_entry.get_custom_parameter() = *data;

            delete data;
        }
    }

    return true;
}

static bool init_data___delphi_specific_handler_data(pe_image_full& image_full, uint32_t handler_rva) {

    pe_image_io image_io(image_full.get_image());

    for (auto& unwind_entry : image_full.get_exceptions().get_unwind_entries()) {
        if (unwind_entry.get_handler_rva() == handler_rva) {

            delphi_specific_handler_parameters_data *data = new delphi_specific_handler_parameters_data;

            delphi_scope_table_entry scope_entry;

            image_io.set_image_offset(
                GET_EXCEPTION_PARAMS_RVA(unwind_entry)
            );

            uint32_t scope_entries_count = 0;

            if (image_io.read(&scope_entries_count, sizeof(scope_entries_count)) != enma_io_success) {
                delete data;
                return false;
            }

            for (uint32_t scope_idx = 0; scope_idx < scope_entries_count; scope_idx++) {

                if (image_io.read(&scope_entry, sizeof(scope_entry)) != enma_io_success) {
                    delete data;
                    return false;
                }

                data->table.push_back(scope_entry);
            }

            unwind_entry.get_custom_parameter() = *data;

            delete data;
        }
    }

    return true;
}

static bool init_data__llvm_specific_handler_data(pe_image_full& image_full, uint32_t handler_rva) {

    pe_image_io image_io(image_full.get_image());

    for (auto& unwind_entry : image_full.get_exceptions().get_unwind_entries()) {
        if (unwind_entry.get_handler_rva() == handler_rva) {

            llvm_specific_handler_parameters_data *data = new llvm_specific_handler_parameters_data;

            image_io.set_image_offset(
                GET_EXCEPTION_PARAMS_RVA(unwind_entry)
            );

            uint64_t llvm_ptr = 0;

            if (image_io.read(&llvm_ptr, sizeof(llvm_ptr)) != enma_io_success) {
                delete data;
                return false;
            }

            data->data_rva = image_full.get_image().va_to_rva(llvm_ptr);

            unwind_entry.get_custom_parameter() = *data;

            delete data;
        }
    }

    return true;
}

static bool init_data__gs_handler_check_data(pe_image_full& image_full, uint32_t handler_rva) {

    pe_image_io image_io(image_full.get_image());

    for (auto& unwind_entry : image_full.get_exceptions().get_unwind_entries()) {
        if (unwind_entry.get_handler_rva() == handler_rva) {

            gs_handler_check_parameters_data *data = new gs_handler_check_parameters_data;

            image_io.set_image_offset(
                GET_EXCEPTION_PARAMS_RVA(unwind_entry)
            );

            uint32_t gs_data = 0;

            if (image_io.read(&gs_data, sizeof(gs_data)) != enma_io_success) {
                delete data;
                return false;
            }

            data->gs_data = gs_data;

            unwind_entry.get_custom_parameter() = *data;

            delete data;
        }
    }

    return true;
}

static bool init_data__gs_handler_check_seh_data(pe_image_full& image_full, uint32_t handler_rva) {

    pe_image_io image_io(image_full.get_image());

    for (auto& unwind_entry : image_full.get_exceptions().get_unwind_entries()) {
        if (unwind_entry.get_handler_rva() == handler_rva) {

            gs_handler_check_seh_parameters_data *data = new gs_handler_check_seh_parameters_data;

            cxx_scope_table_entry scope_entry;

            image_io.set_image_offset(
                GET_EXCEPTION_PARAMS_RVA(unwind_entry)
            );

            uint32_t scope_entries_count = 0;

            if (image_io.read(&scope_entries_count, sizeof(scope_entries_count)) != enma_io_success) {
                delete data;
                return false;
            }

            for (uint32_t scope_idx = 0; scope_idx < scope_entries_count; scope_idx++) {

                if (image_io.read(&scope_entry, sizeof(scope_entry)) != enma_io_success) {
                    delete data;
                    return false;
                }

                data->table.push_back(scope_entry);
            }

            if (image_io.read(&data->gs_data, sizeof(data->gs_data)) != enma_io_success) {
                delete data;
                return false;
            }

            unwind_entry.get_custom_parameter() = *data;
        }
    }

    return true;
}

static bool init_data__gs_handler_check_eh_data(pe_image_full& image_full, uint32_t handler_rva) {

    pe_image_io image_io(image_full.get_image());

    for (auto& unwind_entry : image_full.get_exceptions().get_unwind_entries()) {
        if (unwind_entry.get_handler_rva() == handler_rva) {

            gs_handler_check_eh_parameters_data *data = 0;

            image_io.set_image_offset(
                GET_EXCEPTION_PARAMS_RVA(unwind_entry)
            );

            uint32_t func_desc_rva = 0;
            if (image_io.read(&func_desc_rva, sizeof(func_desc_rva)) != enma_io_success) {
                return false;
            }

            cxx_function_desc func_desc;
            if (image_io.set_image_offset(func_desc_rva).read(&func_desc, sizeof(func_desc)) != enma_io_success) {
                return false;
            }

            data = new gs_handler_check_eh_parameters_data({ func_desc , 0 });

            if (func_desc.max_state) {

                image_io.set_image_offset(func_desc.p_unwind_map);
                for (uint32_t ip_state_idx = 0; ip_state_idx < func_desc.max_state; ip_state_idx++) {
                    cxx_unwind_map_entry map_entry;

                    if (image_io.read(&map_entry, sizeof(map_entry)) != enma_io_success) {
                        delete data;
                        return false;
                    }

                    data->func_info.get_unwind_map_entries().push_back(map_entry);
                }
            }

            if (func_desc.try_blocks) {

                image_io.set_image_offset(func_desc.p_try_block_map);
                for (uint32_t try_block_idx = 0; try_block_idx < func_desc.try_blocks; try_block_idx++) {
                    cxx_try_block_map_entry try_map_entry;

                    if (image_io.set_image_offset(func_desc.p_try_block_map + sizeof(cxx_try_block_map_entry) * try_block_idx)
                        .read(&try_map_entry, sizeof(try_map_entry)) != enma_io_success) {

                        return false;
                    }
                    cxx_try_block_map_info try_block_info = try_map_entry;

                    if (try_map_entry.catches) {
                        cxx_handler_type cache_handler;

                        image_io.set_image_offset(try_map_entry.p_handler_array);
                        for (uint32_t cache_handler_idx = 0; cache_handler_idx < try_map_entry.catches; cache_handler_idx++) {

                            if (image_io.read(&cache_handler, sizeof(cache_handler)) != enma_io_success) {
                                return false;
                            }

                            try_block_info.get_handler_entries().push_back(cache_handler);
                        }
                    }

                    data->func_info.get_try_block_map_entries().push_back(try_block_info);
                }
            }

            if (func_desc.ip_map_entries) {

                image_io.set_image_offset(func_desc.p_ip_to_state_map);
                for (uint32_t ip_to_state_idx = 0; ip_to_state_idx < func_desc.ip_map_entries; ip_to_state_idx++) {

                    cxx_ip_to_state_map_entry state_map_entry;

                    if (image_io.read(&state_map_entry, sizeof(state_map_entry)) != enma_io_success) {
                        return false;
                    }

                    data->func_info.get_ip_to_state_map_entries().push_back(state_map_entry);
                }
            }

            image_io.set_image_offset(GET_EXCEPTION_PARAMS_RVA(unwind_entry) + 4);
            if (image_io.read(&data->gs_data, sizeof(data->gs_data)) != enma_io_success) {
                return false;
            }

            unwind_entry.get_custom_parameter() = *data;
        }
    }

    return true;
}

static bool init_data__cxx_frame_handler3_data(pe_image_full& image_full, uint32_t handler_rva) {

    pe_image_io image_io(image_full.get_image());

    for (auto& unwind_entry : image_full.get_exceptions().get_unwind_entries()) {
        if (unwind_entry.get_handler_rva() == handler_rva) {

            cxx_frame_handler3_parameters_data *data = 0;

            image_io.set_image_offset(
                GET_EXCEPTION_PARAMS_RVA(unwind_entry)
            );

            uint32_t func_desc_rva = 0;
            if (image_io.read(&func_desc_rva, sizeof(func_desc_rva)) != enma_io_success) {
                return false;
            }

            cxx_function_desc func_desc;
            if (image_io.set_image_offset(func_desc_rva).read(&func_desc, sizeof(func_desc)) != enma_io_success) {
                return false;
            }

            data = new cxx_frame_handler3_parameters_data(cxx_exception_func_info(func_desc));

            if (func_desc.max_state) {

                image_io.set_image_offset(func_desc.p_unwind_map);
                for (uint32_t ip_state_idx = 0; ip_state_idx < func_desc.max_state; ip_state_idx++) {
                    cxx_unwind_map_entry map_entry;

                    if (image_io.read(&map_entry, sizeof(map_entry)) != enma_io_success) {
                        delete data;
                        return false;
                    }

                    data->func_info.get_unwind_map_entries().push_back(map_entry);
                }
            }

            if (func_desc.try_blocks) {

                image_io.set_image_offset(func_desc.p_try_block_map);
                for (uint32_t try_block_idx = 0; try_block_idx < func_desc.try_blocks; try_block_idx++) {
                    cxx_try_block_map_entry try_map_entry;

                    if (image_io.set_image_offset(func_desc.p_try_block_map + sizeof(cxx_try_block_map_entry) * try_block_idx)
                        .read(&try_map_entry, sizeof(try_map_entry)) != enma_io_success) {
                        delete data;
                        return false;
                    }
                    cxx_try_block_map_info try_block_info = try_map_entry;

                    if (try_map_entry.catches) {
                        cxx_handler_type cache_handler;

                        image_io.set_image_offset(try_map_entry.p_handler_array);
                        for (uint32_t cache_handler_idx = 0; cache_handler_idx < try_map_entry.catches; cache_handler_idx++) {

                            if (image_io.read(&cache_handler, sizeof(cache_handler)) != enma_io_success) {
                                delete data;
                                return false;
                            }

                            try_block_info.get_handler_entries().push_back(cache_handler);
                        }
                    }

                    data->func_info.get_try_block_map_entries().push_back(try_block_info);
                }
            }

            if (func_desc.ip_map_entries) {

                image_io.set_image_offset(func_desc.p_ip_to_state_map);
                for (uint32_t ip_to_state_idx = 0; ip_to_state_idx < func_desc.ip_map_entries; ip_to_state_idx++) {

                    cxx_ip_to_state_map_entry state_map_entry;

                    if (image_io.read(&state_map_entry, sizeof(state_map_entry)) != enma_io_success) {
                        delete data;
                        return false;
                    }

                    data->func_info.get_ip_to_state_map_entries().push_back(state_map_entry);
                }
            }

            unwind_entry.get_custom_parameter() = *data;
        }
    }

    return true;
}


static uint32_t build_func_info(pe_image_io& func_info_io, cxx_exception_func_info& func_info) {

    if (func_info.get_unwind_map_entries().size()) {
        func_info.set_max_state((uint32_t)func_info.get_unwind_map_entries().size());
        func_info.set_p_unwind_map(func_info_io.get_image_offset());

        func_info_io.write(
            func_info.get_unwind_map_entries().data(),
            func_info.get_unwind_map_entries().size() * sizeof(cxx_unwind_map_entry)
        );
    }
    else {
        func_info.set_max_state(0);
        func_info.set_p_unwind_map(0);
    }

    if (func_info.get_try_block_map_entries().size()) {

        for (auto& try_entry : func_info.get_try_block_map_entries()) {

            try_entry.set_catches((uint32_t)try_entry.get_handler_entries().size());
            try_entry.set_p_handler_array(func_info_io.get_image_offset());

            func_info_io.write(
                try_entry.get_handler_entries().data(),
                try_entry.get_handler_entries().size() * sizeof(cxx_handler_type)
            );
        }

        func_info.set_try_blocks((uint32_t)func_info.get_try_block_map_entries().size());
        func_info.set_p_try_block_map(func_info_io.get_image_offset());

        for (auto& try_entry : func_info.get_try_block_map_entries()) {
            cxx_try_block_map_entry try_block_info;
            try_block_info.try_low = try_entry.get_try_low();
            try_block_info.try_high = try_entry.get_try_high();
            try_block_info.catch_high = try_entry.get_catch_high();
            try_block_info.catches = try_entry.get_catches();
            try_block_info.p_handler_array = try_entry.get_p_handler_array();

            func_info_io.write(&try_block_info, sizeof(try_block_info));
        }

    }
    else {
        func_info.set_try_blocks(0);
        func_info.set_p_try_block_map(0);
    }

    if (func_info.get_ip_to_state_map_entries().size()) {

        func_info.set_ip_map_entries((uint32_t)func_info.get_ip_to_state_map_entries().size());
        func_info.set_p_ip_to_state_map(func_info_io.get_image_offset());

        func_info_io.write(
            func_info.get_ip_to_state_map_entries().data(),
            func_info.get_ip_to_state_map_entries().size() * sizeof(cxx_ip_to_state_map_entry)
        );
    }
    else {
        func_info.set_ip_map_entries(0);
        func_info.set_p_ip_to_state_map(0);
    }

    uint32_t func_info_rva = func_info_io.get_image_offset();

    cxx_function_desc func_desc;
    func_desc.magic_number = func_info.get_magic_number();
    func_desc.max_state = func_info.get_max_state();
    func_desc.p_unwind_map = func_info.get_p_unwind_map();
    func_desc.try_blocks = func_info.get_try_blocks();
    func_desc.p_try_block_map = func_info.get_p_try_block_map();
    func_desc.ip_map_entries = func_info.get_ip_map_entries();
    func_desc.p_ip_to_state_map = func_info.get_p_ip_to_state_map();
    func_desc.unwind_help = func_info.get_unwind_help();
    func_desc.p_es_type_list = func_info.get_p_es_type_list();
    func_desc.eh_flags = func_info.get_eh_flags();

    func_info_io.write(&func_desc, sizeof(func_desc));

    return func_info_rva;
}

void enma::hl::build_extended_exceptions_info(pe_image_full& image_full) {
    if (image_full.get_image().is_x32_image()) { return; }

    pe_image_io ex_info_io(image_full.get_image(), enma_io_mode_allow_expand);
    ex_info_io.seek_to_end();

    for (auto& unwind_entry : image_full.get_exceptions().get_unwind_entries()) {

        if (unwind_entry.get_custom_parameter().get_data_type() != unknown_handler) {
            auto& params = unwind_entry.get_params(); params.clear();

            switch (unwind_entry.get_custom_parameter().get_data_type()) {

            case unknown_handler: {
                break;
            }

            case __c_specific_handler: {
                auto* data = unwind_entry.get_custom_parameter().get_c_specific_handler_parameters_data();


                {
                    unwind_parameter scope_count_parameter;
                    scope_count_parameter.type = unwind_parameter_type::unwind_parameter_raw;
                    scope_count_parameter.param_data.resize(sizeof(uint32_t));

                    uint32_t scope_count = (uint32_t)data->table.size();
                    *(uint32_t*)&scope_count_parameter.param_data.data()[0] = scope_count;

                    params.push_back(scope_count_parameter);
                }

                for (auto& entry : data->table) {
                    unwind_parameter scope_entry_parameter;
                    scope_entry_parameter.type = unwind_parameter_type::unwind_parameter_raw;
                    scope_entry_parameter.param_data.resize(sizeof(cxx_scope_table_entry));

                    cxx_scope_table_entry *p_entry = (cxx_scope_table_entry*)scope_entry_parameter.param_data.data();
                    p_entry->begin_address = entry.begin_address;
                    p_entry->end_address = entry.end_address;
                    p_entry->handler_address = entry.handler_address;
                    p_entry->jump_target = entry.jump_target;

                    params.push_back(scope_entry_parameter);
                }

                break;
            }
            case __delphi_specific_handler: {
                auto* data = unwind_entry.get_custom_parameter().get_delphi_specific_handler_parameters_data();

                {
                    unwind_parameter scope_count_parameter;
                    scope_count_parameter.type = unwind_parameter_type::unwind_parameter_raw;
                    scope_count_parameter.param_data.resize(sizeof(uint32_t));

                    uint32_t scope_count = (uint32_t)data->table.size();
                    *(uint32_t*)&scope_count_parameter.param_data.data()[0] = scope_count;

                    params.push_back(scope_count_parameter);
                }

                for (auto& entry : data->table) {
                    unwind_parameter scope_entry_parameter;
                    scope_entry_parameter.type = unwind_parameter_type::unwind_parameter_raw;
                    scope_entry_parameter.param_data.resize(sizeof(delphi_scope_table_entry));

                    delphi_scope_table_entry *p_entry = (delphi_scope_table_entry*)scope_entry_parameter.param_data.data();
                    p_entry->begin_address = entry.begin_address;
                    p_entry->end_address = entry.end_address;
                    p_entry->handle_type = entry.handle_type;
                    p_entry->jump_target = entry.jump_target;

                    params.push_back(scope_entry_parameter);
                }

                break;
            }
            case __llvm_specific_handler: {
                auto* data = unwind_entry.get_custom_parameter().get_llvm_specific_handler_parameters_data();

                unwind_parameter ptr_parameter;
                ptr_parameter.type = unwind_parameter_type::unwind_parameter_va;
                ptr_parameter.param_data.resize(sizeof(uint64_t));
                *(uint64_t*)&ptr_parameter.param_data.data()[0] = data->data_rva + image_full.get_image().get_image_base();

                params.push_back(ptr_parameter);
                break;
            }
            case __gs_handler_check: {
                auto* data = unwind_entry.get_custom_parameter().get_gs_handler_check_parameters_data();

                unwind_parameter gs_data_parameter;
                gs_data_parameter.type = unwind_parameter_type::unwind_parameter_raw;
                gs_data_parameter.param_data.resize(sizeof(uint32_t));
                *(uint32_t*)&gs_data_parameter.param_data.data()[0] = data->gs_data;

                params.push_back(gs_data_parameter);
                break;
            }
            case __gs_handler_check_seh: {
                auto* data = unwind_entry.get_custom_parameter().get_gs_handler_check_seh_parameters_data();


                {
                    unwind_parameter scope_count_parameter;
                    scope_count_parameter.type = unwind_parameter_type::unwind_parameter_raw;
                    scope_count_parameter.param_data.resize(sizeof(uint32_t));

                    uint32_t scope_count = (uint32_t)data->table.size();
                    *(uint32_t*)&scope_count_parameter.param_data.data()[0] = scope_count;

                    params.push_back(scope_count_parameter);
                }

                for (auto& entry : data->table) {
                    unwind_parameter scope_entry_parameter;
                    scope_entry_parameter.type = unwind_parameter_type::unwind_parameter_raw;
                    scope_entry_parameter.param_data.resize(sizeof(cxx_scope_table_entry));

                    cxx_scope_table_entry *p_entry = (cxx_scope_table_entry*)scope_entry_parameter.param_data.data();
                    p_entry->begin_address = entry.begin_address;
                    p_entry->end_address = entry.end_address;
                    p_entry->handler_address = entry.handler_address;
                    p_entry->jump_target = entry.jump_target;

                    params.push_back(scope_entry_parameter);
                }

                unwind_parameter gs_data_parameter;
                gs_data_parameter.type = unwind_parameter_type::unwind_parameter_raw;
                gs_data_parameter.param_data.resize(sizeof(uint32_t));
                *(uint32_t*)&gs_data_parameter.param_data.data()[0] = data->gs_data;

                params.push_back(gs_data_parameter);
                break;
            }
            case __cxx_frame_handler3: {
                auto* data = unwind_entry.get_custom_parameter().get_cxx_frame_handler3_parameters_data();

                {
                    unwind_parameter func_info_parameter;
                    func_info_parameter.type = unwind_parameter_type::unwind_parameter_raw;
                    func_info_parameter.param_data.resize(sizeof(uint32_t));

                    *(uint32_t*)&func_info_parameter.param_data.data()[0] = build_func_info(ex_info_io, data->func_info);

                    params.push_back(func_info_parameter);
                }

                break;
            }
            case __gs_handler_check_eh: {
                auto* data = unwind_entry.get_custom_parameter().get_gs_handler_check_eh_parameters_data();

                {
                    unwind_parameter func_info_parameter;
                    func_info_parameter.type = unwind_parameter_type::unwind_parameter_raw;
                    func_info_parameter.param_data.resize(sizeof(uint32_t));

                    *(uint32_t*)&func_info_parameter.param_data.data()[0] = build_func_info(ex_info_io, data->func_info);

                    params.push_back(func_info_parameter);
                }

                unwind_parameter gs_data_parameter;
                gs_data_parameter.type = unwind_parameter_type::unwind_parameter_raw;
                gs_data_parameter.param_data.resize(sizeof(uint32_t));
                *(uint32_t*)&gs_data_parameter.param_data.data()[0] = data->gs_data;

                params.push_back(gs_data_parameter);

                break;
            }
            }
        }
    }
}