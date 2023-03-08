file(GLOB resources *.cc)
set(server_shared_name server_shared)
add_library(${server_shared_name} SHARED ${resources})
