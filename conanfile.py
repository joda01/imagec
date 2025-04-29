from conan import ConanFile
from conan.tools.cmake import cmake_layout
from conan.tools.cmake import CMakeToolchain, CMakeDeps


class ImageC(ConanFile):
    name = "ImageC"
    version = "1.0-beta"
    license = ["AGPL-3.0","imagec"]
    author = "Joachim Danmayr <your.email@example.com>"
    url = "https://github.com/joda01/imagec"
    homepage="https://imagec.org"
    description = "High throughput image analysis tool for bio science!"
    topics = ("conan", "image-processing", "science")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "with_onnx": [True, False],
        "with_tensorflow": [True, False],
        "with_pytorch": [True, False],
    }
    default_options = {
        "with_onnx": True,
        "with_tensorflow": True,
        "with_pytorch": True,
    }

    exports_sources = "src/*"
    
    #def config_options(self):
    #    print("")


    def requirements(self):
        self.requires("qt/6.7.1")
        self.requires("opencv/4.10.0")
        self.requires("catch2/3.7.0")
        self.requires("pugixml/1.14")
        self.requires("nlohmann_json/3.11.3")
        self.requires("libxlsxwriter/1.1.8")
        self.requires("duckdb/1.1.3")
        self.requires("cpp-httplib/0.19.0", force=True)
        self.requires("openssl/3.4.1")
        self.requires("onnx/1.17.0", force=True)
        self.requires("rapidyaml/0.7.1")
        if self.options.get_safe("with_onnx"):
            self.requires("onnxruntime/1.18.1")
        if self.options.get_safe("with_pytorch"):
            self.requires("libtorch/2.4.0")
        if self.options.get_safe("with_tensorflow"):
            self.requires("tensorflow-lite/2.15.0")
        self.requires("flatbuffers/23.5.26", force=True)
        self.requires("protobuf/3.21.12", override=True)
        self.requires("xkbcommon/1.6.0", override=True)
        self.requires("libpq/15.5", override=True)
        self.requires("abseil/20240116.1", override=True)
        self.requires("libbacktrace/cci.20240730", override = True)
        self.requires("xnnpack/cci.20240229", override = True)
        self.requires("boost/1.86.0", override = True)

        
    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        toolchain = CMakeToolchain(self)
        toolchain.variables["WITH_ONNX"] = self.options.with_onnx
        toolchain.variables["WITH_TENSORFLOW"] = self.options.with_tensorflow
        toolchain.variables["WITH_PYTORCH"] = self.options.with_pytorch
        toolchain.generate()

    def layout(self):
        cmake_layout(self)
