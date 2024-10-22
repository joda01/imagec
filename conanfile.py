from conan import ConanFile
from conan.tools.cmake import cmake_layout

class ImageC(ConanFile):
    name = "ImageC"
    version = "1.0"
    license = "AGPL"
    author = "Joachim Danmayr <your.email@example.com>"
    url = "https://github.com/your/repo"
    description = "An example Hello World project"
    topics = ("conan", "image-processing", "example")
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"
    exports_sources = "src/*"
    

    def requirements(self):
        self.requires("qt/6.7.1")
        self.requires("opencv/4.10.0")
        self.requires("catch2/3.7.0")
        self.requires("pugixml/1.14")
        self.requires("nlohmann_json/3.11.3")
        self.requires("libxlsxwriter/1.1.8")
        self.requires("duckdb/1.1.0")
        self.requires("onnx/1.17.0")
        self.requires("xkbcommon/1.6.0", override=True)
        self.requires("libpq/15.5", override=True)


    def layout(self):
        cmake_layout(self)
