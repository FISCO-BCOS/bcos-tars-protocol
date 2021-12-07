hunter_config(bcos-framework VERSION 3.0.0-local
	URL https://${URL_BASE}/FISCO-BCOS/bcos-framework/archive/ea82f8130badd53427d2015a7fb6cf031d41c888.tar.gz
	SHA1 0277fd505d6a4767bd9a4ab8c4323ea29b7bcf3e
	CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON
)
hunter_config(tarscpp VERSION 3.0.3-7299ad23)