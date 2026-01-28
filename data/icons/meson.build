application_id = 'net.waltergarcia.LiveCaptions'

png_dir = join_paths('hicolor', '256x256', 'apps')
install_data(
  join_paths('hicolor', 'scalable', 'apps', ('@0@.png').format(application_id)),
  install_dir: join_paths(get_option('datadir'), 'icons', png_dir)
)

symbolic_dir = join_paths('hicolor', 'symbolic', 'apps')
install_data(
  join_paths(symbolic_dir, ('@0@-symbolic.svg').format(application_id)),
  install_dir: join_paths(get_option('datadir'), 'icons', symbolic_dir)
)
