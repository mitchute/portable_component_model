project(PortableComponentModel)
cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)  # export ALL function definitions from DLL, which triggers building the lib file

# this is the type of build rules if the model is built as a standalone library
add_library(model SHARED
        model/model.h
        model/model.cpp
        model/GHE.h
        model/GHE.cpp
)

# this is the type of build rules if this was built as a part of EnergyPlus
add_executable(mock_energyplus
        model/model.h
        model/model.cpp
        energyplus/Model.hh
        energyplus/Model.cc
        energyplus/EnergyPlus.hh
        energyplus/EnergyPlus.cc
)

# there's also the unit tests for the model itself
add_executable(
        model_test
        test/doctest.h
        test/main_test.cpp
        test/GHE.unit.cpp
        test/model.unit.cpp
)
target_link_libraries(model_test model)
target_include_directories(model_test PRIVATE "${PROJECT_SOURCE_DIR}/model")

# this is the type of build rules if this was built as an FMU
# ...
