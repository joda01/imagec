import os
import zipfile

def unzip_and_remove(folder_path):
    # List all files in the directory
    files = os.listdir(folder_path)
    
    # Filter out files that end with .zip
    zip_files = [file for file in files if file.endswith('.zip')]
    
    for zip_file in zip_files:
        zip_file_path = os.path.join(folder_path, zip_file)
        
        # Extract the contents of the zip file
        with zipfile.ZipFile(zip_file_path, 'r') as zip_ref:
            zip_ref.extractall(folder_path)
        
        # Remove the original zip file
        os.remove(zip_file_path)
        print(f"Unzipped and removed: {zip_file_path}")

def list_files_files(folder_path, endian):
    # List all files in the directory
    files = os.listdir(folder_path)
    
    # Filter out files that end with .png
    png_files = [file for file in files if file.endswith(endian)]
    
    return png_files

def create_qresource_file(folder_path, output_file, endian):
    png_files = list_files_files(folder_path,endian)
    subgroup = folder_path.replace("./","")
    with open(output_file, 'a') as f:
        f.write(f'<qresource prefix="/{subgroup}">\n')
        for file in png_files:
            f.write(f'    <file>{subgroup}/{file}</file>\n')
        f.write('</qresource>\n')


# Example usage
output_file = './icons.qrc'
if os.path.exists(output_file):
    os.remove(output_file)

folder_path = './icons'
unzip_and_remove(folder_path)

with open(output_file, 'a') as f:
    f.write(f'<RCC>\n')
create_qresource_file(folder_path, output_file, ".png")
create_qresource_file("./icons-svg/regular-black", output_file, ".svg")
create_qresource_file("./icons-svg/regular-blue", output_file, ".svg")
create_qresource_file("./icons-svg/regular-gray", output_file, ".svg")
create_qresource_file("./icons-svg/regular-red", output_file, ".svg")
create_qresource_file("./icons-svg/duotone-black", output_file, ".svg")
create_qresource_file("./icons-svg/duotone-red", output_file, ".svg")
create_qresource_file("./icons-svg/duotone-green", output_file, ".svg")
create_qresource_file("./icons-svg/duotone-blue", output_file, ".svg")
create_qresource_file("./icons-svg/duotone-yellow", output_file, ".svg")
create_qresource_file("./icons-svg/regular-green", output_file, ".svg")
create_qresource_file("./icons-svg/regular-yellow", output_file, ".svg")
with open(output_file, 'a') as f:
    f.write(f'</RCC>\n')
