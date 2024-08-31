

#include "classifier_filter.hpp"
#include "backend/processor/context/process_context.hpp"

namespace joda::settings {

bool ClassifierFilter::doesFilterMatch(joda::processor::ProcessContext &context, atom::ROI &roi,
                                       const IntensityFilter &intensity) const
{
  if(intensity.minIntensity >= 0 || intensity.maxIntensity >= 0) {
    const auto &cachedImage = context.loadImageFromCache(intensity.imageIn);
    auto intensity          = roi.measureIntensityAndAdd(*cachedImage);
    if(intensity.intensityAvg < intensity.intensityMin || intensity.intensityAvg > intensity.intensityMax) {
      // Intensity filter does not match
      return false;
    }

    if((minParticleSize < 0 || roi.getAreaSize() >= minParticleSize) &&
       (maxParticleSize < 0 || roi.getAreaSize() <= maxParticleSize) && roi.getCircularity() >= minCircularity) {
      return true;
    }
    return false;
  }
  return true;
}

}    // namespace joda::settings
