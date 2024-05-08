///
/// \file      xz_wrapper.cpp
/// \author    Joachim Danmayr
/// \date      2024-05-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include <string>
#include <vector>

namespace joda::helper::xz {

extern int createAndAddFiles(const std::string &archiveFilename, const std::string &pathToResultsFolder,
                             const std::string &fileExtension);
extern std::vector<std::string> listFiles(const std::string &archiveFilename);
extern std::string readFile(const std::string &archiveFilename, const std::string &filename);

}    // namespace joda::helper::xz
