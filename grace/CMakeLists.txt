add_executable(grace_client client.c)
target_link_libraries(grace_client libnet)

add_executable(grace_server server.c)
target_link_libraries(grace_server libnet)