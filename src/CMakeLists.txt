add_executable(
    system76-kbd-led

    main.cpp
    keyboard.cpp
    color/rgb.cpp
    logging.cpp
    fs.cpp
)

target_link_libraries(
    system76-kbd-led
    m
    boost_program_options
)

install(TARGETS system76-kbd-led DESTINATION "bin")
