from conan import ConanFile
from conan.tools.cmake import cmake_layout
from conan.tools.cmake import CMakeToolchain, CMakeDeps


class ImageC(ConanFile):
    name = "ImageC"
    version = "1.0"
    license = "AGPL"
    author = "Joachim Danmayr <your.email@example.com>"
    url = "https://github.com/your/repo"
    description = "High throughput image analysis tool for bio science!"
    topics = ("conan", "image-processing", "science")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "with_tensorflow": [True, False],
        "with_pytorch": [True, False],
    }
    default_options = {
        "with_tensorflow": True,
        "with_pytorch": True,
    }

    #generators = "CMakeDeps", "CMakeToolchain"
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
        if self.options.get_safe("with_pytorch"):
            self.requires("libtorch/2.4.0")
        if self.options.get_safe("with_tensorflow"):
            self.requires("tensorflow-lite/2.18.0")
        self.requires("xkbcommon/1.6.0", override=True)
        self.requires("libpq/15.5", override=True)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        toolchain = CMakeToolchain(self)
        toolchain.variables["WITH_TENSORFLOW"] = self.options.with_tensorflow
        toolchain.variables["WITH_PYTORCH"] = self.options.with_pytorch
        toolchain.generate()

    def layout(self):
        cmake_layout(self)
