add_executable(system76-kbd-led main.c)
target_link_libraries(system76-kbd-led m)
install(TARGETS system76-kbd-led DESTINATION "bin")
