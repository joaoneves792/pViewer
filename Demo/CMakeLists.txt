cmake_minimum_required(VERSION 3.2.2)
project(pViewerApp CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_REQUIRED_FLAGS -std=c++17)

include_directories(
	includes/
	${GLEW_INCLUDE_DIR}
	#${GLUT_INCLUDE_DIR}
	${SDL2_INCLUDE_DIR}
	${GLM_INCLUDE_DIRS}
	${OPENVR_INCLUDE_DIR}
	${OPENHMD_INCLUDE_DIR}
	${GST_INCLUDE_DIRS}
	${GSTVIDEO_INCLUDE_DIRS}
	${GSTGL_INCLUDE_DIRS}
   	${CGJEngine_INCLUDE_DIRS})


set(SOURCE_FILES
		main.cpp
		scene.cpp
		scene.h
		actions.cpp
		actions.h
		meshes.cpp
		meshes.h
		shaders.cpp
		shaders.h
		constants.h
		pipeline.cpp pipeline.h Browser.cpp Browser.h VideoPlayer.cpp VideoPlayer.h MediaInterface.cpp MediaInterface.h)

add_executable(demo ${SOURCE_FILES})
target_link_libraries(demo stdc++fs ${GLEW_LIBRARIES} ${SDL2_LIBRARY} ${OPENGL_LIBRARIES}
                                    ${OPENVR_LIBRARIES} ${OPENHMD_LIBRARIES}
		                            ${GST_LIBRARIES} ${GSTVIDEO_LIBRARIES} ${GSTGL_LIBRARIES}
									CGJEngine)

add_custom_command(TARGET demo POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
		   ${PROJECT_SOURCE_DIR}/res $<TARGET_FILE_DIR:demo>/res)
