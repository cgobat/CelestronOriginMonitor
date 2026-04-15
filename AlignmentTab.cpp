#include "AlignmentTab.hpp"
#include "CoordinateUtils.hpp"
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QScrollBar>

AlignmentTab::AlignmentTab(AlignmentController *controller, QWidget *parent)
    : QWidget(parent)
    , m_controller(controller)
{
    setupUI();

    // Connect controller signals
    connect(m_controller, &AlignmentController::stateChanged,
            this, &AlignmentTab::onStateChanged);
    connect(m_controller, &AlignmentController::logMessage,
            this, &AlignmentTab::onLogMessage);
    connect(m_controller, &AlignmentController::plateSolveCompleted,
            this, &AlignmentTab::onPlateSolveCompleted);
    connect(m_controller, &AlignmentController::modelReady,
            this, &AlignmentTab::onModelReady);
    connect(m_controller, &AlignmentController::starStatusesUpdated,
            this, &AlignmentTab::onStarStatusesUpdated);

    // Mount model leveling signal
    connect(m_controller->mountModel(), &MountModel::levelingUpdated,
            this, &AlignmentTab::onLevelingUpdated);
}

void AlignmentTab::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // --- Top row: controls ---
    QHBoxLayout *controlLayout = new QHBoxLayout();

    m_startButton = new QPushButton("Start Alignment");
    m_abortButton = new QPushButton("Abort");
    m_oneShotButton = new QPushButton("One-Shot Solve");
    m_clearModelButton = new QPushButton("Clear Model");
    m_abortButton->setEnabled(false);

    connect(m_startButton, &QPushButton::clicked, this, &AlignmentTab::onStartClicked);
    connect(m_abortButton, &QPushButton::clicked, this, &AlignmentTab::onAbortClicked);
    connect(m_oneShotButton, &QPushButton::clicked, this, &AlignmentTab::onOneShotClicked);
    connect(m_clearModelButton, &QPushButton::clicked, this, &AlignmentTab::onClearModelClicked);

    controlLayout->addWidget(m_startButton);
    controlLayout->addWidget(m_abortButton);
    controlLayout->addWidget(m_oneShotButton);
    controlLayout->addWidget(m_clearModelButton);

    controlLayout->addSpacing(20);
    controlLayout->addWidget(new QLabel("Stars:"));
    m_numStarsSpin = new QSpinBox();
    m_numStarsSpin->setRange(3, 12);
    m_numStarsSpin->setValue(6);
    controlLayout->addWidget(m_numStarsSpin);

    controlLayout->addStretch();

    m_stateLabel = new QLabel("State: Idle");
    m_stateLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    controlLayout->addWidget(m_stateLabel);

    m_progressLabel = new QLabel("");
    controlLayout->addWidget(m_progressLabel);

    mainLayout->addLayout(controlLayout);

    // --- Middle: three-column layout ---
    QHBoxLayout *middleLayout = new QHBoxLayout();

    // Left: Plate Solve Results
    QGroupBox *solveGroup = new QGroupBox("Last Plate Solve");
    QGridLayout *solveLayout = new QGridLayout(solveGroup);

    int row = 0;
    solveLayout->addWidget(new QLabel("Solved RA:"), row, 0);
    m_solvedRaLabel = new QLabel("--");
    m_solvedRaLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    solveLayout->addWidget(m_solvedRaLabel, row++, 1);

    solveLayout->addWidget(new QLabel("Solved Dec:"), row, 0);
    m_solvedDecLabel = new QLabel("--");
    m_solvedDecLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    solveLayout->addWidget(m_solvedDecLabel, row++, 1);

    solveLayout->addWidget(new QLabel("Error:"), row, 0);
    m_solveErrorLabel = new QLabel("--");
    solveLayout->addWidget(m_solveErrorLabel, row++, 1);

    solveLayout->addWidget(new QLabel("Orientation:"), row, 0);
    m_orientationLabel = new QLabel("--");
    solveLayout->addWidget(m_orientationLabel, row++, 1);

    solveLayout->addWidget(new QLabel("Pixel Scale:"), row, 0);
    m_pixscaleLabel = new QLabel("--");
    solveLayout->addWidget(m_pixscaleLabel, row++, 1);

    solveLayout->addWidget(new QLabel("Field Size:"), row, 0);
    m_fieldSizeLabel = new QLabel("--");
    solveLayout->addWidget(m_fieldSizeLabel, row++, 1);

    solveLayout->addWidget(new QLabel("Solve Time:"), row, 0);
    m_solveTimeLabel = new QLabel("--");
    solveLayout->addWidget(m_solveTimeLabel, row++, 1);

    solveLayout->addWidget(new QLabel("Stars Detected:"), row, 0);
    m_numStarsLabel = new QLabel("--");
    solveLayout->addWidget(m_numStarsLabel, row++, 1);

    solveLayout->setRowStretch(row, 1);
    middleLayout->addWidget(solveGroup);

    // Center: Mount Model
    QVBoxLayout *modelColumn = new QVBoxLayout();

    QGroupBox *modelGroup = new QGroupBox("Mount Model (TPOINT)");
    QVBoxLayout *modelLayout = new QVBoxLayout(modelGroup);

    m_coeffTable = new QTableWidget(8, 2);
    m_coeffTable->setHorizontalHeaderLabels({"Term", "Value (arcsec)"});
    m_coeffTable->verticalHeader()->setVisible(false);
    m_coeffTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_coeffTable->horizontalHeader()->setStretchLastSection(true);

    QStringList termNames = {"IH", "ID", "CH", "NP", "MA", "ME", "TF", "FO"};
    QStringList termDescs = {
        "IH - Azimuth index", "ID - Altitude index",
        "CH - Collimation", "NP - Non-perpendicularity",
        "MA - Az axis misalign (az)", "ME - Az axis misalign (el)",
        "TF - Tube flexure", "FO - Fork flexure"
    };
    for (int i = 0; i < 8; i++) {
        m_coeffTable->setItem(i, 0, new QTableWidgetItem(termDescs[i]));
        m_coeffTable->setItem(i, 1, new QTableWidgetItem("--"));
    }
    m_coeffTable->setMaximumHeight(250);
    modelLayout->addWidget(m_coeffTable);

    QHBoxLayout *rmsLayout = new QHBoxLayout();
    rmsLayout->addWidget(new QLabel("RMS:"));
    m_rmsLabel = new QLabel("--");
    m_rmsLabel->setStyleSheet("font-weight: bold;");
    rmsLayout->addWidget(m_rmsLabel);
    rmsLayout->addSpacing(10);
    rmsLayout->addWidget(new QLabel("Observations:"));
    m_obsCountLabel = new QLabel("0");
    rmsLayout->addWidget(m_obsCountLabel);
    rmsLayout->addStretch();
    modelLayout->addLayout(rmsLayout);

    modelColumn->addWidget(modelGroup);

    // Leveling group
    QGroupBox *levelGroup = new QGroupBox("Leveling (Quaternion)");
    QGridLayout *levelLayout = new QGridLayout(levelGroup);
    levelLayout->addWidget(new QLabel("Quaternion:"), 0, 0);
    m_quaternionLabel = new QLabel("w=1.000 x=0.000 y=0.000 z=0.000");
    m_quaternionLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    levelLayout->addWidget(m_quaternionLabel, 0, 1);
    levelLayout->addWidget(new QLabel("Tilt Angle:"), 1, 0);
    m_tiltAngleLabel = new QLabel("0.000 deg");
    m_tiltAngleLabel->setStyleSheet("font-weight: bold;");
    levelLayout->addWidget(m_tiltAngleLabel, 1, 1);
    modelColumn->addWidget(levelGroup);

    middleLayout->addLayout(modelColumn);

    // Right: Star Status Table
    QGroupBox *starGroup = new QGroupBox("Alignment Stars");
    QVBoxLayout *starLayout = new QVBoxLayout(starGroup);

    m_starTable = new QTableWidget(0, 5);
    m_starTable->setHorizontalHeaderLabels({"Star", "RA", "Dec", "Status", "Error"});
    m_starTable->verticalHeader()->setVisible(false);
    m_starTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_starTable->horizontalHeader()->setStretchLastSection(true);
    m_starTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    starLayout->addWidget(m_starTable);

    middleLayout->addWidget(starGroup);

    mainLayout->addLayout(middleLayout);

    // --- Bottom: log ---
    QGroupBox *logGroup = new QGroupBox("Alignment Log");
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    m_logView = new QTextEdit();
    m_logView->setReadOnly(true);
    m_logView->setMaximumHeight(150);
#ifdef Q_OS_WIN
    m_logView->setFont(QFont("Consolas", 9));
#elif defined(Q_OS_MACOS)
    m_logView->setFont(QFont("Menlo", 10));
#else
    m_logView->setFont(QFont("Monospace", 9));
#endif
    logLayout->addWidget(m_logView);
    mainLayout->addWidget(logGroup);
}

// ---------------------------------------------------------------------------
// Slots
// ---------------------------------------------------------------------------

void AlignmentTab::onStateChanged(AlignmentController::AlignState state)
{
    m_stateLabel->setText(QString("State: %1").arg(m_controller->stateString()));

    bool running = (state != AlignmentController::Idle &&
                    state != AlignmentController::Complete &&
                    state != AlignmentController::Error);

    m_startButton->setEnabled(!running);
    m_abortButton->setEnabled(running);
    m_oneShotButton->setEnabled(!running);
    m_numStarsSpin->setEnabled(!running);

    if (state == AlignmentController::Complete) {
        m_stateLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: green;");
    } else if (state == AlignmentController::Error) {
        m_stateLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: red;");
    } else if (running) {
        m_stateLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: blue;");
    } else {
        m_stateLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    }

    m_progressLabel->setText(QString("%1/%2 stars")
                             .arg(m_controller->completedStars())
                             .arg(m_controller->totalStars()));
}

void AlignmentTab::onLogMessage(const QString &message)
{
    m_logView->append(QString("[%1] %2")
                      .arg(QTime::currentTime().toString("HH:mm:ss"))
                      .arg(message));

    // Auto-scroll to bottom
    QScrollBar *sb = m_logView->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void AlignmentTab::onPlateSolveCompleted(const PlateSolveResult &result)
{
    m_solvedRaLabel->setText(CoordinateUtils::formatRaAsHMS(result.ra));
    m_solvedDecLabel->setText(CoordinateUtils::formatDecAsDMS(result.dec));

    double totalError = std::sqrt(result.raError * result.raError +
                                   result.decError * result.decError);
    m_solveErrorLabel->setText(QString("%1\" (RA=%2\" Dec=%3\")")
                               .arg(totalError, 0, 'f', 1)
                               .arg(result.raError, 0, 'f', 1)
                               .arg(result.decError, 0, 'f', 1));

    m_orientationLabel->setText(QString("%1 deg").arg(result.orientation, 0, 'f', 2));
    m_pixscaleLabel->setText(QString("%1 arcsec/px").arg(result.pixscale, 0, 'f', 3));
    m_fieldSizeLabel->setText(QString("%1' x %2'")
                              .arg(result.fieldWidth, 0, 'f', 1)
                              .arg(result.fieldHeight, 0, 'f', 1));
    m_solveTimeLabel->setText(QString("%1 ms").arg(result.solveTimeMs, 0, 'f', 0));
    m_numStarsLabel->setText(QString::number(result.numStarsDetected));
}

void AlignmentTab::onModelReady(const ModelCoefficients &coeffs)
{
    double values[8] = {
        coeffs.IH, coeffs.ID, coeffs.CH, coeffs.NP,
        coeffs.MA, coeffs.ME, coeffs.TF, coeffs.FO
    };

    for (int i = 0; i < 8; i++) {
        QTableWidgetItem *item = m_coeffTable->item(i, 1);
        if (item) {
            item->setText(QString("%1").arg(values[i], 0, 'f', 2));
            // Color significant terms
            if (std::abs(values[i]) > 30.0) {
                item->setForeground(QBrush(Qt::red));
            } else if (std::abs(values[i]) > 10.0) {
                item->setForeground(QBrush(QColor(200, 100, 0)));
            } else {
                item->setForeground(QBrush(Qt::darkGreen));
            }
        }
    }

    m_rmsLabel->setText(QString("Total=%1\" RA=%2\" Dec=%3\"")
                        .arg(coeffs.rmsTotal, 0, 'f', 1)
                        .arg(coeffs.rmsRaArcsec, 0, 'f', 1)
                        .arg(coeffs.rmsDecArcsec, 0, 'f', 1));
    m_obsCountLabel->setText(QString::number(coeffs.numObservations));
}

void AlignmentTab::onStarStatusesUpdated()
{
    QList<AlignmentController::StarStatus> statuses = m_controller->starStatuses();

    m_starTable->setRowCount(statuses.size());

    for (int i = 0; i < statuses.size(); i++) {
        const auto &ss = statuses[i];

        m_starTable->setItem(i, 0, new QTableWidgetItem(ss.star.name));
        m_starTable->setItem(i, 1, new QTableWidgetItem(
            CoordinateUtils::formatRaAsHMS(ss.star.raDeg)));
        m_starTable->setItem(i, 2, new QTableWidgetItem(
            CoordinateUtils::formatDecAsDMS(ss.star.decDeg)));

        QString statusStr;
        QColor rowColor;
        switch (ss.status) {
        case AlignmentController::StarStatus::Pending:
            statusStr = "Pending"; rowColor = Qt::gray; break;
        case AlignmentController::StarStatus::Slewing:
            statusStr = "Slewing"; rowColor = QColor(100, 100, 255); break;
        case AlignmentController::StarStatus::Solving:
            statusStr = "Solving"; rowColor = QColor(200, 200, 0); break;
        case AlignmentController::StarStatus::Solved:
            statusStr = "Solved"; rowColor = QColor(0, 150, 0); break;
        case AlignmentController::StarStatus::Failed:
            statusStr = "Failed"; rowColor = Qt::red; break;
        case AlignmentController::StarStatus::Skipped:
            statusStr = "Skipped"; rowColor = QColor(150, 150, 150); break;
        }

        QTableWidgetItem *statusItem = new QTableWidgetItem(statusStr);
        statusItem->setForeground(QBrush(rowColor));
        m_starTable->setItem(i, 3, statusItem);

        QString errorStr = (ss.status == AlignmentController::StarStatus::Solved)
                           ? QString("%1\"").arg(ss.errorArcsec, 0, 'f', 1) : "--";
        m_starTable->setItem(i, 4, new QTableWidgetItem(errorStr));
    }
}

void AlignmentTab::onLevelingUpdated(const Quaternion &q)
{
    m_quaternionLabel->setText(QString("w=%1 x=%2 y=%3 z=%4")
                               .arg(q.w, 0, 'f', 6)
                               .arg(q.x, 0, 'f', 6)
                               .arg(q.y, 0, 'f', 6)
                               .arg(q.z, 0, 'f', 6));

    double tiltDeg = q.angle() * 180.0 / M_PI;
    m_tiltAngleLabel->setText(QString("%1 deg").arg(tiltDeg, 0, 'f', 3));

    if (tiltDeg > 2.0) {
        m_tiltAngleLabel->setStyleSheet("font-weight: bold; color: red;");
    } else if (tiltDeg > 0.5) {
        m_tiltAngleLabel->setStyleSheet("font-weight: bold; color: orange;");
    } else {
        m_tiltAngleLabel->setStyleSheet("font-weight: bold; color: green;");
    }
}

// ---------------------------------------------------------------------------
// Button handlers
// ---------------------------------------------------------------------------

void AlignmentTab::onStartClicked()
{
    m_controller->startAlignment(m_numStarsSpin->value());
}

void AlignmentTab::onAbortClicked()
{
    m_controller->abort();
}

void AlignmentTab::onOneShotClicked()
{
    m_controller->oneShotSolve();
}

void AlignmentTab::onClearModelClicked()
{
    m_controller->mountModel()->clear();
    m_rmsLabel->setText("--");
    m_obsCountLabel->setText("0");
    m_quaternionLabel->setText("w=1.000 x=0.000 y=0.000 z=0.000");
    m_tiltAngleLabel->setText("0.000 deg");
    m_tiltAngleLabel->setStyleSheet("font-weight: bold;");

    for (int i = 0; i < 8; i++) {
        QTableWidgetItem *item = m_coeffTable->item(i, 1);
        if (item) {
            item->setText("--");
            item->setForeground(QBrush(Qt::black));
        }
    }
}
