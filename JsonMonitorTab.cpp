#include "JsonMonitorTab.hpp"
#include "OriginBackend.hpp"
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>
#include <QHeaderView>
#include <QFont>

JsonMonitorTab::JsonMonitorTab(OriginBackend *backend, QWidget *parent)
    : QWidget(parent)
    , m_backend(backend)
{
    setupUI();
    connect(m_backend, &OriginBackend::rawJsonTraffic,
            this, &JsonMonitorTab::onRawJson);
}

void JsonMonitorTab::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // --- Top: controls ---
    QHBoxLayout *controlLayout = new QHBoxLayout();

    m_clearButton = new QPushButton("Clear");
    connect(m_clearButton, &QPushButton::clicked, this, &JsonMonitorTab::onClearClicked);
    controlLayout->addWidget(m_clearButton);

    m_pauseButton = new QPushButton("Pause");
    m_pauseButton->setCheckable(true);
    connect(m_pauseButton, &QPushButton::clicked, this, &JsonMonitorTab::onPauseClicked);
    controlLayout->addWidget(m_pauseButton);

    controlLayout->addWidget(new QLabel("Direction:"));
    m_directionFilter = new QComboBox();
    m_directionFilter->addItems({"All", "SEND", "RECV"});
    connect(m_directionFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &JsonMonitorTab::onFilterChanged);
    controlLayout->addWidget(m_directionFilter);

    controlLayout->addWidget(new QLabel("Filter:"));
    m_textFilter = new QLineEdit();
    m_textFilter->setPlaceholderText("Type to filter messages...");
    connect(m_textFilter, &QLineEdit::textChanged, this, &JsonMonitorTab::onFilterChanged);
    controlLayout->addWidget(m_textFilter, 1);

    mainLayout->addLayout(controlLayout);

    // --- Middle: splitter with log view and field tree ---
    QSplitter *splitter = new QSplitter(Qt::Horizontal);

    // Left: JSON message log
    m_logView = new QTextEdit();
    m_logView->setReadOnly(true);
#ifdef Q_OS_WIN
    m_logView->setFont(QFont("Consolas", 10));
#elif defined(Q_OS_MACOS)
    m_logView->setFont(QFont("Menlo", 11));
#else
    m_logView->setFont(QFont("Monospace", 10));
#endif
    m_logView->setLineWrapMode(QTextEdit::NoWrap);
    splitter->addWidget(m_logView);

    // Right: field visibility tree
    QGroupBox *fieldGroup = new QGroupBox("Show/Hide Fields");
    QVBoxLayout *fieldLayout = new QVBoxLayout(fieldGroup);

    m_fieldTree = new QTreeWidget();
    m_fieldTree->setHeaderLabels({"Field", "Last Value"});
    m_fieldTree->setColumnWidth(0, 180);
    m_fieldTree->setAlternatingRowColors(true);
    m_fieldTree->header()->setStretchLastSection(true);

    connect(m_fieldTree, &QTreeWidget::itemChanged, this, [this](QTreeWidgetItem *item, int) {
        QString field = item->text(0);
        if (item->checkState(0) == Qt::Unchecked)
            m_hiddenFields.insert(field);
        else
            m_hiddenFields.remove(field);
    });

    fieldLayout->addWidget(m_fieldTree);

    // Quick toggle buttons
    QHBoxLayout *toggleLayout = new QHBoxLayout();
    QPushButton *showAll = new QPushButton("Show All");
    QPushButton *hideAll = new QPushButton("Hide All");
    connect(showAll, &QPushButton::clicked, this, [this]() {
        m_hiddenFields.clear();
        for (auto it = m_fieldItems.begin(); it != m_fieldItems.end(); ++it)
            it.value()->setCheckState(0, Qt::Checked);
    });
    connect(hideAll, &QPushButton::clicked, this, [this]() {
        for (auto it = m_fieldItems.begin(); it != m_fieldItems.end(); ++it) {
            m_hiddenFields.insert(it.key());
            it.value()->setCheckState(0, Qt::Unchecked);
        }
    });
    toggleLayout->addWidget(showAll);
    toggleLayout->addWidget(hideAll);
    fieldLayout->addLayout(toggleLayout);

    splitter->addWidget(fieldGroup);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);

    mainLayout->addWidget(splitter, 1);
}

void JsonMonitorTab::onRawJson(const QString &direction, const QJsonObject &json)
{
    // Always update field tree with any new fields we see
    for (auto it = json.constBegin(); it != json.constEnd(); ++it) {
        QString key = it.key();
        QString valueStr;
        if (it.value().isObject())
            valueStr = "{...}";
        else if (it.value().isArray())
            valueStr = "[...]";
        else
            valueStr = it.value().toVariant().toString();

        if (!m_fieldItems.contains(key)) {
            QTreeWidgetItem *item = new QTreeWidgetItem(m_fieldTree);
            item->setText(0, key);
            item->setText(1, valueStr);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(0, Qt::Checked);
            m_fieldItems[key] = item;
        } else {
            m_fieldItems[key]->setText(1, valueStr);
        }

        // Recurse one level into objects
        if (it.value().isObject()) {
            QJsonObject sub = it.value().toObject();
            for (auto sit = sub.constBegin(); sit != sub.constEnd(); ++sit) {
                QString subKey = key + "." + sit.key();
                QString subVal = sit.value().toVariant().toString();
                if (sit.value().isObject()) subVal = "{...}";
                else if (sit.value().isArray()) subVal = "[...]";

                if (!m_fieldItems.contains(subKey)) {
                    QTreeWidgetItem *subItem = new QTreeWidgetItem(m_fieldItems[key]);
                    subItem->setText(0, sit.key());
                    subItem->setText(1, subVal);
                    subItem->setFlags(subItem->flags() | Qt::ItemIsUserCheckable);
                    subItem->setCheckState(0, Qt::Checked);
                    m_fieldItems[subKey] = subItem;
                } else {
                    m_fieldItems[subKey]->setText(1, subVal);
                }
            }
        }
    }

    if (m_paused) return;
    if (!passesFilter(direction, json)) return;

    addMessage(direction, json);
}

bool JsonMonitorTab::passesFilter(const QString &direction, const QJsonObject &json) const
{
    // Direction filter
    QString dirFilter = m_directionFilter->currentText();
    if (dirFilter != "All" && dirFilter != direction)
        return false;

    // Text filter
    QString text = m_textFilter->text();
    if (!text.isEmpty()) {
        QJsonDocument doc(json);
        QString jsonStr = doc.toJson(QJsonDocument::Compact);
        if (!jsonStr.contains(text, Qt::CaseInsensitive))
            return false;
    }

    return true;
}

void JsonMonitorTab::addMessage(const QString &direction, const QJsonObject &json)
{
    // Build filtered JSON display
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    bool isSend = (direction == "SEND");

    // Direction indicator with color
    QString dirTag = isSend
        ? "<span style='color:#5599ff;font-weight:bold;'>&gt;&gt;&gt; SEND</span>"
        : "<span style='color:#55cc55;font-weight:bold;'>&lt;&lt;&lt; RECV</span>";

    QString header = QString("<span style='color:#888;'>%1</span> %2").arg(timestamp, dirTag);

    // Command/Source summary if available
    QString command = json["Command"].toString();
    QString source = json["Source"].toString();
    QString type = json["Type"].toString();
    if (!command.isEmpty() || !source.isEmpty()) {
        header += QString(" <span style='color:#dddd55;'>%1</span>").arg(command);
        if (!source.isEmpty())
            header += QString(" <span style='color:#aaa;'>from %1</span>").arg(source);
        if (!type.isEmpty())
            header += QString(" <span style='color:#aaa;'>[%1]</span>").arg(type);
    }

    QString body = formatJson(json, 0);

    m_logView->append(header);
    if (!body.isEmpty())
        m_logView->append(body);
    m_logView->append("");  // blank line separator

    // Auto-scroll
    QScrollBar *sb = m_logView->verticalScrollBar();
    sb->setValue(sb->maximum());

    // Limit message count
    m_messageCount++;
    if (m_messageCount > MAX_MESSAGES) {
        QTextCursor cursor = m_logView->textCursor();
        cursor.movePosition(QTextCursor::Start);
        // Remove first ~20 lines to avoid doing this too often
        for (int i = 0; i < 20; i++)
            cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        m_messageCount -= 10;
    }
}

QString JsonMonitorTab::formatJson(const QJsonObject &json, int indent) const
{
    QString result;
    QString pad = QString(indent * 2, ' ');

    for (auto it = json.constBegin(); it != json.constEnd(); ++it) {
        QString key = it.key();
        QString fullKey = key;

        // Check if this top-level field is hidden
        if (m_hiddenFields.contains(fullKey))
            continue;

        if (it.value().isObject()) {
            QJsonObject sub = it.value().toObject();
            // Filter sub-fields
            QJsonObject filtered;
            bool anyVisible = false;
            for (auto sit = sub.constBegin(); sit != sub.constEnd(); ++sit) {
                QString subKey = fullKey + "." + sit.key();
                if (!m_hiddenFields.contains(subKey)) {
                    filtered[sit.key()] = sit.value();
                    anyVisible = true;
                }
            }
            if (anyVisible) {
                result += QString("%1<span style='color:#88bbff;'>%2</span>: {\n")
                    .arg(pad, key);
                result += formatJson(filtered, indent + 1);
                result += pad + "}\n";
            }
        } else if (it.value().isArray()) {
            QJsonDocument arrDoc;
            QJsonObject wrapper;
            wrapper["_"] = it.value();
            QString arrStr = QJsonDocument(wrapper).toJson(QJsonDocument::Compact);
            // Extract just the array part
            int start = arrStr.indexOf('[');
            int end = arrStr.lastIndexOf(']');
            if (start >= 0 && end >= 0)
                arrStr = arrStr.mid(start, end - start + 1);
            result += QString("%1<span style='color:#88bbff;'>%2</span>: <span style='color:#ccc;'>%3</span>\n")
                .arg(pad, key, arrStr.toHtmlEscaped());
        } else {
            QString val = it.value().toVariant().toString();
            // Color-code values
            QString valColor = "#ccc";
            if (it.value().isBool())
                valColor = it.value().toBool() ? "#55cc55" : "#cc5555";
            else if (it.value().isDouble())
                valColor = "#ddaa55";

            result += QString("%1<span style='color:#88bbff;'>%2</span>: <span style='color:%3;'>%4</span>\n")
                .arg(pad, key, valColor, val.toHtmlEscaped());
        }
    }
    return result;
}

void JsonMonitorTab::onClearClicked()
{
    m_logView->clear();
    m_messageCount = 0;
}

void JsonMonitorTab::onPauseClicked()
{
    m_paused = m_pauseButton->isChecked();
    m_pauseButton->setText(m_paused ? "Resume" : "Pause");
}

void JsonMonitorTab::onFilterChanged()
{
    // Filter only applies to new messages going forward
}
