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

# Esempio costellazione del Leone (completo ma richiede API estese)
add_executable(example_leo leo_constellation.cpp)
target_link_libraries(example_leo PRIVATE starmap)

# Esempio semplificato Leone (funzionante con API corrente)
add_executable(example_leo_simple leo_simple.cpp)
target_link_libraries(example_leo_simple PRIVATE starmap)

# Esempio veloce Leone con catalogo V1
add_executable(leo_quick leo_quick.cpp)
target_link_libraries(leo_quick PRIVATE starmap)

# Test diagnostico catalogo
add_executable(test_catalog test_catalog.cpp)
target_link_libraries(test_catalog PRIVATE ioc_gaialib CURL::libcurl ZLIB::ZLIB LibXml2::LibXml2)

# Test diretto Mag18 V1
add_executable(test_mag18_direct test_mag18_direct.cpp)
target_link_libraries(test_mag18_direct PRIVATE ioc_gaialib ZLIB::ZLIB)

# Installa esempi
install(TARGETS 
    example_basic 
    example_json 
    example_programmatic 
    example_gaia
    example_leo
    example_leo_simple
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
