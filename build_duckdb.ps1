

git clone -b v0.20.2 https://github.com/duckdb/duckdb.git
cd duckdb
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_EXTENSIONS=parquet -DOVERRIDE_GIT_DESCRIBE="v0.10.2"
cmake --build . --config Release
ls
