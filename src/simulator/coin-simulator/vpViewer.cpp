/****************************************************************************
 *
 * $Id: vpViewer.cpp 4056 2013-01-05 13:04:42Z fspindle $
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
 * Simulator based on Coin3d.
 *
 * Authors:
 * Eric Marchand
 *
 *****************************************************************************/
/*!
  \file vpViewer.cpp
  Viewer used by the simulator. Under Windows, the viewer is
  based either on SoWin or SoQt. Under Unix, the viewer is based on SoQt or SoXt.
*/

#include <visp/vpConfig.h>

#ifdef VISP_HAVE_COIN_AND_GUI
#include <visp/vpViewer.h>
#include <visp/vpSimulator.h>


#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/nodes/SoEventCallback.h>


#if defined(VISP_HAVE_SOWIN)
vpViewer::vpViewer(HWND parent,  vpSimulator *_simu, vpViewerType viewerType):
  SoWinExaminerViewer(parent,(char *)NULL,false)
#elif defined(VISP_HAVE_SOQT)
vpViewer::vpViewer(QWidget * parent,  vpSimulator *_simu, vpViewerType viewerType) :
  SoQtExaminerViewer(parent,(char *)NULL,false)
#elif defined(VISP_HAVE_SOXT)
vpViewer::vpViewer(Widget parent,  vpSimulator *_simu, vpViewerType viewerType):
  SoXtExaminerViewer(parent,(char *)NULL,false)
#endif
{
  this->simu = _simu ;
  this->viewerType = viewerType;
  // Coin should not clear the pixel-buffer, so the background image
  // is not removed.

  this->setClearBeforeRender(FALSE, TRUE);
  //  this->setAntialiasing(true, 2) ;
  setAutoRedraw(false);
}

vpViewer::~vpViewer()
{

}

void
vpViewer::actualRedraw(void)
{

  {
   const SbViewportRegion vp = this->getViewportRegion();
   SbVec2s origin = vp.getViewportOriginPixels();
   SbVec2s size = vp.getViewportSizePixels();
   glViewport(origin[0], origin[1], size[0], size[1]);

   const SbColor col = this->getBackgroundColor();
   glClearColor(col[0], col[1], col[2], 0.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // this should be used only with the vpAR:vpSimulator
   // to diplay an image background
   if (simu->image_background != NULL)
   {
     glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
     if (simu->typeImage == vpSimulator::grayImage)
       glDrawPixels((GLsizei)simu->getInternalWidth(), (GLsizei)simu->getInternalHeight(),
		    (GLenum)GL_LUMINANCE,
		    GL_UNSIGNED_BYTE,
		    simu->image_background );
     else
       glDrawPixels((GLsizei)simu->getInternalWidth(), (GLsizei)simu->getInternalHeight(),
		    (GLenum)GL_RGB,
		    GL_UNSIGNED_BYTE,
		    simu->image_background );

     glEnable(GL_DEPTH_TEST);
     glClear(GL_DEPTH_BUFFER_BIT);     // clear the z-buffer
     glClearDepth(100.0);              // Profondeur du Z-Buf
   }
   // Render normal scenegraph.
#if defined(VISP_HAVE_SOWIN)
    SoWinExaminerViewer::actualRedraw();
#elif defined(VISP_HAVE_SOQT)
    SoQtExaminerViewer::actualRedraw();
#elif defined(VISP_HAVE_SOXT)
    SoXtExaminerViewer::actualRedraw();
#endif
   glSwapBuffers() ;
    if(viewerType == vpViewer::internalView){
       simu->get = 0 ;
       glReadPixels(0, 0, (GLsizei)simu->getInternalWidth(), (GLsizei)simu->getInternalHeight(),
		      (GLenum)GL_RGB,
		      GL_UNSIGNED_BYTE,
		      simu->bufferView ) ;
       simu->get =1 ;
    }
 }

}

/*!

  \param x : width
  \param y : height
  \param fixed : set as true to disable mouse window resizing

*/
void
#if defined(VISP_HAVE_SOWIN) || defined (VISP_HAVE_SOQT)
vpViewer::resize(int x, int y, bool fixed)
#else
vpViewer::resize(int x, int y, bool /*fixed*/)
#endif
{
  SbVec2s size(x,y) ;
  setSize(size);
  setGLSize(size) ;

#if defined(VISP_HAVE_SOWIN)
  HWND parent = getParentWidget();

  RECT rcClient, rcWindow;
  POINT ptDiff;
  GetClientRect(parent, &rcClient);
  GetWindowRect(parent, &rcWindow);
  ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
  ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
  MoveWindow(parent,rcWindow.left, rcWindow.top, 
	     x + ptDiff.x, y + ptDiff.y, TRUE);
  if(fixed){
    DWORD dwStyle = GetWindowLong(parent, GWL_STYLE);
    dwStyle &= ~(WS_SIZEBOX);
    SetWindowLong(parent, GWL_STYLE,dwStyle);
  }
#elif defined(VISP_HAVE_SOQT)
  if(fixed){
    QWidget * parent = getParentWidget();
    parent->setFixedSize(x, y);
  }
#endif 
}

/*!
\param left : left coordinate
\param bottom : bottom coordinate
\param x : width
\param y : height

*/

SbBool
vpViewer::processSoEvent(const SoEvent * const event)
{
  if ( this->isViewing() &&
       event->getTypeId() == SoKeyboardEvent::getClassTypeId() )
  {
    SoKeyboardEvent * kbevent = (SoKeyboardEvent *) event;
    switch ( kbevent->getKey() ) {
    case SoKeyboardEvent::H:
      if ( kbevent->getState() == SoButtonEvent::DOWN )
      {
	      std::cout << "H : this help "<<std::endl ;
	      std::cout << "M : get and save the external camera location (matrix)"<<std::endl;
	      std::cout << "V : get and save the external camera location (vector)"<<std::endl;
	      std::cout << "M : load camera location (vector)"<<std::endl;
	      std::cout << "P : get external camera location and set the internal one"<<std::endl;
      }
      return TRUE;

    case SoKeyboardEvent::M:
      if ( kbevent->getState() == SoButtonEvent::DOWN )
      {
	      vpHomogeneousMatrix cMf ;
	      simu->getExternalCameraPosition(cMf) ;
	      std::ofstream f("cMf.dat") ;
	      cMf.save(f) ;
	      f.close() ;
      }
      return TRUE;
    case SoKeyboardEvent::V:
      if ( kbevent->getState() == SoButtonEvent::DOWN )
      {
	      vpHomogeneousMatrix cMf ;
	      simu->getExternalCameraPosition(cMf) ;
	      vpPoseVector vcMf(cMf) ;
	      std::ofstream f("vcMf.dat") ;
	      vcMf.save(f) ;
	      f.close() ;
      }
      return TRUE;
    case SoKeyboardEvent::L:
      if ( kbevent->getState() == SoButtonEvent::DOWN )
      {
	      vpPoseVector vcMf;
	      std::ifstream f("vcMf.dat") ;
	      vcMf.load(f) ;
	      f.close() ;
	      vpHomogeneousMatrix cMf(vcMf) ;
	      simu->setCameraPosition(cMf) ;
	      simu->moveInternalCamera(cMf) ;
      }
      return TRUE;
    case SoKeyboardEvent::P:
      if ( kbevent->getState() == SoButtonEvent::DOWN )
      {
	      vpHomogeneousMatrix cMf ;
	      simu->getExternalCameraPosition(cMf) ;
	      vpPoseVector vcMf(cMf) ;
	      vcMf.print() ;
	      simu->setCameraPosition(cMf) ;
	      simu->moveInternalCamera(cMf) ;
      }
      return TRUE;
    default:
      break;
    }
  }
#if defined(VISP_HAVE_SOWIN)
  return  SoWinExaminerViewer::processSoEvent(event);
#elif defined(VISP_HAVE_SOQT)
  return  SoQtExaminerViewer::processSoEvent(event);
#elif defined(VISP_HAVE_SOXT)
  return  SoXtExaminerViewer::processSoEvent(event);
#endif

}

#endif
