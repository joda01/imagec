cd test
./load_test_data.sh
cd ..
unzip java/jre_linux.zip -d java/
./build/build/tests --reporter junit --out test-results.xml
rm -rf java/jre_linux
