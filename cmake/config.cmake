hunter_config(bcos-framework VERSION 3.0.0-local
	URL https://${URL_BASE}/FISCO-BCOS/bcos-framework/archive/80345086f18586ef1e6122d7bf5eb9688e990472.tar.gz
	SHA1 1fabfc8bb45d3793f5a4789a01f32fb35ec691e2
	CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON
)
hunter_config(tarscpp VERSION 3.0.3-7299ad23)