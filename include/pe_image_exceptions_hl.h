#pragma once

namespace enma {

    namespace hl {

        enum exception_handler_type {
            unknown_handler,
            __c_specific_handler,      // Scope table scope_table_entry
            __delphi_specific_handler, // Scope table delphi_scope_table_entry
            __llvm_specific_handler,   // relocatable ptr
            __gs_handler_check,       // GS data
            __gs_handler_check_seh,   // Scope table + GS data scope_table_entry
            __cxx_frame_handler3,     // RVA to FuncInfo
            __gs_handler_check_eh,    // RVA to FuncInfo + GS data
            handler_type_max
        };

#define CXX_FRAME_MAGIC_VC6 0x19930520
#define CXX_FRAME_MAGIC_VC7 0x19930521
#define CXX_FRAME_MAGIC_VC8 0x19930522
#define CXX_EXCEPTION       0xe06d7363

#pragma pack(push, 1)

        struct cxx_scope_table_entry {
            uint32_t begin_address;
            uint32_t end_address;
            uint32_t handler_address;
            uint32_t jump_target;
        };

        struct delphi_scope_table_entry {
            uint32_t begin_address;
            uint32_t end_address;
            uint32_t handle_type;
            uint32_t jump_target;
        };

        struct cxx_scope_table {
            uint32_t count;
            //scope_table_entry entries[count]
        };

        struct cxx_handler_type {
            uint32_t adjectives; // 0x01: const, 0x02: volatile, 0x08: reference
            uint32_t p_type; //rva type_descriptor RTTI descriptor of the exception type. 0=any (ellipsis)
            uint32_t disp_catch_obj; // ebp-based offset of the exception object in the function stack.  0 = no object (catch by type)
            uint32_t p_address_of_handler; // address of the catch handler code. returns address where to continues execution (i.e. code after the try block)
            uint32_t gs_handler_value;
        };
        struct cxx_es_type_list {
            uint32_t count; // number of entries in the list
            uint32_t type_array; //rva handler_type list of exceptions; it seems only pType field in HandlerType is used
        };

        struct cxx_function_desc {
            uint32_t magic_number;
            uint32_t max_state;
            uint32_t p_unwind_map;
            uint32_t try_blocks;
            uint32_t p_try_block_map;
            uint32_t ip_map_entries;
            uint32_t p_ip_to_state_map;
            uint32_t unwind_help;
            uint32_t p_es_type_list; // VC7+ only
            uint32_t eh_flags;       // VC8+ only bit 0 set if function was compiled with /EHs
        };

        struct cxx_unwind_map_entry {
            uint32_t to_state;   // target state
            uint32_t p_action;   // action to perform (unwind funclet address)
        };

        struct cxx_try_block_map_entry {
            uint32_t try_low;
            uint32_t try_high;      // this try {} covers states ranging from tryLow to tryHigh
            uint32_t catch_high;    // highest state inside catch handlers of this try
            uint32_t catches;       // number of catch handlers
            uint32_t p_handler_array; // rva handler_type catch handlers table
        };

        struct cxx_ip_to_state_map_entry {
            uint32_t ip; // Image relative offset of IP
            uint32_t state;
        };

#pragma pack(pop)


        struct cxx_try_block_map_info {

        private:
            uint32_t _try_low;
            uint32_t _try_high;
            uint32_t _catch_high;
            uint32_t _catches;
            uint32_t _p_handler_array;

            std::vector<cxx_handler_type> _handler_entries;
        public:
            cxx_try_block_map_info()
                : _try_low(0)
                , _try_high(0)
                , _catch_high(0)
                , _catches(0)
                , _p_handler_array(0) {}
            cxx_try_block_map_info(const cxx_try_block_map_entry& info)
                : _try_low(info.try_low)
                , _try_high(info.try_high)
                , _catch_high(info.catch_high)
                , _catches(info.catches)
                , _p_handler_array(info.p_handler_array) {}

            cxx_try_block_map_info(const cxx_try_block_map_info& info) = default;
            ~cxx_try_block_map_info() {}

            cxx_try_block_map_info& operator=(const cxx_try_block_map_info& info) = default;
        
        public:
            constexpr void set_try_low(uint32_t try_low) {
                _try_low = try_low;
            }
            constexpr void set_try_high(uint32_t try_high) {
                _try_high = try_high;
            }
            constexpr void set_catch_high(uint32_t catch_high) {
                _catch_high = catch_high;
            }
            constexpr void set_catches(uint32_t catches) {
                _catches = catches;
            }
            inline void set_p_handler_array(uint32_t p_handler_array) {
                _p_handler_array = p_handler_array;
            }

        public:
            constexpr uint32_t get_try_low() const {
                return _try_low;
            }
            constexpr uint32_t get_try_high() const {
                return _try_high;
            }
            constexpr uint32_t get_catch_high() const {
                return _catch_high;
            }
            constexpr uint32_t get_catches() const {
                return _catches;
            }
            constexpr uint32_t get_p_handler_array() const {
                return _p_handler_array;
            }
            constexpr std::vector<cxx_handler_type>& get_handler_entries() {
                return _handler_entries;
            }
            constexpr const std::vector<cxx_handler_type>& get_handler_entries() const {
                return _handler_entries;
            }
        };

        struct cxx_exception_func_info {

        private:
            uint32_t _magic_number;
            uint32_t _max_state;
            uint32_t _p_unwind_map;
            uint32_t _try_blocks;
            uint32_t _p_try_block_map;
            uint32_t _ip_map_entries;
            uint32_t _p_ip_to_state_map;
            uint32_t _unwind_help;
            uint32_t _p_es_type_list;
            uint32_t _eh_flags;

            std::vector<cxx_unwind_map_entry> _unwind_map_entries{};
            std::vector<cxx_try_block_map_info> _try_block_map_entries{};
            std::vector<cxx_ip_to_state_map_entry> _ip_to_state_map_entries{};

        public:
            cxx_exception_func_info()
                : _magic_number(0)
                , _max_state(0)
                , _p_unwind_map(0)
                , _try_blocks(0)
                , _p_try_block_map(0)
                , _ip_map_entries(0)
                , _p_ip_to_state_map(0)
                , _unwind_help(0)
                , _p_es_type_list(0)
                , _eh_flags(0) {}
            cxx_exception_func_info(const cxx_function_desc& desc)
                : _magic_number(desc.magic_number)
                , _max_state(desc.max_state)
                , _p_unwind_map(desc.p_unwind_map)
                , _try_blocks(desc.try_blocks)
                , _p_try_block_map(desc.p_try_block_map)
                , _ip_map_entries(desc.ip_map_entries)
                , _p_ip_to_state_map(desc.p_ip_to_state_map)
                , _unwind_help(desc.unwind_help)
                , _p_es_type_list(desc.p_es_type_list)
                , _eh_flags(desc.eh_flags) {}

            cxx_exception_func_info(const cxx_exception_func_info& info) = default;
            ~cxx_exception_func_info() {}

            cxx_exception_func_info& operator=(const cxx_exception_func_info& info) = default;
        
        public:
            constexpr void set_magic_number(uint32_t magic_number) {
                _magic_number = magic_number;
            }
            constexpr void set_max_state(uint32_t max_state) {
                _max_state = max_state;
            }
            constexpr void set_p_unwind_map(uint32_t p_unwind_map) {
                _p_unwind_map = p_unwind_map;
            }
            constexpr void set_try_blocks(uint32_t try_blocks) {
                _try_blocks = try_blocks;
            }
            constexpr void set_p_try_block_map(uint32_t p_try_block_map) {
                _p_try_block_map = p_try_block_map;
            }
            constexpr void set_ip_map_entries(uint32_t ip_map_entries) {
                _ip_map_entries = ip_map_entries;
            }
            constexpr void set_p_ip_to_state_map(uint32_t p_ip_to_state_map) {
                _p_ip_to_state_map = p_ip_to_state_map;
            }
            constexpr void set_unwind_help(uint32_t unwind_help) {
                _unwind_help = unwind_help;
            }
            constexpr void set_p_es_type_list(uint32_t p_es_type_list) {
                _magic_number = p_es_type_list;
            }
            constexpr void set_eh_flags(uint32_t eh_flags) {
                _eh_flags = eh_flags;
            }

        public:
            constexpr uint32_t get_magic_number() const {
                return _magic_number;
            }
            constexpr uint32_t get_max_state() const {
                return _max_state;
            }
            constexpr uint32_t get_p_unwind_map() const {
                return _p_unwind_map;
            }
            constexpr uint32_t get_try_blocks() const {
                return _try_blocks;
            }
            constexpr uint32_t get_p_try_block_map() const {
                return _p_try_block_map;
            }
            constexpr uint32_t get_ip_map_entries() const {
                return _ip_map_entries;
            }
            constexpr uint32_t get_p_ip_to_state_map() const {
                return _p_ip_to_state_map;
            }
            constexpr uint32_t get_unwind_help() const {
                return _unwind_help;
            }
            constexpr uint32_t get_p_es_type_list() const {
                return _p_es_type_list;
            }
            constexpr uint32_t get_eh_flags() const {
                return _eh_flags;
            }
            constexpr std::vector<cxx_unwind_map_entry>& get_unwind_map_entries() {
                return _unwind_map_entries;
            }
            constexpr const std::vector<cxx_unwind_map_entry>& get_unwind_map_entries() const {
                return _unwind_map_entries;
            }
            constexpr std::vector<cxx_try_block_map_info>& get_try_block_map_entries() {
                return _try_block_map_entries;
            }
            constexpr const std::vector<cxx_try_block_map_info>& get_try_block_map_entries() const {
                return _try_block_map_entries;
            }
            constexpr std::vector<cxx_ip_to_state_map_entry>& get_ip_to_state_map_entries() {
                return _ip_to_state_map_entries;
            }
            constexpr const std::vector<cxx_ip_to_state_map_entry>& get_ip_to_state_map_entries() const {
                return _ip_to_state_map_entries;
            }
        };

        struct c_specific_handler_parameters_data {
            std::vector<cxx_scope_table_entry> table;

            c_specific_handler_parameters_data() {}
            c_specific_handler_parameters_data(const std::vector<cxx_scope_table_entry>& table)
                : table(table) {}
            c_specific_handler_parameters_data(const c_specific_handler_parameters_data& data)
                : table(data.table) {}
        };

        struct delphi_specific_handler_parameters_data {
            std::vector<delphi_scope_table_entry> table;

            delphi_specific_handler_parameters_data() {}
            delphi_specific_handler_parameters_data(const std::vector<delphi_scope_table_entry>& table)
                : table(table) {}
            delphi_specific_handler_parameters_data(const delphi_specific_handler_parameters_data& data)
                : table(data.table) {}
        };

        struct llvm_specific_handler_parameters_data {
            uint32_t data_rva;

            llvm_specific_handler_parameters_data()
                : data_rva(0) {}
            llvm_specific_handler_parameters_data(uint32_t data_rva)
                : data_rva(data_rva) {}
            llvm_specific_handler_parameters_data(const llvm_specific_handler_parameters_data& data)
                : data_rva(data.data_rva) {}
        };

        struct gs_handler_check_parameters_data {
            uint32_t gs_data;

            gs_handler_check_parameters_data()
                : gs_data(0) {}
            gs_handler_check_parameters_data(uint32_t gs_data)
                : gs_data(gs_data) {}
            gs_handler_check_parameters_data(const gs_handler_check_parameters_data& data)
                : gs_data(data.gs_data) {}
        };

        struct gs_handler_check_seh_parameters_data {
            std::vector<cxx_scope_table_entry> table;
            uint32_t gs_data;

            gs_handler_check_seh_parameters_data()
                : gs_data(0) {}
            gs_handler_check_seh_parameters_data(const std::vector<cxx_scope_table_entry>& table, uint32_t gs_data)
                : table(table), gs_data(gs_data) {}
            gs_handler_check_seh_parameters_data(const gs_handler_check_seh_parameters_data& data)
                : table(data.table), gs_data(data.gs_data) {};
        };

        struct cxx_frame_handler3_parameters_data {
            cxx_exception_func_info func_info;

            cxx_frame_handler3_parameters_data() {}
            cxx_frame_handler3_parameters_data(const cxx_exception_func_info& func_info)
                : func_info(func_info) {}
            cxx_frame_handler3_parameters_data(const cxx_frame_handler3_parameters_data& data)
                : func_info(data.func_info) {}
        };

        struct gs_handler_check_eh_parameters_data {
            cxx_exception_func_info func_info;
            uint32_t gs_data;

            gs_handler_check_eh_parameters_data()
                : gs_data(0) {}
            gs_handler_check_eh_parameters_data(const cxx_exception_func_info& func_info, uint32_t gs_data)
                : func_info(func_info), gs_data(gs_data) {}
            gs_handler_check_eh_parameters_data(const gs_handler_check_eh_parameters_data& data)
                : func_info(data.func_info), gs_data(data.gs_data) {};
        };


        struct exceptions_handler_specific_data {
          
        private:
            void* _data;
            exception_handler_type _data_type;

        public:
            exceptions_handler_specific_data()
                : _data(0)
                , _data_type(unknown_handler) {}
            
            exceptions_handler_specific_data(const exceptions_handler_specific_data& data);
            ~exceptions_handler_specific_data();

            exceptions_handler_specific_data& operator=(const exceptions_handler_specific_data& data);

            exceptions_handler_specific_data& operator=(const c_specific_handler_parameters_data& data) {
                set_c_specific_handler_parameters_data(data);
                return *this;
            }
            exceptions_handler_specific_data& operator=(const delphi_specific_handler_parameters_data& data) {
                set_delphi_specific_handler_parameters_data(data);
                return *this;
            }
            exceptions_handler_specific_data& operator=(const llvm_specific_handler_parameters_data& data) {
                set_llvm_specific_handler_parameters_data(data);
                return *this;
            }
            exceptions_handler_specific_data& operator=(const gs_handler_check_parameters_data& data) {
                set_gs_handler_check_parameters_data(data);
                return *this;
            }
            exceptions_handler_specific_data& operator=(const gs_handler_check_seh_parameters_data& data) {
                set_gs_handler_check_seh_parameters_data(data);
                return *this;
            }
            exceptions_handler_specific_data& operator=(const cxx_frame_handler3_parameters_data& data) {
                set_cxx_frame_handler3_parameters_data(data);
                return *this;
            }
            exceptions_handler_specific_data& operator=(const gs_handler_check_eh_parameters_data& data) {
                set_gs_handler_check_eh_parameters_data(data);
                return *this;
            }

        public:
            inline void set_c_specific_handler_parameters_data(const  c_specific_handler_parameters_data& data) {
                _data = new c_specific_handler_parameters_data(data);
                _data_type = __c_specific_handler;
            }
            inline void set_delphi_specific_handler_parameters_data(const  delphi_specific_handler_parameters_data& data) {
                _data = new delphi_specific_handler_parameters_data(data);
                _data_type = __delphi_specific_handler;
            }
            inline void set_llvm_specific_handler_parameters_data(const  llvm_specific_handler_parameters_data& data) {
                _data = new llvm_specific_handler_parameters_data(data);
                _data_type = __llvm_specific_handler;
            }
            inline void set_gs_handler_check_parameters_data(const  gs_handler_check_parameters_data& data) {
                _data = new gs_handler_check_parameters_data(data);
                _data_type = __gs_handler_check;
            }
            inline void set_gs_handler_check_seh_parameters_data(const  gs_handler_check_seh_parameters_data& data) {
                _data = new gs_handler_check_seh_parameters_data(data);
                _data_type = __gs_handler_check_seh;
            }
            inline void set_cxx_frame_handler3_parameters_data(const  cxx_frame_handler3_parameters_data& data) {
                _data = new cxx_frame_handler3_parameters_data(data);
                _data_type = __cxx_frame_handler3;
            }
            inline void set_gs_handler_check_eh_parameters_data(const  gs_handler_check_eh_parameters_data& data) {
                _data = new gs_handler_check_eh_parameters_data(data);
                _data_type = __gs_handler_check_eh;
            }

        public:
            constexpr const c_specific_handler_parameters_data* get_c_specific_handler_parameters_data() const {
                return (const c_specific_handler_parameters_data*)_data;
            }
            constexpr const delphi_specific_handler_parameters_data* get_delphi_specific_handler_parameters_data() const {
                return (const delphi_specific_handler_parameters_data*)_data;
            }
            constexpr const llvm_specific_handler_parameters_data* get_llvm_specific_handler_parameters_data() const {
                return (const llvm_specific_handler_parameters_data*)_data;
            }
            constexpr const gs_handler_check_parameters_data* get_gs_handler_check_parameters_data() const {
                return (const gs_handler_check_parameters_data*)_data;
            }
            constexpr const gs_handler_check_seh_parameters_data* get_gs_handler_check_seh_parameters_data() const {
                return (const gs_handler_check_seh_parameters_data*)_data;
            }
            constexpr const cxx_frame_handler3_parameters_data* get_cxx_frame_handler3_parameters_data() const {
                return (const cxx_frame_handler3_parameters_data*)_data;
            }
            constexpr const gs_handler_check_eh_parameters_data* get_gs_handler_check_eh_parameters_data() const {
                return (const gs_handler_check_eh_parameters_data*)_data;
            }

            constexpr c_specific_handler_parameters_data* get_c_specific_handler_parameters_data() {
                return (c_specific_handler_parameters_data*)_data;
            }
            constexpr delphi_specific_handler_parameters_data* get_delphi_specific_handler_parameters_data() {
                return (delphi_specific_handler_parameters_data*)_data;
            }
            constexpr llvm_specific_handler_parameters_data* get_llvm_specific_handler_parameters_data() {
                return (llvm_specific_handler_parameters_data*)_data;
            }
            constexpr gs_handler_check_parameters_data* get_gs_handler_check_parameters_data() {
                return (gs_handler_check_parameters_data*)_data;
            }
            constexpr gs_handler_check_seh_parameters_data* get_gs_handler_check_seh_parameters_data() {
                return (gs_handler_check_seh_parameters_data*)_data;
            }
            constexpr cxx_frame_handler3_parameters_data* get_cxx_frame_handler3_parameters_data() {
                return (cxx_frame_handler3_parameters_data*)_data;
            }
            constexpr gs_handler_check_eh_parameters_data* get_gs_handler_check_eh_parameters_data() {
                return (gs_handler_check_eh_parameters_data*)_data;
            }
            constexpr exception_handler_type get_data_type() const {
                return _data_type;
            }
        };


        enum ex_exceptions_info_result {
            ex_exceptions_info_ok,
            ex_exceptions_info_has_unknown,
            ex_exceptions_info_has_error,
        };

        ex_exceptions_info_result get_extended_exception_info(_Inout_ pe_image_full& image_full);
        ex_exceptions_info_result get_extended_exception_info_placement(_In_ const pe_image_full& image_full, _Inout_ pe_placement& placement);

        void build_extended_exceptions_info(_Inout_ pe_image_full& image_full);

    }
}