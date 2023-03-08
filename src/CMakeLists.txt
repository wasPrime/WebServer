file(GLOB resources *.cc)
set(SERVER_SHARED_NAME server_shared)
add_library(${SERVER_SHARED_NAME} SHARED ${resources})

