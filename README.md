# `asahi-smcd`: Optimized Battery Charging in Asahi Linux

`asahi-smcd` finesses sysfs into performing userspace-controlled battery charging optimization, keeping the battery charged to 80% (or a level of your chosing) to reduce degradation.

## Installation

After cloning the repository, simply:
```console
$ sudo make install
```
to build and install `asahi-smcd`.

Enable the service with:
```console
$ systemctl enable asahi-smcd@80
```
where 80 is the desired charge level.

Start the service with:
```console
$ systemctl start asahi-smcd@80
```

## Uninstalling

To uninstall, stop / disable the service and run
```console
$ sudo make uninstall
```

## Caveats

- Does not function in S0
- The uninstall target will not remove the service for you
