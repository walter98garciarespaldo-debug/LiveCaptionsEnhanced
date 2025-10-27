# Procesa el archivo de recursos XML para empaquetar los archivos UI, CSS e iconos.
livecaptions_resources = gnome.compile_resources('livecaptions-resources',
  'livecaptions.gresource.xml',
  c_name: 'livecaptions'
)

# Procesa el archivo de interfaz D-Bus XML.
dbus_interface = gnome.gdbus_codegen('dbus-interface',
  'dbus-interface.xml',
  interface_prefix: 'net.sapples.',
  namespace: 'DBLCap'
)

# Define el ejecutable principal y todos sus archivos fuente
executable('livecaptions',
  # Fuentes generadas (objetos)
  livecaptions_resources,
  dbus_interface,

  # Fuentes de código C
  'main.c',
  'dbus-interface.c',
  'livecaptions-application.c',
  'livecaptions-window.c',
  'livecaptions-settings.c',
  'livecaptions-welcome.c',
  'livecaptions-history-window.c',
  'audiocap.c',
  'audiocap-pa.c',
  'audiocap-pw.c',
  'asrproc.c',
  'line-gen.c',
  'profanity-filter.c',
  'window-helper.c',
  'history.c',

  # Dependencias (ya definidas en el meson.build principal)
  dependencies: [
    glib_dep,
    gio_dep,
    adw_dep,
    pulse_dep,
    gtk_dep,
    april_asr_dep,
    x11_dep,
  ],

  c_args: [
    '-Wall',
    '-Wextra',
    '-Wno-unused-parameter',
    '-DAPRIL_API_VER=3'
  ],

  install: true,
)
