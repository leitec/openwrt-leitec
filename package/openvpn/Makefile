#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=openvpn
PKG_VERSION:=2.3.6
PKG_RELEASE:=leitec1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://swupdate.openvpn.net/community/releases
PKG_MD5SUM:=6ca03fe0fd093e0d01601abee808835c

PKG_FIXUP:=autoreconf


PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/openvpn
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=VPN
  DEPENDS:=+kmod-tun +libopenssl +!OPENVPN_DISABLE_LZO:liblzo
  TITLE:=Open source VPN solution using SSL
  URL:=http://openvpn.net
  MENU:=1
endef

define Package/openvpn/description
 Open source VPN solution using SSL
endef

define Package/openvpn/config
	source "$(SOURCE)/Config.in"
endef


CONFIGURE_ARGS+= \
	--with-ifconfig-path=/sbin/ifconfig \
	--with-iproute-path=/usr/sbin/ip \
	--with-route-path=/sbin/route \
	--enable-password-save \
	--disable-debug \
	--disable-plugins \
	--disable-pthread \
	--disable-selinux \
	--disable-socks \
	--enable-small

ifeq ($(CONFIG_OPENVPN_DISABLE_LZO),y)
CONFIGURE_ARGS += \
	--disable-lzo
endif

ifeq ($(CONFIG_OPENVPN_DISABLE_CRYPTO),y)
CONFIGURE_ARGS += \
	--disable-crypto
endif

ifeq ($(CONFIG_OPENVPN_DISABLE_SSL),y)
CONFIGURE_ARGS += \
	--disable-ssl
endif

ifeq ($(CONFIG_OPENVPN_X509_ALT_USERNAME),y)
CONFIGURE_ARGS += \
	--enable-x509-alt-username
endif

ifeq ($(CONFIG_OPENVPN_DISABLE_MULTI),y)
CONFIGURE_ARGS += \
	--disable-multi
endif

ifeq ($(CONFIG_OPENVPN_DISABLE_SERVER),y)
CONFIGURE_ARGS += \
	--disable-server
endif

ifeq ($(CONFIG_OPENVPN_DISABLE_EUREPHIA),y)
CONFIGURE_ARGS += \
	--disable-eurephia
endif

ifeq ($(CONFIG_OPENVPN_DISABLE_MANAGEMENT),y)
CONFIGURE_ARGS += \
	--disable-management
endif

ifeq ($(CONFIG_OPENVPN_DISABLE_PKCS11),y)
CONFIGURE_ARGS += \
	--disable-pkcs11
endif

ifeq ($(CONFIG_OPENVPN_DISABLE_HTTP),y)
CONFIGURE_ARGS += \
	--disable-http
endif

ifeq ($(CONFIG_OPENVPN_DISABLE_FRAGMENT),y)
CONFIGURE_ARGS += \
	--disable-fragment
endif

ifeq ($(CONFIG_OPENVPN_DISABLE_MULTIHOME),y)
CONFIGURE_ARGS += \
	--disable-multihome
endif

ifeq ($(CONFIG_OPENVPN_DISABLE_PORT_SHARE),y)
CONFIGURE_ARGS += \
	--disable-port-share
endif

ifeq ($(CONFIG_OPENVPN_DISABLE_DEF_AUTH),y)
CONFIGURE_ARGS += \
	--disable-def-auth
endif

ifeq ($(CONFIG_OPENVPN_DISABLE_PF),y)
CONFIGURE_ARGS += \
	--disable-pf
endif

define Package/openvpn/conffiles
/etc/config/openvpn
endef

define Package/openvpn/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/openvpn $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_BIN) files/$(PKG_NAME).init $(1)/etc/init.d/$(PKG_NAME)
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) files/$(PKG_NAME).config $(1)/etc/config/$(PKG_NAME)
	$(INSTALL_DIR) $(1)/etc/openvpn
	$(INSTALL_DIR) $(1)/lib/upgrade/keep.d
	$(INSTALL_DATA) files/openvpn.upgrade $(1)/lib/upgrade/keep.d/openvpn
endef

$(eval $(call BuildPackage,openvpn))
