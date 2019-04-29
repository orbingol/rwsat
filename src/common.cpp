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
    * Neither the name of the Iowa State University nor the
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

#include "common.h"


const char* Config::acis_license()
{ 
    if (!params.at("license_file").first.empty()) 
        params["license_key"].first = readLicenseFile(params.at("license_file").first); 
    return params.at("license_key").first.c_str();
}

bool Config::show_config()
{ 
    return bool(std::atoi(params.at("show_config").first.c_str())); 
}

bool Config::acis_warnings()
{
    return bool(std::atoi(params.at("acis_warnings").first.c_str()));
}

bool Config::warnings() {
    return bool(std::atoi(params.at("warnings").first.c_str()));
}

bool Config::normalize() 
{
    return bool(std::atoi(params.at("normalize").first.c_str())); 
}

bool Config::trims()
{
    return bool(std::atoi(params.at("trims").first.c_str())); 
}

bool Config::sense()
{
    return bool(std::atoi(params.at("sense").first.c_str())); 
}

bool Config::transform()
{
    return bool(std::atoi(params.at("transform").first.c_str()));
}

bool Config::bspline()
{
    return bool(std::atoi(params.at("bspline").first.c_str()));
}

// Read license file
std::string readLicenseFile(std::string &fileName, bool clean)
{
    std::string fileContent;
    std::ifstream fileRead(fileName);
    if (fileRead)
    {
        std::stringstream buf;
        buf << fileRead.rdbuf();
        fileContent = buf.str();
        if (clean)
        {
            fileContent.erase(std::remove(fileContent.begin(), fileContent.end(), ' '), fileContent.end());
            fileContent.erase(std::remove(fileContent.begin(), fileContent.end(), '\n'), fileContent.end());
            fileContent.erase(std::remove(fileContent.begin(), fileContent.end(), '\r'), fileContent.end());
            fileContent.erase(std::remove(fileContent.begin(), fileContent.end(), '\t'), fileContent.end());
            fileContent.erase(std::remove(fileContent.begin(), fileContent.end(), '"'), fileContent.end());
            fileContent.erase(std::remove(fileContent.begin(), fileContent.end(), ';'), fileContent.end());
        }
    }
    return fileContent;
}

// Parse configuration from a string
void parseConfig(char *conf_str, Config & cfg)
{
    // Define delimiters
    std::string delimiter = ";";
    std::string cfg_delimiter = "=";

    // Parse user config string
    std::string s(conf_str);
    while (true)
    {
        std::size_t pos = s.find(delimiter);
        std::string cfg_directive = s.substr(0, pos);
        std::size_t cfg_pos = cfg_directive.find(cfg_delimiter);
        if (cfg_pos != std::string::npos)
        {
            std::string key = cfg_directive.substr(0, cfg_pos);
            std::string value = cfg_directive.substr(cfg_pos + 1);
            updateConfig(key, value, cfg);
        }
        if (pos == std::string::npos)
            break;
        else
            s = s.substr(++pos);
    }
}

// Update application configuration
void updateConfig(std::string &key, std::string &value, Config &cfg)
{
    auto search = cfg.params.find(key);
    if (search != cfg.params.end())
    {
        std::string val;
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        if (value == "false" || value == "0")
            val = "0";
        else if (value == "true" || value == "0")
            val = "1";
        else
            val = std::string(value);
        cfg.params[key].first = val;
    }
}

// Unlock ACIS
bool unlockACIS(Config &cfg)
{
    spa_unlock_result out = spa_unlock_products(cfg.acis_license());

    bool retVal = true;
    switch (out.get_state())
    {
    case SPA_UNLOCK_PASS_WARN:
        if (cfg.acis_warnings())
            std::cout << "[LICENSE WARNING] " << out.get_message_text() << std::endl;
        break;
    case SPA_UNLOCK_FAIL:
        std::cout << "[LICENSE ERROR] " << out.get_message_text() << std::endl;
        retVal = false;
        break;
    default:
        /* License is valid -- SPA_UNLOCK_PASS */;
    }

    return retVal;
}

// Check ACIS API outcome
void checkOutcome(const outcome &res, const char *apiCall, int lineNumber, Config &cfg)
{
    // Check if ACIS has encountered any errors (fail-safe or critical)
    if (res.encountered_errors())
    {
        // Query for ACIS error number
        err_mess_type err_no = res.error_number();

        // Get error message
        std::string error_str = std::string(find_err_mess(err_no)) + " (" + std::string(find_err_ident(err_no)) + ")";

        // Flag to stop the application
        bool stopApp = false;

        // Display error information, crash program if critical
        if (res.ok())
        {
            if (cfg.acis_warnings())
                std::cout << "[ERROR] ACIS encountered a non-critical error: " << error_str << std::endl;
        }
        else
        {
            std::cout << "[ERROR] ACIS encountered a critical error: " << error_str << std::endl;
            stopApp = true;
        }

        // Print function name and its line number
        std::cout << "Function: " << apiCall << "; Line: " << lineNumber << std::endl;

        // Stop the application in the ACIS way
        if (stopApp)
            sys_error(err_no);
    }

    // Print warnings
    if (cfg.acis_warnings())
    {
        err_mess_type *warnings;
        int nwarn = get_warnings(warnings);
        if (nwarn > 0)
        {
            std::cout << "[WARNING] ACIS produced the following warnings:" << std::endl;
            for (int i = 0; i < nwarn; ++i)
                std::cout << "  - " << warnings[i] << ": " << find_err_mess(warnings[i]) << std::endl;
            // Reset number of warnings to zero
            init_warnings();
        }
    }

}

// Read ACIS file
bool readSatFile(std::string &fileName, ENTITY_LIST &readList, Config &cfg)
{
    // Try to open SAT file for reading
    FILE *fp = fopen(fileName.c_str(), "r");
    if (fp == NULL)
    {
        std::cerr << "[ERROR] Cannot open file '" << fileName << "' for reading!" << std::endl;
        return false;
    }

    // Initialize a variable to store ACIS API outcome
    outcome res;

    // Read the SAT file into an ENTITY_LIST
    res = api_restore_entity_list(fp, TRUE, readList);
    checkOutcome(res, "api_restore_entity_list", __LINE__, cfg);

    // Close file
    fclose(fp);

    return true;
}

// Save ACIS file
bool saveSatFile(ENTITY_LIST &saveList, std::string &fileName, Config &cfg)
{
    // Create FileInfo object
    FileInfo info;
    info.set_product_id(fileName.c_str());
    info.set_units(1.0);  // millimeters
    api_set_file_info(FileUnits | FileIdent, info);
    api_save_version(18, 0); // compatibility with the older versions

    // Set line numbers on
    api_set_int_option("sequence_save_files", 1);

    // Open file
    FILE *fp = fopen(fileName.c_str(), "w");
    if (fp == NULL)
    {
        std::cerr << "[ERROR] Cannot open file '" << fileName << "' for writing!" << std::endl;
        return false;
    }
    
    // Initialize a variable to store ACIS API outcome
    outcome res;

    // Save SAT file
    res = api_save_entity_list(fp, true, saveList);
    checkOutcome(res, "api_save_entity_list", __LINE__, cfg);

    // Close file
    fclose(fp);

    return true;
}
