ecbuild_add_library(

    TARGET eckit_option

    INSTALL_HEADERS LISTED

    HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/option

	SOURCES
        FactoryOption.cc
        FactoryOption.h
        Option.cc
        Option.h
        Separator.cc
        Separator.h
        SimpleOption.cc
        SimpleOption.h
        VectorOption.cc
        VectorOption.h
        CmdArgs.cc
        CmdArgs.h

    TEMPLATES
        FactoryOption.cc
        VectorOption.cc
        SimpleOption.cc

    LIBS eckit )

