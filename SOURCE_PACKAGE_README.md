# Full offline source package

This ZIP is a clean, buildable source reference. It deliberately excludes:

- parent and submodule `.git` metadata;
- generated CMake/Ninja build directories;
- compiled executables and object files;
- local caches.

It includes complete checked-out source snapshots for:

- Umicom Studio IDE;
- Umicom Framework;
- Umicom AuthorEngine AI;
- AuthorEngine's pinned llama.cpp dependency.

To contribute changes to GitHub, use a normal Git clone and follow the separate
HTML guide. Do not copy an offline ZIP over `.git`.
