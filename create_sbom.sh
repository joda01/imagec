conan sbom:cyclonedx --format 1.4_json . > sbom/sbom.cdx.json

#conan config install https://github.com/conan-io/conan-extensions.git
#pip install conan --upgrade --break-system-packages
#pip install 'cyclonedx-python-lib>=5.0.0,<6' --break-system-packages

#

curl -X "POST" "http://localhost:8081/api/v1/bom" \
     -H "X-Api-Key: odt_YrQzD2bI_48C9Otc5g49fIl3FlN3KU4GLpflzZQ74" \
     -H "Content-Type: multipart/form-data" \
     -F "projectName=ImageC" \
     -F "bom=@sbom.cdx.json"
