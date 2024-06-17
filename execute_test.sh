cd test
./load_test_data.sh
cd ..
./build/build/tests --reporter junit --out result.xml
