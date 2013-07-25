/****************************************************************************
 *
 * $Id: vpRobotCamera.cpp 2456 2010-01-07 10:33:12Z nmelchio $
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
 * Defines the simplest robot : a free flying camera.
 *
 * Authors:
 * Eric Marchand
 *
 *****************************************************************************/


/*!
  \file vpRobotCamera.cpp
  \brief class that defines the simplest robot : a free flying camera
*/

#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpRobotCamera.h>
#include <visp/vpRobotException.h>
#include <visp/vpDebug.h>
#include <visp/vpExponentialMap.h>


/*!
  Constructor.

  Initialise the robot by a call to init().

  Sampling time is set to 40 ms. To change it you should call
  setSamplingTime().

  Robot jacobian expressed in the end-effector frame \f$ {^e}{\bf J}_e \f$
  is set to identity (see get_eJe()).

  \code
  vpRobotCamera robot;

  robot.setSamplingTime(0.020); // Set the sampling time to 20 ms.

  \endcode

*/
vpRobotCamera::vpRobotCamera()
{
  init() ;
}

/*!
  Robot initialisation.

  Robot jacobian expressed in the end-effector frame \f$ {^e}{\bf J}_e \f$
  is set to identity (see get_eJe()).

*/
void vpRobotCamera::init()
{
  nDof = 6;
  eJe.resize(6,6) ;
  eJe.setIdentity() ;
  eJeAvailable = true;
  fJeAvailable = false;
  areJointLimitsAvailable = false;
  qmin = NULL;
  qmax = NULL;

  setMaxTranslationVelocity(1.); // vx, vy and vz max set to 1 m/s
  setMaxRotationVelocity(vpMath::rad(90)); // wx, wy and wz max set to 90 deg/s
}


/*!
  Destructor.

*/
vpRobotCamera::~vpRobotCamera()
{
}

/*!

  Get the twist transformation from camera frame to end-effector
  frame.  This transformation allows to compute a velocity expressed
  in the end-effector frame into the camera frame.

  \param cVe : Twist transformation. Here this transformation is equal to identity
  since camera frame and end-effector frame are at the same location.

*/
void
vpRobotCamera::get_cVe(vpVelocityTwistMatrix &cVe)
{
  vpVelocityTwistMatrix cVe_;
  cVe = cVe_;
}

/*!
  Get the robot jacobian expressed in the end-effector frame.
  For that simple robot the Jacobian is the identity.

  \param eJe : A 6 by 6 matrix representing the robot jacobian \f$ {^e}{\bf
  J}_e\f$ expressed in the end-effector frame.
*/
void
vpRobotCamera::get_eJe(vpMatrix &eJe)
{
  eJe = this->eJe ;
}

/*!
  Send to the controller a velocity.

  \param frame : Control frame type. Only articular (vpRobot::ARTICULAR_FRAME)
  and camera frame (vpRobot::CAMERA_FRAME) are implemented.

  \param v : Velocity to apply to the robot.

  - In the camera frame, this velocity is represented by a vector of dimension 6
  \f$ {\bf v} = [{\bf t}, {\bf \theta u }]^t \f$ where \f$ \bf t \f$ is a
  translation vector and \f$ {\bf \theta u} \f$ is a rotation vector (see
  vpThetaUVector): \f$ {\bf v} = [t_x, t_y, t_z, {\theta u}_x, {\theta u}_y,
  {\theta u}_z] \f$ (see vpTranslationVector and vpThetaUVector).

  - In articular, this velocity is represented by a 6 dimension vector \f$
  \dot{{\bf q}} = [{\bf t}, {\bf \theta u}]^t \f$ where \f$ \bf t \f$ is a
  translation vector and \f$ {\bf \theta u} \f$ is a rotation vector (see
  vpThetaUVector): \f$ \dot{{\bf q}} = [t_x, t_y, t_z, {\theta u}_x, {\theta
  u}_y, {\theta u}_z] \f$ (see vpTranslationVector and vpThetaUVector). The
  robot jacobian \f$ {^e}{\bf J}_e\f$ expressed in the end-effector frame is
  here set to identity.

  We use the exponential map (vpExponentialMap) to update the camera location.
  Sampling time can be set using setSamplingTime().

  \sa setSamplingTime()

*/
void
vpRobotCamera::setVelocity(const vpRobot::vpControlFrameType frame,
                           const vpColVector &v)
{
  switch (frame)
  {
  case vpRobot::ARTICULAR_FRAME:
  case vpRobot::CAMERA_FRAME: {
      if (vpRobot::STATE_VELOCITY_CONTROL != getRobotState ()) {
        setRobotState(vpRobot::STATE_VELOCITY_CONTROL);
      }

      vpColVector v_max(6);

      for (unsigned int i=0; i<3; i++)
        v_max[i] = getMaxTranslationVelocity();
      for (unsigned int i=3; i<6; i++)
        v_max[i] = getMaxRotationVelocity();

      vpColVector v_sat = vpRobot::saturateVelocities(v, v_max, true);

      this->cMw_ = vpExponentialMap::direct(v_sat, delta_t_).inverse()*this->cMw_ ;
      break ;
    }
  case vpRobot::REFERENCE_FRAME:
    vpERROR_TRACE ("Cannot set a velocity in the reference frame: "
                   "functionality not implemented");
    throw vpRobotException (vpRobotException::wrongStateError,
                            "Cannot set a velocity in the reference frame:"
                            "functionality not implemented");
    break ;
  case vpRobot::MIXT_FRAME:
    vpERROR_TRACE ("Cannot set a velocity in the mixt frame: "
                   "functionality not implemented");
    throw vpRobotException (vpRobotException::wrongStateError,
                            "Cannot set a velocity in the mixt frame:"
                            "functionality not implemented");

    break ;
  }
}


/*!
  Get the robot position as the transformation from camera frame to world frame.
*/
void
vpRobotCamera::getPosition(vpHomogeneousMatrix &cMw) const
{
  cMw = this->cMw_ ;
}

/*
  Get the current position of the robot.

  \param frame : Control frame type in which to get the position, either :
  - in the camera cartesien frame,
  - joint (articular) coordinates of each axes
  - in a reference or fixed cartesien frame attached to the robot base
  - in a mixt cartesien frame (translation in reference frame, and rotation in camera frame)

  \param position : Measured position of the robot:
  - in camera cartesien frame, a 6 dimension vector, set to 0.

  - in articular, a 6 dimension vector corresponding to the articular
  position of each dof, first the 3 translations, then the 3
  articular rotation positions represented by a vpRxyzVector.

  - in reference frame, a 6 dimension vector, the first 3 values correspond to
  the translation tx, ty, tz in meters (like a vpTranslationVector), and the
  last 3 values to the rx, ry, rz rotation (like a vpRxyzVector).
*/
void vpRobotCamera::getPosition(const vpRobot::vpControlFrameType frame, vpColVector &q)
{
  q.resize (6);

  switch (frame) {
  case vpRobot::CAMERA_FRAME :
    q = 0;
    break;

  case vpRobot::ARTICULAR_FRAME :
  case vpRobot::REFERENCE_FRAME : {
    // Convert wMc_ to a position
    // From fMc extract the pose
    vpRotationMatrix cRw;
    this->cMw_.extract(cRw);
    vpRxyzVector rxyz;
    rxyz.buildFrom(cRw);

    for (unsigned int i=0; i < 3; i++) {
      q[i] = this->cMw_[i][3]; // translation x,y,z
      q[i+3] = rxyz[i]; // Euler rotation x,y,z
    }

    break;
    }
  case vpRobot::MIXT_FRAME :
    std::cout << "MIXT_FRAME is not implemented in vpSimulatorCamera::getPosition()" << std::endl;
  }
}

/*!
  Set the robot position as the transformation from camera frame to world frame.
*/
void
vpRobotCamera::setPosition(const vpHomogeneousMatrix &cMw)
{
  if (vpRobot::STATE_POSITION_CONTROL != getRobotState ()) {
    setRobotState(vpRobot::STATE_POSITION_CONTROL);
  }

  this->cMw_ = cMw ;
}
