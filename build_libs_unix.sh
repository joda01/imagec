#conan profile detect --force
conan install . --profile conan/profile_linux --output-folder=build --build=missing
cyclonedx-conan . --output sbom.spdx


conan graph info . --profile conan/profile_win_mingw --format=html > graph.html
