cmake --build build --target imagec --parallel 4

rm -rf ./deploy/imagec/usr/local/bin/imagec
rm -rf ./deploy/imagec/usr/local/bin/imagec_models
rm -rf ./deploy/imagec/usr/local/bin/imagec_gui

mkdir -p ./deploy/imagec/usr/local/bin/imagec_models
mkdir -p ./deploy/imagec/usr/local/bin/imagec_gui

cp ./build/build/imagec ./deploy/imagec/usr/local/bin/imagec
cp -r ./imagec_models/. ./deploy/imagec/usr/local/bin/imagec_models/.
cp -r ./imagec_gui/. ./deploy/imagec/usr/local/bin/imagec_gui/.

chown root:root -R ./deploy/imagec
chmod 755 ./deploy/imagec/usr/local/bin/imagec
chmod 755 ./deploy/imagec/DEBIAN/preinst
chmod 755 ./deploy/imagec/DEBIAN/postinst


dpkg-deb --build ./deploy/imagec
