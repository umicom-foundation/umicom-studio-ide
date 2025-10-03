cmake_minimum_required(VERSION 3.22)
project(wt_crud_sqlite CXX)
set(CMAKE_CXX_STANDARD 17)
find_package(Wt REQUIRED COMPONENTS Wt WtHttp Wt::Dbo)
add_executable(wt_crud_sqlite main.cpp)
target_link_libraries(wt_crud_sqlite PRIVATE Wt::Wt Wt::WtHttp Wt::Dbo 
