<h1 align="center">ENMA PE</h1>

[![Build status](https://ci.appveyor.com/api/projects/status/b6bq9w9b1b7rjaoy)](https://ci.appveyor.com/project/jnastarot/enma-pe-u6wir)
[![Build Status](https://travis-ci.org/jnastarot/enma_pe.svg?branch=master)](https://travis-ci.org/jnastarot/enma_pe)
[![License](https://img.shields.io/badge/license-BSD3-blue.svg)](https://github.com/jnastarot/enma_pe/blob/master/LICENSE) 




```
--------------------------------------------------------------------------------
Name....: enma pe
Author..: JNA
e.mail..: jnastarot@yandex.ru
--------------------------------------------------------------------------------
```

### Supported directories
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

### Additional features
| feature name | description |
| :------ | :----------- |
| `build_pe_image` | build pe_image to packed binary version |
| `load_virtual_image` | load image in pe_image format from loaded in memory |
| `get_runtime_type_information` | parsing runtime type information(MSVC only) |
| `get_extended_exception_info` | parsing of extended variables for x64 exceptions |
| `build_extended_exceptions_info` | building of extended variables for x64 exceptions |
| `get_strings_from_image` | extracting ascii and wide strings |
| `get_image_rich_header` | getting and building rich data |
| `get_section_entropy` | calculating entropy of section |
| `calculate_checksum` | calculating checksum of `pe image` |

### References

[https://github.com/dishather/richprint/](https://github.com/dishather/richprint/)<br>
[https://github.com/radare/radare2](https://github.com/radare/radare2)<br>
[https://github.com/cmu-sei/pharos](https://github.com/cmu-sei/pharos)<br>
[https://github.com/JusticeRage/Manalyze](https://github.com/JusticeRage/Manalyze)<br>
[https://kaimi.io/2012/09/portable-executable-library/](https://kaimi.io/2012/09/portable-executable-library/)<br>