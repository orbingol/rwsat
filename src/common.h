/*
Copyright (c) 2019, Integrated Design and Engineering Analysis Laboratory (IDEA Lab),
Iowa State University. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL IDEA LAB OR IOWA STATE UNIVERSITY BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef COMMON_H
#define COMMON_H

// C++ includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <exception>
#include <cstddef>
#include <cstdio>
#include <cmath>

// External libraries
#include "ACIS.h"


// Application configuration
struct Config {
    // Config parameters
    std::map< std::string, std::pair<std::string, std::string> > params = {
        { "show_config", { "0", "Prints the configuration" } },
        { "license_file",{  "", "Name of the ACIS license file" } },
        { "license_key", { "", "ACIS unlock key" } },
        { "acis_warnings", { "0", "Enable ACIS warnings" } },
        { "warnings", { "0", "Enable application warnings" } },
        { "normalize", { "1", "Normalize knot vectors and rescale trim curves" } },
        { "trims", { "1", "Extract trim curves" } },
        { "sense", { "1", "Extract surface and trim curve sense data" } },
        { "transform", { "0", "Apply transforms" } },
        {  "bspline", { "1", "Convert the underlying geometry to B-Spline" } }
    };

    // Methods
    const char* acis_license();
    bool show_config();
    bool acis_warnings();
    bool warnings();
    bool normalize();
    bool trims();
    bool sense();
    bool transform();
    bool bspline();
};

// Function prototypes
std::string readLicenseFile(std::string &, bool = true);
void parseConfig(char *, Config &);
void updateConfig(std::string &, std::string &, Config &);
void checkOutcome(const outcome&, const char*, int, Config &);
bool unlockACIS(Config &cfg);
bool saveSatFile(ENTITY_LIST &, std::string &, Config &);
bool readSatFile(std::string &, ENTITY_LIST &, Config &);

#endif /* COMMON_H */
