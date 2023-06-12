cmake --build build --target imagec --parallel 4

cp ./build/build/imagec ./deb/imagec/usr/local/bin/imagec
cp -r ./imagec_models/. ./deb/imagec/usr/local/bin/imagec_models/.
cp -r ./build/build/imagec_gui/. ./deb/imagec/usr/local/bin/imagec_gui/.


chown root:root -R ./deb/imagec
chmod 755 ./deb/imagec/usr/local/bin/imagec
chmod 755 ./deb/imagec/DEBIAN/preinst
chmod 755 ./deb/imagec/DEBIAN/postinst


dpkg-deb --build ./deb/imagec
