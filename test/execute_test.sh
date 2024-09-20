cd test
./load_test_data.sh
cd ..
unzip java/jre_linux.zip -d java/
./build/build/tests --reporter junit --out test-results.xml
ret_value=$?
rm -rf java/jre_linux
if [ $ret_value -eq 0 ]; then
  echo "Success"
else
  echo "Failure, test return status $ret_value"
fi
exit $ret_value
