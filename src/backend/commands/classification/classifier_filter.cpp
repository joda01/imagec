

#include "classifier_filter.hpp"
#include <string>
#include "backend/processor/context/process_context.hpp"

namespace joda::settings {

bool ClassifierFilter::doesFilterMatch(joda::processor::ProcessContext &context, atom::ROI &roi, const MetricsFilter &metrics,
                                       const IntensityFilter &intensity)
{
  if((intensity.minIntensity >= 0 && intensity.maxIntensity >= 0)) {
    const auto &cachedImage = context.loadImageFromCache(intensity.imageIn);
    auto intensityMeasured  = roi.measureIntensityAndAdd(*cachedImage);
    if(intensityMeasured.intensityAvg < intensity.minIntensity || intensityMeasured.intensityAvg > intensity.maxIntensity) {
      // Intensity filter does not match
      return false;
    }
  }
  if((metrics.minParticleSize < 0 || roi.getAreaSize() >= metrics.minParticleSize) &&
     (metrics.maxParticleSize < 0 || roi.getAreaSize() <= metrics.maxParticleSize) && roi.getCircularity() >= metrics.minCircularity) {
    return true;
  }

  return false;
}

}    // namespace joda::settings
