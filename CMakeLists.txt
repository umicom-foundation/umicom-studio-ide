# -----------------------------------------------------------------------------
# Umicom Studio IDE
# File: CMakeLists.txt
#
# PURPOSE (read me first):
#   This is the single script that teaches CMake how to build the project.
#   Think of it like a recipe: we tell CMake which sources exist, which
#   libraries are needed (GTK, GLib, etc.), which compiler options to use,
#   and how to produce the final program (the .exe on Windows).
#
#   IMPORTANT FIX INCLUDED HERE:
#   ----------------------------
#   A previous version called `target_include_directories()` and
#   `target_link_libraries()` BEFORE `add_executable()`. In CMake, a target
#   must EXIST before you can set properties on it. The wrong order produced:
#     "Cannot specify include directories for target 'umicom-studio-ide'
#      which is not built by this project."
#   This file creates the executable FIRST, then configures it.
#
# AUDIENCE:
#   New contributors (including absolute beginners to C/CMake). The comments
#   are intentionally long and practical; please keep them generous when you
#   edit this file so future devs can learn by reading.
#
# LICENSE:
#   MIT — see repository license file.
# -----------------------------------------------------------------------------


# ---- 1) CMake version gate ---------------------------------------------------
# We ask for a modern CMake because we rely on features introduced in 3.20+.
cmake_minimum_required(VERSION 3.20)


# ---- 2) Project identity -----------------------------------------------------
# `project()` sets a name and the language(s). We only compile C sources.
project(umicom_studio_ide LANGUAGES C)


# ---- 3) Human-readable version (optional but nice to have) -------------------
# SemVer triplet you can later embed in the binary or print in About dialog.
set(UMICOM_STUDIO_IDE_VERSION_MAJOR 0)
set(UMICOM_STUDIO_IDE_VERSION_MINOR 9)
set(UMICOM_STUDIO_IDE_VERSION_PATCH 0)
set(UMICOM_STUDIO_IDE_VERSION
    "${UMICOM_STUDIO_IDE_VERSION_MAJOR}.${UMICOM_STUDIO_IDE_VERSION_MINOR}.${UMICOM_STUDIO_IDE_VERSION_PATCH}")


# ---- 4) Build toggles (you pass -DNAME=ON/OFF to cmake) ----------------------
# ON means the default is enabled unless the user overrides it.

# When ON (default) we build as a GUI app (no console window on Windows); when
# OFF we build as a console app (handy for debug logs). Same single binary,
# just a different Windows subsystem setting.
option(USIDE_GUI  "Build GUI app (ON=GUI subsystem, OFF=console)" ON)

# Big features you might temporarily skip while onboarding or prototyping.
option(USIDE_ENABLE_LLM_LAB   "Build the LLM Lab UI module" ON)
option(USIDE_ENABLE_TRANSPILE "Build the Transpile plugin module" OFF)

# A pure-C resource path (icons/splash) is preferred. If you *do* want to use
# GTK’s XML `gresource` system, flip this to ON and add an XML manifest.
# We keep it OFF to honor your “avoid XML” preference.
option(USIDE_USE_GRESOURCE "Use gtk/GLib XML gresource instead of pure-C assets" OFF)

# Skip folders to keep first builds small and fast for beginners.
option(USIDE_EXCLUDE_PALETTE "Exclude src/gui/theme/palette.c" ON)
option(USIDE_EXCLUDE_TESTS   "Exclude src/tests/*" ON)
option(USIDE_EXCLUDE_PREFS   "Exclude src/gui/prefs/*" ON)

# Perf/compat switches for x86 compilers (harmless elsewhere).
option(USIDE_ENABLE_SSE     "Enable -mfpmath=sse -msse -msse2 on GCC/Clang x86" ON)

# Some third-party libs prefer this macro on Windows.
option(USIDE_LIBDEFLATE_DLL "Define LIBDEFLATE_DLL for compatibility" ON)


# ---- 5) Language standard (C23 for modern comfort; drop to 17 if needed) ----
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)


# ---- 6) Find dependencies via pkg-config (portable, simple) ------------------
# PkgConfig helps find GTK/GLib/etc and gives us IMPORTED targets to link.
find_package(PkgConfig REQUIRED)

# The IMPORTED_TARGET flag means CMake will create `PkgConfig::<name>` targets
# that carry their own include and library paths, so linking is clean.
pkg_check_modules(GLIB       REQUIRED IMPORTED_TARGET glib-2.0 gobject-2.0 gio-2.0)
pkg_check_modules(JSON_GLIB  REQUIRED IMPORTED_TARGET json-glib-1.0)
pkg_check_modules(LIBSOUP3   REQUIRED IMPORTED_TARGET libsoup-3.0)
pkg_check_modules(CURL       REQUIRED IMPORTED_TARGET libcurl)
pkg_check_modules(GTK4       REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(GDK_PIXBUF REQUIRED IMPORTED_TARGET gdk-pixbuf-2.0)
pkg_check_modules(CAIRO      REQUIRED IMPORTED_TARGET cairo)
pkg_check_modules(PANGO      REQUIRED IMPORTED_TARGET pango)
pkg_check_modules(HARFBUZZ   REQUIRED IMPORTED_TARGET harfbuzz)

# Optional: if present we link it; if not, we continue without it.
pkg_check_modules(GRAPHENE QUIET IMPORTED_TARGET graphene-1.0)
pkg_check_modules(GSV      QUIET IMPORTED_TARGET gtksourceview-5)


# ---- 7) Gather project sources (recursively) --------------------------------
# `CONFIGURE_DEPENDS` lets CMake re-scan if files appear later.
file(GLOB_RECURSE USIDE_SOURCES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

# Filter a few folders for smaller/faster first builds.
if(USIDE_EXCLUDE_TESTS)
  list(FILTER USIDE_SOURCES EXCLUDE REGEX ".*/tests/.*\\.c$")
endif()
if(USIDE_EXCLUDE_PREFS)
  list(FILTER USIDE_SOURCES EXCLUDE REGEX ".*/gui/prefs/.*\\.c$")
endif()
if(USIDE_EXCLUDE_PALETTE)
  list(FILTER USIDE_SOURCES EXCLUDE REGEX ".*/gui/theme/palette\\.c$")
endif()
if(NOT USIDE_ENABLE_LLM_LAB)
  list(FILTER USIDE_SOURCES EXCLUDE REGEX ".*/gui/llm_lab/.*\\.c$")
endif()
if(NOT USIDE_ENABLE_TRANSPILE)
  list(FILTER USIDE_SOURCES EXCLUDE REGEX ".*/plugins/transpile/.*\\.c$")
endif()

# Avoid accidental duplicates.
list(REMOVE_DUPLICATES USIDE_SOURCES)


# ---- 8) Optional: XML gresource (kept OFF by default) -----------------------
# If you later decide you *want* gresource, drop an XML manifest at this path
# and pass -DUSIDE_USE_GRESOURCE=ON. Otherwise, we rely on pure-C assets in
# src/ui/icons.c and src/ui/splash.c (no XML involved).
if(USIDE_USE_GRESOURCE)
  set(USIDE_GRESOURCE_XML "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/resources/ustudio.gresource.xml")
  if(EXISTS "${USIDE_GRESOURCE_XML}")
    set(USIDE_GRESOURCE_C  "${CMAKE_CURRENT_BINARY_DIR}/ustudio_gresource.c")
    add_custom_command(
      OUTPUT  "${USIDE_GRESOURCE_C}"                      # generated .c
      COMMAND glib-compile-resources                      # GLib tool
              --generate-source                           # emit C source
              --target="${USIDE_GRESOURCE_C}"             # output path
              "${USIDE_GRESOURCE_XML}"                    # input XML
      MAIN_DEPENDENCY "${USIDE_GRESOURCE_XML}"            # rebuild if changed
      VERBATIM)
    list(APPEND USIDE_SOURCES "${USIDE_GRESOURCE_C}")     # compile it
  else()
    message(WARNING "USIDE_USE_GRESOURCE=ON but XML not found at ${USIDE_GRESOURCE_XML}")
  endif()
endif()


# ---- 9) Friendly failure if nothing matched (helps new contributors) --------
if(NOT USIDE_SOURCES)
  message(FATAL_ERROR "No sources found under ${CMAKE_CURRENT_SOURCE_DIR}/src")
endif()


# ---- 10) Project include paths (so #include \"foo.h\" just works) -----------
# We *explicitly* add the most common header roots. This is teaching material:
# reading this list tells you where headers live in the tree.
set(USIDE_INCLUDE_DIRS
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/core"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/core/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/build"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/build/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/editor"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/editor/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/gui"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/app"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/chrome"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/dialogs"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/menu"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/llm_lab"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/theme"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/panes"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/panes/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/panes/output"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/panes/problems"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util/fs"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util/fs/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util/git"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util/log"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util/log/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util/sys"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util/sys/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util/watchers"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ui"              # splash/icons live here
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/resources"    # non-XML assets if any
)

# Also pick any ".../include" dir under src/ automatically (handy for plugins).
file(GLOB_RECURSE USIDE_AUTO_INCLUDE_DIRS LIST_DIRECTORIES true
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*/include")
list(APPEND USIDE_INCLUDE_DIRS ${USIDE_AUTO_INCLUDE_DIRS})
list(REMOVE_DUPLICATES USIDE_INCLUDE_DIRS)


# =========================== 11) THE EXECUTABLE TARGET ========================
# From here on we are allowed to attach properties to the target.
add_executable(umicom-studio-ide ${USIDE_SOURCES})

# Make our headers visible to all compilation units in this target.
target_include_directories(umicom-studio-ide PRIVATE ${USIDE_INCLUDE_DIRS})

# Tiny project-wide defines:
#   - USIDE_GUI as 0/1 so code can tweak behavior.
#   - alias gtk_widget_destroy -> g_object_unref to avoid old API use.
target_compile_definitions(umicom-studio-ide PRIVATE
  USIDE_GUI=$<BOOL:${USIDE_GUI}>
  gtk_widget_destroy=g_object_unref
)
if(USIDE_LIBDEFLATE_DLL)
  target_compile_definitions(umicom-studio-ide PRIVATE LIBDEFLATE_DLL)
endif()


# ---- 12) Warnings / compiler flags (developers learn from warnings) ---------
if(MSVC)
  target_compile_options(umicom-studio-ide PRIVATE /W4)
else()
  target_compile_options(umicom-studio-ide PRIVATE
    -Wall -Wextra -Wpedantic
    -Wno-unused-parameter        # keep placeholders legal without noise
  )
  if(USIDE_ENABLE_SSE AND (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
     AND (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|i.86"))
    target_compile_options(umicom-studio-ide PRIVATE -mfpmath=sse -msse -msse2)
  endif()
endif()


# ---- 13) Link libraries (the pkg-config IMPORTED targets) -------------------
target_link_libraries(umicom-studio-ide PRIVATE
  PkgConfig::GLIB
  PkgConfig::JSON_GLIB
  PkgConfig::LIBSOUP3
  PkgConfig::CURL
  PkgConfig::GTK4
  PkgConfig::GDK_PIXBUF
  PkgConfig::CAIRO
  PkgConfig::PANGO
  PkgConfig::HARFBUZZ
)
if(TARGET PkgConfig::GRAPHENE)
  target_link_libraries(umicom-studio-ide PRIVATE PkgConfig::GRAPHENE)
endif()
if(TARGET PkgConfig::GSV)
  target_link_libraries(umicom-studio-ide PRIVATE PkgConfig::GSV)
endif()


# ---- 14) Windows specifics (subsystem + optional .ico resource) -------------
if(WIN32)
  # Choose GUI v. console subsystem.
  if(USIDE_GUI)
    set_target_properties(umicom-studio-ide PROPERTIES WIN32_EXECUTABLE TRUE)
  else()
    set_target_properties(umicom-studio-ide PROPERTIES WIN32_EXECUTABLE FALSE)
  endif()

  # MinGW: explicitly tell the linker which subsystem to use (avoids WinMain
  # vs main mismatches).
  if(MINGW)
    target_link_options(umicom-studio-ide PRIVATE
      $<$<BOOL:${USIDE_GUI}>:-Wl,--subsystem,windows>
      $<$<NOT:$<BOOL:${USIDE_GUI}>>:-Wl,--subsystem,console>
    )
  endif()

  # Optional native app icon (best for Windows taskbar/Alt-Tab):
  # If you put a `res/umicom.ico` and `win/appicon.rc` in the tree, we add it.
  # The .rc is tiny:
  #   // win/appicon.rc
  #   // 1 ICON "res/umicom.ico"
  # This is NOT XML and keeps your “pure C” runtime; it’s just a build-time
  # resource for the executable.
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/win/appicon.rc")
    target_sources(umicom-studio-ide PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/win/appicon.rc")
  endif()

  # Quality of life: suppress insecure CRT warnings; avoid min/max macro leaks.
  target_compile_definitions(umicom-studio-ide PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX)
endif()


# ---- 15) (Optional) install step (copy the exe to bin/) ---------------------
# install(TARGETS umicom-studio-ide RUNTIME DESTINATION bin)


# ---- Appendix: Teaching notes ------------------------------------------------
#[[
BAD EXAMPLE (kept here to teach what NOT to do):
  add_definitions(-IC:/some/include/path)
That injects an *include directory* using a macro mechanism. Wrong tool.
Correct approach (what we used above):
  target_include_directories(umicom-studio-ide PRIVATE <paths...>)

Palette / Theme notes:
  src/gui/theme/palette.c is excluded by default. Pass -DUSIDE_EXCLUDE_PALETTE=OFF
  to include it in the build once you’re comfortable.

Splash / Icons:
  The pure-C splash/icon helpers live under src/ui/. No XML involved. If you
  want GTK gresource later, flip -DUSIDE_USE_GRESOURCE=ON and add the XML.

Windows icon:
  Create res/umicom.ico (multi-size 16/32/48/64/128). A quick way is:
    magick logo.svg -define icon:auto-resize=16,32,48,64,128 res/umicom.ico
  Then add a tiny win/appicon.rc with: 1 ICON "res/umicom.ico"
  CMake will add it automatically if the file exists.
]]
# -------------------------------- End of file --------------------------------
