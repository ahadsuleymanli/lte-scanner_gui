# Create a library of all the shared functions.
add_library(LTE_MISC capbuf.cpp constants.cpp itpp_ext.cpp macros.cpp searcher.cpp common.cpp dsp.cpp lte_lib.cpp from_osmocom.cpp)

SET (common_link_libs ${Boost_LIBRARIES} ${Boost_THREAD_LIBRARY} ${LAPACK_LIBRARIES} ${FFTW_LIBRARIES} ${RTLSDR_LIBRARIES} ${CURSES_LIBRARIES})

# Create the cell search executable
ADD_EXECUTABLE (CellSearch CellSearch.cpp)
TARGET_LINK_LIBRARIES (CellSearch general LTE_MISC)
TARGET_LINK_LIBRARIES (CellSearch debug itpp_debug ${common_link_libs})
TARGET_LINK_LIBRARIES (CellSearch optimized itpp ${common_link_libs})

# Create the cell tracker
set(LTE-Tracker_helpers producer_thread.cpp tracker_thread.cpp searcher_thread.cpp)
add_library (LTE-Tracker_helpers_lib ${LTE-Tracker_helpers})

ADD_EXECUTABLE (LTE-Tracker LTE-Tracker.cpp display_thread.cpp ${LTE-Tracker_helpers})
TARGET_LINK_LIBRARIES (LTE-Tracker general LTE_MISC)
TARGET_LINK_LIBRARIES (LTE-Tracker debug itpp_debug ${common_link_libs})
TARGET_LINK_LIBRARIES (LTE-Tracker optimized itpp ${common_link_libs})

set(project_ui
    mainwindow.ui)

set(project_headers
    ../include/mainwindow.h
    ../include/qcustomplot.h
    ../include/worker.h
    ../include/LTE-Tracker_adapter.h
    ../include/lte-cell_data.h
    ../include/producer_consumer.h
    ../include/lte_cell_info_container.h
    ../include/mousereactiveqlabel.h
    ../include/mousereactiveqwidget.h
    ../usrp_interface_src/ascii_art_dft.hpp
)
#    ../usrp_interface_src/ascii_art_dft.hpp
set(project_sources
    main.cpp
    mainwindow.cpp
    qcustomplot.cpp
    worker.cpp
    LTE-Tracker_adapter.cpp
    display_thread_gui_adapter.cpp
    lte_cell_info_container.cpp)




qt5_wrap_ui(UI_HEADERS ${project_ui})
qt5_wrap_cpp(PROJECT_SOURCES_MOC ${project_headers} )

# add_executable(helloworld main.cpp mainwindow.cpp ${PROJECT_SOURCES_MOC} ${UI_HEADERS})
add_executable(helloworld ${project_sources}  ${PROJECT_SOURCES_MOC} ${UI_HEADERS} )
#TARGET_LINK_LIBRARIES (helloworld PUBLIC Qt5::Core Qt5::Widgets Qt5::Gui Qt5::PrintSupport )
TARGET_LINK_LIBRARIES (helloworld Qt5::Core Qt5::Widgets Qt5::Gui Qt5::PrintSupport )
TARGET_LINK_LIBRARIES (helloworld general LTE-Tracker_helpers_lib)
TARGET_LINK_LIBRARIES (helloworld general LTE_MISC)
TARGET_LINK_LIBRARIES (helloworld debug itpp_debug ${common_link_libs})
TARGET_LINK_LIBRARIES (helloworld optimized itpp ${common_link_libs})
TARGET_LINK_LIBRARIES (helloworld usrp_library_module)


