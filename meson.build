
project('umicom-studio-ide', 'c',
  version: '0.1.0',
  default_options: ['warning_level=3', 'c_std=c17']
)

gtkdep = dependency('gtk4', version: '>=4.10')
gio = dependency('gio-2.0')
glib = dependency('glib-2.0')
gnome = import('gnome')

gres = gnome.compile_resources(
  'ustudio-resources',
  'data/ustudio.gresource.xml',
  source_dir: 'data',
  c_name: 'ustudio'
)

srcs = [
  'src/main.c',
  'src/app.c',
  'src/window.c',
  gres
]

exe = executable('ustudio', srcs,
  dependencies: [gtkdep, gio, glib],
  install: true
)
