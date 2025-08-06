import os
from conan import ConanFile
from conan.tools.cmake import cmake_layout
from conan.tools.cmake import CMakeToolchain, CMakeDeps
from conan.tools.cmake import CMake, cmake_layout





class ImageC(ConanFile):
    name = "imagec"
    version = "1.0.0"
    license = ["AGPL-3.0","imagec"]
    author = "Joachim Danmayr <support@imagec.org>"
    url = "https://github.com/joda01/imagec"
    homepage="https://imagec.org"
    description = "High throughput image analysis tool for bio science!"
    topics = ("image-processing", "science")
    settings = "os", "compiler", "build_type", "arch"
    package_type = "application"
    options = {
        "with_cuda": [True, False]
    }
    default_options = {
        "with_cuda": True
    }

    exports_sources = "src/*"

    def set_version(self):
        self.version = self.version

    def requirements(self):
        self.requires("qt/6.7.1", force=True)
        self.requires("opencv/4.10.0")
        self.requires("catch2/3.7.0")
        self.requires("pugixml/1.14")
        self.requires("nlohmann_json/3.11.3")
        self.requires("libxlsxwriter/1.1.8")
        self.requires("duckdb/1.1.3")
        self.requires("cpp-httplib/0.19.0", force=True)
        self.requires("openssl/3.4.1")
        self.requires("cli11/2.5.0")
        self.requires("onnx/1.17.0", force=True)
        self.requires("rapidyaml/0.7.1")
        self.requires("onnxruntime/1.18.1")
        if self.options.with_cuda:
            self.requires("libtorch/2.7.1", options={"with_cuda": True})
        else:
            self.requires("libtorch/2.7.1", options={"with_cuda": False})
        self.requires("tensorflow-lite/2.15.0")
        self.requires("flatbuffers/23.5.26", force=True)
        self.requires("protobuf/3.21.12", override=True)
        self.requires("xkbcommon/1.6.0", override=True)
        self.requires("libpq/15.5", override=True)
        self.requires("abseil/20240116.1", override=True)
        self.requires("libbacktrace/cci.20240730", override = True)
        self.requires("xnnpack/cci.20240229", override = True)
        self.requires("boost/1.86.0", override = True)
        #self.requires("cpp-httplib/0.19.0")

    def generate(self):
        print("=====GENERATE=========")
        deps = CMakeDeps(self)
        deps.generate()
        toolchain = CMakeToolchain(self)
        toolchain.variables["WITH_CUDA"] = self.options.with_cuda
        toolchain.variables["TAG_NAME"] = "1.0.0"
        toolchain.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def layout(self):
        cmake_layout(self)


# conan build . --profile conan/profile_linux
# conan remote disable imageclibs
