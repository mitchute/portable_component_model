project(PortableComponentModel)
cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 17)

message(${PROJECT_SOURCE_DIR})

# this is the type of build rules if the model is built as a standalone library
add_library(
        model
        ${PROJECT_SOURCE_DIR}/model/include/model/model.h
        ${PROJECT_SOURCE_DIR}/model/model.cpp
)

target_include_directories(model PRIVATE ${PROJECT_SOURCE_DIR}/model/include)

# this is the type of build rules if this was built as a part of EnergyPlus
add_executable(
        mock_energyplus
        ${PROJECT_SOURCE_DIR}/model/include/model/model.h
        ${PROJECT_SOURCE_DIR}/model/model.cpp
        ${PROJECT_SOURCE_DIR}/energyplus/include/energyplus/ModelWrapper.hh
        ${PROJECT_SOURCE_DIR}/energyplus/ModelWrapper.cc
        ${PROJECT_SOURCE_DIR}/energyplus/include/energyplus/EnergyPlus.hh
        ${PROJECT_SOURCE_DIR}/energyplus/EnergyPlus.cc
)

target_include_directories(mock_energyplus PRIVATE ${PROJECT_SOURCE_DIR}/energyplus/include)
target_include_directories(mock_energyplus PRIVATE ${PROJECT_SOURCE_DIR}/model/include)

# # this is the type of build rules if this was built as a standalone model
# add_executable(
#         standalone_model
#         model/include/model/model.h
#         model/model.cpp
# )
