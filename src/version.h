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

///

#pragma once

#include <openssl/evp.h>
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
    return VERSION;
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

  static std::string getProgamName()
  {
    return "imageC";
  }

  static std::string getTitle()
  {
    return "EVAnalayzer2";
  }

  static std::string getSubtitle()
  {
    return "powered by " + getProgamName();
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

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if(!mdctx) {
      std::cerr << "Error: EVP_MD_CTX_new failed" << std::endl;
      return;
    }

    if(!EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL)) {
      std::cerr << "Error: EVP_DigestInit_ex failed" << std::endl;
      EVP_MD_CTX_free(mdctx);
      return;
    }

    unsigned char buffer[1024];
    while(file.read((char *) buffer, sizeof(buffer))) {
      if(!EVP_DigestUpdate(mdctx, buffer, file.gcount())) {
        std::cerr << "Error: EVP_DigestUpdate failed" << std::endl;
        EVP_MD_CTX_free(mdctx);
        return;
      }
    }
    file.close();

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;
    if(!EVP_DigestFinal_ex(mdctx, hash, &hash_len)) {
      std::cerr << "Error: EVP_DigestFinal_ex failed" << std::endl;
      EVP_MD_CTX_free(mdctx);
      return;
    }

    EVP_MD_CTX_free(mdctx);

    std::string result;
    for(unsigned int i = 0; i < hash_len; ++i) {
      std::stringstream ss;
      ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
      result += ss.str();
    }
    mCalculatedHash = result;
  }
};
