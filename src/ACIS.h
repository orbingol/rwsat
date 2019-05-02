/*
Copyright (c) 2019, Integrated Design and Engineering Analysis Laboratory (IDEA Lab) at Iowa State University.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef ACIS_H
#define ACIS_H

//ACIS Includes
#include <acis.hxx>					        // Declares system wide parameters
#include <license.hxx>		    	    // Licensing
#include <spa_unlock_result.hxx>	  // Licensing
#include <vers.hxx>                 // ACIS versioning 
#include <cstrapi.hxx>				      // Declares constructor APIs
#include <kernapi.hxx>				      // Declares kernel APIs
#include <body.hxx>					        // Declares BODY class
#include <boolapi.hxx>				      // Declares boolean APIs
#include <fileinfo.hxx> 			      // Declares FileInfo class
#include <spa_progress_info.hxx>	  // Outcome Progress
#include <position.hxx> 			      // Declares position class
#include <vector.hxx>				        // Declares vector class
#include <unitvec.hxx>				      // Declares unit vector class
#include <vector_utils.hxx>			    // Declares distance utilites
#include <param.hxx>				        // Declares parametric coordinates
#include <lists.hxx>    			      // Declares ENTITY_LIST class
#include <surface.hxx>				      // Declares SURFACE class
#include <sur.hxx>					        // Declares surface class
#include <coverapi.hxx>				      // Declares face covering APIs
#include <sweepapi.hxx>				      // Declares sweep APIs
#include <stchapi.hxx>				      // Declares stitching APIs
#include <mt_stitch_apis.hxx>		    // Declares multi-threaded stitching APIs
#include <shl_api.hxx>				      // Declares shelling APIs
#include <cover_options.hxx>		    // Declares wire cover oOptions
#include <bs3surf.hxx>				      // Declares 3-dimensional spline surface class
#include <bs2curve.hxx>				      // Declares 2-dimensional spline curve class
#include <bs3curve.hxx>				      // Declares 3-dimensional spline curve class
#include <sps3srtn.hxx>				      // Declares 3-D B-spline surface class
#include <sps2crtn.hxx> 			      // Declares 2-D B-spline curve class
#include <bs2c_def.hxx>             // Declares 2-D B-spline curve class
#include <bs3c_def.hxx>             // Declares 3-D B-spline curve class
#include <proj_opts.hxx>			      // Declares projection options
#include <intrapi.hxx>				      // Declares APIs for point in face and check entity
#include <ofstapi.hxx>				      // Declares APIs for offseting
#include <wire_offset_options.hxx>	// Declares offset options class
#include <ga_api.hxx>				        // Declares Generic Attributes API
#include <at_name.hxx>				      // Declares a generic attribute which stores a string value
#include <at_int.hxx>				        // Declares a generic attribute which stores an integer value
#include <queryapi.hxx>				      // Declares interfaces related to Model Query
#include <container_utils.hxx>		  // Provides std::vector like interfaces within ACIS
#include <faceutil.hxx>
#include <geom_utl.hxx>
#include <getowner.hxx>
#include <cnv2spl_opts.hxx>			    // Declares Convert to Spline options class
#include <lop_api.hxx>				      // Declares Local Operations API
#include <sgquery.hxx>
#include <blendapi.hxx>				      // Declares Basic Blending API
#include <spline.hxx>
#include <transfrm.hxx>
#include <edge.hxx>					        // Declares EDGE class
#include <curve.hxx>				        // Declares CURVE class
#include <curdef.hxx>				        // Declares curve class
#include <mprop.hxx>				        // Declares mass_prop class for storing physical properties
#include <box.hxx>					        // Declares SPAbox (bounding box)
#include <acistol.hxx>
#include <getbox.hxx>
#include <raytest.hxx>
#include <geometry.hxx>
#include <eulerapi.hxx>
#include <pcudef.hxx>				        // Declares pcurve class
#include <pcurve.hxx>				        // Declares PCURVE class
#include <rgbcolor.hxx> 			      // Declares colors for entities
#include <rnd_api.hxx>				      // Declares rendering apis
#include <ppmeshmg.hxx> 			      // Declares poly_point_mesh manager
#include <refine.hxx>				        // Declares refinement class
#include <facet_options.hxx>		    // Declares facet options class
#include <af_enum.hxx>				      // Declares enum types
#include <af_api.hxx>				        // Declares Faceter API
#include <logical.h>                // Declares "logical" variable
#include <sp3srtn.hxx>              // Declares bs3_surface API
#include <loop.hxx>                 // Declares LOOP class
#include <ptfcenum.hxx>             // Declares enumarations for relationships between faces and points
#include <split_api.hxx>            // Declares entitiy splitting API

// ACIS Debugging
#include <debug.hxx>				        // Declares debugging routines

#endif /* ACIS_H */
