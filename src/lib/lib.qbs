import qbs

DynamicLibrary {
    name: "futurelib"
	Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: ["core", "network"] }

    files: [ "*.h", "*.hpp", "*.cpp" ]
    cpp.defines: ["MAKE_FUTURELIB"]

    Group {
        name: "coap"
        files: [ "coap/*.hpp", "coap/*.cpp" ]
    }

    qbs.installRoot: product.buildDirectory + "/../../.."
    Group {
        qbs.install: true
        qbs.installDir: "bin"
        fileTagsFilter: "dynamiclibrary"
    }
}
