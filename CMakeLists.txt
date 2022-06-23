project(PortableComponentModel)
cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 17)

# this is the type of build rules if the model is built as a standalone library
add_library(
        model
        model/model.h
        model/model.cpp
)

# this is the type of build rules if this was built as a part of EnergyPlus
add_executable(
        mock_energyplus
        model/model.h
        model/model.cpp
        energyplus/Model.hh
        energyplus/Model.cc
        energyplus/EnergyPlus.hh
        energyplus/EnergyPlus.cc
)

# this is the type of build rules if this was built as a part of EnergyPlus
add_executable(
        simple_ghe
        model/GHE_main.cpp
)

# this is the type of build rules if this was built as an FMU
# ...
