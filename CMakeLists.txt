# cmake_minimum_required(VERSION 3.16)
# project(projectOCRx LANGUAGES CXX)


# set(CMAKE_AUTOUIC ON)
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)


# set(Qt6_DIR "/home/mana/Qt/6.9.1/gcc_64/lib/cmake/Qt6")
# # set(Tesseract_DIR "/usr/lib/x86_64-linux-gnu/cmake/tesseract")
# set(OpenCV_DIR "/usr/lib/x86_64-linux-gnu/cmake/opencv4")

# find_package(OpenCV REQUIRED)
# find_package(Tesseract REQUIRED)
# find_package(Qt6 COMPONENTS Core Gui Qml Quick REQUIRED)


# qt_add_executable(projectOCRx
#     main.cpp
#     imageProvider.h
#     imageProvider.cpp
#     cameraCapture.h
#     cameraCapture.cpp
#     resources.qrc
#     backend.h backend.cpp
# )

# qt_add_resources(projectOCRx "resources.qrc")



# if(OpenCV_FOUND)
#     # target_link_libraries(ocrProject ${OpenCV_LIBS})
#     target_include_directories(projectOCRx PRIVATE ${OpenCV_INCLUDE_DIRS})
# else()
#     message(FATAL_ERROR "OpenCV bulunamadı. Lütfen kurulum yolunu kontrol edin.")
# endif()


# target_link_libraries(projectOCRx
#     PRIVATE
#     Qt6::Gui
#     Qt6::Qml
#     Qt6::Quick
#     ${OpenCV_LIBS}
#     ${TESSERACT_LIBRARIES}
# )



# target_include_directories(projectOCRx  PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}
#     ${CMAKE_CURRENT_SOURCE_DIR}/src
# )



cmake_minimum_required(VERSION 3.16)
project(projectOCRx LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OpenCV_DIR "/usr/local/lib/cmake/opencv4")



# Qt
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

# OpenCV
find_package(OpenCV REQUIRED)

# Tesseract ve Leptonica için pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(TESSERACT REQUIRED tesseract)
pkg_check_modules(LEPTONICA REQUIRED lept)
link_directories(${TESSERACT_LIBRARY_DIRS} ${LEPTONICA_LIBRARY_DIRS})


# Kaynaklar
qt_add_executable(projectOCRx
    main.cpp
    backend.cpp
    backend.h
    imageProvider.cpp
    imageProvider.h
    cameraCapture.cpp
    cameraCapture.h
    resources.qrc
)

# Dahili klasörleri ekle
target_include_directories(projectOCRx PRIVATE
    ${TESSERACT_INCLUDE_DIRS}
    ${LEPTONICA_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Kütüphaneleri bağla
target_link_libraries(projectOCRx PRIVATE
    Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick
    ${OpenCV_LIBS}
    ${TESSERACT_LDFLAGS}
    ${LEPTONICA_LDFLAGS}
)

install(TARGETS projectOCRx RUNTIME DESTINATION bin)

