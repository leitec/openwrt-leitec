OpenWRT for old RT2880 routers
==============================

This is a fork of OpenWRT Attitude Adjustment 12.09.1 that backs out of some
changes in the upstream tree (notably a merge of trunk''s mac80211 that breaks our
wireless drivers) and adds support for a few new-to-OpenWRT routers using the
RT2880 SoC:

- Airlink101 AR725W
- Asante SmartHub 600 (AWRT-600N)
- Linksys WRT110 (in progress)

I intend to fix the brokenness if possible, but I am not sure it''s within
my ability just yet. The updated mac80211 has some good new features like DFS
support.

Any contributions are very much welcome!
