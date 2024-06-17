cd test
./load_test_data.sh
cd ..
./build/build/tests --reporter junit --out test-results.xml
