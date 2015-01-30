OpenWrt for old RT2880 routers
==============================

This is a fork of OpenWrt Attitude Adjustment 12.09.1 that adds support for
some older Ralink RT2880-based routers previously unsupported by OpenWrt and/or
other alternative firmware. This is built on existing RT2880 support by the OpenWrt team.

This fork also updates a few packages, notably _dropbear_ and _openssl_.

**Fully supported:**

- Airlink101 AR670W - _supported upstream in Chaos Calmer r43153_
- Airlink101 AR725W - _supported upstream in Barrier Breaker_
- Asante SmartHub AWRT-600N

**Supported with limitations:**

- Linksys WRT100
- Linksys WRT110
- Linksys WRT160N v2

The limitations stem from having only 16MB of RAM. More info is in the wiki.

**Installation:**

Please see the wiki for any notes on your model. Please see also the page on [limitations](https://github.com/leitec/openwrt-leitec/wiki/Limitations-on-Routers-with-16MB-RAM) for models with 16MB RAM, and the important [notes on performance](https://github.com/leitec/openwrt-leitec/wiki/Performance). Basic build instructions are also found in the wiki.

Any contributions are very much welcome!
