/****************************************************************************
 *
 * $Id: vpForwardProjection.cpp 4056 2013-01-05 13:04:42Z fspindle $
 *
 * This file is part of the ViSP software.
 * Copyright (C) 2005 - 2013 by INRIA. All rights reserved.
 * 
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * ("GPL") version 2 as published by the Free Software Foundation.
 * See the file LICENSE.txt at the root directory of this source
 * distribution for additional information about the GNU GPL.
 *
 * For using ViSP with software that can not be combined with the GNU
 * GPL, please contact INRIA about acquiring a ViSP Professional 
 * Edition License.
 *
 * See http://www.irisa.fr/lagadic/visp/visp.html for more information.
 * 
 * This software was developed at:
 * INRIA Rennes - Bretagne Atlantique
 * Campus Universitaire de Beaulieu
 * 35042 Rennes Cedex
 * France
 * http://www.irisa.fr/lagadic
 *
 * If you have questions regarding the use of this file, please contact
 * INRIA at visp@inria.fr
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 * Description:
 * Forward projection.
 *
 * Authors:
 * Eric Marchand
 *
 *****************************************************************************/


#include <visp/vpForwardProjection.h>
#include <visp/vpDebug.h>


/*!
  \file vpForwardProjection.cpp
  \brief   class that defines what is a point
*/





/*!
  Print to stdout the feature parameters in:
  - the object frame
  - the camera frame
  - the image plane.
*/
void
vpForwardProjection::print() const
{
  std::cout << "oP : " << oP.t() ;
  std::cout << "cP : " << cP.t() ;
  std::cout << "p : " << p.t() ;
}

/*!

  Compute the feature parameters in the image plane (vpTracker::p)
  from the parameters in the camera frame (vpTracker::cP).

  \warning Be careful to update vpTracker::p and vpTracker::cP public
  attributes before the call of this method.
*/
void
vpForwardProjection::project()
{
  projection(cP, p) ;
}

/*! 
  
  Compute the feature parameters in the camera frame (vpTracker::cP)
  and than compute the projection of these parameters in the image
  plane (vpTracker::p).

  \warning The feature parameters in the object frame
  (vpForwardProjection:oP) need to be set prior the use of this
  method. To initialize these parameters see setWorldCoordinates().
 
  \param cMo : The homogeneous matrix corresponding to the pose
  between the camera frame and the object frame.

*/
void
vpForwardProjection::project(const vpHomogeneousMatrix &cMo)
{
  try{
    changeFrame(cMo) ;
    projection() ;
  }
  catch(...)
  {
    vpERROR_TRACE("Error caught") ;
    throw ;
  }
}


/*! 
  
  Track the feature parameters in the camera frame (vpTracker::cP)
  and than compute the projection of these parameters in the image
  plane (vpTracker::p).

  This method is similar to project(const vpHomogeneousMatrix &).

  \warning The feature parameters in the object frame
  (vpForwardProjection:oP) need to be set prior the use of this
  method. To initialize these parameters see setWorldCoordinates().
 
  \param cMo : The homogeneous matrix corresponding to the pose
  between the camera frame and the object frame.

*/
void
vpForwardProjection::track(const vpHomogeneousMatrix &cMo)
{
  try{
    project(cMo) ;
  }
  catch(...)
  {
    vpERROR_TRACE("Error caught") ;
    throw ;
  }
}

/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
