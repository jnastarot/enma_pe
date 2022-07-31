<h1 align="center">ENMA PE</h1>

[![Build status](https://ci.appveyor.com/api/projects/status/b6bq9w9b1b7rjaoy)](https://ci.appveyor.com/project/jnastarot/enma-pe-u6wir)
[![Build Status](https://travis-ci.org/jnastarot/enma_pe.svg?branch=master)](https://travis-ci.org/jnastarot/enma_pe)
[![License](https://img.shields.io/badge/license-BSD3-blue.svg)](https://github.com/jnastarot/enma_pe/blob/master/LICENSE) 




```
--------------------------------------------------------------------------------
Name....: enma pe
Author..: JNA
e.mail..: jnastarot@gmail.com
--------------------------------------------------------------------------------
```

## Supported directories
| directory name | reading | building | getting placement |
| :-------------- | :-------: | :-------: | :-------: |
| `export` | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| `import` | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| `import bound ` | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| `import delay` | :heavy_check_mark: | :heavy_multiplication_x: | :heavy_check_mark: |
| `resources` | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| `exceptions` | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| `security` | :heavy_check_mark: | :heavy_multiplication_x: | :heavy_check_mark: |
| `relocations` | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| `debug` | :heavy_check_mark: | :heavy_multiplication_x: | :heavy_check_mark: |
| `tls` | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| `load config` | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| `.NET meta data` | :heavy_check_mark: | :heavy_multiplication_x: | :heavy_multiplication_x: |

---

## Additional features
| feature name | description |
| :------ | :----------- |
| `build_pe_image` | build pe_image to packed binary version |
| `load_virtual_pe_image` | load image in pe_image format from loaded in memory |
| `get_runtime_type_information` | parsing runtime type information(MSVC only) |
| `get_extended_exception_info` | parsing of extended variables for x64 exceptions |
| `build_extended_exceptions_info` | building of extended variables for x64 exceptions |
| `get_strings_from_image` | extracting ascii and wide strings |
| `get_image_rich_header` | getting and building rich data |
| `get_section_entropy` | calculating entropy of section |
| `calculate_checksum` | calculating checksum of `pe image` |

## Usage examples

### Image/Section IO

#### IO using file offset (`raw`)
```c++
pe_image_io image_io(image, enma_io_addressing_type::enma_io_address_raw);
```
```c++
pe_section_io section_io(section, enma_io_addressing_type::enma_io_address_raw);
```
#### IO using relative virtual offset (`rva`)
```c++
pe_image_io image_io(image, enma_io_addressing_type::enma_io_address_rva);
```
```c++
pe_image_io section_io(section, enma_io_addressing_type::enma_io_address_rva);
```

#### IO using virtual address (`va`)
```c++
pe_image_io image_io(image, enma_io_addressing_type::enma_io_address_va);
```
```c++
pe_image_io section_io(section, enma_io_addressing_type::enma_io_address_va);
```

#### IO with size changing policy (allow incrase size)

```c++
pe_image_io image_io(image, enma_io_mode::enma_io_mode_allow_expand, enma_io_addressing_type::enma_io_address_rva);
```
```c++
pe_image_io section_io(image, enma_io_mode::enma_io_mode_allow_expand, enma_io_addressing_type::enma_io_address_rva);
```
#### IO with size changing policy (disallow incrase size)

```c++
pe_image_io image_io(image, enma_io_mode::enma_io_mode_default, enma_io_addressing_type::enma_io_address_rva);
```
```c++
pe_image_io section_io(section, enma_io_mode::enma_io_mode_default, enma_io_addressing_type::enma_io_address_rva);
```

#### Image read data
```c++
using namespace enma;

int main() {

    pe_image image(L"C:\\Windows\\System32\\kernel32.dll");
    
    pe_image_io image_io(image, enma_io_addressing_type::enma_io_address_raw);

    image_dos_header header;
    image.get_dos_header(header); 

    uint32_t magic;

    if (image_io.set_image_offset(header.e_lfanew).read(magic) == enma_io_code::enma_io_success) {

        if (magic == IMAGE_NT_SIGNATURE) {

            //...
        }
    }
    
    return 0;
}
```
#### Image write data

```c++
using namespace enma;

int main() {

    pe_image image(L"C:\\Windows\\System32\\kernel32.dll");
    
    pe_image_io image_io(image, enma_io_addressing_type::enma_io_address_rva);

    std::vector<uint8_t> bytes_of_entry{0xCC};
            
    if (image_io.set_image_offset(image.get_entry_point()).write(bytes_of_entry) == enma_io_code::enma_io_success) {

        //...
    }
    
    return 0;
}
```

### Imports

```c++
using namespace enma;

int main() {

    pe_image image(L"C:\\Windows\\System32\\kernel32.dll");
    
    pe_bound_import_directory bound_imports;
    pe_import_directory imports;

    get_bound_import_directory(image, bound_imports);

    if (get_import_directory(image, imports, bound_imports) == pe_directory_code::pe_directory_code_success) {

        for (auto& imp_library : imports.get_libraries()) {

            cout << imp_library.get_library_name();

            if (bound_imports.has_library(imp_library.get_library_name())) {
                cout << "  BOUNDED: " << bound_imports.get_library(imp_library.get_library_name())->get_timestamp();
            }

            cout << endl;

            for (auto& imp_function : imp_library.get_functions()) {

                cout << "RVA: " << hex << imp_function.get_iat_rva();
                cout << " ORD: " << imp_function.get_ordinal();
                cout << " HINT: " << hex << imp_function.get_hint();

                if (imp_function.is_import_by_name()) {
                    cout << " NAME: " << imp_function.get_func_name();
                }

                cout << endl;
            }
        }
    }
    
    return 0;
}
```

### Exports

```c++
using namespace enma;

int main() {

    pe_image image(L"C:\\Windows\\System32\\kernel32.dll");
    
    pe_export_directory exports;
    if (get_export_directory(image, exports) == pe_directory_code::pe_directory_code_success) {

        for (auto& func : exports.get_functions()) {

            cout << "RVA: " << hex << func.get_rva();
            cout << " NAME ORD: " << hex << func.get_name_ordinal();
            cout << " ORD: " << hex << func.get_ordinal();

            if (func.has_name()) {
                cout << " NAME: " << hex << func.get_func_name();
            }

            if (func.is_forward()) {
                cout << " FWD: " << hex << func.get_forward_name();
            }

            cout << endl;
        }
    }
    
    return 0;
}
```

### Preview information

```c++
using namespace enma;

bool get_image_icon(pe_resource_directory& resources) {
  
    hl::pe_image_preview_icon preview(resources);

    hl::pe_image_icon_group icon_group;
    uint16_t language_id;

    if (!preview.get_default_icon_group(icon_group, language_id)) {

        return false;
    }

    std::vector<uint8_t> icon_buf;
    
    if (!icon_group.get_icon(resources, language_id, icon_buf)) {

        return false;
    }

    //icon_buf <- raw icon
}

bool get_image_version_info(pe_resource_directory& resources) {

    hl::pe_image_preview_version_info preview(resources);

    hl::pe_image_version_info version;
    uint16_t language_id;

    if (!preview.get_default_version(version, language_id)) {

        return false;
    }

    for (auto& translate : version.get_string_table()) {

        for (auto& kv : translate.second) {

            wcout << kv.first << L" : " << kv.second << endl;
        }
    }
}

int main() {

    pe_image image(L"C:\\Windows\\System32\\kernel32.dll");
    
    pe_resource_directory resources;
    if (get_resources_directory(image, resources) == 	  pe_directory_code::pe_directory_code_success) {

        get_image_icon(resources);
        get_image_version_info(resources);
    }
    
    return 0;
}
```
### Image manifests

```c++

bool get_internal_app_assembly(const pe_image& image, std::list<std::string>& assemblies) {

    pe_resource_directory resources;

    if (get_resources_directory(image, resources) !=
        pe_directory_code_success) {

        return false;
    }

    auto& res_list = resources.get_entry_list();

    for (auto& lv1_res_entry : res_list) {

        if (lv1_res_entry.get_id() == hl::kResourceManifest) {

            auto& lv2_res_entry = lv1_res_entry.get_resource_directory();

            for (auto& lv2_res_entry : lv2_res_entry.get_entry_list()) {

                lv2_res_entry.get_id(); // <- this is lang id

                auto& lv3_res_entry = lv2_res_entry.get_resource_directory();

                for (auto& lv3_res_entry : lv3_res_entry.get_entry_list()) {

                    if (lv3_res_entry.is_includes_data()) {

                        auto& data_res_entry = lv3_res_entry.get_data_entry();

                        std::string assembly_a;

                        lv3_res_entry.get_id(); // <- this is resource id in third tree root

                        assembly_a.resize(data_res_entry.get_data().size());

                        memcpy((void*)assembly_a.data(),
                            data_res_entry.get_data().data(), assembly_a.size());

                        if ((uint8_t)assembly_a[0] == 0xEF &&
                            (uint8_t)assembly_a[1] == 0xBB &&
                            (uint8_t)assembly_a[2] == 0xBF) {

                            assembly_a.erase(assembly_a.begin(), assembly_a.begin() + 3);
                        }

                        data_res_entry.get_codepage(); // <- code page of resource


                        assemblies.push_back(assembly_a); //push text of manifest
                    }
                }
            }
        }
    }

    return true;
}
```


### References

[https://github.com/dishather/richprint/](https://github.com/dishather/richprint/)<br>
[https://github.com/radare/radare2](https://github.com/radare/radare2)<br>
[https://github.com/cmu-sei/pharos](https://github.com/cmu-sei/pharos)<br>
[https://github.com/JusticeRage/Manalyze](https://github.com/JusticeRage/Manalyze)<br>
[https://kaimi.io/2012/09/portable-executable-library/](https://kaimi.io/2012/09/portable-executable-library/)<br>