

#include "image_processor_base.hpp"
#include <filesystem>

namespace joda::processor {

using namespace std;
using namespace std::filesystem;

ImageProcessorBase::ImageProcessorBase(const std::string &inputFolder, const std::string &outputFolder) :

    mInputFolder(inputFolder), mOutputFolder(outputFolder)
{
}

///
/// \brief      Starts the analyzing. First looks for images in the input folder
///             Second starts the analyzing process.
/// \author     Joachim Danmayr
///
auto ImageProcessorBase::start() -> std::future<void> &
{
  auto mainThreadFunc = [this] { mainThread(); };
  mFuture             = std::async(std::launch::async, mainThreadFunc);
  return mFuture;
}

///
/// \brief      Returns the future of the threaded object
/// \author     Joachim Danmayr
///
auto ImageProcessorBase::getFuture() -> std::future<void> &
{
  return mFuture;
}

///
/// \brief      Returns the future of the threaded object
/// \author     Joachim Danmayr
///
void ImageProcessorBase::wait()
{
  mFuture.wait();
}

///
/// \brief      Returns true if analyzing has been finished
/// \author     Joachim Danmayr
///
bool ImageProcessorBase::isFinished() const
{
  return isStopped();
}

void ImageProcessorBase::mainThread()
{
  lookForImagesInFolderAndSubfolder();
  if(!isStopped()) {
    analyzeAllImages();
  }
}

///
/// \brief      Stop image processing
/// \author     Joachim Danmayr
///
void ImageProcessorBase::stop()
{
  mStopped = true;
}

///
/// \brief      Get actual progress of total and act image
/// \author     Joachim Danmayr
/// \return     Total Progress, Progress of act image
///
auto ImageProcessorBase::getProgress() const -> std::tuple<Progress, Progress, State>
{
  State state = State::FINISHED;
  if(mStopped) {
    state = State::STOPPING;
  }
  if(!mStopped) {
    state = State::RUNNING;
  }

  return {mProgressTotal, mProgressActImage, state};
}

void ImageProcessorBase::setTotalImages(uint32_t total)
{
  mProgressTotal.total    = total;
  mProgressTotal.finished = 0;
}
void ImageProcessorBase::setProgressTotal(uint32_t finished)
{
  mProgressTotal.finished = finished;
}
void ImageProcessorBase::setTotalActImages(uint32_t total)
{
  mProgressActImage.total    = total;
  mProgressActImage.finished = 0;
}
void ImageProcessorBase::setProgressActImage(uint32_t finished)
{
  mProgressActImage.finished = finished;
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
        mListOfImagePaths.push_back(i->path());
      }
    }
    if(isStopped()) {
      break;
    }
  }
}

}    // namespace joda::processor
