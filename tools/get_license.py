import json
import subprocess
import re


def remove_after_hash(s):
    return s.split("#")[0]


def update_readme(new_text):
    file_path = "/workspaces/imagec/README.md"

    with open(file_path, "r", encoding="utf-8") as file:
        content = file.read()
    # Regular expression to replace content between markers
    updated_content = re.sub(
        r"(<!---EXT-LIBS-->)(.*?)(<!---EXT-LIBS-->)",
        rf"\1\n{new_text}\n\3",
        content,
        flags=re.DOTALL
    )

    with open(file_path, "w", encoding="utf-8") as file:
        file.write(updated_content)

def write_html(html_text):
    with open("/workspaces/imagec/resources/other/open_source_libs.html", "w", encoding="utf-8") as file:
        file.write(html_text)
    



# Run conan info and capture output
output = subprocess.run(["conan","graph", "info", ".", "--out-file","deps.json","-f","json","--profile","conan/profile_linux"], capture_output=True, text=True)

# Load JSON data
with open("deps.json", "r") as file:
    data = json.load(file)

    my_deps = set()  # Empty dictionary

    stringToPrint = "Title | Link | License\n"
    stringToPrint += "------|------|--------\n"
    htmlString = "<html><div style=\"text-align: center;\">"
    for key, value in data["graph"]["nodes"].items():
        package_type = value["package_type"]
        context = value["context"]
        if (package_type == "shared-library" or package_type == "static-library" or package_type == "header-library") and context != "build":
            if value["ref"] not in my_deps:
                my_deps.add(value["ref"])
                stringToPrint += remove_after_hash(value["ref"]) + "|"+str(value["homepage"]) + "|" + str(value["license"])+"\n"
                htmlString += remove_after_hash(value["ref"]) + " : <a href=\""+str(value["homepage"])+"\">"+str(value["homepage"])+"</a> | "+ str(value["license"])+"<br/>\n"
    
    update_readme(stringToPrint)
    htmlString+=" </div></html>"
    write_html(htmlString)
