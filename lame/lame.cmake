
include(ExternalProject)

ExternalProject_Add(lame-build
  URL http://downloads.sourceforge.net/project/lame/lame/3.99/lame-3.99.5.tar.gz
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/lame-3.99.5
  CONFIGURE_COMMAND ${CMAKE_CURRENT_BINARY_DIR}/lame-3.99.5/src/lame-build/configure
  BUILD_COMMAND make
  INSTALL_COMMAND make install
)

ExternalProject_Get_Property(lame-build source_dir)
ExternalProject_Get_Property(lame-build binary_dir)

