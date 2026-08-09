# Framework dependency matrix — Studio 0.12.0

| Capability | Current owner | Framework support | Migration state | Next action |
|---|---|---|---|---|
| Lifecycle and module registration | Framework Master Controller | Yes | Active | Split product shell into bounded Slave Controllers |
| Diagnostic fan-out | Framework diagnostic hub | Yes | Active | Route remaining product log helpers through Studio services |
| Retained diagnostics | Framework diagnostic store | Yes in 0.4.3 | Active in Studio 0.12.0 | Add GTK4 diagnostics pane adapter later |
| Clock and scheduling | Framework | Yes | Service handle exposed | Replace remaining private timestamp helpers |
| Configuration | Studio JSON/settings code | Basic Framework config exists | Not migrated | Implement typed settings in 0.13.0 |
| Filesystem and paths | Studio util/fs | Basic Framework filesystem exists | Not migrated | Extend Framework path/file APIs |
| File watching | Studio util/watchers | Missing public watcher contract | Not migrated | Add Windows/POSIX watcher adapters |
| Process and build tasks | Studio build module | Basic Framework process exists | Not migrated | Add structured task and diagnostic events |
| Workspace and documents | Studio core/editor | Framework models exist | Not migrated | Adapt product state to Framework models |
| Problems and diagnostics | Studio routers/panes | Framework diagnostics foundation exists | Partial | Add collection model and GTK4 adapter |
| Commands, queries and events | Mixed Studio actions/settings bus | Framework buses exist | Partial | Define Studio command and event schemas |
| Git integration | Studio util/git | No VCS provider contract | Not migrated | Add Framework VCS provider API |
| AI provider gateway | Studio llm | No Framework AI API yet | Not migrated | Add provider-neutral AI contracts |
| Plug-ins | Studio plug-ins | Framework plug-in boundary exists | Partial | Add manifests, permissions and discovery |
| GTK editor and panes | Studio | UI-neutral contracts only | Correctly product-owned | Keep GTK4 presentation in Studio |
