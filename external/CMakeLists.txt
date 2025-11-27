cmake_minimum_required(VERSION 3.15)

# Scarica nlohmann/json se non trovato nel sistema
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found in system, using bundled version")
    
    # Controlla se è già stato scaricato
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/json/CMakeLists.txt")
        message(STATUS "Downloading nlohmann/json...")
        
        include(FetchContent)
        FetchContent_Declare(
            json
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG v3.11.2
        )
        
        FetchContent_MakeAvailable(json)
    else()
        # Usa la versione locale
        add_subdirectory(json)
    endif()
endif()
