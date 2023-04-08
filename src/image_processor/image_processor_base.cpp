

#include "image_processor_base.hpp"
#include <filesystem>

namespace joda::processor {

using namespace std;
using namespace std::filesystem;

ImageProcessorBase::ImageProcessorBase(const std::string &&inputFolder, const std::string &&outputFolder) :

    mInputFolder(inputFolder), mOutputFolder(outputFolder)
{
}

///
/// \brief      Starts the analyzing. First looks for images in the input folder
///             Second starts the analyzing process.
/// \author     Joachim Danmayr
///
void ImageProcessorBase::start()
{
  lookForImagesInFolderAndSubfolder();
  analyzeAllImages();
}

auto ImageProcessorBase::getListOfImagePaths() const -> const std::vector<std::string> &
{
  return mListOfImagePaths;
}

auto ImageProcessorBase::getOutputFolder() const -> const std::string &
{
  return mOutputFolder;
}

auto ImageProcessorBase::getAllOverReporting() -> joda::reporting::Table &
{
  return mAllOverReporting;
}

///
/// \brief      Find all images in the given inputfolder and its subfolders.
/// \author     Joachim Danmayr
///
void ImageProcessorBase::lookForImagesInFolderAndSubfolder()
{
  mListOfImagePaths.clear();

  for(recursive_directory_iterator i(mInputFolder), end; i != end; ++i) {
    if(!is_directory(i->path())) {
      if(ALLOWED_EXTENSIONS.contains(i->path().extension())) {
        mListOfImagePaths.push_back(i->path().filename());
      }
    }
  }
}

}    // namespace joda::processor
