
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was UmicomFrameworkConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

include(CMakeFindDependencyMacro)
find_dependency(Threads)
find_dependency(SQLite3)

include("${CMAKE_CURRENT_LIST_DIR}/UmicomFrameworkTargets.cmake")

# Components map directly to exported Umicom::<component> targets. This keeps
# find_package(UmicomFramework COMPONENTS sdk delivery) honest for consumers.
foreach(_umicom_component IN LISTS UmicomFramework_FIND_COMPONENTS)
    if(TARGET "Umicom::${_umicom_component}")
        set("UmicomFramework_${_umicom_component}_FOUND" TRUE)
    else()
        set("UmicomFramework_${_umicom_component}_FOUND" FALSE)
        if(NOT UmicomFramework_FIND_QUIETLY)
            message(STATUS
                "Umicom Framework component not found: ${_umicom_component}")
        endif()
    endif()
endforeach()

check_required_components(UmicomFramework)
