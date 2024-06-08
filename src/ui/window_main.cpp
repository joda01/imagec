///
/// \file      window_main.cpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "window_main.hpp"
#include <qcombobox.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qstackedwidget.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <QAction>
#include <QIcon>
#include <QMainWindow>
#include <QToolBar>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/random_name_generator.hpp"
#include "backend/results/results.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/channel/channel_settings.hpp"
#include "backend/settings/settings.hpp"
#include "backend/settings/vchannel/vchannel_settings.hpp"
#include "backend/settings/vchannel/vchannel_voronoi_settings.hpp"
#include "container/channel/container_channel.hpp"
#include "container/giraf/container_giraf.hpp"
#include "container/intersection/container_intersection.hpp"
#include "container/voronoi/container_voronoi.hpp"
#include "ui/container/giraf/container_giraf.hpp"
#include "ui/dialog_analyze_running.hpp"
#include "ui/dialog_experiment_settings.hpp"
#include "ui/dialog_shadow/dialog_shadow.h"
#include "ui/reporting/panel_reporting.hpp"
#include "build_info.h"
#include "version.h"

namespace joda::ui::qt {

using namespace std::chrono_literals;

WindowMain::WindowMain(joda::ctrl::Controller *controller) : mController(controller)
{
  const QIcon myIcon(":/icons/outlined/icon.png");
  setWindowIcon(myIcon);
  setWindowTitle("EVAnalyzer2 powered by imageC");
  createTopToolbar();
  createBottomToolbar();
  setMinimumSize(1600, 800);
  setObjectName("windowMain");
  setStyleSheet(
      "QMainWindow#windowMain {"
      "   background-color: rgb(251, 252, 253); "
      "   border: none;"
      "}");
  setCentralWidget(createStackedWidget());

  // Start with the main page
  onBackClicked();

  mMainThread = new std::thread(&WindowMain::waitForFileSearchFinished, this);
  connect(this, &WindowMain::lookingForFilesFinished, this, &WindowMain::onLookingForFilesFinished);
  connect(this, &WindowMain::lookingForTemplateFinished, this, &WindowMain::onFindTemplatesFinished);
}

void WindowMain::createBottomToolbar()
{
  auto *toolbar = new QToolBar(this);
  toolbar->setStyleSheet("QToolBar{spacing:8px;}");

  toolbar->setMinimumHeight(48);
  toolbar->setMovable(false);
  toolbar->setStyleSheet("QToolBar {background-color: rgb(251, 252, 253); border: 0px; border-bottom: 0px;}");
  // Middle

  {
    // Add a spacer to push the next action to the middle
    QWidget *spacerWidget = new QWidget();
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolbar->setMaximumHeight(32);
    toolbar->addWidget(spacerWidget);
  }

  // Add the QComboBox in the middle
  mFoundFilesCombo = new QComboBox(toolbar);
  mFoundFilesCombo->setMinimumWidth(250);
  mFoundFilesCombo->setMaximumWidth(300);
  mFileSelectorComboBox = toolbar->addWidget(mFoundFilesCombo);
  mFileSelectorComboBox->setVisible(false);

  mImageSeriesCombo = new QComboBox(toolbar);
  mImageSeriesCombo->addItem("Series 0", 0);
  mImageSeriesCombo->addItem("Series 1", 1);
  mImageSeriesCombo->addItem("Series 2", 2);
  mImageSeriesCombo->addItem("Series 3", 3);
  mImageSeriesComboBox = toolbar->addWidget(mImageSeriesCombo);
  mImageSeriesComboBox->setVisible(false);

  mImageTilesCombo = new QComboBox(toolbar);
  mImageTilesCombo->addItem("0", 0);
  mImageTilesCombo->setToolTip("Select image tile");
  mImageTilesComboBox = toolbar->addWidget(mImageTilesCombo);
  mImageTilesComboBox->setVisible(false);

  mFoundFilesHint = new ClickableLabel(toolbar);
  mFoundFilesHint->setText("Please open a working directory ...");
  mFileSearchHintLabel = toolbar->addWidget(mFoundFilesHint);
  connect(mFoundFilesHint, &ClickableLabel::clicked, this, &WindowMain::onOpenProjectClicked);

  addToolBar(Qt::ToolBarArea::BottomToolBarArea, toolbar);

  // Right
  {
    QWidget *spacerWidget = new QWidget();
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolbar->addWidget(spacerWidget);
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::createTopToolbar()
{
  auto *toolbar = addToolBar("toolbar");
  toolbar->setMinimumHeight(48);

  toolbar->setMovable(false);
  toolbar->setStyleSheet("QToolBar {background-color: rgb(251, 252, 253); border: 0px; border-bottom: 0px;}");

  // Left
  {
    mBackButton = new QAction(QIcon(":/icons/outlined/icons8-left-50.png"), "Back", toolbar);
    mBackButton->setEnabled(false);
    connect(mBackButton, &QAction::triggered, this, &WindowMain::onBackClicked);
    toolbar->addAction(mBackButton);
    mFirstSeparator = toolbar->addSeparator();

    mSaveProject = new QAction(QIcon(":/icons/outlined/icons8-save-50.png"), "Save", toolbar);
    mSaveProject->setToolTip("Save project!");
    connect(mSaveProject, &QAction::triggered, this, &WindowMain::onSaveProjectClicked);
    toolbar->addAction(mSaveProject);

    mOPenProject = new QAction(QIcon(":/icons/outlined/icons8-folder-50.png"), "Open", toolbar);
    mOPenProject->setToolTip("Open folder!");
    connect(mOPenProject, &QAction::triggered, this, &WindowMain::onOpenProjectClicked);
    toolbar->addAction(mOPenProject);
    mSecondSeparator = toolbar->addSeparator();

    mOpenReportingArea = new QAction(QIcon(":/icons/outlined/icons8-graph-50.png"), "Reporting area", toolbar);
    mOpenReportingArea->setToolTip("Open reporting area");
    connect(mOpenReportingArea, &QAction::triggered, this, &WindowMain::onOpenReportingAreaClicked);
    toolbar->addAction(mOpenReportingArea);

    mStartAnalysis = new QAction(QIcon(":/icons/outlined/icons8-play-50.png"), "Start", toolbar);
    mStartAnalysis->setEnabled(false);
    mStartAnalysis->setToolTip("Start analysis!");
    connect(mStartAnalysis, &QAction::triggered, this, &WindowMain::onStartClicked);
    toolbar->addAction(mStartAnalysis);

    {
      const QIcon myIcon(":/icons/outlined/icons8-topic-50.png");
      mJobName = new QLineEdit();
      mJobName->setObjectName("JobName");
      mJobName->setStyleSheet("border: 0px solid rgb(190, 209, 207);");
      mJobName->setText("");
      mJobName->setClearButtonEnabled(true);
      // mJobName->addAction(QIcon(myIcon.pixmap(28, 28)), QLineEdit::LeadingPosition);
      mJobName->setPlaceholderText(joda::helper::RandomNameGenerator::GetRandomName().data());
      mJobName->setMaximumWidth(200);
      mJobNameAction = toolbar->addWidget(mJobName);
    }
    // toolbar->addSeparator();

    mDeleteChannel = new QAction(QIcon(":/icons/outlined/icons8-trash-50.png"), "Remove channel", toolbar);
    mDeleteChannel->setToolTip("Delete channel!");
    connect(mDeleteChannel, &QAction::triggered, this, &WindowMain::onRemoveChannelClicked);
    toolbar->addAction(mDeleteChannel);
  }

  // Middle

  {
    // Add a spacer to push the next action to the middle
    QWidget *spacerWidget = new QWidget();
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolbar->setMaximumHeight(32);
    toolbar->addWidget(spacerWidget);
  }

  // Place middle here
  mMiddle = new QLabel();
  toolbar->addWidget(mMiddle);

  // Right
  {
    QWidget *spacerWidget = new QWidget();
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolbar->addWidget(spacerWidget);
  }

  toolbar->addSeparator();

  {
    mSettings = new QAction(QIcon(":/icons/outlined/icons8-settings-50.png"), "Settings", toolbar);
    mSettings->setToolTip("Settings");
    connect(mSettings, &QAction::triggered, this, &WindowMain::onOpenSettingsDialog);
    toolbar->addAction(mSettings);

    toolbar->addSeparator();

    mShowInfoDialog = new QAction(QIcon(":/icons/outlined/icons8-info-50.png"), "Info", toolbar);
    mShowInfoDialog->setToolTip("Info");
    connect(mShowInfoDialog, &QAction::triggered, this, &WindowMain::onShowInfoDialog);
    toolbar->addAction(mShowInfoDialog);
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
QWidget *WindowMain::createStackedWidget()
{
  mStackedWidget = new QStackedWidget();
  mStackedWidget->setObjectName("stackedWidget");
  mStackedWidget->addWidget(createOverviewWidget());
  mStackedWidget->addWidget(createChannelWidget());
  mStackedWidget->addWidget(createReportingWidget());
  return mStackedWidget;
}

///
/// \brief
/// \author     Joachim Danmayr
///
QWidget *WindowMain::createOverviewWidget()
{
  QScrollArea *scrollArea = new QScrollArea(this);
  scrollArea->setFrameStyle(0);
  scrollArea->setObjectName("scrollAreaOverview");
  scrollArea->setStyleSheet("QScrollArea#scrollAreaOverview { background-color: rgb(251, 252, 253);}");

  // Create a widget to hold the panels
  QWidget *contentWidget = new QWidget;
  contentWidget->setObjectName("contentOverview");
  contentWidget->setStyleSheet("QWidget#contentOverview { background-color: rgb(251, 252, 253);}");

  scrollArea->setWidget(contentWidget);
  scrollArea->setWidgetResizable(true);

  // Create a horizontal layout for the panels
  QHBoxLayout *horizontalLayout = new QHBoxLayout(contentWidget);
  horizontalLayout->setObjectName("mainWindowHLayout");
  horizontalLayout->setContentsMargins(16, 16, 16, 16);
  horizontalLayout->setSpacing(16);    // Adjust this value as needed
  contentWidget->setLayout(horizontalLayout);

  auto createVerticalContainer = []() -> std::tuple<QGridLayout *, QWidget *> {
    QWidget *contentWidget = new QWidget;
    QGridLayout *layout    = new QGridLayout(contentWidget);
    layout->setObjectName("mainWindowGridLayout");
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(8);    // Adjust this value as needed
    contentWidget->setLayout(layout);
    return {layout, contentWidget};
  };

  {
    auto [channelsOverViewLayout, channelsOverviewWidget] = createVerticalContainer();
    mLayoutChannelOverview                                = channelsOverViewLayout;

    channelsOverViewLayout->addWidget(createAddChannelPanel());

    mLastElement = new QLabel();
    channelsOverViewLayout->addWidget(mLastElement, 1, 0, 1, 3);

    channelsOverViewLayout->setRowStretch(0, 1);
    channelsOverViewLayout->setRowStretch(1, 1);
    channelsOverViewLayout->setRowStretch(2, 1);
    channelsOverViewLayout->setRowStretch(4, 3);

    // channelsOverViewLayout->addStretch();

    horizontalLayout->addStretch();
    horizontalLayout->addWidget(channelsOverviewWidget);
  }

  {
    /*
    auto [channelsOverViewLayout, channelsOverviewWidget] = createVerticalContainer();
    PanelChannelOverview *panel1                          = new PanelChannelOverview();
    channelsOverViewLayout->addWidget(panel1);
    PanelChannelOverview *panel2 = new PanelChannelOverview();
    channelsOverViewLayout->addWidget(panel2);
    channelsOverViewLayout->addStretch();
    horizontalLayout->addWidget(channelsOverviewWidget);
    */
  }

  horizontalLayout->addStretch();

  return scrollArea;
}

///
/// \brief
/// \author     Joachim Danmayr
///
QWidget *WindowMain::createChannelWidget()
{
  return new QWidget(this);
}

///
/// \brief
/// \author     Joachim Danmayr
///
QWidget *WindowMain::createReportingWidget()
{
  mPanelReporting = new PanelReporting(this);
  return mPanelReporting;
}

QWidget *WindowMain::createAddChannelPanel()
{
  QWidget *addChannelWidget = new QWidget();
  // setStyleSheet("border: 1px solid black; padding: 10px;");
  addChannelWidget->setObjectName("PanelChannelOverview");
  addChannelWidget->setMinimumHeight(250);
  addChannelWidget->setMinimumWidth(350);
  addChannelWidget->setMaximumWidth(350);
  QVBoxLayout *layout = new QVBoxLayout(); /*this*/
  layout->setContentsMargins(16, 16, 16, 16);

  layout->setObjectName("mainWindowChannelGridLayout");
  addChannelWidget->setStyleSheet(
      "QWidget#PanelChannelOverview { border-radius: 12px; border: 1px solid rgb(170, 170, 170); padding-top: "
      "10px; "
      "padding-bottom: 10px;"
      "background-color: rgba(0, 104, 117, 0);}");

  addChannelWidget->setLayout(layout);
  layout->setSpacing(0);

  QWidget *widgetAddChannel     = new QWidget();
  QHBoxLayout *layoutAddChannel = new QHBoxLayout();
  layoutAddChannel->setContentsMargins(0, 0, 0, 0);

  widgetAddChannel->setLayout(layoutAddChannel);

  //
  // Open template
  //
  mTemplateSelection = new QComboBox();
  mTemplateSelection->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  layoutAddChannel->addWidget(mTemplateSelection);

  //
  // Add channel
  //
  QPushButton *addChannelButton = new QPushButton();
  addChannelButton->setText("Add image Channel");
  addChannelButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  connect(addChannelButton, &QPushButton::pressed, this, &WindowMain::onAddChannelClicked);
  layoutAddChannel->addWidget(addChannelButton);

  layout->addWidget(widgetAddChannel);

  //
  // Add cell voronoi
  //
  QPushButton *addVoronoiButton = new QPushButton();
  const QIcon voronoiIcon(":/icons/outlined/dom-voronoi-50.png");
  addVoronoiButton->setText("Add voronoi channel");
  addVoronoiButton->setIconSize({16, 16});
  addVoronoiButton->setIcon(voronoiIcon);
  connect(addVoronoiButton, &QPushButton::pressed, this, &WindowMain::onAddCellApproxClicked);
  layout->addWidget(addVoronoiButton);

  //
  // Add intersection voronoi
  //
  QPushButton *addIntersection = new QPushButton();
  const QIcon intersectionIcon(":/icons/outlined/icons8-query-inner-join-50.png");
  addIntersection->setIconSize({16, 16});
  addIntersection->setIcon(intersectionIcon);
  addIntersection->setText("Add intersection channel");
  connect(addIntersection, &QPushButton::pressed, this, &WindowMain::onAddIntersectionClicked);
  layout->addWidget(addIntersection);

  //
  // Open settings
  //
  QPushButton *openSettingsButton = new QPushButton();
  openSettingsButton->setText("Load channel settings");
  connect(openSettingsButton, &QPushButton::pressed, this, &WindowMain::onOpenAnalyzeSettingsClicked);
  layout->addWidget(openSettingsButton);

  //
  // Add giraf
  //
  QPushButton *addGiraf = new QPushButton();
  addGiraf->setText("or add the Giraf");
  connect(addGiraf, &QPushButton::pressed, this, &WindowMain::onAddGirafClicked);
  layout->addWidget(addGiraf);

  layout->setSpacing(8);    // Adjust this value as needed
  layout->addStretch();
  addChannelWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  mAddChannelPanel = addChannelWidget;
  return addChannelWidget;
}

///
/// \brief      On add giraf clicked
/// \author     Joachim Danmayr
///
void WindowMain::onAddGirafClicked()
{
  if(mAddChannelPanel != nullptr) {
    {
      int row = (mChannels.size() + 1) / OVERVIEW_COLS;
      int col = (mChannels.size() + 1) % OVERVIEW_COLS;
      mLayoutChannelOverview->removeWidget(mAddChannelPanel);
      mLayoutChannelOverview->removeWidget(mLastElement);
      mLayoutChannelOverview->addWidget(mAddChannelPanel, row, col);
      mLayoutChannelOverview->addWidget(mLastElement, row + 1, 0, 1, OVERVIEW_COLS);
    }

    int row                = mChannels.size() / OVERVIEW_COLS;
    int col                = mChannels.size() % OVERVIEW_COLS;
    ContainerGiraf *panel1 = new ContainerGiraf(this);
    panel1->fromSettings();
    panel1->toSettings();
    mChannels.emplace(panel1, &panel1);
    mLayoutChannelOverview->addWidget(panel1->getOverviewPanel(), row, col);
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onOpenProjectClicked()
{
  QString folderToOpen = QDir::homePath();
  if(!mSelectedWorkingDirectory.isEmpty()) {
    folderToOpen = mSelectedWorkingDirectory;
  }
  QString selectedDirectory = QFileDialog::getExistingDirectory(this, "Select a directory", folderToOpen);

  if(selectedDirectory.isEmpty()) {
    return;
  }

  setWorkingDirectory(selectedDirectory.toStdString());
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onOpenAnalyzeSettingsClicked()
{
  QString folderToOpen = QDir::homePath();
  if(!mSelectedWorkingDirectory.isEmpty()) {
    folderToOpen = mSelectedWorkingDirectory;
  }

  QFileDialog::Options opt;
  opt.setFlag(QFileDialog::DontUseNativeDialog, false);

  QString filePath =
      QFileDialog::getOpenFileName(this, "Open File", folderToOpen, "JSON Files (*.json);;All Files (*)", nullptr, opt);

  if(filePath.isEmpty()) {
    return;
  }

  try {
    std::ifstream ifs(filePath.toStdString());
    joda::settings::AnalyzeSettings analyzeSettings = nlohmann::json::parse(ifs);
    removeAllChannels();

    for(const auto &channel : analyzeSettings.channels) {
      addChannel(channel);
    }

    for(const auto &channel : analyzeSettings.vChannels) {
      if(channel.$voronoi.has_value()) {
        addVChannelVoronoi(channel.$voronoi.value());
      }

      if(channel.$intersection.has_value()) {
        addVChannelIntersection(channel.$intersection.value());
      }
    }

    mAnalyzeSettings.experimentSettings = analyzeSettings.experimentSettings;

  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
    QMessageBox messageBox(this);
    auto *icon = new QIcon(":/icons/outlined/icons8-warning-50.png");
    messageBox.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    messageBox.setIconPixmap(icon->pixmap(42, 42));
    messageBox.setWindowTitle("Could not load settings!");
    messageBox.setText("Could not load settings, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
ContainerBase *WindowMain::addChannelFromTemplate(const QString &filePath)
{
  try {
    std::ifstream ifs(filePath.toStdString());
    settings::ChannelSettings settings = nlohmann::json::parse(ifs);
    auto *newChannel                   = addChannel(settings);
    return newChannel;
  } catch(const std::exception &ex) {
    if(mSelectedChannel != nullptr) {
      QMessageBox messageBox(this);
      auto *icon = new QIcon(":/icons/outlined/icons8-warning-50.png");
      messageBox.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
      messageBox.setIconPixmap(icon->pixmap(42, 42));
      messageBox.setWindowTitle("Could not load settings!");
      messageBox.setText("Could not load settings, got error >" + QString(ex.what()) + "<!");
      messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
      auto reply = messageBox.exec();
    }
  }
  return nullptr;
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::setWorkingDirectory(const std::string &workingDir)
{
  mSelectedWorkingDirectory = workingDir.data();

  std::lock_guard<std::mutex> lock(mLookingForFilesMutex);
  mFoundFilesHint->setText("Looking for images ...");
  mFoundFilesCombo->clear();
  mFileSelectorComboBox->setVisible(false);
  mImageSeriesComboBox->setVisible(false);
  mImageTilesComboBox->setVisible(false);
  mFileSearchHintLabel->setVisible(true);
  mController->setWorkingDirectory(mSelectedWorkingDirectory.toStdString());
  mNewFolderSelected = true;
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::waitForFileSearchFinished()
{
  auto result = helper::templates::TemplateParser::findTemplates();
  emit lookingForTemplateFinished(result);

  while(true) {
    while(true) {
      {
        std::lock_guard<std::mutex> lock(mLookingForFilesMutex);
        if(mNewFolderSelected) {
          break;
        }
      }
      std::this_thread::sleep_for(2s);
    }
    while(mController->isLookingForFiles()) {
      std::this_thread::sleep_for(500ms);
    }
    {
      std::lock_guard<std::mutex> lock(mLookingForFilesMutex);
      mNewFolderSelected = false;
    }
    emit lookingForFilesFinished();
  }
}

///
/// \brief      Remove all channels
/// \author     Joachim Danmayr
///
void WindowMain::removeAllChannels()
{
  std::map<ContainerBase *, void *> channelsToDelete = mChannels;
  for(const auto &[channel, _] : channelsToDelete) {
    removeChannel(channel);
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onLookingForFilesFinished()
{
  int idx = 0;
  for(const auto &file : mController->getListOfFoundImages()) {
    mFoundFilesCombo->addItem(QString(file.getFilename().data()), idx);
    idx++;
  }
  if(mController->getNrOfFoundImages() > 0) {
    mFoundFilesCombo->setCurrentIndex(0);
    auto props = mController->getImageProperties(0, 0);
    mFoundFilesHint->setText("Finished");
    mFileSearchHintLabel->setVisible(false);
    mFileSelectorComboBox->setVisible(true);
    mImageSeriesComboBox->setVisible(true);
    mImageTilesComboBox->setVisible(true);
    mStartAnalysis->setEnabled(true);

    mImageTilesCombo->clear();
    if(props.nrOfTiles == 0) {
      mImageTilesCombo->addItem("0", 0);
      mImageTilesCombo->setCurrentIndex(0);
    } else {
      for(int n = 0; n < props.nrOfTiles; n++) {
        mImageTilesCombo->addItem(QString::number(n), n);
      }
      mImageTilesCombo->setCurrentIndex(0);
    }

  } else {
    // mFoundFilesCombo->setVisible(false);
    mFoundFilesHint->setText("No images found!");
    mStartAnalysis->setEnabled(false);
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onSaveProjectClicked()
{
  QString filePath =
      QFileDialog::getSaveFileName(this, "Save File", QDir::homePath(), "JSON Files (*.json);;All Files (*)");
  if(!filePath.isEmpty()) {
    joda::settings::Settings::storeSettings(filePath.toStdString(), mAnalyzeSettings);
  }
}

///
/// \brief      Templates loaded from templates folder
/// \author     Joachim Danmayr
///
void WindowMain::onFindTemplatesFinished(std::map<std::string, helper::templates::TemplateParser::Data> foundTemplates)
{
  mTemplateSelection->clear();
  const QIcon empty(":/icons/outlined/icons8-select-none-50.png");
  mTemplateSelection->addItem(QIcon(empty.pixmap(28, 28)), "Empty channel", "");
  const QIcon myIcon(":/icons/outlined/icon_eva.png");
  for(const auto &[_, data] : foundTemplates) {
    mTemplateSelection->addItem(QIcon(myIcon.pixmap(28, 28)), data.title.data(), data.path.data());
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onStartClicked()
{
  try {
    joda::settings::Settings::checkSettings(mAnalyzeSettings);

    DialogAnalyzeRunning dialg(this, mAnalyzeSettings);
    dialg.exec();

    // Analysis finished -> generate new name
    mJobName->setText("");
    mJobName->setPlaceholderText(joda::helper::RandomNameGenerator::GetRandomName().data());
  } catch(const std::exception &ex) {
    QMessageBox messageBox(this);
    auto *icon = new QIcon(":/icons/outlined/icons8-error-50.png");
    messageBox.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    // messageBox.setAttribute(Qt::WA_TranslucentBackground);
    messageBox.setIconPixmap(icon->pixmap(42, 42));
    messageBox.setWindowTitle("Error in settings!");
    messageBox.setText(ex.what());
    messageBox.addButton(tr("Okay"), QMessageBox::YesRole);
    // Rounded borders -->
    const int radius = 12;
    messageBox.setStyleSheet(QString("QDialog { "
                                     "border-radius: %1px; "
                                     "border: 2px solid palette(shadow); "
                                     "background-color: palette(base); "
                                     "}")
                                 .arg(radius));

    // The effect will not be actually visible outside the rounded window,
    // but it does help get rid of the pixelated rounded corners.
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
    // The color should match the border color set in CSS.
    effect->setColor(QApplication::palette().color(QPalette::Shadow));
    effect->setBlurRadius(8);
    messageBox.setGraphicsEffect(effect);

    // Need to show the box before we can get its proper dimensions.
    messageBox.show();

    // Here we draw the mask to cover the "cut off" corners, otherwise they show through.
    // The mask is sized based on the current window geometry. If the window were resizable (somehow)
    // then the mask would need to be set in resizeEvent().
    const QRect rect(QPoint(0, 0), messageBox.geometry().size());
    QBitmap b(rect.size());
    b.fill(QColor(Qt::color0));
    QPainter painter(&b);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::color1);
    // this radius should match the CSS radius
    painter.drawRoundedRect(rect, radius, radius, Qt::AbsoluteSize);
    painter.end();
    messageBox.setMask(b);
    // <--

    messageBox.exec();
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onBackClicked()
{
  setMiddelLabelText("");
  mBackButton->setEnabled(false);
  mSaveProject->setVisible(true);
  mSaveProject->setVisible(true);
  mOPenProject->setVisible(true);
  mStartAnalysis->setVisible(true);
  mJobNameAction->setVisible(true);
  mDeleteChannel->setVisible(false);
  mFirstSeparator->setVisible(true);
  mSecondSeparator->setVisible(true);
  mOpenReportingArea->setVisible(true);
  mStackedWidget->setCurrentIndex(0);
  if(mPanelReporting != nullptr) {
    mPanelReporting->close();
  }
  if(mSelectedChannel != nullptr) {
    mSelectedChannel->toSettings();
    mSelectedChannel->setActive(false);
    mSelectedChannel = nullptr;
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::showChannelEdit(ContainerBase *selectedChannel)
{
  mSelectedChannel = selectedChannel;
  selectedChannel->setActive(true);

  mBackButton->setEnabled(true);
  mSaveProject->setVisible(false);
  mSaveProject->setVisible(false);
  mOPenProject->setVisible(false);
  mStartAnalysis->setVisible(false);
  mJobNameAction->setVisible(false);
  mDeleteChannel->setVisible(true);
  mFirstSeparator->setVisible(false);
  mSecondSeparator->setVisible(false);
  mOpenReportingArea->setVisible(false);
  mStackedWidget->removeWidget(mStackedWidget->widget(1));
  mStackedWidget->insertWidget(1, selectedChannel->getEditPanel());
  mStackedWidget->setCurrentIndex(1);
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onOpenReportingAreaClicked()
{
  QString folderToOpen = QDir::homePath();
  if(!mSelectedWorkingDirectory.isEmpty()) {
    folderToOpen = mSelectedWorkingDirectory;
  }

  QFileDialog::Options opt;
  opt.setFlag(QFileDialog::DontUseNativeDialog, false);

  QString filePath =
      QFileDialog::getOpenFileName(this, "Open File", folderToOpen, "imageC Files (*.duckdb)", nullptr, opt);

  if(filePath.isEmpty()) {
    return;
  }
  try {
    mPanelReporting->setActualSelectedWorkingFile(filePath.toStdString());

    // Open reporting area
    mBackButton->setEnabled(true);
    mOPenProject->setVisible(false);
    mSaveProject->setVisible(false);
    mSaveProject->setVisible(false);
    mStartAnalysis->setVisible(false);
    mJobNameAction->setVisible(false);
    mDeleteChannel->setVisible(false);
    mFirstSeparator->setVisible(false);
    mSecondSeparator->setVisible(false);
    mOpenReportingArea->setVisible(false);
    mStackedWidget->setCurrentIndex(mStackedWidget->count() - 1);
  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
    QMessageBox messageBox(this);
    auto *icon = new QIcon(":/icons/outlined/icons8-warning-50.png");
    messageBox.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    messageBox.setIconPixmap(icon->pixmap(42, 42));
    messageBox.setWindowTitle("Could not load database!");
    messageBox.setText("Could not load settings, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onRemoveChannelClicked()
{
  if(mSelectedChannel != nullptr) {
    QMessageBox messageBox(this);
    auto *icon = new QIcon(":/icons/outlined/icons8-warning-50.png");
    messageBox.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    // messageBox.setAttribute(Qt::WA_TranslucentBackground);
    messageBox.setIconPixmap(icon->pixmap(42, 42));
    messageBox.setWindowTitle("Remove channel?");
    messageBox.setText("Do you want to remove the channel?");
    messageBox.addButton(tr("No"), QMessageBox::NoRole);
    messageBox.addButton(tr("Yes"), QMessageBox::YesRole);
    // Rounded borders -->
    const int radius = 12;
    messageBox.setStyleSheet(QString("QDialog { "
                                     "border-radius: %1px; "
                                     "border: 2px solid palette(shadow); "
                                     "background-color: palette(base); "
                                     "}")
                                 .arg(radius));

    // The effect will not be actually visible outside the rounded window,
    // but it does help get rid of the pixelated rounded corners.
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
    // The color should match the border color set in CSS.
    effect->setColor(QApplication::palette().color(QPalette::Shadow));
    effect->setBlurRadius(8);
    messageBox.setGraphicsEffect(effect);

    // Need to show the box before we can get its proper dimensions.
    messageBox.show();

    // Here we draw the mask to cover the "cut off" corners, otherwise they show through.
    // The mask is sized based on the current window geometry. If the window were resizable (somehow)
    // then the mask would need to be set in resizeEvent().
    const QRect rect(QPoint(0, 0), messageBox.geometry().size());
    QBitmap b(rect.size());
    b.fill(QColor(Qt::color0));
    QPainter painter(&b);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::color1);
    // this radius should match the CSS radius
    painter.drawRoundedRect(rect, radius, radius, Qt::AbsoluteSize);
    painter.end();
    messageBox.setMask(b);
    // <--

    auto reply = messageBox.exec();
    if(reply == 1) {
      removeChannel(mSelectedChannel);
    }
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onOpenSettingsDialog()
{
  DialogExperimentSettings di(this, mAnalyzeSettings.experimentSettings);
  di.exec();
}

///
/// \brief
/// \author     Joachim Danmayr
///
ContainerBase *WindowMain::addChannel(joda::settings::ChannelSettings settings)
{
  if(mAddChannelPanel != nullptr) {
    {
      int row = (mChannels.size() + 1) / OVERVIEW_COLS;
      int col = (mChannels.size() + 1) % OVERVIEW_COLS;
      mLayoutChannelOverview->removeWidget(mAddChannelPanel);
      mLayoutChannelOverview->removeWidget(mLastElement);
      mLayoutChannelOverview->addWidget(mAddChannelPanel, row, col);
      mLayoutChannelOverview->addWidget(mLastElement, row + 1, 0, 1, OVERVIEW_COLS);
    }

    int row = mChannels.size() / OVERVIEW_COLS;
    int col = mChannels.size() % OVERVIEW_COLS;
    ContainerBase *panel1;

    mAnalyzeSettings.channels.push_back(settings);
    joda::settings::ChannelSettings &newlyAdded = mAnalyzeSettings.channels.back();
    panel1                                      = new ContainerChannel(this, newlyAdded);
    panel1->fromSettings();
    panel1->toSettings();
    mChannels.emplace(panel1, &newlyAdded);
    mLayoutChannelOverview->addWidget(panel1->getOverviewPanel(), row, col);
    return panel1;
  }
  return nullptr;
}

///
/// \brief
/// \author     Joachim Danmayr
///
ContainerBase *WindowMain::addVChannelVoronoi(joda::settings::VChannelVoronoi settings)
{
  if(mAddChannelPanel != nullptr) {
    {
      int row = (mChannels.size() + 1) / OVERVIEW_COLS;
      int col = (mChannels.size() + 1) % OVERVIEW_COLS;
      mLayoutChannelOverview->removeWidget(mAddChannelPanel);
      mLayoutChannelOverview->removeWidget(mLastElement);
      mLayoutChannelOverview->addWidget(mAddChannelPanel, row, col);
      mLayoutChannelOverview->addWidget(mLastElement, row + 1, 0, 1, OVERVIEW_COLS);
    }

    int row = mChannels.size() / OVERVIEW_COLS;
    int col = mChannels.size() % OVERVIEW_COLS;
    ContainerBase *panel1;

    mAnalyzeSettings.vChannels.push_back(joda::settings::VChannelSettings{.$voronoi = settings});
    joda::settings::VChannelSettings &newlyAdded = mAnalyzeSettings.vChannels.back();
    panel1                                       = new ContainerVoronoi(this, newlyAdded.$voronoi.value());
    panel1->fromSettings();
    panel1->toSettings();
    mChannels.emplace(panel1, &newlyAdded);

    mLayoutChannelOverview->addWidget(panel1->getOverviewPanel(), row, col);
    return panel1;
  }
  return nullptr;
}

///
/// \brief
/// \author     Joachim Danmayr
///
ContainerBase *WindowMain::addVChannelIntersection(joda::settings::VChannelIntersection settings)
{
  if(mAddChannelPanel != nullptr) {
    {
      int row = (mChannels.size() + 1) / OVERVIEW_COLS;
      int col = (mChannels.size() + 1) % OVERVIEW_COLS;
      mLayoutChannelOverview->removeWidget(mAddChannelPanel);
      mLayoutChannelOverview->removeWidget(mLastElement);
      mLayoutChannelOverview->addWidget(mAddChannelPanel, row, col);
      mLayoutChannelOverview->addWidget(mLastElement, row + 1, 0, 1, OVERVIEW_COLS);
    }

    int row = mChannels.size() / OVERVIEW_COLS;
    int col = mChannels.size() % OVERVIEW_COLS;
    ContainerBase *panel1;

    mAnalyzeSettings.vChannels.push_back(joda::settings::VChannelSettings{.$intersection = settings});
    joda::settings::VChannelSettings &newlyAdded = mAnalyzeSettings.vChannels.back();
    panel1                                       = new ContainerIntersection(this, newlyAdded.$intersection.value());
    panel1->fromSettings();
    panel1->toSettings();
    mChannels.emplace(panel1, &newlyAdded);

    mLayoutChannelOverview->addWidget(panel1->getOverviewPanel(), row, col);
    return panel1;
  }
  return nullptr;
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onAddChannelClicked()
{
  if(mTemplateSelection->currentIndex() > 0) {
    addChannelFromTemplate(mTemplateSelection->currentData().toString());
  } else {
    addChannel({});
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onAddCellApproxClicked()
{
  addVChannelVoronoi({});
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onAddIntersectionClicked()
{
  addVChannelIntersection({});
}

void WindowMain::removeChannel(ContainerBase *toRemove)
{
  /// \todo reorder
  if(toRemove != nullptr) {
    toRemove->setActive(false);
    void *elementInSettings = mChannels.at(toRemove);
    mChannels.erase(toRemove);

    mAnalyzeSettings.channels.remove_if(
        [&elementInSettings](const joda::settings::ChannelSettings &item) { return &item == elementInSettings; });

    mAnalyzeSettings.vChannels.remove_if(
        [&elementInSettings](const joda::settings::VChannelSettings &item) { return &item == elementInSettings; });

    mLayoutChannelOverview->removeWidget(toRemove->getOverviewPanel());
    toRemove->getOverviewPanel()->setParent(nullptr);
    delete toRemove;

    // Reorder all panels
    int cnt = 0;
    for(const auto &[panelToReorder, _] : mChannels) {
      mLayoutChannelOverview->removeWidget(panelToReorder->getOverviewPanel());
      int row = (cnt) / OVERVIEW_COLS;
      int col = (cnt) % OVERVIEW_COLS;
      mLayoutChannelOverview->addWidget(panelToReorder->getOverviewPanel(), row, col);
      cnt++;
    }

    {
      int row = (mChannels.size()) / OVERVIEW_COLS;
      int col = (mChannels.size()) % OVERVIEW_COLS;
      mLayoutChannelOverview->removeWidget(mAddChannelPanel);
      mLayoutChannelOverview->removeWidget(mLastElement);
      mLayoutChannelOverview->addWidget(mAddChannelPanel, row, col);
      mLayoutChannelOverview->addWidget(mLastElement, row + 1, 0, 1, OVERVIEW_COLS);
    }
    mSelectedChannel = nullptr;
    onBackClicked();
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \return
///
void WindowMain::onShowInfoDialog()
{
  DialogShadow messageBox(this);
  messageBox.setWindowTitle("Info");
  auto *mainLayout = new QVBoxLayout(&messageBox);
  mainLayout->setContentsMargins(28, 28, 28, 28);
  QLabel *helpTextLabel = new QLabel(
      "<p style=\"text-align: left;\"><strong>imageC " + QString(Version::getVersion().data()) + " (" +
      QString(Version::getBuildTime().data()) +
      ")</strong></p>"
      "<p style=\"text-align: left;\"><em>Licensed under AGPL-3.0<br />Free for non commercial use."
      "</em></p>"
      "<p style=\"text-align: left;\"><strong>Many thanks</strong> for help in setting this project to Melanie "
      "Schuerz</p>"
      "<p style=\"text-align: left;\"><strong>Thank you very much for your help in training the AI "
      "models</strong><br "
      "/>Melanie Schuerz, Anna Mueller, Tanja Plank, Maria Jaritsch, Heloisa Melobenirschke, Patricia Hrasnova and "
      "Ritesh Khanna</p>"
      "<p style=\"text-align: left;\"><em>Icons from <a href=\"https://icons8.com/\">https://icons8.com/</a> and "
      "Dominik Handl.<br /> Special thanks to Tanja Plank for the logo design.</em></p>"
      "<p style=\"text-align: left;\">(c) 2022-2024 Joachim Danmayr</p>");
  helpTextLabel->setOpenExternalLinks(true);
  helpTextLabel->setWordWrap(true);
  QFont fontLineEdit;
  fontLineEdit.setPixelSize(16);
  helpTextLabel->setFont(fontLineEdit);
  mainLayout->addWidget(helpTextLabel);
  mainLayout->addStretch();
  mainLayout->invalidate();
  mainLayout->activate();
  helpTextLabel->adjustSize();
  helpTextLabel->setMinimumHeight(helpTextLabel->height() + 56);

  messageBox.exec();
}

}    // namespace joda::ui::qt
