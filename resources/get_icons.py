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

def list_png_files(folder_path):
    # List all files in the directory
    files = os.listdir(folder_path)
    
    # Filter out files that end with .png
    png_files = [file for file in files if file.endswith('.png')]
    
    return png_files

def create_qresource_file(folder_path, output_file):
    png_files = list_png_files(folder_path)
    
    with open(output_file, 'w') as f:
        f.write('<RCC>\n<qresource prefix="/icons">\n')
        for file in png_files:
            f.write(f'    <file>icons/{file}</file>\n')
        f.write('</qresource>\n</RCC>')


# Example usage
folder_path = './icons'
unzip_and_remove(folder_path)
png_files = list_png_files(folder_path)
output_file = './icons.qrc'
create_qresource_file(folder_path, output_file)
