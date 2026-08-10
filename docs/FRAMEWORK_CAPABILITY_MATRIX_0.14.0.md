# Framework Capability Matrix — Studio 0.14.0

| Capability | Framework owner | Studio use | State in 0.14.0 |
|---|---|---|---|
| Lifecycle and modules | Master Controller, module registry | Studio bootstrap and Slave Controller | Implemented |
| Capability injection | Capability registry and catalogue | Startup dependency validation | Implemented |
| Diagnostics | Hub and retained store | Console, Doctor, diagnostics tool | Implemented |
| Typed settings | Settings repository | Studio schema and settings tool | Implemented |
| Filesystem | Portable filesystem API | Doctor, scaffolding, build services | Implemented |
| Child processes | Process API | CMake, Ninja, CTest, Git, GitHub CLI | Implemented |
| Toolchain discovery | Toolchain service | Studio platform command | Implemented |
| Build orchestration | Build service | Native configure/build/test/run | Implemented |
| Repository operations | Repository service | Local and optional remote creation | Implemented |
| Application scaffolding | Repository scaffold service | Designer/Trader/TMS starter generation | Implemented |
| Application manifests | Runtime manifest contract | Studio and generated products | Implemented |
| Suite composition | Suite contract | Future combined Studio/Trader/TMS host | Implemented foundation |
| Security policy | Capability policy | Module permission checks | Implemented foundation |
| Secrets | Secret-provider contract | Future provider credentials | Foundation |
| UI contracts | UI-neutral contract header | GTK4 remains product composition | Foundation |
| File watching | Catalogue entry | Existing Studio watcher remains product-owned | Planned Framework migration |
| Tasks/workers | Catalogue entry | Existing Studio build queue remains product-owned | Planned Framework migration |
| Git provider API | Catalogue entry | Native repository creation uses Git process today | Planned richer provider |
| AI provider | Catalogue entry | Existing Studio LLM remains product-owned | Planned Framework migration |
| Delivery | Catalogue entry | Existing CI/package material remains product-owned | Planned Framework migration |
