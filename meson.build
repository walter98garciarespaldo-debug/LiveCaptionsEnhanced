project('livecaptions', 'c',
  version: '0.1.0',
  meson_version: '>= 0.59.0',
  default_options: [ 'warning_level=2',
                     'werror=false',
                     'c_std=gnu11',
                   ],
)

i18n = import('i18n')
gnome = import('gnome')
cc = meson.get_compiler('c')
cmake = import('cmake')

# Dependencias del sistema
glib_dep = dependency('glib-2.0', version: '>=2.72')
gio_dep = dependency('gio-2.0', version: '>=2.72')
adw_dep = dependency('libadwaita-1', version: '>=1.2')
pulse_dep = dependency('libpulse', version: '>=15.0')
gtk_dep = dependency('gtk4')
x11_dep = dependency('x11')

# Dependencia ONNX (usando variable de entorno)
onnx_root = run_command('sh', '-c', 'echo $ONNX_ROOT', check: true).stdout().strip()
if onnx_root == ''
  error('La variable de entorno ONNX_ROOT no está definida. Por favor, ejecute: export ONNX_ROOT=/ruta/a/onnxruntime')
endif

# Preparar opciones para el subproyecto CMake
# Forzamos las rutas exactas porque su script FindONNXRuntime.cmake no es fiable.
cmake_opts = cmake.subproject_options()
cmake_opts.add_cmake_defines({
  'ONNXRuntime_INCLUDE_DIR': join_paths(onnx_root, 'include'),
  'ONNXRuntime_LIBRARY': join_paths(onnx_root, 'lib/libonnxruntime.so')
})

# Compilar el subproyecto 'april-asr' usando el módulo CMake con las opciones correctas
april_asr_sub = cmake.subproject(
  'april-asr',
  options: cmake_opts
)

# Obtener la dependencia 'aprilasr' compilada por CMake para usarla en nuestro proyecto
april_asr_dep = april_asr_sub.dependency('aprilasr')

# Configuración del proyecto
config_h = configuration_data()
config_h.set_quoted('PACKAGE_VERSION', meson.project_version())
config_h.set_quoted('GETTEXT_PACKAGE', 'livecaptions')
config_h.set_quoted('LOCALEDIR', join_paths(get_option('prefix'), get_option('localedir')))
configure_file(
  output: 'livecaptions-config.h',
  configuration: config_h,
)
add_project_arguments('-I' + meson.project_build_root(), language: 'c')

# Procesar subdirectorios
subdir('data')
subdir('src')
subdir('po')

install_data('april-english-dev-01110_en.april',
  install_dir: join_paths(get_option('datadir'), 'livecaptions')
)

# Tareas Post-Instalación
gnome.post_install(
  glib_compile_schemas: true,
  gtk_update_icon_cache: true,
  update_desktop_database: true,
)
