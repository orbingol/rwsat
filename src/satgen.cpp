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

#include "common.h"


// SATGEN executable
int main(int argc, char **argv)
{
    // Print app information
    std::cout << "SATGEN: Geometry Generator for ACIS" << std::endl;
    std::cout << "Copyright (c) 2019 IDEA Lab at Iowa State University. " << "Licensed under the terms of BSD License.\n" << std::endl;

    // File name to write
    std::string fileName;

    if (argc < 2 || argc > 3)
    {
        std::cout << "Usage: " << argv[0] << " FILENAME OPTIONS\n" << std::endl;
#ifdef _MSC_VER
        std::cout << "Note: A license key should be provided using 'license_key' or 'license_file' arguments." << std::endl;
#endif
        return EXIT_FAILURE;
    }
    else
        fileName = std::string(argv[1]);

    // Initialize configuration
    Config cfg;

    // Update configuration
    if (argc == 3)
        parseConfig(argv[2], cfg);

    // Print configuration
    if (cfg.show_config())
    {
        std::cout << "Using configuration:" << std::endl;
        for (auto p : cfg.params)
            std::cout << "  - " << p.first << ": " << p.second.first << std::endl;
    }

    // Initialize a variable to store ACIS API outcome
    outcome res;

    // Start ACIS
    res = api_start_modeller();
    checkOutcome(res, "api_start_modeller", __LINE__, cfg);
 
    // Unlock ACIS (required only on Windows)
#ifdef _MSC_VER
    if (!unlockACIS(cfg))
        return EXIT_FAILURE;
#endif

    // Entities to be saved
    ENTITY_LIST saveList;

    // Create a cuboid
    BODY *cuboid;
    res = api_make_cuboid(5, 10, 20, cuboid);
    checkOutcome(res, "api_make_cuboid", __LINE__, cfg);

    // Create transformation
    SPAposition rotate_pos(20, 0, 0);
    SPAtransf r = rotate_transf(M_PI / 4, SPAvector(0, 0, 1));
    SPAvector to_rotate_pos = rotate_pos - SPAposition(0, 0, 0);
    SPAtransf t = translate_transf(to_rotate_pos);
    SPAtransf tc = t.inverse() * r * t;

    // Transform cuboid
    res = api_apply_transf(cuboid, tc);
    checkOutcome(res, "api_apply_transf", __LINE__, cfg);
    res = api_change_body_trans(cuboid, NULL);
    checkOutcome(res, "api_change_body_trans", __LINE__, cfg);

    // Add to save list
    saveList.add(cuboid);

    // Create a torus
    BODY *torus;
    res = api_make_torus(25, 12.5, torus);
    checkOutcome(res, "api_make_torus", __LINE__, cfg);

    // Create transformation
    SPAposition translate_pos1(0, 0, -100);
    SPAvector to_translate_pos1 = translate_pos1 - SPAposition(0, 0, 0);
    SPAtransf tt1 = translate_transf(to_translate_pos1);

    // Transform torus
    res = api_apply_transf(torus, tt1);
    checkOutcome(res, "api_apply_transf", __LINE__, cfg);
    res = api_change_body_trans(torus, NULL);
    checkOutcome(res, "api_change_body_trans", __LINE__, cfg);

    // Add to save list
    saveList.add(torus);

    // Create a toroidal face
    FACE *torodialFace;
    SPAposition center(25, 25, 25);
    SPAvector normal(0, 0, 1);
    res = api_face_torus(center, 10, 2.5, 0, 360, 0, 180, &normal, torodialFace);
    checkOutcome(res, "api_face_torus", __LINE__, cfg);

    // Create a sheet body from the toroidal face
    FACE *faces[1];
    faces[0] = torodialFace;
    BODY *torus2;
    res = api_sheet_from_ff(1, faces, torus2);
    checkOutcome(res, "api_sheet_from_ff", __LINE__, cfg);

    // Create transformation
    SPAposition translate_pos2(0, 0, -100);
    SPAvector to_translate_pos2 = translate_pos2 - SPAposition(0, 0, 0);
    SPAtransf tt2 = translate_transf(to_translate_pos1);

    // Transform toroidal sheet body
    res = api_apply_transf(torus2, tt2);
    checkOutcome(res, "api_apply_transf", __LINE__, cfg);
    res = api_change_body_trans(torus2, NULL);
    checkOutcome(res, "api_change_body_trans", __LINE__, cfg);

    // Add to save list
    saveList.add(torus2);

    // Make a sphere
    BODY *sphere;
    res = api_make_sphere(2.5, sphere);
    checkOutcome(res, "api_make_sphere", __LINE__, cfg);

    // Create transformation
    SPAposition translate_pos3(100, 100, 0);
    SPAvector to_translate_pos3 = translate_pos3 - SPAposition(0, 0, 0);
    SPAtransf ts = translate_transf(to_translate_pos3);

    // Transform sphere
    res = api_apply_transf(sphere, ts);
    checkOutcome(res, "api_apply_transf", __LINE__, cfg);
    res = api_change_body_trans(sphere, NULL);
    checkOutcome(res, "api_change_body_trans", __LINE__, cfg);

    // Add to save list
    saveList.add(sphere);
    
    // Save list as a .SAT file
    if (saveSatFile(saveList, fileName, cfg))
        std::cout << "[SUCCESS] '" << fileName << "' was generated successfully!" << std::endl;

    // Stop ACIS
    res = api_stop_modeller();
    checkOutcome(res, "api_stop_modeller", __LINE__, cfg);

    return EXIT_SUCCESS;
}
