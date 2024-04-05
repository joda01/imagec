///
/// \file      version.h
/// \author    Joachim Danmayr
/// \date      2023-04-23
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <openssl/sha.h>
#include <sys/stat.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include "build_info.h"

class Version
{
public:
  static auto getVersion() -> std::string
  {
    return "v1.0.0-alpha.40";
  }

  static auto getBuildTime() -> std::string
  {
    return BUILD_TIME;
  }

  static void initVersion(const std::string &bin)
  {
    calcHash(bin);
  }

  static auto getHash() -> std::string
  {
    return mCalculatedHash;
  }

  static std::string getLogo()
  {
    std::string logo = "    _                            ______ \n";
    logo += "   (_)___ ___  ____ _____  ___  / ____/\n";
    logo += "  / / __ `__ \\/ __ `/ __ `/ _ \\/ /   \n";
    logo += " / / / / / / / /_/ / /_/ /  __/ /___   \n";
    logo += "/_/_/ /_/ /_/\\__,_/\\__, /\\___/\\____/\n";
    logo += "                  /____/               \n";
    logo += "_______________________________________\n";
    return logo;
  }

private:
  /////////////////////////////////////////////////////
  static inline std::string mCalculatedHash;

  /////////////////////////////////////////////////////
  static void calcHash(const std::string &filename)
  {
    std::ifstream file(filename, std::ios::binary);
    if(!file) {
      std::cerr << "Error: Could not open file " << filename << std::endl;
    }

    // Calculate the SHA-256 hash of the file
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    unsigned char buffer[1024];
    while(file.read((char *) buffer, sizeof(buffer))) {
      SHA256_Update(&sha256, buffer, file.gcount());
    }
    SHA256_Final(buffer, &sha256);

    // Convert the hash to a hex string
    std::ostringstream hashStream;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
      hashStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(buffer[i]);
    }
    std::string hashString = hashStream.str();
    mCalculatedHash        = hashString;
  }
};
