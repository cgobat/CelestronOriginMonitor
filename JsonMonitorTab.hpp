#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QSet>
#include <QMap>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QScrollBar>
#include <QSplitter>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>

class OriginBackend;

class JsonMonitorTab : public QWidget {
    Q_OBJECT
public:
    explicit JsonMonitorTab(OriginBackend *backend, QWidget *parent = nullptr);

private slots:
    void onRawJson(const QString &direction, const QJsonObject &json);
    void onClearClicked();
    void onPauseClicked();
    void onFilterChanged();

private:
    void setupUI();
    void addMessage(const QString &direction, const QJsonObject &json);
    QString formatJson(const QJsonObject &json, int indent = 0) const;
    bool passesFilter(const QString &direction, const QJsonObject &json) const;

    OriginBackend *m_backend;

    // Message log
    QTextEdit *m_logView;
    bool m_paused = false;

    // Controls
    QPushButton *m_clearButton;
    QPushButton *m_pauseButton;
    QComboBox *m_directionFilter;   // All / SEND / RECV
    QLineEdit *m_textFilter;        // free-text search

    // Field visibility tree
    QTreeWidget *m_fieldTree;
    QSet<QString> m_hiddenFields;   // fields the user has unchecked
    QMap<QString, QTreeWidgetItem*> m_fieldItems;  // field name -> tree item

    int m_messageCount = 0;
    static const int MAX_MESSAGES = 2000;
};
