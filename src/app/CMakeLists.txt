qt_add_executable(platrix
    main.cpp
    mainwindow.cpp mainwindow.h filemodel.h settingsdialog.h
)

target_link_libraries(platrix PRIVATE 
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

set_target_properties(platrix PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)