
// https://stackoverflow.com/questions/2050462/prevent-a-qmenu-from-closing-when-one-of-its-qaction-is-triggered/2054884#2054884

#pragma once

// this menu don't hide, if action in actions_with_showed_menu is chosen.
#include <qmenu.h>
#include <QMouseEvent>

class ShowedMenu : public QMenu
{
  Q_OBJECT
public:
  ShowedMenu(QWidget *parent = 0);
  ShowedMenu(const QString &title, QWidget *parent = 0) : QMenu(title, parent)
  {
    is_ignore_hide = false;
  }
  void add_action_with_showed_menu(const QAction *action)
  {
    actions_with_showed_menu.insert(action);
  }

  void setVisible(bool visible) override
  {
    if(is_ignore_hide) {
      is_ignore_hide = false;
      return;
    }
    QMenu::setVisible(visible);
  }

  void mouseReleaseEvent(QMouseEvent *e) override
  {
    const QAction *action = actionAt(e->pos());
    if(action != nullptr) {
      if(actions_with_showed_menu.contains(action)) {
        is_ignore_hide = true;
      }
    }
    QMenu::mouseReleaseEvent(e);
  }

private:
  // clicking on this actions don't close menu
  QSet<const QAction *> actions_with_showed_menu;
  bool is_ignore_hide;
};
