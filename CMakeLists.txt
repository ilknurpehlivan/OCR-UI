cmake_minimum_required(VERSION 3.16)
project(projectOCRx LANGUAGES CXX)


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(Qt6_DIR "/home/mana/Qt/6.9.1/gcc_64/lib/cmake/Qt6")

# set(Tesseract_DIR "/usr/lib/x86_64-linux-gnu/cmake/tesseract")

find_package(Qt6 COMPONENTS Core Gui Qml Quick REQUIRED)



set(OpenCV_DIR "/usr/lib/x86_64-linux-gnu/cmake/opencv4")
find_package(OpenCV REQUIRED)



# find_package(Tesseract REQUIRED)


qt_add_executable(projectOCRx
    main.cpp
    imageProvider.h
    imageProvider.cpp
    cameraCapture.h
    cameraCapture.cpp
    resources.qrc
)

qt_add_resources(projectOCRx "resources.qrc")



if(OpenCV_FOUND)
    # target_link_libraries(ocrProject ${OpenCV_LIBS})
    target_include_directories(projectOCRx PRIVATE ${OpenCV_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "OpenCV bulunamadı. Lütfen kurulum yolunu kontrol edin.")
endif()
target_link_libraries(projectOCRx
    PRIVATE
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    ${OpenCV_LIBS}
)


# --- Tesseract Kütüphanelerini Bağla ---
# Tesseract kütüphanelerini "ocrProject" hedefine bağlar.
# if(Tesseract_FOUND)
#     target_link_libraries(ocrProject ${Tesseract_LIBRARIES})
#     # Tesseract başlık dosyalarının yolunu derleyiciye bildirir.
#     target_include_directories(ocrProject PRIVATE ${Tesseract_INCLUDE_DIRS})


# endif()



target_include_directories(projectOCRx  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)



install(TARGETS projectOCRx
    RUNTIME DESTINATION bin
)
