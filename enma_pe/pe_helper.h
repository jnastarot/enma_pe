#pragma once



void demangle(std::string name, std::string& demangled_name);


double   get_data_entropy(const std::vector<uint8_t> &data);
uint32_t calculate_checksum(const std::vector<uint8_t> &file);

#include "pe_demangler.h"
#include "pe_map_format.h"
#include "pe_tds_parser.h"
#include "pe_string_finder.h"
