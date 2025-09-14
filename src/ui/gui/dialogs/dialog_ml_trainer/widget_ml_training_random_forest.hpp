///
/// \file      widget_ml_training_random_forest.hpp
/// \author    Joachim Danmayr
/// \date      2025-09-14
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include <qwidget.h>
#include "backend/commands/classification/pixel_classifier/pixel_classifier_training_settings.hpp"

namespace joda::ui::gui {

class SettingsRandomForest : public QWidget
{
public:
  SettingsRandomForest(QWidget *parent) :
      QWidget(parent){

      };

  [[nodiscard]] auto toRandomForestSettings() const -> joda::settings::RandomForestTrainingSettings{

  };
};

}    // namespace joda::ui::gui
