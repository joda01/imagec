conan profile detect --force
conan install . --profile conan/profile_linux --output-folder=build --build=missing
cyclonedx-conan . --output sbom.spdx
