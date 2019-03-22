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
#include "extract.h"


// RWSAT executable
int main(int argc, char **argv)
{
    // Print app information
    std::cout << "RWSAT: Spline Geometry Extractor for ACIS" << std::endl;
    std::cout << "Copyright (c) 2019 IDEA Lab at Iowa State University. " << "Licensed under the terms of BSD License.\n" << std::endl;

    // File name to read
    std::string filename;

    // Initialize configuration
    Config cfg;

    if (argc < 2 || argc > 3)
    {
        std::cout << "Usage: " << argv[0] << " FILENAME OPTIONS\n" << std::endl;
        std::cout << "Available options:" << std::endl;
        for (auto p : cfg.params)
            std::cout << "  - " << p.first << ": " << p.second.second << std::endl;
        std::cout << "\nExample: " << argv[0] << " my_file.sat normalize=false;trims=true" << std::endl;
#ifdef _MSC_VER
        std::cout << "Note: A license key should be provided using 'license_key' or 'license_file' arguments." << std::endl;
#endif
        return EXIT_FAILURE;
    }
    else
        filename = std::string(argv[1]);

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

    // Read the SAT file into an ENTITY_LIST
    ENTITY_LIST entities;
    if (!readSatFile(filename, entities, cfg))
    {
        res = api_stop_modeller();
        checkOutcome(res, "api_stop_modeller", __LINE__, cfg);
        return EXIT_FAILURE;
    }

    int ent_count = entities.iteration_count();
    for (int i = 0; i < ent_count; i++)
    {
        // Get current body
        BODY *currentBody = (BODY *)entities[i];

        // Workaround for periodic faces
        res = api_set_int_option("new_periodic_splitting", 1);
        checkOutcome(res, "api_set_int_option", __LINE__, cfg);
        res = api_split_periodic_faces(currentBody);
        checkOutcome(res, "api_split_periodic_faces", __LINE__, cfg);

        // Remove transformations
        if (!cfg.transform())
        {
            res = api_remove_transf(currentBody);
            checkOutcome(res, "api_remove_transf", __LINE__, cfg);
        }

        // Create the root JSON object
        Json::Value root;

        // Create shape definition for JSON
        Json::Value shapeDef;
        shapeDef["type"] = "surface";

        // Create data definition for JSON
        Json::Value dataDef;

        // Get the face list
        ENTITY_LIST face_list;
        res = api_get_faces(currentBody, face_list);
        checkOutcome(res, "api_get_faces", __LINE__, cfg);

        // Get face count
        int face_count = face_list.iteration_count();

        // Face count is equal to the number of surfaces
        shapeDef["count"] = face_count;

        for (int j = 0; j < face_count; j++)
        {
            // Get the current face
            FACE *f = (FACE *)face_list[j];

            // Get face sense
            logical faceSense = f->sense();

            // Convert the underlying geometry to B-spline representation
            if (cfg.bspline())
            {
                convert_to_spline_options convertOptions;
                convertOptions.set_do_edges(true);
                convertOptions.set_do_faces(true);
                convertOptions.set_in_place(true);
                res = api_convert_to_spline(f, &convertOptions);
                checkOutcome(res, "api_convert_to_spline", __LINE__, cfg);
            }

            /*** SURFACE EXTRACTION ***/

            // Check if the face has a spline surface or skip the face
            SURFACE *faceSurf = f->geometry();
            if (faceSurf->identity() != SPLINE_TYPE)
            {
                if (cfg.warnings())
                    std::cout << "[WARNING] Face #" << j << " of Body #" << i << " does not have a spline surface. Skipping..." << std::endl;
                continue;
            }

            // Extract the spline surface from the face
            bs3_surface bsurf;
            if (cfg.transform())
            {
                surface *surf = f->geometry()->trans_surface(get_owner_transf(f), f->sense());
                spline *spsurf = (spline *)surf;
                bsurf = spsurf->sur();
                bs3_surface_trans(bsurf, get_owner_transf(f));
            }
            else
            { 
                const surface &surf = f->geometry()->equation();
                const spline &spsurf = (spline &)surf;
                bsurf = spsurf.sur();
            }

            // Check if ACIS was able to compute the B-spline representation
            if (bsurf == NULL)
            {
                if (cfg.warnings())
                    std::cout << "[WARNING] Cannot extract B-spline surface from Face #" << j << " of Body #" << i << ". Skipping..." << std::endl;
                continue;
            }

            // Get the parametric range of the initial surface
            SPAinterval u_range = bs3_surface_range_u(bsurf);
            SPAinterval v_range = bs3_surface_range_v(bsurf);

            // Length of the parametric dimensions
            double surf_param_len[2];
            surf_param_len[0] = u_range.length();
            surf_param_len[1] = v_range.length();

            // Offset of the parametric dimensions (not to get negative parameters for trim curves)
            double surf_param_offset[2];
            surf_param_offset[0] = u_range.start_pt();
            surf_param_offset[1] = v_range.start_pt();

            // Extract spline surface data
            Json::Value surfDef;
            surfDef["sense"] = faceSense;
            extractSurfaceData(bsurf, cfg, surfDef);

            /*** TRIM CURVE EXTRACTION ***/

            // Get the list of loops (face boundaries)
            ENTITY_LIST loop_list;
            res = api_get_loops(f, loop_list);
            checkOutcome(res, "api_get_loops", __LINE__, cfg);

            // Get number of loops
            int loop_count = loop_list.iteration_count();

            if (cfg.trims())
            {
                // Create a JSON object to store trim curve data
                Json::Value tDataDef;

                for (int lid = 0; lid < loop_count; lid++)
                {
                    // Get the current loop
                    LOOP *currLoop = (LOOP *)loop_list[lid];

                    // Get the coedges
                    ENTITY_LIST coedge_list;
                    res = api_get_coedges(currLoop, coedge_list);
                    checkOutcome(res, "api_get_coedges", __LINE__, cfg);

                    // Get the number of coedges
                    int coedge_count = coedge_list.iteration_count();

                    // Detect loop type and skip if necessary
                    loop_type currLoopType;
                    res = api_loop_type(currLoop, currLoopType);
                    checkOutcome(res, "api_loop_type", __LINE__, cfg);

                    int trimSense = -1;
                    switch (currLoopType)
                    {
                    case loop_type::loop_hole:      // closed loop
                        trimSense = 0;
                        break;
                    case loop_type::loop_periphery: // closed loop
                        trimSense = 1;
                        break;
                    }

                    // Store each coedge data in a list
                    Json::Value tCurvesDataDef;

                    // Loop through the trim curves
                    for (int ce = 0; ce < coedge_count; ce++)
                    {
                        // Get the current coedge
                        COEDGE *coedge = (COEDGE *)coedge_list[ce];

                        // Get coedge sense
                        logical coedgeSense = coedge->sense();

                        // Extract the spline geometry from the parametric curve object
                        bs2_curve bcurve2d;
                        if (cfg.transform())
                        {
                            pcurve* parametric_curve = coedge->geometry()->trans_pcurve(get_owner_transf(f), f->sense());
                            bcurve2d = parametric_curve->cur();
                        }
                        else
                        {
                            pcurve parametric_curve = coedge->geometry()->equation();
                            bcurve2d = parametric_curve.cur();
                        }

                        // Extract trim curve data to the JSON object
                        Json::Value curveDef;
                        curveDef["sense"] = coedgeSense;
                        extractTrimCurveData(bcurve2d, cfg, surf_param_offset, surf_param_len, curveDef);

                        // Add trim curve to the parent JSON object
                        tCurvesDataDef[ce] = curveDef;
                    }

                    // Store each loop data in a list
                    Json::Value tDataDataDef;
                    tDataDataDef["type"] = "container";
                    tDataDataDef["data"] = tCurvesDataDef;
                    tDataDataDef["loop_type"] = currLoopType;
                    if (trimSense >= 0 && cfg.trims())
                        tDataDataDef["sense"] = trimSense;
                    tDataDef[lid] = tDataDataDef;
                }

                // Create a JSON object for trim curves
                Json::Value surfTrimDef;

                // Add trim curves to the parent JSON object
                surfTrimDef["count"] = loop_count;
                surfTrimDef["data"] = tDataDef;

                // Add trim data to the parent surface JSON object
                surfDef["trims"] = surfTrimDef;
            }

            // Add ID field to surface
            surfDef["id"] = j + (ent_count * i);

            // Add surface to the data array
            dataDef[j] = surfDef;
        }

        // Update root JSON object
        shapeDef["data"] = dataDef;
        root["shape"] = shapeDef;

        // Try to open JSON file for writing
        std::string fnameSave = filename.substr(0, filename.find_last_of(".")) + ((ent_count > 1) ? "." + std::to_string(i) : "") + ".json";
        std::ofstream fileSave(fnameSave.c_str(), std::ios::out);
        if (!fileSave)
        {
            std::cerr << "[ERROR] Cannot open file '" << fnameSave << "' for writing!" << std::endl;
            return EXIT_FAILURE;
        }
        else
        {
            // Convert JSON data structure into a string
            Json::StreamWriterBuilder wbuilder;
            wbuilder["indentation"] = "\t";
            std::string jsonDocument = Json::writeString(wbuilder, root);

            // Write JSON string to a file
            fileSave << jsonDocument << std::endl;
            fileSave.close();

            // Print success message
            std::cout << "[SUCCESS] Data was extracted to file '" << fnameSave << "' successfully" << std::endl;
        }
    }

    // Stop ACIS
    res = api_stop_modeller();
    checkOutcome(res, "api_stop_modeller", __LINE__, cfg);

    // Exit successfully
    return EXIT_SUCCESS;
}
