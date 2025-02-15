
#pragma once

#include <QComboBox>
#include <QStyledItemDelegate>
#include <QWidget>

class MyDelegate : public QStyledItemDelegate
{
  Q_OBJECT
public:
  MyDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent)
  {
  }

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override
  {
    QComboBox *editor = new QComboBox(parent);
    // Populate the editor with your desired items
    editor->addItem("Option 1");
    editor->addItem("Option 2");
    // ...
    return editor;
  }

  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override
  {
    QComboBox *comboBox = static_cast<QComboBox *>(editor);
    QString value       = comboBox->currentText();
    model->setData(index, value, Qt::EditRole);
  }

  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const override
  {
    editor->setGeometry(option.rect);
  }
};
