# Framework dependency matrix — Studio 0.11.1

| Capability | Current owner | Framework support | Migration state | Next action |
|---|---|---|---|---|
| Lifecycle and module registration | Framework Master Controller | Yes | Active | Split product shell into bounded Slave Controllers |
| Diagnostics fan-out | Framework diagnostic hub | Yes in 0.4.2 | Foundation added | Route Studio GLib/UI logging through hub |
| Clock and scheduling | Framework | Yes | Service handle exposed | Replace private timestamp helpers |
| Configuration | Studio JSON/settings code | Basic Framework config exists | Not migrated | Define typed settings repository and adapt callers |
| Filesystem and paths | Studio util/fs | Basic Framework filesystem exists | Not migrated | Extend Framework path/file APIs and replace product helpers |
| File watching | Studio util/watchers | Missing public Framework watcher | Not migrated | Add watcher contract and Windows/POSIX adapters |
| Process and build tasks | Studio build module | Basic Framework process exists | Not migrated | Add task/build service contracts and diagnostic events |
| Workspace and documents | Studio core/editor | Framework models exist | Not migrated | Adapt product state to Framework models |
| Commands, queries and events | Mixed Studio actions/settings bus | Framework buses exist | Partial | Define Studio command/event schemas |
| Problems and diagnostics | Studio routers/panes | Framework diagnostics exists | Not migrated | Add diagnostic collection model and GTK adapter |
| Git integration | Studio util/git | No VCS provider contract | Not migrated | Add Framework VCS provider API |
| AI provider gateway | Studio llm | No Framework AI API yet | Not migrated | Add provider-neutral AI contracts |
| Plug-ins | Studio plug-ins | Framework plug-in boundary exists | Partial | Add manifests, permissions and discovery |
| GTK editor and panes | Studio | UI-neutral contracts only | Correctly product-owned | Keep GTK code in Studio; extract only toolkit-neutral models |
