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

#include "extract.h"


// Extract spline surface data
void extractSurfaceData(bs3_surface &splineSurf, Config &cfg, Json::Value &surfDef)
{
    // Surface spatial dimension
    int dim;

    // Surface form = 0 (open -clamped-), 1 (closed -unclamped-), 2 (periodic)
    int form_u, form_v;

    // Flag indicating the existence of poles (singularities)
    int pole_u, pole_v;

    // Rational/non-rational flag
    int rat_u, rat_v;

    // Degrees
    int degree_u, degree_v;

    // Knot vectors
    int num_knots_u, num_knots_v;
    double *knots_u;
    double *knots_v;

    // Extract control points and weights (weights != NULL if rational)
    int num_u, num_v;
    SPAposition *ctrlpts;
    double *weights;

    // Extract surface data
    bs3_surface_to_array(splineSurf, dim, rat_u, rat_v, form_u, form_v, pole_u, pole_v,
        num_u, num_v, ctrlpts, weights,
        degree_u, num_knots_u, knots_u,
        degree_v, num_knots_v, knots_v
    );

    // Create JSON object for each surface
    surfDef["rational"] = (rat_u || rat_v) ? true : false;
    surfDef["form_u"] = degree_u;
    surfDef["form_v"] = degree_v;
    surfDef["degree_u"] = degree_u;
    surfDef["degree_v"] = degree_v;
    Json::Value kvU;
    for (int k = 0; k < num_knots_u; k++)
    {
        if (cfg.normalize())
            kvU[k] = (knots_u[k] - knots_u[0]) / (knots_u[num_knots_u - 1] - knots_u[0]);
        else
            kvU[k] = knots_u[k];
    }
    surfDef["knotvector_u"] = kvU;
    Json::Value kvV;
    for (int k = 0; k < num_knots_v; k++)
    {
        if (cfg.normalize())
            kvV[k] = (knots_v[k] - knots_v[0]) / (knots_v[num_knots_v - 1] - knots_v[0]);
        else
            kvV[k] = knots_v[k];
    }
    surfDef["knotvector_v"] = kvV;
    surfDef["size_u"] = num_u;
    surfDef["size_v"] = num_v;
    Json::Value ctrlptsDef;
    Json::Value pointsDef;
    for (int ku = 0; ku < num_u; ku++)
    {
        for (int kv = 0; kv < num_v; kv++)
        {
            int idx = kv + (num_v * ku);
            Json::Value ptDef;
            for (int c = 0; c < 3; c++)
            {
                ptDef[c] = ctrlpts[idx].coordinate(c);
            }
            pointsDef[idx] = ptDef;
        }
    }
    ctrlptsDef["points"] = pointsDef;
    if (weights != nullptr)
    {
        Json::Value weightsDef;
        for (int ku = 0; ku < num_u; ku++)
        {
            for (int kv = 0; kv < num_v; kv++)
            {
                int idx = kv + (num_v * ku);
                weightsDef[idx] = weights[idx];
            }
        }
        ctrlptsDef["weights"] = weightsDef;
    }
    surfDef["control_points"] = ctrlptsDef;

    // Delete arrays
    free(knots_u);
    free(knots_v);
    free(ctrlpts);
    if (weights != NULL)
        free(weights);
}

// Extract the trim curve data
void extractTrimCurveData(bs2_curve &trimCurve, Config &cfg, double *paramOffset, double *paramLength, Json::Value &curveDef)
{
    // Curve spatial dimension
    int cdim;

    // Rational/non-rational flag
    int crat;

    // Curve degree
    int cdegree;

    // Knot vector
    int num_cknots;
    double *cknots;

    // Control points (parametric coordinates as [u,v,0])
    int num_cctrlpts;
    SPAposition *cctrlpts;
    double *cweights;

    // Extract trim curve data
    bs2_curve_to_array(trimCurve, cdim, cdegree, crat, num_cctrlpts, cctrlpts, cweights, num_cknots, cknots);

    // Update JSON object for the trim curve
    curveDef["type"] = "spline";  // make sure that you are always calling api_convert_to_spline()
    curveDef["rational"] = bool(crat);
    curveDef["degree"] = cdegree;
    Json::Value ckv;
    for (int k = 0; k < num_cknots; k++)
    {
        if (cfg.normalize())
            ckv[k] = (cknots[k] - cknots[0]) / (cknots[num_cknots - 1] - cknots[0]);
        else
            ckv[k] = cknots[k];
    }
    curveDef["knotvector"] = ckv;
    Json::Value cctrlptsDef;
    Json::Value cpointsDef;
    for (int idx = 0; idx < num_cctrlpts; idx++)
    {
        Json::Value cptDef;
        for (int c = 0; c < 2; c++)
        {
            if (cfg.normalize())
                cptDef[c] = (cctrlpts[idx].coordinate(c) - paramOffset[c]) / paramLength[c];
            else
                cptDef[c] = cctrlpts[idx].coordinate(c);
        }
        cpointsDef[idx] = cptDef;
    }
    cctrlptsDef["points"] = cpointsDef;
    if (cweights != nullptr)
    {
        Json::Value cweightsDef;
        for (int idx = 0; idx < num_cctrlpts; idx++)
        {
            cweightsDef[idx] = cweights[idx];
        }
        cctrlptsDef["weights"] = cweightsDef;
    }
    curveDef["control_points"] = cctrlptsDef;

    // Delete arrays
    free(cknots);
    free(cctrlpts);
    if (cweights != NULL)
        free(cweights);
}
