pip install conan
apt-get install libva-dev libvdpau-dev xkb-data

conan profile detect --force
conan install . --profile conan/profile_linux --output-folder=build --build=missing
