add_executable(Indoor
	main.cpp
	Indoor.cpp
	Player.cpp
)

include_directories(
	${EIGEN3_INCLUDE_DIR}
	src
	/usr/include/freetype2
	${Darwin_SOURCE_DIR}/Candy/src
)

target_link_libraries(Indoor
	Candy
	glut
)
