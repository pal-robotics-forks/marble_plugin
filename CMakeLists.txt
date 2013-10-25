cmake_minimum_required(VERSION 2.8.3)

project(rqt_marble_plugin)

find_package(catkin REQUIRED COMPONENTS
  rqt_gui
  rqt_gui_cpp
  sensor_msgs
  visualization_msgs
)

find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)

include(${QT_USE_FILE})

SET( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};/usr/share/kde4/apps/cmake/modules/" )

find_package(Marble REQUIRED)

set(rqt_marble_plugin_SRCS
  src/rqt_marble_plugin/marble_plugin.cpp
  src/rqt_marble_plugin/drawable_marble_widget.cpp
  src/rqt_marble_plugin/manage_kml_dialog.cpp
)

set(rqt_marble_plugin_HDRS
  include/rqt_marble_plugin/marble_plugin.h
  include/rqt_marble_plugin/drawable_marble_widget.h
#  include/rqt_marble_plugin/visualisation_marker/circle.h
#  include/rqt_marble_plugin/visualisation_marker/colored_polygon.h
  include/rqt_marble_plugin/manage_kml_dialog.h
)

set(rqt_marble_plugin_UIS
  src/rqt_marble_plugin/marble_plugin.ui
  src/rqt_marble_plugin/manage_kml_dialog.ui
)

set(rqt_marble_plugin_INCLUDE_DIRECTORIES
  include
  ${CMAKE_CURRENT_BINARY_DIR}
  include/rqt_marble_plugin
)

catkin_package(
  INCLUDE_DIRS ${rqt_marble_INCLUDE_DIRECTORIES}
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS roscpp rqt_gui rqt_gui_cpp
#  DEPENDS system_lib
)

catkin_python_setup()

qt4_wrap_cpp(rqt_marble_plugin_MOCS ${rqt_marble_plugin_HDRS})
qt4_wrap_ui(rqt_marble_plugin_UIS_H ${rqt_marble_plugin_UIS})

include_directories(
  ${catkin_INCLUDE_DIRS}
  ${rqt_marble_plugin_INCLUDE_DIRECTORIES} 
  ${MARBLE_INCLUDE_DIR}
)

add_library( ${PROJECT_NAME}
  ${rqt_marble_plugin_SRCS} 
  ${rqt_marble_plugin_MOCS} 
  ${rqt_marble_plugin_UIS_H}
)

target_link_libraries( ${PROJECT_NAME}
   ${catkin_LIBRARIES}
   ${QT_QTCORE_LIBRARY} 
   ${QT_QTGUI_LIBRARY}
   ${MARBLE_LIBRARIES}
)


find_package(class_loader)
class_loader_hide_library_symbols(${PROJECT_NAME})

install(FILES
 plugin.xml
 DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

install(TARGETS ${PROJECT_NAME}
   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
   RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
)

install(PROGRAMS scripts/rqt_marble_plugin
  DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}

)

install(PROGRAMS scripts/rqt_marble_plugin
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}

)

install(DIRECTORY include/${PROJECT_NAME}
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
)
