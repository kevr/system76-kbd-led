add_executable(
    # Target
    system76-kbd-led

    # Source files
    main.c
    cache.c
    colors.c
    fs.c
    buffer.c
)

target_link_libraries(
    # Target
    system76-kbd-led

    # Libraries
    m
)

install(TARGETS system76-kbd-led DESTINATION "bin")
