#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include "AutopilotController.hpp"

class AutopilotTab : public QWidget {
    Q_OBJECT
public:
    explicit AutopilotTab(AutopilotController *controller, QWidget *parent = nullptr);

private slots:
    void onStateChanged(AutopilotController::State state);
    void onLogMessage(const QString &message);
    void onTargetScoresUpdated();
    void onFrameProcessed(const ImageAnalyzer::FrameStats &stats);
    void onStackUpdated(int totalFrames, double totalExposure);
    void onExposureChanged(double seconds, int iso);

    void onStartClicked();
    void onPauseClicked();
    void onStopClicked();

private:
    void setupUI();
    void updateStackPreview();
    void updateHistogramPreview();

    AutopilotController *m_controller;

    // Control buttons
    QPushButton *m_startButton;
    QPushButton *m_pauseButton;
    QPushButton *m_stopButton;
    QLabel *m_stateLabel;

    // Configuration
    QSpinBox *m_frameCountSpin;
    QDoubleSpinBox *m_exposureSpin;
    QSpinBox *m_isoSpin;
    QDoubleSpinBox *m_minAltSpin;
    QDoubleSpinBox *m_minMoonDistSpin;

    // Target scores table
    QTableWidget *m_targetTable;

    // Stack preview
    QLabel *m_stackPreview;
    QLabel *m_stackInfoLabel;

    // Histogram
    QLabel *m_histogramPreview;
    QLabel *m_statsLabel;

    // Decision log
    QTextEdit *m_logView;
};
