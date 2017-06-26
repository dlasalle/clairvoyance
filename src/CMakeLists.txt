include_directories(.)
file(GLOB clairvoyance_sources *.c)
list(REMOVE_ITEM clairvoyance_sources "clairvoyance_bin.c")
file(GLOB domlib_sources ${CMAKE_SOURCE_DIR}/${DOMLIB_PATH}/*.c)

# library                                         
add_library(clairvoyance ${CLAIRVOYANCE_LIBRARY_TYPE} 
  ${clairvoyance_sources}
  ${domlib_sources}
) 
install(TARGETS clairvoyance 
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# binary
add_executable(clairvoyance_bin clairvoyance_bin.c)
set_target_properties(clairvoyance_bin PROPERTIES OUTPUT_NAME clairvoyance)
target_link_libraries(clairvoyance_bin clairvoyance ${PNG_LIBRARIES}
    ${LIBJPEG_LIBRARIES} m)
install(TARGETS clairvoyance_bin
  RUNTIME DESTINATION bin
)

