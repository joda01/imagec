

#include <opencv2/core/hal/interface.h>
#include <exception>
#include <filesystem>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include "../initializer/pipeline_initializer.hpp"
#include "../processor.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/settings/analze_settings.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <nlohmann/json_fwd.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::test {

///
/// \brief  Spot test
/// \author Joachim Danmayr
///
SCENARIO("pipeline:test:heatmap", "[pipeline]")
{
  std::ifstream file("src/backend/processor/test/test_run.json");
  joda::settings::AnalyzeSettings settings = nlohmann::json::parse(file);
  file.close();
  joda::processor::Processor processor;
  processor::imagesList_t workingdirs;
  processor.execute(settings, workingdirs);
}

SCENARIO("pipeline:test:bigdata", "[bigdata]")
{
  std::ifstream file("src/backend/processor/test/test_run.json");
  joda::settings::AnalyzeSettings settings = nlohmann::json::parse(file);
  db::Database db;
  db.openDatabase(std::filesystem::path("results_bigdata.icdb"));
  auto jobId = db.startJob(settings);

  processor::PipelineInitializer initializer(settings.imageSetup);

  std::random_device rd;
  std::mt19937 gen(rd());

  std::uniform_int_distribution<uint64_t> dist0to1024(0, 800);
  std::uniform_int_distribution<uint64_t> dist4to20(4, 20);
  std::uniform_int_distribution<uint64_t> dist0to65535(0, 65535);
  std::uniform_int_distribution<uint64_t> dist(0, std::numeric_limits<uint64_t>::max());

  // 384 groups
  for(int well = 0; well < 384; well++) {
    joda::grp::GroupInformation groupInfo;
    groupInfo.groupName = "Name(" + std::to_string(well) + ")";
    groupInfo.groupId   = well;
    groupInfo.wellPosX  = well % 24;
    groupInfo.wellPosY  = well / 24;
    db.insertGroup(0, groupInfo);

    // 4 images per group
    uint64_t roiIdx = 0;
    for(int n = 0; n < 4; n++) {
      uint64_t random_number = dist(gen);
      joda::processor::ImageContext imageContext{.imageLoader = initializer};
      imageContext.imageId   = random_number;
      imageContext.imagePath = "path/" + std::to_string(random_number);

      imageContext.imageMeta.getImageInfoSeries().emplace(0, ome::OmeInfo::ImageInfo{});
      imageContext.imageMeta.getImageInfoSeries()[0].resolutions.emplace(
          0, ome::OmeInfo::ImageInfo::Pyramid{.imageWidth = 1024, .imageHeight = 1024});

      imageContext.imageMeta.getImageInfoSeries()[0].channels.emplace(
          0, ome::OmeInfo::ChannelInfo{
                 .channelId = "0", .name = "cy5", .planes = {{0, {{0, ome::OmeInfo::ImagePlane{}}}}}});
      imageContext.imageMeta.getImageInfoSeries()[0].channels.emplace(
          1, ome::OmeInfo::ChannelInfo{
                 .channelId = "1", .name = "cy7", .planes = {{0, {{0, ome::OmeInfo::ImagePlane{}}}}}});
      imageContext.imageMeta.getImageInfoSeries()[0].channels.emplace(
          2, ome::OmeInfo::ChannelInfo{
                 .channelId = "2", .name = "nucleus", .planes = {{0, {{0, ome::OmeInfo::ImagePlane{}}}}}});
      imageContext.imageMeta.getImageInfoSeries()[0].channels.emplace(
          3, ome::OmeInfo::ChannelInfo{
                 .channelId = "3", .name = "cell", .planes = {{0, {{0, ome::OmeInfo::ImagePlane{}}}}}});
      imageContext.imageMeta.getImageInfoSeries()[0].nrOfChannels = 4;

      // imageContext.imageMeta.getSize();
      groupInfo.imageIdx = n;
      auto id            = DurationCount::start("Insert image (" + std::to_string(well) + ")");
      db.insertImage(imageContext, groupInfo);
      db.insertImageChannels(imageContext.imageId, imageContext.imageMeta);
      db.insetImageToGroup(0, imageContext.imageId, groupInfo.imageIdx, groupInfo);
      DurationCount::stop(id);

      // 4 channels per image
      for(int ch = 0; ch < 4; ch++) {
        db.insertImagePlane(imageContext.imageId, enums::PlaneId{.tStack = 0, .zStack = 0, .cStack = ch},
                            imageContext.imageMeta.getChannelInfos().at(ch).planes.at(0).at(0));

        // 100 Objects per image
        joda::atom::ObjectList objectsList;
        for(int obj = 0; obj < 500; obj++) {
          for(int classs = 0; classs < 4; classs++) {
            atom::ROI::RoiObjectId objId{.objectId   = roiIdx,
                                         .clusterId  = static_cast<enums::ClusterId>(ch),
                                         .classId    = (enums::ClassId) classs,
                                         .imagePlane = {.tStack = 0, .zStack = 0, .cStack = ch}};

            atom::Boxes box{static_cast<int>(dist0to1024(gen)), static_cast<int>(dist0to1024(gen)),
                            static_cast<int>(dist4to20(gen)), static_cast<int>(dist4to20(gen))};
            auto confidence = static_cast<float>(dist0to65535(gen));
            cv::Mat mask{cv::Size{box.width, box.height}, CV_8UC1};

            std::uniform_int_distribution<int> rand_x(0, box.width - 1);
            std::uniform_int_distribution<int> rand_y(0, box.height - 1);
            std::uniform_int_distribution<int> rand_nr(0, box.height * box.width);
            // Set a specified number of pixels to 255
            int num_pixels_to_set = rand_nr(gen);    // Adjust this value as needed
            for(int i = 0; i < num_pixels_to_set; ++i) {
              int x                = rand_x(gen);
              int y                = rand_y(gen);
              mask.at<uchar>(y, x) = 255;
            }

            std::vector<cv::Point> contour;

            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(mask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
            if(!contours.empty()) {
              contour = contours[0];
            }

            cv::Size imageSize{1024, 1024};
            enums::tile_t tile{0, 0};
            atom::ROI roi(objId, confidence, 0, box, mask, contour, imageSize, tile, imageSize);
            for(int chMeasure = 0; chMeasure < 3; chMeasure++) {
              auto otherImg = cv::Mat{imageSize, CV_16UC1};
              otherImg      = static_cast<uint16_t>(dist0to65535(gen));
              roi.measureIntensityAndAdd(
                  joda::atom::ImagePlane{.tile{0, 0},
                                         .image = otherImg,
                                         .mId   = {.imageIdx   = joda::enums::ZProjection::MAX_INTENSITY,
                                                   .imagePlane = {.tStack = 0, .zStack = 0, .cStack = chMeasure}}});
            }
            roi.addIntersectingRoi(&roi);

            objectsList[static_cast<enums::ClusterId>(ch)].emplace(roi);
            roiIdx++;
          }
        }

        auto id3 = DurationCount::start("Insert objects (" + std::to_string(well) + ")");
        db.insertObjects(imageContext, objectsList);
        DurationCount::stop(id3);
      }
    }
  }

  db.finishJob(jobId);
}

}    // namespace joda::test
