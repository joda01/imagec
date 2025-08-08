FROM mcr.microsoft.com/windows/servercore:ltsc2022

SHELL ["powershell", "-Command"]

#
# Install Chocolatey
#
RUN Set-ExecutionPolicy Bypass -Scope Process -Force; \
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; \
    iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

#
# Install Visual Studio Build Tools with C++ workload
#
RUN choco install -y visualstudio2022buildtools `
    --package-parameters "--add Microsoft.VisualStudio.Workload.VCTools --includeRecommended --includeOptional"

#
# Install CMake, Git, Python, Conan
#
RUN choco install -y cmake git python --installargs 'ADD_CMAKE_TO_PATH=System' ; \
    pip install --upgrade conan

#
# Install CUDA Toolkit (no GPU required for compilation)
#
RUN Invoke-WebRequest -Uri https://developer.download.nvidia.com/compute/cuda/12.8.0/network_installers/cuda_12.8.0_windows_network.exe -OutFile cuda_installer.exe ; \
    Start-Process -Wait -FilePath cuda_installer.exe -ArgumentList '-s','nvcc_12.8','visual_studio_integration_12.8' ; \
    Remove-Item cuda_installer.exe


#
# Set ENV variables
#
ENV CUDA_PATH="C:\\Program Files\\NVIDIA GPU Computing Toolkit\\CUDA\\v12.8"
ENV PATH="${CUDA_PATH}\\bin;${CUDA_PATH}\\libnvvp;${PATH}"

#
# Verify tools
#
RUN cl && cmake --version && git --version && python --version && conan --version && nvcc --version

CMD ["powershell"]
