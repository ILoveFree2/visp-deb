#############################################################################
#
# $Id: CMakeLists.txt 2897 2010-11-04 11:17:58Z rtallonn $
#
# This file is part of the ViSP software.
# Copyright (C) 2005 - 2013 by INRIA. All rights reserved.
# 
# This software is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# ("GPL") version 2 as published by the Free Software Foundation.
# See the file LICENSE.txt at the root directory of this source
# distribution for additional information about the GNU GPL.
#
# For using ViSP with software that can not be combined with the GNU
# GPL, please contact INRIA about acquiring a ViSP Professional 
# Edition License.
#
# See http://www.irisa.fr/lagadic/visp/visp.html for more information.
# 
# This software was developed at:
# INRIA Rennes - Bretagne Atlantique
# Campus Universitaire de Beaulieu
# 35042 Rennes Cedex
# France
# http://www.irisa.fr/lagadic
#
# If you have questions regarding the use of this file, please contact
# INRIA at visp@inria.fr
# 
# This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
# WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#
# Description:
# ViSP overall configuration file. 
#
# Authors:
# Fabien Spindler
#
#############################################################################

# SOURCE variable corresponds to the list of all the sources to build binaries.
# The generate binary comes by removing the .cpp extension to
# the source name.
#
# If you want to add/remove a source, modify here
SET (SOURCE
  testPolygon.cpp
)

# rule for binary build
FOREACH(source ${SOURCE})
  # Compute the name of the binary to create
  GET_FILENAME_COMPONENT(binary ${source} NAME_WE)

  # From source compile the binary and add link rules
  ADD_EXECUTABLE(${binary} ${source})
  TARGET_LINK_LIBRARIES(${binary} ${VISP_INTERN_LIBRARY} ${VISP_EXTERN_LIBRARIES})

  # Add test
  ADD_TEST(${binary} ${binary} -c ${OPTION_TO_DESACTIVE_DISPLAY})

ENDFOREACH(source)

# customize clean target 
SET_DIRECTORY_PROPERTIES(PROPERTIES 
  ADDITIONAL_MAKE_CLEAN_FILES "core*;*~;gmon.out;DartTestfile.txt"
)
