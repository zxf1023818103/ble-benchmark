set(TRIPLET arm-linux-gnueabihf)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER ${TRIPLET}-gcc)
set(CMAKE_CXX_COMPILER ${TRIPLET}-g++)
set(CMAKE_SYSTEM_PROCESSOR armhf)
set(CMAKE_FIND_ROOT_PATH "/usr/${TRIPLET};/usr/lib/${TRIPLET}")
set(OPENSSL_INCLUDE_DIR "/usr/include/${TRIPLET}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)