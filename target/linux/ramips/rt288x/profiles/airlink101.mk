#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/AR725W
       NAME:=Airlink101 AR725W
       PACKAGES:=kmod-switch-ip17xx kmod-swconfig swconfig
endef

define Profile/AR725W/Description
       Package set for Airlink101 AR725W
endef

$(eval $(call Profile,AR725W))
