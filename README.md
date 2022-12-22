# `asahi-smcd`: Optimized Battery Charging in Asahi Linux

`asahi-smcd` finesses sysfs into performing userspace-controlled battery charging optimization, keeping the battery charged to 80% to reduce degradation.

## Installation

After cloning the repository, simply
```console
$ sudo make install
```
to build and install `asahi-smcd`, and
```console
$ sudo systemctl start asahi-smcd
$ sudo systemctl enable asahi-smcd
```
to start the systemd service.

## Uninstalling

Use
```console
$ sudo make uninstall
```
to cleanly remove `asahi-smcd`.
