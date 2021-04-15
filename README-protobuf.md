## How to build protobuf for Windows Clients
Open a CMD windows and run the following:

    gh repo clone protocolbuffers/protobuf
    cd protobuf
    git checkout v3.5.2
    cd cmake
    mkdir build
    cd build
    mkdir solution
    cd solution
    cmake -G "Visual Studio 16 2019" -Dprotobuf_MSVC_STATIC_RUNTIME=OFF -Dprotobuf_BUILD_TESTS=OFF ..\..
    cmake --build . --config Release --target protoc
