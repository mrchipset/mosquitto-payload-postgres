include_directories(${mosquitto_SOURCE_DIR} ${mosquitto_SOURCE_DIR}/include
			${OPENSSL_INCLUDE_DIR} ${STDBOOL_H_PATH} ${STDINT_H_PATH})
link_directories(${mosquitto_SOURCE_DIR})

add_library(mosquitto_payload_postgres MODULE mosquitto_payload_postgres.c)
set_target_properties(mosquitto_payload_postgres PROPERTIES
	POSITION_INDEPENDENT_CODE 1
)
set_target_properties(mosquitto_payload_postgres PROPERTIES PREFIX "")
if(WIN32)
	target_link_libraries(mosquitto_payload_postgres mosquitto)
endif(WIN32)

# Don't install, these are example plugins only.
#install(TARGETS mosquitto_payload_postgres RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
