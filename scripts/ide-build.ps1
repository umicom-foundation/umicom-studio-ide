
New-Item -ItemType Directory -Force -Path build | Out-Null
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
