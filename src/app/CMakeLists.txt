set(CMAKE_AUTORCC ON)

add_executable(platrix)

target_sources(platrix PRIVATE
    main.cpp         

    src/mainwindow.cpp
    src/filemodel.cpp
    src/settingsdialog.cpp   
    src/settingsmanager.cpp

    includes/mainwindow.h
    includes/settingsdialog.h
    includes/settingsmanager.h
    includes/filemodel.h

    ressources/ressources.qrc
)

target_link_libraries(platrix PRIVATE 
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_include_directories(platrix PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/includes")

set_target_properties(platrix PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)