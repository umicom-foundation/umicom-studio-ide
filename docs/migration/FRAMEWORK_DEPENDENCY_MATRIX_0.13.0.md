# Framework dependency matrix — Studio 0.13.0

| Capability | Owner after 0.13.0 | State | Next action |
|---|---|---|---|
| Lifecycle and modules | Framework | Active | Split product shell into bounded Slave Controllers |
| Diagnostics hub and store | Framework | Active | Add GTK diagnostics presentation adapter |
| Clock | Framework | Active | Replace remaining private timestamp helpers |
| Typed settings mechanics | Framework | Active | Add change notifications and layered sources later |
| Studio settings schema | StudioCore | Active | Bind GTK preferences to the typed repository |
| Filesystem and paths | Mixed | Not migrated | Extend Framework path contracts |
| File watching | Studio | Not migrated | Add Framework watcher contract and adapters |
| Process and build tasks | Mixed | Not migrated | Add Framework task service |
| Workspace and documents | Mixed | Not migrated | Adapt to Framework models |
| Problems and diagnostics | Mixed | Partial | Add toolkit-neutral problem collection |
| Git integration | Studio | Not migrated | Add Framework VCS provider contract |
| AI provider gateway | Studio | Not migrated | Add provider-neutral Framework AI API |
| Plug-ins | Mixed | Partial | Add manifests, permissions and discovery |
| GTK editor and panes | Studio | Correctly product-owned | Keep toolkit code in Studio |
