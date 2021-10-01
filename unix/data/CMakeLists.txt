add_executable(unix_data_server server.c)
target_link_libraries(unix_data_server libnet)

add_executable(unix_data_client client.c)
target_link_libraries(unix_data_client libnet)