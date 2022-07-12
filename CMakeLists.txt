project(PortableComponentModel)
cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)  # export ALL function definitions from DLL, which triggers building the lib file

message(${PROJECT_SOURCE_DIR})

# this is the type of build rules if the model is built as a standalone library
add_library(
        model_lib
        ${PROJECT_SOURCE_DIR}/model/include/GHE.h
        ${PROJECT_SOURCE_DIR}/model/GHE.cpp
)

target_include_directories(model_lib PRIVATE ${PROJECT_SOURCE_DIR}/model/include)

# model library tests
add_executable(
        model_test
        ${PROJECT_SOURCE_DIR}/test/doctest.h
        ${PROJECT_SOURCE_DIR}/test/main_test.cpp
        ${PROJECT_SOURCE_DIR}/test/GHE.unit.cpp
)

target_link_libraries(model_test model_lib)
target_include_directories(model_test PRIVATE ${PROJECT_SOURCE_DIR}/model/include)


# standalone executable
add_executable(
        model
        ${PROJECT_SOURCE_DIR}/standalone/main.cpp
)

target_link_libraries(model model_lib)
target_include_directories(model PRIVATE ${PROJECT_SOURCE_DIR}/model/include)
target_include_directories(model PRIVATE ${PROJECT_SOURCE_DIR}/standalone/include/nlohmann)

# this is the type of build rules if this was built as a part of EnergyPlus
# add_executable(
#         mock_energyplus
#         ${PROJECT_SOURCE_DIR}/model/include/model/model.h
#         ${PROJECT_SOURCE_DIR}/model/model.cpp
#         ${PROJECT_SOURCE_DIR}/energyplus/include/energyplus/ModelWrapper.hh
#         ${PROJECT_SOURCE_DIR}/energyplus/ModelWrapper.cc
#         ${PROJECT_SOURCE_DIR}/energyplus/include/energyplus/EnergyPlus.hh
#         ${PROJECT_SOURCE_DIR}/energyplus/EnergyPlus.cc
# )

# target_include_directories(mock_energyplus PRIVATE ${PROJECT_SOURCE_DIR}/energyplus/include)
# target_include_directories(mock_energyplus PRIVATE ${PROJECT_SOURCE_DIR}/model/include)

# # this is the type of build rules if this was built as a standalone model
# add_executable(
#         standalone_model
#         model/include/model/model.h
#         model/model.cpp
# )
