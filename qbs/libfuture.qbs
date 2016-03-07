import qbs

Module {
	Depends { name: "cpp" }
	Depends { name: "Qt"; submodules: ["core", "network"] }
	cpp.includePath: "../src"

}