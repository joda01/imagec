

#include "classifier_filter.hpp"
#include <string>
#include "backend/enums/enum_memory_idx.hpp"
#include "backend/processor/context/process_context.hpp"

namespace joda::settings {

bool ClassifierFilter::doesFilterMatch(joda::processor::ProcessContext &context, atom::ROI &roi, const MetricsFilter &metrics,
                                       const IntensityFilter &intensity)
{
  if(metrics.excludeObjectsAtTheEdge) {
    if(roi.isTouchingTheImageEdge()) {
      return false;
    }
  }

  if((intensity.minIntensity >= 0 && intensity.maxIntensity >= 0)) {
    const auto &cachedImage = context.loadImageFromCache(enums::MemoryScope::ITERATION, intensity.imageIn);
    auto intensityMeasured  = roi.measureIntensityAndAdd(*cachedImage);
    if(intensityMeasured.intensityAvg < intensity.minIntensity || intensityMeasured.intensityAvg > intensity.maxIntensity) {
      // Intensity filter does not match
      return false;
    }
  }
  const auto &physicalSize = context.getPhysicalPixelSIzeOfImage();
  return (metrics.minParticleSize < 0 || roi.getAreaSize(physicalSize) >= static_cast<double>(metrics.minParticleSize)) &&
         (metrics.maxParticleSize < 0 || roi.getAreaSize(physicalSize) <= static_cast<double>(metrics.maxParticleSize)) &&
         roi.getCircularity() >= metrics.minCircularity;
}

}    // namespace joda::settings
