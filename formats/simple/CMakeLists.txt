add_library(formats-simple SHARED
    simple_lattice_writer.cpp
    simple_lattice_writer_stream_output_iterator.cpp
)

add_subdirectory(t)

install(TARGETS formats-simple DESTINATION lib/)
install(DIRECTORY data/ DESTINATION ${PSITOOLKIT_INSTALL_DATA_DIR}/simple-lattice-writer)
