#include "AutopilotTab.hpp"
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QScrollBar>

AutopilotTab::AutopilotTab(AutopilotController *controller, QWidget *parent)
    : QWidget(parent)
    , m_controller(controller)
{
    setupUI();

    // Connect controller signals
    connect(m_controller, &AutopilotController::stateChanged,
            this, &AutopilotTab::onStateChanged);
    connect(m_controller, &AutopilotController::logMessage,
            this, &AutopilotTab::onLogMessage);
    connect(m_controller, &AutopilotController::targetScoresUpdated,
            this, &AutopilotTab::onTargetScoresUpdated);
    connect(m_controller, &AutopilotController::exposureChanged,
            this, &AutopilotTab::onExposureChanged);

    ImageAnalyzer *analyzer = m_controller->imageAnalyzer();
    connect(analyzer, &ImageAnalyzer::frameProcessed,
            this, &AutopilotTab::onFrameProcessed);
    connect(analyzer, &ImageAnalyzer::stackUpdated,
            this, &AutopilotTab::onStackUpdated);
}

void AutopilotTab::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // --- Top row: controls and state ---
    QHBoxLayout *controlLayout = new QHBoxLayout();

    m_startButton = new QPushButton("Start");
    m_pauseButton = new QPushButton("Pause");
    m_stopButton = new QPushButton("Stop");
    m_pauseButton->setEnabled(false);
    m_stopButton->setEnabled(false);

    connect(m_startButton, &QPushButton::clicked, this, &AutopilotTab::onStartClicked);
    connect(m_pauseButton, &QPushButton::clicked, this, &AutopilotTab::onPauseClicked);
    connect(m_stopButton, &QPushButton::clicked, this, &AutopilotTab::onStopClicked);

    controlLayout->addWidget(m_startButton);
    controlLayout->addWidget(m_pauseButton);
    controlLayout->addWidget(m_stopButton);

    m_stateLabel = new QLabel("State: Idle");
    m_stateLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    controlLayout->addStretch();
    controlLayout->addWidget(m_stateLabel);

    mainLayout->addLayout(controlLayout);

    // --- Configuration row ---
    QHBoxLayout *configLayout = new QHBoxLayout();

    configLayout->addWidget(new QLabel("Frames/target:"));
    m_frameCountSpin = new QSpinBox();
    m_frameCountSpin->setRange(10, 500);
    m_frameCountSpin->setValue(60);
    configLayout->addWidget(m_frameCountSpin);

    configLayout->addWidget(new QLabel("Exposure:"));
    m_exposureSpin = new QDoubleSpinBox();
    m_exposureSpin->setRange(0.5, 30.0);
    m_exposureSpin->setValue(8.0);
    m_exposureSpin->setSuffix("s");
    configLayout->addWidget(m_exposureSpin);

    configLayout->addWidget(new QLabel("ISO:"));
    m_isoSpin = new QSpinBox();
    m_isoSpin->setRange(100, 3200);
    m_isoSpin->setSingleStep(100);
    m_isoSpin->setValue(800);
    configLayout->addWidget(m_isoSpin);

    configLayout->addWidget(new QLabel("Min Alt:"));
    m_minAltSpin = new QDoubleSpinBox();
    m_minAltSpin->setRange(10.0, 45.0);
    m_minAltSpin->setValue(20.0);
    m_minAltSpin->setSuffix(QString(QChar(0x00B0)));
    configLayout->addWidget(m_minAltSpin);

    configLayout->addWidget(new QLabel("Min Moon:"));
    m_minMoonDistSpin = new QDoubleSpinBox();
    m_minMoonDistSpin->setRange(5.0, 60.0);
    m_minMoonDistSpin->setValue(15.0);
    m_minMoonDistSpin->setSuffix(QString(QChar(0x00B0)));
    configLayout->addWidget(m_minMoonDistSpin);

    configLayout->addStretch();
    mainLayout->addLayout(configLayout);

    // --- Middle: target table + stack/histogram ---
    QHBoxLayout *middleLayout = new QHBoxLayout();

    // Left: target scores table
    QGroupBox *targetGroup = new QGroupBox("Target Scores");
    QVBoxLayout *targetLayout = new QVBoxLayout(targetGroup);

    m_targetTable = new QTableWidget(0, 6);
    m_targetTable->setHorizontalHeaderLabels(
        {"Name", "Alt", "Moon", "Time", "HA", "Score"});
    m_targetTable->horizontalHeader()->setStretchLastSection(true);
    m_targetTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_targetTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_targetTable->verticalHeader()->setDefaultSectionSize(20);
    m_targetTable->setAlternatingRowColors(true);
    targetLayout->addWidget(m_targetTable);

    middleLayout->addWidget(targetGroup, 1);

    // Right: stack preview + histogram
    QVBoxLayout *rightLayout = new QVBoxLayout();

    QGroupBox *stackGroup = new QGroupBox("Live Stack");
    QVBoxLayout *stackLayout = new QVBoxLayout(stackGroup);
    m_stackPreview = new QLabel();
    m_stackPreview->setMinimumSize(320, 240);
    m_stackPreview->setAlignment(Qt::AlignCenter);
    m_stackPreview->setStyleSheet("background-color: black;");
    m_stackPreview->setText("No frames");
    stackLayout->addWidget(m_stackPreview);
    m_stackInfoLabel = new QLabel("Frames: 0  Exposure: 0s");
    stackLayout->addWidget(m_stackInfoLabel);
    rightLayout->addWidget(stackGroup);

    QGroupBox *histGroup = new QGroupBox("Histogram");
    QVBoxLayout *histLayout = new QVBoxLayout(histGroup);
    m_histogramPreview = new QLabel();
    m_histogramPreview->setMinimumSize(320, 100);
    m_histogramPreview->setAlignment(Qt::AlignCenter);
    m_histogramPreview->setStyleSheet("background-color: black;");
    histLayout->addWidget(m_histogramPreview);
    m_statsLabel = new QLabel("BG: -  SNR: -  Sharp: -");
    histLayout->addWidget(m_statsLabel);
    rightLayout->addWidget(histGroup);

    middleLayout->addLayout(rightLayout, 1);
    mainLayout->addLayout(middleLayout, 1);

    // --- Bottom: decision log ---
    QGroupBox *logGroup = new QGroupBox("Decision Log");
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    m_logView = new QTextEdit();
    m_logView->setReadOnly(true);
    m_logView->setMaximumHeight(150);
    m_logView->setStyleSheet("font-family: monospace; font-size: 11px;");
    logLayout->addWidget(m_logView);
    mainLayout->addWidget(logGroup);
}

// ---------------------------------------------------------------------------
// Slots
// ---------------------------------------------------------------------------

void AutopilotTab::onStartClicked()
{
    // Apply config to controller
    m_controller->setTargetFrameCount(m_frameCountSpin->value());
    m_controller->setExposure(m_exposureSpin->value());
    m_controller->setISO(m_isoSpin->value());
    m_controller->setMinAltitude(m_minAltSpin->value());
    m_controller->setMinMoonDistance(m_minMoonDistSpin->value());

    m_controller->start();

    m_startButton->setEnabled(false);
    m_pauseButton->setEnabled(true);
    m_stopButton->setEnabled(true);
}

void AutopilotTab::onPauseClicked()
{
    m_controller->pause();
    m_startButton->setEnabled(true);
    m_startButton->setText("Resume");
    m_pauseButton->setEnabled(false);
}

void AutopilotTab::onStopClicked()
{
    m_controller->stop();
    m_startButton->setEnabled(true);
    m_startButton->setText("Start");
    m_pauseButton->setEnabled(false);
    m_stopButton->setEnabled(false);
}

void AutopilotTab::onStateChanged(AutopilotController::State state)
{
    QString target = m_controller->currentTargetName();
    QString text = QString("State: %1").arg(m_controller->stateString());
    if (!target.isEmpty())
        text += QString(" - %1").arg(target);
    m_stateLabel->setText(text);

    if (state == AutopilotController::Idle) {
        m_startButton->setEnabled(true);
        m_startButton->setText("Start");
        m_pauseButton->setEnabled(false);
        m_stopButton->setEnabled(false);
    }
}

void AutopilotTab::onLogMessage(const QString &message)
{
    m_logView->append(message);
    // Auto-scroll to bottom
    QScrollBar *sb = m_logView->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void AutopilotTab::onTargetScoresUpdated()
{
    const QList<AutopilotController::TargetScore> &scores = m_controller->targetScores();

    // Show top 20 visible targets
    int maxRows = scores.size();
    m_targetTable->setRowCount(maxRows);

    for (int i = 0; i < maxRows; i++) {
        const AutopilotController::TargetScore &ts = scores[i];

        m_targetTable->setItem(i, 0, new QTableWidgetItem(
            QString("%1 %2").arg(ts.object.name, ts.object.common_name)));
        m_targetTable->setItem(i, 1, new QTableWidgetItem(
            QString("%1%2").arg(ts.altitude, 0, 'f', 1).arg(QChar(0x00B0))));
        m_targetTable->setItem(i, 2, new QTableWidgetItem(
            QString("%1%2").arg(ts.moonDistance, 0, 'f', 0).arg(QChar(0x00B0))));
        m_targetTable->setItem(i, 3, new QTableWidgetItem(
            QString("%1h").arg(ts.timeToSet, 0, 'f', 1)));
        m_targetTable->setItem(i, 4, new QTableWidgetItem(
            QString("%1h").arg(ts.hourAngle, 0, 'f', 1)));
        m_targetTable->setItem(i, 5, new QTableWidgetItem(
            QString::number(ts.score, 'f', 0)));

        // Highlight current target
        if (ts.object.name == m_controller->currentTargetName()) {
            for (int c = 0; c < 6; c++) {
                QTableWidgetItem *item = m_targetTable->item(i, c);
                if (item) item->setBackground(QColor(40, 80, 40));
            }
        }

        // Dim invisible targets
        if (!ts.visible) {
            for (int c = 0; c < 6; c++) {
                QTableWidgetItem *item = m_targetTable->item(i, c);
                if (item) item->setForeground(Qt::gray);
            }
        }
    }

    m_targetTable->resizeColumnsToContents();
}

void AutopilotTab::onFrameProcessed(const ImageAnalyzer::FrameStats &stats)
{
    m_statsLabel->setText(QString("BG: %1  SNR: %2  Sharp: %3  Hot: %4")
        .arg(stats.medianBackground, 0, 'f', 0)
        .arg(stats.snrEstimate, 0, 'f', 1)
        .arg(stats.sharpness, 0, 'f', 0)
        .arg(stats.hotPixelCount));

    updateStackPreview();
    updateHistogramPreview();
}

void AutopilotTab::onStackUpdated(int totalFrames, double totalExposure)
{
    m_stackInfoLabel->setText(QString("Frames: %1  Exposure: %2s")
        .arg(totalFrames).arg(totalExposure, 0, 'f', 1));
}

void AutopilotTab::onExposureChanged(double seconds, int iso)
{
    m_exposureSpin->setValue(seconds);
    m_isoSpin->setValue(iso);
}

void AutopilotTab::updateStackPreview()
{
    QImage img = m_controller->imageAnalyzer()->getStackedImage();
    if (img.isNull()) return;

    QPixmap pm = QPixmap::fromImage(img).scaled(
        m_stackPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_stackPreview->setPixmap(pm);
}

void AutopilotTab::updateHistogramPreview()
{
    QImage img = m_controller->imageAnalyzer()->getHistogramImage(
        m_histogramPreview->width(), m_histogramPreview->height());
    if (img.isNull()) return;

    m_histogramPreview->setPixmap(QPixmap::fromImage(img));
}
