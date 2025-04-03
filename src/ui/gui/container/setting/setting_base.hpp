///
/// \file      container_function_base.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-25
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qwidget.h>
#include <QtWidgets>
#include "backend/enums/enums_classes.hpp"

#include "backend/settings/setting.hpp"

namespace joda::ui::gui {

class WindowMain;

template <typename T>
struct is_enum
{
  static constexpr bool value = std::is_enum<T>::value;
};

template <typename T>
concept NumberOrString =
    std::same_as<T, int32_t> || std::same_as<T, uint32_t> || std::same_as<T, uint16_t> || std::same_as<T, float> || std::same_as<T, std::string>;

template <typename T>
concept Number_t = std::same_as<T, int> || std::same_as<T, uint32_t> || std::same_as<T, uint16_t> || std::same_as<T, float> || std::same_as<T, bool>;

template <typename T>
concept NumberOrEnum_t = std::same_as<T, int> || std::same_as<T, uint32_t> || std::same_as<T, uint16_t> || std::same_as<T, float> ||
                         std::same_as<T, bool> || std::is_enum<T>::value;

///
/// \class      SettingBase
/// \author     Joachim Danmayr
/// \brief
///
class SettingBase : public QWidget
{
  Q_OBJECT

public:
  template <class T>
  static std::unique_ptr<T> create(QWidget *parent, const QIcon &icon, const QString &description, int32_t maxTextLengthToDisplay = 100)
  {
    auto instance = std::make_unique<T>(parent, icon, description, maxTextLengthToDisplay);
    instance->createEditableWidget();
    return instance;
  }

  /////////////////////////////////////////////////////
  SettingBase(QWidget *parent, const QIcon &icon, const QString &description, int32_t maxTextLengthToDisplay = 100);
  ~SettingBase() = default;
  void blockAllSignals(bool);
  void setUnit(const QString &unit);
  void setDisplayIconVisible(bool visible);
  void setShortDescription(const QString &description);
  void setPathToHelpFile(const QString &helpFilePath);

  QWidget *getDisplayLabelWidget();
  QWidget *getEditableWidget();
  QWidget *getInputObject();

  [[nodiscard]] QString getDisplayLabelText() const;
  virtual void reset() = 0;
  virtual void clear() = 0;

signals:
  /////////////////////////////////////////////////////
  void valueChanged();
  void displayTextChanged();

protected:
  /////////////////////////////////////////////////////
  static constexpr int32_t TXT_ICON_SIZE  = 16;
  static constexpr int32_t DISP_ICON_SIZE = 16;
  static constexpr int32_t HELP_ICON_SIZE = 8;

  /////////////////////////////////////////////////////
  void triggerValueChanged(const QString &newValue, bool hasValueChanged, const QIcon &icon = {});
  void updateDisplayLabel();
  auto getIcon() -> const QIcon &;

  WindowMain *getParent()
  {
    return mParent;
  }

  [[nodiscard]] auto getClasses() const -> std::map<enums::ClassIdIn, QString>;

private:
  /////////////////////////////////////////////////////
  virtual void blockComponentSignals(bool) = 0;
  void createDisplayAbleWidget(const QIcon &icon, const QString &tooltip);
  QWidget *createDisplayAbleWidgetPlaceholder();
  void createEditableWidget();
  void createHelperText(QVBoxLayout *layout, const QString &helpText);
  virtual QWidget *createInputObject() = 0;

  virtual void classsNamesChanged()
  {
  }

  virtual void outputClassesChanges()
  {
  }

  /////////////////////////////////////////////////////
  WindowMain *mParent = nullptr;
  QIcon mIcon;
  QWidget *mInputObject     = nullptr;
  QWidget *mEditable        = nullptr;
  QWidget *mDisplayable     = nullptr;
  QLabel *mDisplayLabel     = nullptr;
  QLabel *mDisplayLabelIcon = nullptr;
  QPushButton *mHelpButton  = nullptr;
  QString mDescription;
  QString mShortDescription;
  QString mDisplayValue;
  QString mUnit;
  QString mHelpFilePath;
  int32_t mMaxTextLengthToDisplay = 0;

private slots:
  void onHelpButtonClicked();
  void onClassificationNameChanged();
  void onOutputClassifierChanges();
};

}    // namespace joda::ui::gui
