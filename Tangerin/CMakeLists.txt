add_executable(Tangerin
	src/Tangerin/main.cpp
	src/Tangerin/Player.cpp
	src/Tangerin/Tangerin.cpp
	src/Tangerin/Entities/Creature.cpp
	src/Tangerin/Entities/CreatureSpawner.cpp
	src/Tangerin/Entities/EntityManager.cpp
	src/Tangerin/Entities/EntityRenderGroup.cpp
	src/Tangerin/Entities/EntityRenderInfo.cpp
	src/Tangerin/Entities/StaticEntity.cpp
	src/Tangerin/Entities/Creatures/AutoDrone.cpp
	src/Tangerin/Entities/Static/Pipe.cpp
)

include_directories(
	${EIGEN3_INCLUDE_DIR}
	src
	/usr/include/freetype2
	${Darwin_SOURCE_DIR}/Candy/src
)

target_link_libraries(Tangerin
	Candy
	glut
)
