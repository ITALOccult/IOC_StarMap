cmake_minimum_required(VERSION 3.15)

# Esempio base
add_executable(example_basic basic_usage.cpp)
target_link_libraries(example_basic PRIVATE starmap)

# Esempio configurazione JSON
add_executable(example_json json_config.cpp)
target_link_libraries(example_json PRIVATE starmap)

# Esempio configurazione programmatica
add_executable(example_programmatic programmatic_config.cpp)
target_link_libraries(example_programmatic PRIVATE starmap)

# Esempio query GAIA
add_executable(example_gaia gaia_query.cpp)
target_link_libraries(example_gaia PRIVATE starmap)

# Esempio costellazione del Leone
add_executable(example_leo leo_constellation.cpp)
target_link_libraries(example_leo PRIVATE starmap)

# Installa esempi
install(TARGETS 
    example_basic 
    example_json 
    example_programmatic 
    example_gaia
    example_leo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/examples
)

# Installa file di configurazione esempio
install(FILES 
    config_examples/orion.json
    config_examples/andromeda.json
    config_examples/custom.json
    config_examples/leo.json
    DESTINATION ${CMAKE_INSTALL_DATADIR}/starmap/examples
)
