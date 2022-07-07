cmake_minimum_required(VERSION 3.5)

project(Algo LANGUAGES CXX)

# common settings
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(ROOT    ${CMAKE_CURRENT_LIST_DIR})
set(DIR     ${CMAKE_CURRENT_SOURCE_DIR})
set(OUTPUT  ${ROOT}/bin)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${OUTPUT})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${OUTPUT})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${OUTPUT})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${OUTPUT})

if (WIN32)
    set(PYWIN C:/Python36)
    set(PYTHON ${PYWIN}/python.exe)
    set(PYINC ${PYWIN}/include)
    set(PYLIBPATH ${PYWIN}/libs)
    set(PYLIB python36)
    set(GTEST_LIBS "")
    set(MAIN_FLAG WIN32)

    # multithreading build with minimum warnings
    # add_definitions(/bigobj)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
    add_definitions(/wd4005 /wd4068 /wd4996)

    #set(CMAKE_SHARED_LINKER_FLAGS "/NODEFAULTLIB:MSVCRTD")
    set(CMAKE_EXE_LINKER_FLAGS "/NODEFAULTLIB:LIBCMT")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD")
    set(CMAKE_PREFIX_PATH C:/Qt/Qt5.11.3/5.11.3/msvc2017_64)


elseif(APPLE)
    set(PYTHON /Library/Frameworks/Python.framework/Versions/3.7)
    set(PYINC ${PYTHON}/include/python3.7m)
    set(PYLIB python3.7m)
    set(PYLIBPATH ${PYTHON}/lib/python3.7/config-3.7m-darwin)
    set(MAIN_FLAG MACOSX_BUNDLE) # this makes the app blurry
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ldl -framework CoreFoundation")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-command-line-argument -Wno-macro-redefined ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -arch x86_64")

elseif(UNIX)
    # LIBS += -L/usr/local/lib/python2.7/config-x86_64-linux-gnu -L/usr/lib -lpython2.7 -lpthread -ldl  -lutil -lm -Xlinker -export-dynamic
    # /opt/bin/python2.7-config --cflags
    # /opt/bin/python2.7-config --ldflags
    # python dynamic link, this will make a dependency in libpython2.7.so

    set(PYINC /usr/include/python3.6m)
    set(PYLIB python3.6m)
    set(PYLIBPATH "")
    set(GTEST_LIBS pthread)
    set(MAIN_FLAG "")

    # gcc flags
    set(CMAKE_BUILD_RPATH "\$ORIGIN")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -march=native")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-register")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reorder")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable")

#    set(CMAKE_PREFIX_PATH "/red/ssd/appl/Qt/Qt5.9.8-build")

endif()



# ignore warnings of unused variables
set(IGNORE ${QT_QMAKE_EXECUTABLE})

set(SRC         ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TEST        ${CMAKE_CURRENT_SOURCE_DIR}/test)

set(VERSION 0.1)

add_definitions(-DAPP_VERSION=\"${VERSION}\")

#ADD_DEFINITIONS(-DQT_NO_KEYWORDS)

# qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

include_directories(
    ${SRC}
    ${PYINC}
)

link_directories(
    ${PYLIBPATH}
)

add_executable(Algo
    ${SRC}/main.cpp
    ${SRC}/view/mainwindow.cpp
    ${SRC}/view/mainwindow.h
    ${SRC}/view/mainwindow.ui
    ${SRC}/controller/worker.cpp
    ${SRC}/controller/worker.h
    ${SRC}/controller/controller.cpp
    ${SRC}/controller/controller.h
    ${SRC}/controller/pyutil.cpp
    ${SRC}/controller/pyutil.h
    ${SRC}/controller/util.cpp
    ${SRC}/controller/pyutil.h
    ${SRC}/windows.rc
)


if(WIN32)
    target_compile_definitions(Algo PRIVATE WIN_EXPORT)
endif()




set_target_properties(Algo PROPERTIES OUTPUT_NAME Algo)
set_target_properties(Algo PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${SRC}/resources/Info.plist)
target_link_libraries(Algo PRIVATE Qt5::Widgets ${PYLIB})

