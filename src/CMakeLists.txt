add_executable(imgui-sfml-example
  main.cpp Game.cpp Entity.cpp EntityManager.cpp Vec2.cpp 	
  )

configure_file(./font1.ttf ${CMAKE_CURRENT_BINARY_DIR}/font1.ttf COPYONLY)
# Yep, that's it!
target_link_libraries(imgui-sfml-example
  PUBLIC
    ImGui-SFML::ImGui-SFML
)

include(Install.cmake)
