add_executable(imgui-sfml-example
  main.cpp Game.cpp Entity.cpp EntityManager.cpp Vec2.cpp 	
  )

# Yep, that's it!
target_link_libraries(imgui-sfml-example
  PUBLIC
    ImGui-SFML::ImGui-SFML
)

include(Install.cmake)
