add_executable(
    system76-kbd-led

    main.cpp
    keyboard.cpp
    color/rgb.cpp
    logging.cpp
    fs.cpp
)

set(Boost_USE_STATIC_LIBS "ON")
find_package(boost_program_options)
target_link_libraries(
    system76-kbd-led
    Boost::program_options
)

install(TARGETS system76-kbd-led DESTINATION "bin")
