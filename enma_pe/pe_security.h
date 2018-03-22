#pragma once



bool erase_security_table(_Inout_ pe_image &image,
    _Inout_opt_ std::vector<erased_zone>* zones = 0);