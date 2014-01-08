include $(top_srcdir)/PreRules.am

basedir = $(localstatedir)/lib/fuppes
dist_base_DATA = \
    fuppes.cfg.default

devicesdir = $(localstatedir)/lib/fuppes/devices
dist_devices_DATA = \
    devices/base.cfg \
    devices/default.cfg \
    devices/default-audio-only.cfg \
    devices/default-transcoding-enabled.cfg \
    devices/xbox.cfg \
    devices/ps3.cfg \
    devices/lg-tv.cfg

vfoldersdir = $(localstatedir)/lib/fuppes/vfolders
dist_vfolders_DATA = \
    vfolders/default.cfg \
    vfolders/xbox.cfg

#install-data-hook:
#	test -f $(basedir)/fuppes.cfg || cp $(basedir)/fuppes.cfg.default $(basedir)/fuppes.cfg

