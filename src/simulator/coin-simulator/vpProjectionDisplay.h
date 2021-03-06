/****************************************************************************
 *
 * $Id: vpProjectionDisplay.h 4238 2013-05-06 13:29:30Z fspindle $
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
 * Interface with the image for feature display.
 *
 * Authors:
 * Eric Marchand
 *
 *****************************************************************************/


#ifndef vpProjectionDisplay_H
#define vpProjectionDisplay_H

/*!
  \file vpProjectionDisplay.h
  \brief interface with the image for feature display
*/

#include <visp/vpConfig.h>
#if defined (VISP_HAVE_DISPLAY)

#include <visp/vpCameraParameters.h>
#include <visp/vpColor.h>
#include <visp/vpImage.h>
#include <visp/vpDisplayX.h>
#include <visp/vpDisplayGTK.h>
#include <visp/vpDisplayGDI.h>
#include <visp/vpDisplayOpenCV.h>
#include <visp/vpDisplayD3D.h>
#include <visp/vpForwardProjection.h>

#include <list>

/*!
  \class vpProjectionDisplay
  \brief interface with the image for feature display
*/
class VISP_EXPORT vpProjectionDisplay
{
private:
  vpImage<unsigned char> Icam ;
  vpImage<unsigned char> Iext ;

#if defined VISP_HAVE_X11
  vpDisplayX dIcam ;
  vpDisplayX dIext ;
#elif defined VISP_HAVE_GTK
  vpDisplayGTK dIcam ;
  vpDisplayGTK dIext ;
#elif defined VISP_HAVE_GDI
  vpDisplayGDI dIcam ;
  vpDisplayGDI dIext ;
#elif defined VISP_HAVE_OPENCV
  vpDisplayOpenCV dIcam ;
  vpDisplayOpenCV dIext ;
#elif defined(VISP_HAVE_D3D9)
  vpDisplayD3D dIcam ;
  vpDisplayD3D dIext ;
#endif
public:
  void init() ;
  void init(int select) ;
  void close() ;
  static int internalView() { return 0x01 ; }
  static int externalView() { return 0x02 ; }

  vpProjectionDisplay() { init() ;}
  vpProjectionDisplay(int select) { init(select) ;}

  void insert( vpForwardProjection &fp) ;
  void display(vpImage<unsigned char> &I,
               const vpHomogeneousMatrix &cextMo,
               const vpHomogeneousMatrix &cMo,
               const vpCameraParameters &cam,
               const vpColor &color,
               const bool &displayTraj = false,
               const unsigned int thickness=1);
  void displayCamera(vpImage<unsigned char> &I,
                     const vpHomogeneousMatrix &cextMo,
                     const vpHomogeneousMatrix &cMo,
                     const vpCameraParameters &cam,
                     const unsigned int thickness=1) ;

private:
  std::list<vpForwardProjection *> listFp ;
  vpPoint o ;
  vpPoint x ;
  vpPoint y ;
  vpPoint z ;
  vpMatrix traj;
} ;



#endif
#endif

/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
